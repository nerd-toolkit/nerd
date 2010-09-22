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



#include "SetLocationPropertiesCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new SetLocationPropertiesCommand.
 */
SetLocationPropertiesCommand::SetLocationPropertiesCommand(NetworkVisualization *context, 
						const QList<NeuralNetworkElement*> &elems)
	: Command("Change TransferFunction"), mVisualizationContext(context), mChangedElements(elems)
{
	
}



/**
 * Destructor.
 */
SetLocationPropertiesCommand::~SetLocationPropertiesCommand() {
}


bool SetLocationPropertiesCommand::isUndoable() const {
	return true;
}


bool SetLocationPropertiesCommand::doCommand() {
	
	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	mOldPropertyEntires.clear();
	mOldSizeEntries.clear();

	for(QListIterator<NeuralNetworkElement*> i(mChangedElements); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();

		if(dynamic_cast<Neuron*>(elem) != 0) {
			Neuron *neuron = dynamic_cast<Neuron*>(elem);
			NeuronItem *item = handler->getNeuronItem(neuron);

			if(item == 0) {
				mOldPropertyEntires.append("");
				continue;
			}
			mOldPropertyEntires.append(neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			mOldSizeEntries.append("");
			
			neuron->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
		}
		else if(dynamic_cast<Synapse*>(elem) != 0) {
			Synapse *synapse = dynamic_cast<Synapse*>(elem);
			SynapseItem *item = handler->getSynapseItem(synapse);

			if(item == 0) {
				mOldPropertyEntires.append("");
				continue;
			}
			mOldPropertyEntires.append(synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));	
			mOldSizeEntries.append("");

			synapse->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
		}
		else if(dynamic_cast<NeuroModule*>(elem) != 0) {
			NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
			ModuleItem *item = handler->getModuleItem(module);

			if(item == 0) {
				mOldPropertyEntires.append("");
				continue;
			}
			mOldPropertyEntires.append(module->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			mOldSizeEntries.append(module->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE));

			module->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
			module->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE,
					QString::number(item->getSize().width()) + ","
					  + QString::number(item->getSize().height()));
		}
		else if(dynamic_cast<NeuronGroup*>(elem) != 0) {
			NeuronGroup *group = dynamic_cast<NeuronGroup*>(elem);
			GroupItem *item = handler->getGroupItem(group);

			if(item == 0) {
				mOldPropertyEntires.append("");
				continue;
			}
			mOldPropertyEntires.append(group->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			mOldSizeEntries.append("");

			group->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
		}
	}

	return true;
}


bool SetLocationPropertiesCommand::undoCommand() {

	if(mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHander());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	for(int i = 0; i < mChangedElements.size() && mOldPropertyEntires.size(); ++i) {
		NeuralNetworkElement *elem = mChangedElements.at(i);
		QString oldProp = mOldPropertyEntires.at(i);
		QString oldSizeProp = mOldSizeEntries.at(i);

		if(dynamic_cast<Neuron*>(elem) != 0) {
			Neuron *neuron = dynamic_cast<Neuron*>(elem);
			NeuronItem *item = handler->getNeuronItem(neuron);

			if(item == 0) {
				continue;
			}

			if(oldProp == "") {
				neuron->removeProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION);
			}	
			else {
				neuron->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, oldProp);
			}
		}
		else if(dynamic_cast<Synapse*>(elem) != 0) {
			Synapse *synapse = dynamic_cast<Synapse*>(elem);
			SynapseItem *item = handler->getSynapseItem(synapse);

			if(item == 0) {
				continue;
			}

			if(oldProp == "") {
				synapse->removeProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION);
			}	
			else {
				synapse->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, oldProp);
			}
		}
		else if(dynamic_cast<NeuroModule*>(elem) != 0) {
			NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
			ModuleItem *item = handler->getModuleItem(module);

			if(item == 0) {
				continue;
			}

			if(oldProp == "") {
				module->removeProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION);
			}	
			else {
				module->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, oldProp);
			}
			if(oldSizeProp == "") {
				module->removeProperty(NeuralNetworkConstants::TAG_MODULE_SIZE);
			}
			else {
				module->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE, oldSizeProp);
			}
		}
		else if(dynamic_cast<NeuronGroup*>(elem) != 0) {
			NeuronGroup *group = dynamic_cast<NeuronGroup*>(elem);
			GroupItem *item = handler->getGroupItem(group);

			if(item == 0) {
				continue;
			}

			if(oldProp == "") {
				group->removeProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION);
			}	
			else {
				group->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, oldProp);
			}
		}
	}
	return true;
}



}



