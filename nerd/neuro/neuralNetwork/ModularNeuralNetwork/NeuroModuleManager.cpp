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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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



#include "NeuroModuleManager.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NeuralNetworkConstants.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include <QDir>
#include <QFile>
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "IO/NeuralNetworkIO.h"
#include "Network/NeuroTagManager.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {



NeuroModulePrototype::NeuroModulePrototype(const QString &name, ModularNeuralNetwork *network)
	: mNetwork(network), mUseDuringEvolution(false), mName(name)
{
}

/**
 * Constructs a new NeuroModuleManager.
 */
NeuroModuleManager::NeuroModuleManager()
{
	Core *core = Core::getInstance();

	QString globalModuleDirectory = QDir::homePath() + "/.nerd/neuroModules";
	if(!core->isUsingReducedFileWriting()) {
		core->enforceDirectoryPath(globalModuleDirectory);	
	}

	QString path = core->getConfigDirectoryPath() + "/neuroModules";
	if(!core->isUsingReducedFileWriting()) {
		core->enforceDirectoryPath(path);	
	}

	mOptionalModuleDirectory = new StringValue(globalModuleDirectory + "," + path);
	mOptionalModuleDirectory->useAsFileName(true);

	core->getValueManager()->addValue(
			"/NeuroModules/OptionalModuleDirectory", mOptionalModuleDirectory);

	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_INPUT, 
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Indicates that a neuron is part of the interface of a module (input)."
						 "\nAn optional number deterines the visibility depth."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_OUTPUT, 
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Indicates that a neuron is part of the interface of a module (output)."
						 "\nAn optional number deterines the visibility depth."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Indicates that a neuron is an input / output also for its parent modules."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::NeuralNetworkConstants::TAG_MODULE_TYPE,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "The type(s) this module is compatible with."));
}



/**
 * Destructor.
 */
NeuroModuleManager::~NeuroModuleManager() {
	clearPrototypes();
}

NeuroModuleManager* NeuroModuleManager::getInstance() {
	NeuroModuleManager *nm = dynamic_cast<NeuroModuleManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER));
	if(nm == 0) {
		nm = new NeuroModuleManager();
		if(!nm->registerAsGlobalObject()) {
			delete nm;
		}
		nm = dynamic_cast<NeuroModuleManager*>(Core::getInstance()
			->getGlobalObject(NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER));
	}
	return nm;
}

QString NeuroModuleManager::getName() const {
	return "NeuroModuleManager";
}


bool NeuroModuleManager::registerAsGlobalObject() {
	TRACE("NeuroModuleManager::registerAsGlobalObject");

	Core::getInstance()->addGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER) == this;
}


bool NeuroModuleManager::init() {
	TRACE("NeuroModuleManager::init");

	bool ok = true;
	
	if(Core::getInstance()->getGlobalObject(
			NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER) != this) 
	{
		Core::log("NeuroModuleManager: Was not registered as global object at Core! "
				"[TERMINATING]");
		ok = false;
	}
	if(ok) {
		clearPrototypes();
		QStringList directories = mOptionalModuleDirectory->get().split(",");
		for(QListIterator<QString> i(directories); i.hasNext();) {
			loadNeuroModulePrototypes(i.next());
		}
	}

	return ok;
}


bool NeuroModuleManager::bind() {
	return true;
}


bool NeuroModuleManager::cleanUp() {
	return true;
}


bool NeuroModuleManager::addNeuroModulePrototype(const QString &name, ModularNeuralNetwork *prototype) {
	if(prototype == 0) {
		return false;
	}
	NeuroModulePrototype *p = new NeuroModulePrototype(name, prototype);
	if(!addNeuroModulePrototype(p)) {
		delete p;
		return false;
	}
	return true;
}


bool NeuroModuleManager::addNeuroModulePrototype(NeuroModulePrototype *prototype) {
	if(prototype == 0 || mNeuroModulePrototypes.contains(prototype)) 
	{
		return false;
	}
	mNeuroModulePrototypes.append(prototype);
	return true;
}


bool NeuroModuleManager::removeNeuroModulePrototype(NeuroModulePrototype *prototype) {
	if(prototype == 0 || !mNeuroModulePrototypes.contains(prototype)) {
		return false;
	}
	mNeuroModulePrototypes.removeAll(prototype);
	return true;
}


QList<NeuroModulePrototype*> NeuroModuleManager::getNeuroModulePrototpyes() const {
	return mNeuroModulePrototypes;
}

NeuroModulePrototype* NeuroModuleManager::getNeuroModuleByName(const QString &name) const {
	for(QListIterator<NeuroModulePrototype*> i(mNeuroModulePrototypes); i.hasNext();) {
		NeuroModulePrototype *prototype = i.next();
		if(prototype->mName == name) {
			return prototype;
		}
	}
	return 0;
}


bool NeuroModuleManager::loadNeuroModulePrototypes(const QString &directoryName) {

	QDir dir(directoryName);
	if(!dir.exists()) {
		return false;
	}
	QStringList filters;
	filters << "*.onn";

	QStringList files = dir.entryList(filters);
	for(QListIterator<QString> i(files); i.hasNext();) {
		QString fileName = i.next();
		ModularNeuralNetwork *net = dynamic_cast<ModularNeuralNetwork*>(
					NeuralNetworkIO::createNetworkFromFile(dir.absolutePath() + "/" + fileName));
		if(net != 0) {
			NeuroModulePrototype *prototype = new NeuroModulePrototype(
							fileName.mid(0, fileName.lastIndexOf(".")), net);
			
			QString types = net->getProperty("TYPE_LIST");
			QStringList typeList = types.split(",");
			for(QListIterator<QString> i(typeList); i.hasNext();) {
				prototype->mTypes.append(i.next().trimmed().toLower());
			}
			addNeuroModulePrototype(prototype);
		}
	}
	return true;
}


bool NeuroModuleManager::saveNeuroModulePrototypes(const QString &directoryName) {
	Core::getInstance()->enforceDirectoryPath(directoryName);
	QDir dir(directoryName);
	if(!dir.exists()) {
		return false;
	}

	bool ok = true;
	
	for(QListIterator<NeuroModulePrototype*> i(mNeuroModulePrototypes); i.hasNext();) {
		NeuroModulePrototype *prototype = i.next();
		ModularNeuralNetwork *net = prototype->mNetwork;
		if(net != 0) {
			if(!NeuralNetworkIO::createFileFromNetwork(dir.absolutePath() + "/" 
						+ prototype->mName + ".onn", net)) 
			{
				ok = false;
			}
		}
	}

	return false;
}


void NeuroModuleManager::clearPrototypes() {
	while(!mNeuroModulePrototypes.empty()) {
		NeuroModulePrototype *p = mNeuroModulePrototypes.at(0);
		removeNeuroModulePrototype(p);
		ModularNeuralNetwork *net = p->mNetwork;
		if(net != 0) {
			delete net;
		}
		delete p;
	}
}	


StringValue* NeuroModuleManager::getOptionalModuleDirectoryValue() const {
	return mOptionalModuleDirectory;
}


}



