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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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


#ifndef SimpleObjectFileParser_H_
#define SimpleObjectFileParser_H_

#include "Core/SystemObject.h"
#include <QFile>
#include <qxml.h>
#include <QDir>
#include <QDomDocument>
#include "Physics/SimBody.h"
#include "Models/ModelInterface.h"
#include "PlugIns/CommandLineArgument.h"
#include "Value/StringValue.h"
#include "Event/Event.h"
#include "Event/EventListener.h"
#include "Value/ValueChangedListener.h"
#include "Value/FileNameValue.h"


/**
* SimpleObjectFileParser. This plugin reacts on a specific commandline parameter to load simple environment objects from a user choosen file. 
* The SimpleObjectFileParser reacts to the commandline argument -env/-loadEnvironment. This commandline argument has one parameter which specifies the file to be loaded as environment file. The parsing of the xml-environment file takes place during the init-phase of the simulation setup and creates all simulation objects and rules which are defines via the given xml. There are currently to Versions 1.0/1.1 which are supported by the parser. V1.1 allows the definition of SimBody-objects and models (both based on prototypes that are known in the simulation), definition of Ccollision rules and randomization rules.
* Due to the fact, that the randomization rules are parsed after the environment, model and collision rule definitions were parsed, also values and parameters of models and environment objects defined in the xml can be randomized.
**/

namespace nerd {

class SimpleObjectFileParser : public virtual SystemObject, public virtual EventListener,
								public virtual ValueChangedListener 
{

	public:
		SimpleObjectFileParser();
		~SimpleObjectFileParser();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;

		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

	private:
		bool loadXmlDescription();
		SimObject* generateEnvironmentObject(QDomElement element, bool addToEnvironmentObjects = true);
		ModelInterface* generateModel(QDomElement modelElement);
		bool generateEnvironmentObjectChain(QDomElement element);
		bool generateEnvironmentObjects(QDomElement element);

		bool createAgents(QDomElement element);
		bool createPrototypes(QDomElement element);
		void parseEventCollisionRule(QDomElement ruleNode);
		void parseCollisionRule(QDomElement collisionRuleNode);

		void parseRandomiseListDefinition(QDomElement mainNode);
		void parseRandomiseDefinition(QDomElement mainNode);
		void parseList(Value *value, QDomElement mainNode);
		void parseInfoNeuronDeclaration(QDomElement mainNode);
		void parseValueDefinition(QDomElement valueNode);
	
	private:
		QString mFileName;
		QString mVersion;
		CommandLineArgument *mEnvironmentCommandLineArgument;
		QVector<SimObject*> mObjectsToAdd;
		QVector<ModelInterface*> mAgentsToAdd;
		QList<ModelInterface*> mNewAgents;
		FileNameValue *mXMLFileName;
		QString mLastXMLFileName;
		Event *mLoadEnvironmentXMLEvent;
		QList<SimObject*> mSimObjectTrash;
		QList<SimObjectGroup*> mSimObjectGroupTrash;
};
}
#endif

