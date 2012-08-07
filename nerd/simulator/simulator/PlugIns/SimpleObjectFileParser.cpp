/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work was funded (2008 - 2011) by EU-Project Number ICT 214856    *
 *                                                                         *
 *                                                                         *
 *   License Agreement:                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 *                                                                         *
 *   Publications based on work using the NERD kit have to state this      *
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/


#include "SimpleObjectFileParser.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Physics/PhysicsManager.h"
#include "Value/ColorValue.h"
#include "PlugIns/CommandLineParser.h"
#include "Physics/SimulationEnvironmentManager.h"
#include "Physics/Physics.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/CommandLineArgument.h"
#include "SimulationConstants.h"
#include "Collision/EventCollisionRule.h"

#include "Randomization/ListRandomizer.h"
#include "Randomization/DoubleRandomizer.h"
#include <iostream>
#include "Event/EventManager.h"
#include "Value/ValueManager.h"
#include <QMutexLocker>
#include "Models/DummyModel.h"
#include "Models/CustomModel.h"


using namespace std;

namespace nerd {

SimpleObjectFileParser::SimpleObjectFileParser() 
	: mEnvironmentCommandLineArgument(0), mLoadEnvironmentXMLEvent(0)
{
	Core::getInstance()->addSystemObject(this);
	mEnvironmentCommandLineArgument = new CommandLineArgument(
		"loadEnvironment", "env", "<environmentFileName>",
		"Loads an environment XML file.", 1, 0, true);

	mXMLFileName = new FileNameValue("");
	
	QStringList fileNames = mEnvironmentCommandLineArgument->getEntries();
	if(!fileNames.empty()) {
		mXMLFileName->set(fileNames.at(0));
	}

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/Physics/XmlEnvironmentFileToLoad", mXMLFileName);
	mXMLFileName->addValueChangedListener(this);
	//mXMLFileName->useAsFileName(true);
	
}

SimpleObjectFileParser::~SimpleObjectFileParser() {
}

/**
 * During the init phase, the commandline arguments are checked for occurances of a  leadEnvironment/env argument. 
 * The according xml-files are afterwards parsed to add the defines objects and rules to the simulation.
 * @return 
 */
bool SimpleObjectFileParser::init() {

	StringValue *scriptedModelArgument = Core::getInstance()->getValueManager()
				->getStringValue("/CommandLineArguments/installScriptedModel");

	if(mEnvironmentCommandLineArgument->getParameterValue()->get() != "") {
		QStringList fileNames = mEnvironmentCommandLineArgument->getEntries();

		if(!fileNames.empty()) {
			mXMLFileName->set(fileNames.at(0));
			mLastXMLFileName = fileNames.at(0);
		}
		for(int i = 0; i < fileNames.size(); ++i) {
			mFileName = fileNames.at(i);

			Core::log("Trying to load file " + mFileName, true);

			if(!loadXmlDescription()) {
				cerr << "There was an error while parsing environment file. "
					 << "See log file for details." << endl;
			}
		}
		Physics::getSimulationEnvironmentManager()->createSnapshot();
	}
	else if(scriptedModelArgument == 0 || scriptedModelArgument->get().trimmed() == "") {
		PhysicsManager *pm = Physics::getPhysicsManager();
		DummyModel *dummy = new DummyModel("Dummy");
		dummy->setup();
		pm->addSimObject(dummy);
		Physics::getSimulationEnvironmentManager()->createSnapshot();
	}

	Event *resetEvent = Physics::getPhysicsManager()->getResetEvent();
	mLoadEnvironmentXMLEvent = Core::getInstance()->getEventManager()
									->createEvent("/Physics/LoadEnvironmentXml");
	mLoadEnvironmentXMLEvent->addEventListener(this);
	resetEvent->addUpstreamEvent(mLoadEnvironmentXMLEvent);

	return true;
}


bool SimpleObjectFileParser::bind() {	
	return true;
}


bool SimpleObjectFileParser::cleanUp() {
	return true;
}

bool SimpleObjectFileParser::loadXmlDescription() {	
	QDomDocument doc("myDocument");
	QFile file(mFileName.trimmed());
	bool successful = true;
	if (!file.open(QIODevice::ReadOnly)) {
		Core::log("SimpleObjectFileParser: File \"" + mFileName.trimmed() + "\" could not be opened!", true);
		file.close();
		return false;
	}

	if(!doc.setContent(&file)) {
		file.close();
		Core::log("SimpleObjectFileParser: Error while reading xml-file \"" + mFileName.trimmed() + "\".", true);
		return false;
	}
	file.close();
	QDomElement docElem = doc.documentElement();

	//checks, whether the environment description is in a valid version
	mVersion = docElem.attribute("version");
	if(mVersion != "Environment XML V1.0" && mVersion != "Environment XML V1.1") {
		Core::log("Version number not Environment XML V1.0 or V1.1!", true);
		file.close();
		return false;
	}	

	QDomNode startNode = docElem.firstChild();
	QDomElement e = startNode.toElement(); 

	//all childs of the main node are sequentially parsed and the according methods 
	//to create objects, models or rules are called. If a unknown tag name is encountered, 
	//the loading of the file is aborted and the objects, models and rules already parsed 
	//are ignored as well. Thereby, if there is one error in the xml-file, nothing will 
	//be added to the simulation at all. 

	//parse environment objects
	QDomNode n = startNode;
	while(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if(e.tagName().compare("Environment") == 0 && successful) {
				if(generateEnvironmentObjects(e) == 0) {
					Core::log("SimpleObjectFileParser: Could not load xml-File: " 
						+ mFileName.trimmed() + ". Error while reading object definitions.", true);
					successful = false;
				}
			} 
		}
		n = n.nextSibling();
	}
	if(successful) {
		//in case of a successful parsing of the whole xml-file, all objects, 
		//models and rules previously parsed are added to the simulation.
		PhysicsManager *pm = Physics::getPhysicsManager();
		for(int i = 0; i < mObjectsToAdd.size(); i++) {
			pm->addSimObject(mObjectsToAdd.at(i));
		}
	}
	else {
		//In case of an error during parsing all parsed objects and rules are ignored.
		while(mObjectsToAdd.size() > 0) {
			delete mObjectsToAdd.at(0);
			mObjectsToAdd.pop_front();
		}
		file.close();
		return false;
	}

	successful = true;
	//parse models and agents
	n = startNode;
	while(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if (((e.tagName().toLower().compare("agent") == 0 
				&& mVersion.compare("Environment XML V1.0") == 0) 
				|| (e.tagName().toLower().compare("models") == 0 
				&& mVersion.compare("Environment XML V1.1") == 0)) 
				&& successful) 
			{
				if(!createAgents(e)) {
					Core::log("SimpleObjectFileParser: Could not load xml-File: " 
						+ mFileName.trimmed() + ". Error while reading agent definitions.", true);
					successful = false;
				}
			}
			else if((e.tagName().toLower()  == "prototypes") && successful) {
				if(!createPrototypes(e)) {
					Core::log("SimpleObjectFileParser: Could not load xml-File: " 
						+ mFileName.trimmed() + ". Error while reading prototype definitions.", true);
					successful = false;
				}
			}
		}
		n = n.nextSibling();
	}
	if(successful) {
		//in case of a successful parsing of the whole xml-file, all objects, 
		//models and rules previously parsed are added to the simulation.
		PhysicsManager *pm = Physics::getPhysicsManager();
		for(int i = 0; i < mAgentsToAdd.size(); i++) {
			mAgentsToAdd.at(i)->setup();
			//TODO check how to do this better (is for randomization)
			pm->addSimObject(mAgentsToAdd.at(i)); 
		}
	}
	else {
		//In case of an error during parsing all parsed objects are ignored.
		while(mAgentsToAdd.size() > 0) {
			delete mAgentsToAdd.at(0);
			mAgentsToAdd.pop_front();
		}
		file.close();
		return false;
	}

	//If the parsing of environment definitions, model definitions and 
	//model definitions was successful finally the randomization rules are parsed.
	QDomNodeList randomizeListDefinitions = docElem.elementsByTagName("randomizeList");
	for(unsigned int i = 0; i < randomizeListDefinitions.length(); i++) {
		parseRandomiseListDefinition(randomizeListDefinitions.item(i).toElement());
	}
	
	QDomNodeList randomizeDefinitions = docElem.elementsByTagName("randomize");
	for(unsigned int i = 0; i < randomizeDefinitions.length(); i++) {
		parseRandomiseDefinition(randomizeDefinitions.item(i).toElement());
	}

	QDomNodeList valueDefinitions = docElem.elementsByTagName("value");
	for(unsigned int i = 0; i < valueDefinitions.length(); i++) {
		parseValueDefinition(valueDefinitions.item(i).toElement());
	}

	if(successful) {
		//add collision rules.
		QDomNodeList terminateRules = docElem.elementsByTagName("CollisionRule");
		for(unsigned int i = 0; i < terminateRules.length(); i++) {
			parseCollisionRule(terminateRules.item(i).toElement());
		}
	}

	Physics::getSimulationEnvironmentManager()->createSnapshot();

	file.close();

	return true;
}

/**
 * Parses a collision rule definition.
 * @param ruleElement 
 */
void SimpleObjectFileParser::parseEventCollisionRule(QDomElement ruleElement) {

	CollisionManager *cm = Physics::getCollisionManager();
	QDomAttr a = ruleElement.attributeNode("name");
	QString ruleName = a.value();
	EventCollisionRule *rule = 0;
	if(cm->getCollisionRule(ruleName) != 0 
		&& dynamic_cast<EventCollisionRule*>(cm->getCollisionRule(ruleName)) != 0) 
	{
		rule = dynamic_cast<EventCollisionRule*>(cm->getCollisionRule(ruleName));
	}
	else {
		if(ruleElement.hasAttribute("event")) {
			QDomAttr event = ruleElement.attributeNode("event");
			QString eventName = event.value();
			if(eventName.compare("terminate") == 0) {
				eventName = SimulationConstants::EVENT_EXECUTION_TERMINATE_TRY;
			}
			rule = new EventCollisionRule(ruleName); 
			rule->addEvent(eventName);
			cm->addCollisionRule(rule);
		}
		else {
			Core::log("SimpleObjectFileParser: Error while creating collision rules. "
				"Missing event-attribute for new rule.", true);
		}
	}
	QDomNode n = ruleElement.firstChild();
	while(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if(e.tagName().compare("source") == 0) {
				if(e.hasAttribute("object")) {
					a = e.attributeNode("object");
					QString sourceName = a.value();

					QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
									->getSimObjects(sourceName);
					for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
						SimBody *sourceObject = dynamic_cast<SimBody*>(j.next());
						if(sourceObject != 0) {
							for(int i = 0; i < sourceObject->getCollisionObjects().size(); i++) {
								rule->addToSourceGroup(sourceObject->getCollisionObjects().at(i));
							}
						}
					}
				}
			}
			else if(e.tagName().compare("target") == 0) {
				if(e.hasAttribute("object")) {
					a = e.attributeNode("object");
					QString targetName = a.value();
					QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
									->getSimObjects(targetName);
					for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
						SimBody *targetObject = dynamic_cast<SimBody*>(j.next());
						if(targetObject != 0) {
							for(int i = 0; i < targetObject->getCollisionObjects().size(); i++) {
								rule->addToTargetGroup(targetObject->getCollisionObjects().at(i));
							}
						}
					}
				}
			}
		}
		n = n.nextSibling();
	}
}

/**
 * Parses an environment definition to create all simulation objects defines.
 * @param element 
 * @return 
 */
bool SimpleObjectFileParser::generateEnvironmentObjects(QDomElement element) {
	bool succeeded = true;
	QDomNode n = element.firstChild();
	while(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if(e.tagName().compare("model") == 0) {
				ModelInterface *model = generateModel(e);
				if(model == 0) {
					return false;
				}
				mNewAgents.push_back(model);
				model->setup();
				Physics::getPhysicsManager()->addSimObject(model);
			}
			else if(e.tagName().compare("object") == 0) {
				if(generateEnvironmentObject(e) == 0) {
					return false;
				}
			}
			else if(e.tagName().compare("objectChain") == 0) {
				if(generateEnvironmentObjectChain(e) == 0) {
					return false;
				}
			}
			else if(e.tagName().compare("switchInput") == 0) {
				parseInfoNeuronDeclaration(e);
			}
		}
		n = n.nextSibling();
	}
	return succeeded;
}

/**
 * Parses the model definition.
 * @param element 
 * @return 
 */
bool SimpleObjectFileParser::createAgents(QDomElement element) {
	QDomNode modelNode = element.firstChild();
	while(!modelNode.isNull()) {
		QDomElement modelElement = modelNode.toElement(); 

		if(!modelElement.isNull()) {
			if(modelElement.tagName().compare("switchInput") == 0) {
				parseInfoNeuronDeclaration(modelElement);
			}
			else {
// 				QString modelName;
// 				if(mVersion.compare("Environment XML V1.0") == 0) {
// 					modelName = modelElement.tagName();
// 				}
// 				else if (mVersion.compare("Environment XML V1.1") == 0
// 					&& modelElement.tagName().compare("model") == 0) 
// 				{
// 					if(modelElement.hasAttribute("prototype")) {
// 						modelName = modelElement.attributeNode("prototype").value();
// 						modelElement.removeAttribute("prototype");
// 					}
// 					else {
// 						Core::log("SimpleObjectFileParser: Could not finde attribute "
// 							"prototype while parsing model definition.", true);
// 					}
// 				}
// 				else {
// 					Core::log("SimpleObjectFileParser: Wrong environment file version! Could "
// 						"not parse model-definition.", true);
// 				}
// 				PhysicsManager *pm = Physics::getPhysicsManager();
// 				SimObject *model  = pm->getPrototype("Prototypes/"+ modelName);
// 				if(model != 0 && dynamic_cast<ModelInterface*>(model) != 0) {
// 					QString name;
// 					QDomAttr a = modelElement.attributeNode("name"); 
// 					name = a.value();
// 					modelElement.removeAttribute("name");
// 					Vector3DValue position(0.0, 0.0, 0.0);
// 					if(modelElement.hasAttribute("position")) {
// 						a = modelElement.attributeNode("position"); 
// 						modelElement.removeAttribute("position");	
// 						if(position.setValueFromString(a.value()) == false) {
// 							Core::log("SimpleObjectFileParser: Position tag could not be"
// 								" parsed.", true);
// 							return false;
// 						}
// 					}
// 					Vector3DValue orientation(0.0, 0.0, 0.0);
// 					if(modelElement.hasAttribute("orientation")) {
// 						a = modelElement.attributeNode("orientation"); 
// 						modelElement.removeAttribute("orientation");	
// 						if(orientation.setValueFromString(a.value()) == false) {
// 							Core::log("SimpleObjectFileParser: Orientation tag could not be "
// 								"parsed.", true);
// 							return false;
// 						}
// 					}
// 			
// 					ModelInterface *agentModel = dynamic_cast<ModelInterface*>(
// 						model->createCopy());
// 					agentModel->setName(name);
// 					if(!agentModel->getParameter("Position")
// 						->setValueFromString(position.getValueAsString())) 
// 					{
// 						Core::log("SimpleObjectFileParser: Could not set position.", true);
// 						return false;
// 					}
// 					agentModel->getParameter("Orientation")
// 						->setValueFromString(orientation.getValueAsString());
// 					QDomNode transformation = modelElement.firstChild();
// 					while(!transformation.isNull()) {
// 						QDomElement transformationElement = transformation.toElement();
// 						if(!transformationElement.isNull()) {
// 							if(transformationElement.tagName().compare("rotation") == 0) {
// 								QDomAttr value = transformationElement.attributeNode("value");
// 								Vector3DValue rotation;
// 								if(!rotation.setValueFromString(value.value())) {
// 									Core::log("SimpleObjectFileParser: Error while parsing "
// 										"transformation tag of simualtion model.", true);
// 									return false;
// 								}
// 								agentModel->addTransformation("rotation", rotation.get());
// 							}
// 							else if(transformationElement.tagName().compare("translation") == 0) {
// 								QDomAttr value = transformationElement.attributeNode("value");
// 								Vector3DValue translation;
// 								if(!translation.setValueFromString(value.value())) {
// 									Core::log("SimpleObjectFileParser: Error while parsing "
// 										"transformation tag of simualtion model.", true);
// 									return false;
// 								}
// 								agentModel->addTransformation("translation", translation.get());
// 							}
// 							else {
// 								Core::log("SimpleObjectFileParser: Found a tag, that is no "
// 									"transformation tag: " + transformationElement.tagName(), true);
// 								return false;
// 							}
// 						}
// 						transformation = transformation.nextSibling();
// 					}
// 					
// 					QDomNamedNodeMap attributes = modelElement.attributes();
// 					for(uint i = 0; i < attributes.length(); i++) {
// 						QDomAttr attribute = attributes.item(i).toAttr();	
// 						if(agentModel->getParameter(attribute.name()) == 0) {
// 							Core::log(QString("SimpleObjectFileParser: Attribute \" ")
// 								.append(attribute.name()).append( "\" does not exist!"), true);
// 							return false;
// 						}
// 						if(!agentModel->getParameter(attribute.name())->setValueFromString(attribute.value())) {
// 							Core::log(QString("SimpleObjectFileParser: Could not set value \" ")
// 								.append(attribute.name()).append( "\" of object \"").append(a.value())
// 								.append("\"!"), true);
// 							return false;
// 						}
// 					}
// 
// 					mAgentsToAdd.push_back(agentModel);
// 				}
// 				else {
// 					Core::log(QString("SimpleObjectFileParser: Could not find prototype of"
// 						" model \"").append(modelName.toStdString().c_str()).append("\""), true);
// 					return false;
// 				}
				ModelInterface *model = generateModel(modelElement);
				if(model == 0) {
					return false;
				}
				mAgentsToAdd.push_back(model);
				mNewAgents.append(model);
			}
		}
		modelNode = modelNode.nextSibling();
  	}
	return true;
}

bool SimpleObjectFileParser::createPrototypes(QDomElement element) {
	QDomNode protoNode = element.firstChild();
	while(!protoNode.isNull()) {
		QDomElement protoElement = protoNode.toElement(); 

		if(!protoElement.isNull()) {
			if(protoElement.tagName() == "prototype") {
				QString prototypeName = "Default";
				if(protoElement.hasAttribute("name")) {
					prototypeName = protoElement.attributeNode("name").value();
					protoElement.removeAttribute("name");
				}
				else {
					Core::log("SimpleObjectFileParser: Could not finde attribute "
						"<name> while parsing prototype definition.", true);
				}

				PhysicsManager *pm = Physics::getPhysicsManager();
				
				CustomModel *model = new CustomModel(prototypeName);

				QDomNode n = protoElement.firstChild();
				while(!n.isNull()) {
					QDomElement e = n.toElement();
					if(!e.isNull()) {
						if(e.tagName().toLower() == "object") {
							SimObject *object = generateEnvironmentObject(e, false);
							if(object != 0) {
								model->addSimObject(object);
							}
							else {
								return false;
							}
						}
						else if(e.tagName().toLower() == "model") {
							CustomModel *childModel = dynamic_cast<CustomModel*>(generateModel(e));
							if(childModel == 0) {
								return false;
							}
							childModel->createModel(true);
							childModel->layoutObjects();
							childModel->performTransformations();
							childModel->addVariablePrefix(childModel->getName());
							childModel->addSimObjectPrefix(childModel->getName());

							QList<SimObject*> simObjects = childModel->getSimObjects();
							for(QListIterator<SimObject*> k(simObjects); k.hasNext();) {
								model->addSimObject(k.next());
							}
							QList<CustomModelVariable*> variables = childModel->getVariables();
							for(QListIterator<CustomModelVariable*> k(variables); k.hasNext();) {
								model->addVariable(k.next());
							}
							delete childModel;
						}
					}
					n = n.nextSibling();
				}

				pm->addPrototype(model->getName(), model);
			}
		}
		protoNode = protoNode.nextSibling();
  	}
	return true;
}


/**
 * Parsing of a single object definition. 
 * @param element 
 * @return The boolean return value states, whether the parsing of the 
 * object definition has been successfull or not.
 */
SimObject* SimpleObjectFileParser::generateEnvironmentObject(QDomElement element, bool addToEnvironmentObjects) {
	if(element.tagName().compare("object") != 0) {
		Core::log("SimpleObjectFileParser: Wrong tag name.", true);
		return 0;
	}
	QDomAttr a = element.attributeNode("type"); 
	if(mVersion.compare("Environment XML V1.0") == 0) {
		Core::log("SimpleObjectFileParser: XML V1.0 is not supported any more.", true);
		return 0;
	}
	else if(mVersion.compare("Environment XML V1.1") == 0) {
		if(element.tagName().compare("object") != 0) {
			Core::log("SimpleObjectFileParser: Invalid tag name for object definition.", true);
			return 0;
		}
		QDomAttr a = element.attributeNode("type"); 
		SimObject *object = 0;
		SimObject *bodyPrototype = Physics::getPhysicsManager()->getPrototype("Prototypes/"
			 + a.value());
		if(bodyPrototype == 0) {
			Core::log(QString("SimpleObjectFileParser: Could not find prototype of object \"")
				.append(a.value()).append("\""), true);
			return 0;
		}
		object = bodyPrototype->createCopy();
		if(object == 0) {
			Core::log(QString("SimpleObjectFileParser: Error while copying prototpye \"")
				.append(a.value()).append("\""), true);
			return 0;
		}
		element.removeAttribute("type");	
		QDomNamedNodeMap attributes = element.attributes();
		for(uint i = 0; i < attributes.length(); i++) {
			QDomAttr attribute = attributes.item(i).toAttr();	
			if(object->getParameter(attribute.name()) == 0) {
				Core::log(QString("SimpleObjectFileParser: Attribute \" ")
					.append(attribute.name()).append( "\" does not exist!"), true);
				return 0;
			}
			if(!object->getParameter(attribute.name())->setValueFromString(attribute.value())) {
				Core::log(QString("SimpleObjectFileParser: Could not set value \" ")
					.append(attribute.name()).append( "\" of object \"").append(a.value())
					.append("\"!"), true);
				return 0;
			}
		}
		if(addToEnvironmentObjects) {
			mObjectsToAdd.push_back(object);
		}
		return object;
	}
	return 0;
}

ModelInterface* SimpleObjectFileParser::generateModel(QDomElement modelElement) {

	QString modelName;
	if(mVersion.compare("Environment XML V1.0") == 0) {
		modelName = modelElement.tagName();
	}
	else if (mVersion.compare("Environment XML V1.1") == 0
		&& modelElement.tagName().compare("model") == 0) 
	{
		if(modelElement.hasAttribute("prototype")) {
			modelName = modelElement.attributeNode("prototype").value();
			modelElement.removeAttribute("prototype");
		}
		else {
			Core::log("SimpleObjectFileParser: Could not finde attribute "
				"prototype while parsing model definition.", true);
		}
	}
	else {
		Core::log("SimpleObjectFileParser: Wrong environment file version! Could "
			"not parse model-definition.", true);
	}
	PhysicsManager *pm = Physics::getPhysicsManager();
	SimObject *model  = pm->getPrototype("Prototypes/" + modelName);
	if(model != 0 && dynamic_cast<ModelInterface*>(model) != 0) {
		QString name;
		QDomAttr a = modelElement.attributeNode("name"); 
		name = a.value();
		modelElement.removeAttribute("name");
		Vector3DValue position(0.0, 0.0, 0.0);
		if(modelElement.hasAttribute("position")) {
			a = modelElement.attributeNode("position"); 
			modelElement.removeAttribute("position");	
			if(position.setValueFromString(a.value()) == false) {
				Core::log("SimpleObjectFileParser: Position tag could not be"
					" parsed.", true);
				return 0;
			}
		}
		Vector3DValue orientation(0.0, 0.0, 0.0);
		if(modelElement.hasAttribute("orientation")) {
			a = modelElement.attributeNode("orientation"); 
			modelElement.removeAttribute("orientation");	
			if(orientation.setValueFromString(a.value()) == false) {
				Core::log("SimpleObjectFileParser: Orientation tag could not be parsed.", true);
				return 0;
			}
		}

		ModelInterface *agentModel = dynamic_cast<ModelInterface*>(model->createCopy());
		agentModel->setName(name);
		if(!agentModel->getParameter("Position")
					->setValueFromString(position.getValueAsString())) 
		{
			Core::log("SimpleObjectFileParser: Could not set position.", true);
			return 0;
		}
		if(!agentModel->getParameter("Orientation")
			->setValueFromString(orientation.getValueAsString()))
		{
			Core::log("SimpleObjectFileParser: Could not set orientation.", true);
			return 0;
		}
		QDomNode transformation = modelElement.firstChild();
		while(!transformation.isNull()) {
			QDomElement transformationElement = transformation.toElement();
			if(!transformationElement.isNull()) {
				if(transformationElement.tagName().compare("rotation") == 0) {
					QDomAttr value = transformationElement.attributeNode("value");
					Vector3DValue rotation;
					if(!rotation.setValueFromString(value.value())) {
						Core::log("SimpleObjectFileParser: Error while parsing "
							"transformation tag of simualtion model.", true);
						return 0;
					}
					agentModel->addTransformation("rotation", rotation.get());
				}
				else if(transformationElement.tagName().compare("translation") == 0) {
					QDomAttr value = transformationElement.attributeNode("value");
					Vector3DValue translation;
					if(!translation.setValueFromString(value.value())) {
						Core::log("SimpleObjectFileParser: Error while parsing "
							"transformation tag of simualtion model.", true);
						return 0;
					}
					agentModel->addTransformation("translation", translation.get());
				}
				else {
					Core::log("SimpleObjectFileParser: Found a tag, that is no "
						"transformation tag: " + transformationElement.tagName(), true);
					return 0;
				}
			}
			transformation = transformation.nextSibling();
		}
		
		QDomNamedNodeMap attributes = modelElement.attributes();
		for(uint i = 0; i < attributes.length(); i++) {
			QDomAttr attribute = attributes.item(i).toAttr();	
			if(agentModel->getParameter(attribute.name()) == 0) {
				Core::log(QString("SimpleObjectFileParser: Attribute \" ")
					.append(attribute.name()).append( "\" does not exist!"), true);
				return 0;
			}
			if(!agentModel->getParameter(attribute.name())->setValueFromString(attribute.value())) {
				Core::log(QString("SimpleObjectFileParser: Could not set value \" ")
					.append(attribute.name()).append( "\" of object \"").append(a.value())
					.append("\"!"), true);
				return 0;
			}
		}

		return agentModel;
	}
	else {
		Core::log(QString("SimpleObjectFileParser: Could not find prototype of"
			" model \"").append(modelName.toStdString().c_str()).append("\""), true);
		return 0;
	}
}


bool SimpleObjectFileParser::generateEnvironmentObjectChain(QDomElement element) {

	if(element.tagName().compare("objectChain") != 0) {
		Core::log("SimpleObjectFileParser: Wrong tag name for " + element.tagName() 
					+ " instead of objectChain.", true);
		return false;
	}
	QDomAttr a = element.attributeNode("type"); 
	QDomAttr increments = element.attributeNode("PositionIncrement");
	QDomAttr chainSize = element.attributeNode("ChainSize");
	element.removeAttribute("type");
	element.removeAttribute("PositionIncrement");
	element.removeAttribute("ChainSize");

	//get position increments.
	QString incrementEntry = increments.value();
	if(incrementEntry.size() < 7) {
		Core::log("SimpleObjectFileParser: IncrementEntry was invalid.", true);
		return false;
	}
	QStringList incrementStrings = incrementEntry.mid(1, incrementEntry.size() - 2).split(",");
	if(incrementStrings.size() != 3) {
		Core::log("SimpleObjectFileParser: IncrementEntry was invalid.", true);
		return false;
	}
	bool ok = true;
	bool parsingOk = true;
	double incrementX = incrementStrings.at(0).toDouble(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	double incrementY = incrementStrings.at(1).toDouble(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	double incrementZ = incrementStrings.at(2).toDouble(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	if(!ok) {
		Core::log("SimpleObjectFileParser: IncrementEntry was invalid.", true);
		return false;
	}

	//parse numberOfObjects.
	QString chainSizeEntry = chainSize.value();
	if(chainSizeEntry.size() < 7) {
		Core::log("SimpleObjectFileParser: ChainSize was invalid.", true);
		return false;
	}
	QStringList chainSizeStrings = chainSizeEntry.mid(1, chainSizeEntry.size() - 2).split(",");
	if(chainSizeStrings.size() != 3) {
		Core::log("SimpleObjectFileParser: ChainSize was invalid.", true);
		return false;
	}
	int sizeX = chainSizeStrings.at(0).toInt(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	int sizeY = chainSizeStrings.at(1).toInt(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	int sizeZ = chainSizeStrings.at(2).toInt(&parsingOk);
	if(!parsingOk) {
		ok = false;
	}
	if(!ok) {
		Core::log("SimpleObjectFileParser: ChainSize was invalid.", true);
		return false;
	}
	

	SimBody *body;
	SimObject *bodyPrototype = Physics::getPhysicsManager()->getPrototype("Prototypes/"
			+ a.value());
	if(bodyPrototype == 0) {
		Core::log(QString("SimpleObjectFileParser: Could not find prototype of object \"")
			.append(a.value()).append("\""), true);
		return false;
	}

	for(int x = 0; x < sizeX; ++x) {
		for(int y = 0; y < sizeY; ++y) {
			for(int z = 0; z < sizeZ; ++z) {
				body = dynamic_cast<SimBody*>(bodyPrototype->createCopy());
				if(body == 0) {
					Core::log(QString("SimpleObjectFileParser: Error while copying prototpye \"")
						.append(a.value()).append("\""), true);
					return false;
				}
					
				QDomNamedNodeMap attributes = element.attributes();
				for(uint i = 0; i < attributes.length(); i++) {
					QDomAttr attribute = attributes.item(i).toAttr();	
					if(body->getParameter(attribute.name()) == 0) {
						Core::log(QString("SimpleObjectFileParser: Attribute \" ")
							.append(attribute.name()).append( "\" does not exist!"), true);
						return false;
					}
					if(!body->getParameter(attribute.name())->setValueFromString(attribute.value())) {
						Core::log(QString("SimpleObjectFileParser: Could not set value \" ")
							.append(attribute.name()).append( "\" of object \"").append(a.value())
							.append("\"!"), true);
						return false;
					}
				}
		
				//increment position.
				Vector3DValue *pos = dynamic_cast<Vector3DValue*>(body->getParameter("Position"));
				if(pos == 0) {
					Core::log(QString("SimpleObjectFileParser: Could not find Value [Position]!"), true);
					return false;
				}
				pos->set(pos->getX() + (x * incrementX), 
						 pos->getY() + (y * incrementY), 
						 pos->getZ() + (z * incrementZ));
		
				//modify name
				StringValue *nameValue = dynamic_cast<StringValue*>(body->getParameter("Name"));
				if(nameValue == 0) {
					Core::log(QString("SimpleObjectFileParser: Could not find Value [Name]!"), true);
					return false;
				}
				QString name = nameValue->get();
				name = name.replace("$x$", QString::number(x));
				name = name.replace("$y$", QString::number(y));
				name = name.replace("$z$", QString::number(z));
				nameValue->set(name);

				mObjectsToAdd.push_back(body);
			}	
		}
	}
	return true;
}

QString SimpleObjectFileParser::getName()  const{
	return "SimpleObjectFileParser";
}


void SimpleObjectFileParser::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mLoadEnvironmentXMLEvent) {

		//destroy old objects (if there are any)
		while(!mSimObjectGroupTrash.empty()) {
			SimObjectGroup *group = mSimObjectGroupTrash.at(0);
			mSimObjectGroupTrash.removeAll(group);
			delete group;
		}
		while(!mSimObjectTrash.empty()) {
			SimObject *obj = mSimObjectTrash.at(0);
			mSimObjectTrash.removeAll(obj);
			delete obj;
		}

		if(mLastXMLFileName != mXMLFileName->get().trimmed()) {

			mObjectsToAdd.clear();
			mAgentsToAdd.clear();

			mFileName = mXMLFileName->get();
			mLastXMLFileName = mFileName;
			
			//clear physics manager
			PhysicsManager *pm = Physics::getPhysicsManager();
			QMutexLocker(pm->getResetMutex());

			SimulationEnvironmentManager *envManager = Physics::getSimulationEnvironmentManager();
			pm->clearPhysics();
			envManager->clearAllParameters();

			mSimObjectTrash = pm->getSimObjects();
			mSimObjectGroupTrash = pm->getSimObjectGroups();

			for(QListIterator<SimObject*> i(mSimObjectTrash); i.hasNext();) {
				SimObject *obj = i.next();
				pm->removeSimObject(obj);
			}
			for(QListIterator<SimObjectGroup*> i(mSimObjectGroupTrash); i.hasNext();) {
				SimObjectGroup *group = i.next();
				pm->removeSimObjectGroup(group);
			}
			
			//loadNewEnvironment
			loadXmlDescription();

			//relink controllers
			for(QListIterator<SimObjectGroup*> i(mSimObjectGroupTrash); i.hasNext();) {
				SimObjectGroup *group = i.next();
				if(group->getController() != 0) {
					SimObjectGroup *newGroup = pm->getSimObjectGroup(group->getName());
					if(newGroup != 0) {
						//a new group with the same name is available
						newGroup->setController(group->getController());
					}
				}
				group->setController(0);
			}

			//notify all objects that the environment was changed.
			pm->triggerPhysicsEnvironmentChangedEvent();
		}
	}
}


void SimpleObjectFileParser::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mXMLFileName) {
		mLastXMLFileName = "";
	}
}


/**
 * Parses a "randomiseList" definition.
 * @param mainElement 
 */
void SimpleObjectFileParser::parseRandomiseListDefinition(QDomElement mainElement) {
	PhysicsManager *pManager = Physics::getPhysicsManager();
	if(!mainElement.hasAttribute("parameter")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"parameter\" in "
			"randomise list definition.", true);
		return;
	}
	QString parameterName = mainElement.attributeNode("parameter").value();
	Value *parameter;
	if(mainElement.hasAttribute("object")) {
		QString objectName = mainElement.attributeNode("object").value();
		SimObject *object = pManager->getSimObject(objectName);
		if(object == 0) {
			Core::log("SimpleObjectFileParser: Error while parsing randomizeList. Object "
				"could not be found.", true);
			return;
		}
		parameter = object->getParameter(parameterName);
	}
	else {
		parameter = Core::getInstance()->getValueManager()->getValue(parameterName);
	}
	if(parameter == 0) {
		Core::log("SimpleObjectFileParser: Error while parsing randomizeList. Parameter "
			"could not be found.", true);
		return;
	}
	parseList(parameter, mainElement);

}

void SimpleObjectFileParser::parseList(Value* value, QDomElement mainElement) {

	if(!mainElement.hasAttribute("method")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"method\" in randomize "
			"list definition.", true);
		return;
	}
	QString method = mainElement.attributeNode("method").value();
	ListRandomizer *listRandomiser = new ListRandomizer(value);

	bool sequential = false;
	bool additive = false;
	bool incremental = false;
	if(method.compare("sequential") == 0) {
		sequential = true;
	}
	else if(method.compare("additive-incremental") == 0) {
		additive = true;
		incremental = true;
	}
	else if(method.compare("additive-decremental") == 0) {	
		additive = true;
		incremental = false;
	}
	else if(method.compare("random") == 0) {
	}
	listRandomiser->setSequential(sequential);
	listRandomiser->setAdditive(additive);
	listRandomiser->setIncremental(incremental);

	QDomNode itemNode = mainElement.firstChild();
	
	while(!itemNode.isNull()) {
		QDomElement itemElement = itemNode.toElement();
			if(!itemElement.isNull() && itemElement.tagName().compare("item") == 0 
				&& itemElement.hasAttribute("value")) 
			{
				QString valueString = itemElement.attributeNode("value").value();
				Value *copy = value->createCopy();
				if(!copy->setValueFromString(valueString)) {
					Core::log("SimpleObjectFileParser: Error while parsing listRandomization."
						" Found an invalid item.", true);
					delete copy;
					return;
				}
				delete copy;
				listRandomiser->addListItem(valueString);
			}
		itemNode = itemNode.nextSibling();
	}
	Physics::getSimulationEnvironmentManager()->addRandomizer(listRandomiser);
}


void SimpleObjectFileParser::parseInfoNeuronDeclaration(QDomElement mainNode) {
	if(!mainNode.hasAttribute("object")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"object\" in switchInput "
			"definition.", true);
		return;
	}
	if(!mainNode.hasAttribute("input")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"input\" in switchInput "
			"definition.", true);
		return;
	}
	if(!mainNode.hasAttribute("agent")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"agent\" in switchInput "
			"definition.", true);
		return;
	}
	QString target = mainNode.attributeNode("object").value();
	QString targetValue = mainNode.attributeNode("input").value();
	QString agentName = mainNode.attributeNode("agent").value();
	bool disable = true;
	if(mainNode.hasAttribute("mode")) {
		QString mode = mainNode.attributeNode("mode").value();
		if(mode.toLower() == "input") {
			disable = false;
		}
	}

	for(int i = 0; i < mNewAgents.size(); ++i) {
		ModelInterface *agent = mNewAgents.at(i);
		if(agent->getName() == agentName) {
			agent->switchInput(target, targetValue, disable);
			break;
		}
	}
	
}

void SimpleObjectFileParser::parseValueDefinition(QDomElement valueNode) {
	if(!valueNode.hasAttribute("name")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"name\" in value "
			"definition.", true);
		return;
	}
	if(!valueNode.hasAttribute("content")) {
		Core::log("SimpleObjectFileParser: Missing attribute \"content\" in value "
			"definition.", true);
		return;
	}

	QString name = valueNode.attributeNode("name").value();
	QString content = valueNode.attributeNode("content").value();
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	Value *value = vm->getValue(name);
	if(value == 0) {
		Core::log("SimpleObjectFileParser: Could not find value [" + name + "] in ValueManager", true);
	}
	else {
		if(!value->setValueFromString(content)) {
			Core::log("SimpleObjectFileParser: Error while setting value [" + name 
							+ "] to content [" + content + "]", true);
		}
	}
}


/**
 * Parses a "randomise" definition.
 * @param mainElement 
 */
void SimpleObjectFileParser::parseRandomiseDefinition(QDomElement mainElement) {
	PhysicsManager *pManager = Physics::getPhysicsManager();
	if(!mainElement.hasAttribute("parameter")) {
		Core::log("SimpleObjectFileParser: Attribute \"parameter\" is missing in randomize " 
				"definition.", true);
		return;
	}
	QString parameterName = mainElement.attributeNode("parameter").value();
	Value *parameter = 0;
	if(mainElement.hasAttribute("object")) {
		QString objectName = mainElement.attributeNode("object").value();
		
		SimObject *object = pManager->getSimObject(objectName);
		if(object == 0) {
			Core::log(QString("SimpleObjectFileParser: Error while parsing randomizeList. "
				"Object [").append(objectName).append(" ] could not be found."), true);
			return;
		}
		parameter = object->getParameter(parameterName);
	}
	else {
		parameter = Core::getInstance()->getValueManager()->getValue(parameterName);
	}
	if(parameter == 0) {
		Core::log(QString("SimpleObjectFileParser: Error while parsing randomize. "
			"Parameter [").append(parameterName).append("] could not be found."), true);
		return;
	}
	QString method = "";
	if(mainElement.hasAttribute("method")) {
		method = mainElement.attributeNode("method").value();
	}
	
	QDomNodeList minNodes = mainElement.elementsByTagName("min");
	QDomNodeList maxNodes = mainElement.elementsByTagName("max");
	QDomNodeList probNodes = mainElement.elementsByTagName("probability");
	QDomNodeList devNodes = mainElement.elementsByTagName("deviation");

	if(minNodes.length() !=  1 || maxNodes.length() != 1 || probNodes.length() != 1 
		|| devNodes.length() > 2) 
	{
		Core::log("SimpleObjectFileParser: Wrong arguments!", true);
		return;
	}
	QString minValue ="";
	QString maxValue ="";
	QString probValue ="";
	QString devValue ="";
	if(minNodes.item(0).toElement().hasAttribute("value")) {		
		minValue = minNodes.item(0).toElement().attributeNode("value").value();
	}
	if(maxNodes.item(0).toElement().hasAttribute("value")) {		
		maxValue = maxNodes.item(0).toElement().attributeNode("value").value();
	}
	if(probNodes.item(0).toElement().hasAttribute("value")) {		
		probValue = probNodes.item(0).toElement().attributeNode("value").value();
	}
	if(devNodes.length() == 1 && method.compare("additive") == 0) {
		if(devNodes.item(0).toElement().hasAttribute("value")) {		
			devValue = devNodes.item(0).toElement().attributeNode("value").value();
		}
	}

	if(dynamic_cast<Vector3DValue*>(parameter) != 0) {

		DoubleValue *xParameter = dynamic_cast<DoubleValue*>(
				dynamic_cast<Vector3DValue*>(parameter)->getValuePart(0));
		DoubleValue *yParameter = dynamic_cast<DoubleValue*>(
				dynamic_cast<Vector3DValue*>(parameter)->getValuePart(1));
		DoubleValue *zParameter = dynamic_cast<DoubleValue*>(
				dynamic_cast<Vector3DValue*>(parameter)->getValuePart(2));
		if(xParameter == 0 || yParameter == 0 || zParameter == 0) {
			Core::log("SimpleObjectFileParser: Error while parsing randomize of Vector3D "
				"value", true);
			return;
		}
		DoubleRandomizer *xRandomiser = new DoubleRandomizer(xParameter);
		DoubleRandomizer *yRandomiser = new DoubleRandomizer(yParameter);
		DoubleRandomizer *zRandomiser = new DoubleRandomizer(zParameter);

		Vector3DValue tmp;
		tmp.setValueFromString(minValue);
		xRandomiser->setMin(tmp.getX());
		yRandomiser->setMin(tmp.getY());
		zRandomiser->setMin(tmp.getZ());

		tmp.setValueFromString(maxValue);
		xRandomiser->setMax(tmp.getX());
		yRandomiser->setMax(tmp.getY());
		zRandomiser->setMax(tmp.getZ());

		tmp.setValueFromString(probValue);
		xRandomiser->setProbability(tmp.getX());
		yRandomiser->setProbability(tmp.getY());
		zRandomiser->setProbability(tmp.getZ());

		if(method.compare("additive") == 0 && devNodes.length() == 1) {
			xRandomiser->setAdditive(true);
			yRandomiser->setAdditive(true);
			zRandomiser->setAdditive(true);
				
			tmp.setValueFromString(devValue);
			xRandomiser->setDeviation(tmp.getX());
			yRandomiser->setDeviation(tmp.getY());
			zRandomiser->setDeviation(tmp.getZ());
		}
		else if(method.compare("global") == 0) {
			xRandomiser->setAdditive(false);
			yRandomiser->setAdditive(false);
			zRandomiser->setAdditive(false);
		}
		else {
			Core::log(QString("SimpleObjectFileParser: Found an invalid method definition "
				"[ ").append(method).append(" ] while parsing randomize-node."), true);
			return;
		}
		Physics::getSimulationEnvironmentManager()->addRandomizer(xRandomiser);
		Physics::getSimulationEnvironmentManager()->addRandomizer(yRandomiser);
		Physics::getSimulationEnvironmentManager()->addRandomizer(zRandomiser);
	}
	else if(dynamic_cast<DoubleValue*>(parameter) != 0) {
		DoubleRandomizer *randomizer = new 
			DoubleRandomizer(dynamic_cast<DoubleValue*>(parameter));

		randomizer->setMin(minValue.toDouble());
		randomizer->setMax(maxValue.toDouble());
		randomizer->setProbability(probValue.toDouble());

		if(method.compare("additive") == 0 && devNodes.length() == 1) {
			randomizer->setAdditive(true);
			randomizer->setDeviation(devValue.toDouble());
		}
		else if(method.compare("global") == 0) {
			randomizer->setAdditive(false);
		}
		else {
			Core::log(QString("SimpleObjectFileParser: Found an invalid method definition"
				" [ ").append(method).append(" ] while parsing randomize-node."), true);
			return;
		}
		Physics::getSimulationEnvironmentManager()->addRandomizer(randomizer);
	}
	else {
		Core::log("SimpleObjectFileParser: Error while parsing randomizeList.", true);
		return;
	}
}


void SimpleObjectFileParser::parseCollisionRule(QDomElement collisionRuleNode) {

	CollisionManager *cm = Physics::getCollisionManager();
	if(!collisionRuleNode.hasAttribute("name")) {
		Core::log("SimpleObjectFileParser: Could not find attribute: \"name\" in collision "
			"rule definition.", true);
		return;
	}
	QDomAttr a = collisionRuleNode.attributeNode("name");
	
	if(!collisionRuleNode.hasAttribute("prototype")) {
		Core::log("SimpleObjectFileParser: Could not find attribute: \"prototype\" in collision "
			"rule definition.", true);
		return;
	}
	QString ruleName = a.value();
	a = collisionRuleNode.attributeNode("prototype");
	QString prototypeName = a.value();
	CollisionRule *prototype = cm->getCollisionRulePrototype(prototypeName);
	if(prototype == 0) {	
		Core::log(QString("SimpleObjectFileParser: Could not find collision rule "
			"prototype \"").append(prototypeName).append("\"."), true);
		return;
	}
	CollisionRule *rule = prototype->createCopy();
	rule->setName(ruleName);
	
	QDomNode n = collisionRuleNode.firstChild();
	
	QString sourceList;
	QString targetList;

	while(!n.isNull()) {
		QDomElement e = n.toElement();
		if(!e.isNull()) {
			if(e.tagName().compare("source") == 0) {
				if(e.hasAttribute("object")) {
					a = e.attributeNode("object");
					QString sourceName = a.value();

					if(sourceName.startsWith("/")) {
						sourceName.mid(1);
					}

					if(sourceList != "") {
						sourceList += ",";
					}
					sourceList += sourceName;

// 					sourceName.prepend("/");
// 					
// 
// 					QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
// 									->getSimObjects(sourceName);
// 					for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
// 						SimBody *sourceObject = dynamic_cast<SimBody*>(j.next());
// 						if(sourceObject != 0) {
// 							for(int i = 0; i < sourceObject->getCollisionObjects().size(); i++) {
// 								rule->addToSourceGroup(sourceObject->getCollisionObjects().at(i));
// 							}
// 						}
// 					}
				}
			}
			else if(e.tagName().compare("target") == 0) {
				if(e.hasAttribute("object")) {
					a = e.attributeNode("object");
					QString targetName = a.value();
	
					if(targetName.startsWith("/")) {
						targetName = targetName.mid(1);
					}

					if(targetList != "") {
						targetList += ",";
					}
					targetList += targetName;

// 					targetName.prepend("/");
// 
// 					
// 
// 					QList<SimObject*> matchingObjects = Physics::getPhysicsManager()
// 									->getSimObjects(targetName);
// 					for(QListIterator<SimObject*> j(matchingObjects); j.hasNext();) {
// 						SimBody *targetObject = dynamic_cast<SimBody*>(j.next());
// 						if(targetObject != 0) {
// 							for(int i = 0; i < targetObject->getCollisionObjects().size(); i++) {
// 								rule->addToTargetGroup(targetObject->getCollisionObjects().at(i));
// 							}
// 						}
// 					}
				}
			}
			else if(e.tagName().compare("param") == 0) {
				QString paramName;
				QString valueAsString;
				if(e.hasAttribute("name")) {
					paramName = e.attributeNode("name").value();
				}
				else {
					Core::log("SimpleObjectFileParser: Error while parsing collision rule."
						" Could not find attribute \"name\" for tag \"param\".", true);
				}
				if(e.hasAttribute("value")) {
					valueAsString = e.attributeNode("value").value();
				}
				else {
					Core::log("SimpleObjectFileParser: Error while parsing collision rule."
						" Could not find attribute \"value\" for tag \"param\".", true);
				}
				if(rule->getParameter(paramName) != 0) {	
					if(!rule->getParameter(paramName)->setValueFromString(valueAsString)) {
						Core::log(QString("SimpleObjectFileParser: Error while parsing "
							"collision rule. Parsing param-tag \"")
							.append(paramName).append("\" was not successful."), true);
					}
				}
				else {
					Core::log(QString("SimpleObjectFileParser: Error while parsing collision rule."
						" Could not find parameter \"").append(paramName).append("\"."), true);
				}
			}
		}
		n = n.nextSibling();
	}
	rule->init();
	rule->getSourceList()->set(sourceList);
	rule->getTargetList()->set(targetList);

	cm->addCollisionRule(rule);
}
}
