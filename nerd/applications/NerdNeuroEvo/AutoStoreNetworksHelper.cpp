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

#include "AutoStoreNetworksHelper.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include "Evolution/Evolution.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Evolution/Individual.h"
#include "IO/NeuralNetworkIO.h"
#include <iostream>
#include "IO/NeuralNetworkIONerdV1Xml.h"
#include "IO/NeuralNetworkIOSimbaV3Xml.h"
#include "Evolution/Evolution.h"
#include "EvolutionConstants.h"
#include <QDir>

using namespace std;

namespace nerd {

AutoStoreNetworksHelper::AutoStoreNetworksHelper()
	: mEvaluationStartedEvent(0)
{
	Core::getInstance()->addSystemObject(this);

	mStoreNetworksValue = new BoolValue(false);
	mStoreNetworksValue->setNotifyAllSetAttempts(true);
	mStoreNetworksValue->addValueChangedListener(this);

	mAutoStoreNetworks = new BoolValue(true);
	mStoreSimbaFiles = new BoolValue(false);
	
	Core::getInstance()->getValueManager()->addValue("NetworkStorage/StoreNetworks", mStoreNetworksValue);
	Core::getInstance()->getValueManager()->addValue("NetworkStorage/AutoStoreNetworks", mAutoStoreNetworks);
	Core::getInstance()->getValueManager()->addValue("NetworkStorage/StoreSimbaFormat", mStoreSimbaFiles);
}

AutoStoreNetworksHelper::~AutoStoreNetworksHelper() {
}

bool AutoStoreNetworksHelper::init() {
	bool ok = true;

	return ok;
}


bool AutoStoreNetworksHelper::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mEvaluationStartedEvent = em->registerForEvent(EvolutionConstants::EVENT_EVO_EVALUATION_STARTED, this);

	if(mEvaluationStartedEvent == 0) {
		Core::log(QString("AutoStoreNetworksHelper: Could not find Event [")
						+ EvolutionConstants::EVENT_EVO_EVALUATION_STARTED + "]");
		ok = false;
	}

	return ok;
}


bool AutoStoreNetworksHelper::cleanUp() {
	return true;
}



QString AutoStoreNetworksHelper::getName() const {
	return "AutoStoreNetoworksHelper";
}

void AutoStoreNetworksHelper::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mEvaluationStartedEvent) {
		if(mAutoStoreNetworks->get()) {
			storeNetworks();
		}
	}
}


void AutoStoreNetworksHelper::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mStoreNetworksValue) {
		storeNetworks();
	}
}

void AutoStoreNetworksHelper::storeNetworks() {
	World *world = Evolution::getEvolutionManager()->getEvolutionWorlds().value(0);

	if(world != 0) {
		Population *pop = world->getPopulations().value(0);
		if(pop != 0) {
			QList<Individual*> individuals = pop->getIndividuals();

			for(int i = 0; i < individuals.size(); ++i) {


				NeuralNetwork *net = dynamic_cast<NeuralNetwork*>(individuals.at(i)->getGenome());

				int id = individuals.at(i)->getId();

				int genNo = Evolution::getEvolutionManager()->getCurrentGenerationValue()->get();

				

				QString netDir = Evolution::getEvolutionManager()->getEvolutionWorkingDirectory() 
							+ "gen" + QString::number(genNo) + "/nets";
				Core::getInstance()->enforceDirectoryPath(netDir);

				QString fileName = netDir + "/net" + QString::number(i) + "_" + QString::number(id);
				QString errorMsg;
				QList<QString> warnings;

				QString onnFileName = fileName + ".onn";

				//cerr << "Saving to " << onnFileName.toStdString().c_str() << endl;
				QString xml = NeuralNetworkIONerdV1Xml::createXmlFromNetwork(net);

				QFile file(onnFileName);
				if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
					Core::log(QString("Could not open file ")
								.append(onnFileName).append(" to write network."));
					file.close();
					continue;
				}
				QTextStream output(&file);
				output << xml;

				if(mStoreSimbaFiles->get()) {
					QString smbFileName = fileName + ".smb";

					NeuralNetworkIOSimbaV3Xml::createFileFromNetwork(smbFileName, net, 0);
				}

				//delete previous generation directory
				QString oldNetDir = Evolution::getEvolutionManager()->getEvolutionWorkingDirectory() 
							+ "gen" + QString::number(genNo - 1) + "/nets";

				QDir oldDir(oldNetDir);
				if(oldDir.exists()) {
					QStringList files = oldDir.entryList();
					for(QListIterator<QString> i(files); i.hasNext();) {
						oldDir.remove(i.next());
					}
					oldDir.rmdir(oldNetDir);
				}
				file.close();
			}
		}
	}

}


}


