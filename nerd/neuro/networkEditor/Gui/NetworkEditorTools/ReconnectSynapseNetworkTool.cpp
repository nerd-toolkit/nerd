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



#include "ReconnectSynapseNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/ReconnectSynapseCommand.h"
#include "Math/Math.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new ReconnectSynapseNetworkTool.
 */
ReconnectSynapseNetworkTool::ReconnectSynapseNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner), mSynapse(0), mPreviousNeuron(0), mPreviousSynapse(0)
{
}



/**
 * Destructor.
 */
ReconnectSynapseNetworkTool::~ReconnectSynapseNetworkTool() {
}

void ReconnectSynapseNetworkTool::clear() {
	TRACE("ReconnectSynapseNetworkTool::clear");

	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}

	mSynapse = 0;
	mPreviousNeuron = 0;
	mPreviousSynapse = 0;
	NetworkManipulationTool::clear();
}


void ReconnectSynapseNetworkTool::activate(NetworkVisualization *visu) {
	TRACE("ReconnectSynapseNetworkTool::activate");

	NetworkManipulationTool::activate(visu);

	mSynapse = 0;
	mPreviousNeuron = 0;
	mPreviousSynapse = 0;
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
}

QString ReconnectSynapseNetworkTool::getStatusMessage() {
	TRACE("ReconnectSynapseNetworkTool::getStatusMessage");

	return "Reconnect Synapse [Choose Synapse]";
}

void ReconnectSynapseNetworkTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	TRACE("ReconnectSynapseNetworkTool::mouseButtonPressed");

	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::LeftButton)) {
		emit done();
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		emit done();
		return;
	}

	ModularNeuralNetwork *network = mVisuContext->getNeuralNetwork();
	if(network == 0) {
		emit done();
		return;
	}

	if(mSynapse == 0) {
		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			SynapseItem *item = dynamic_cast<SynapseItem*>(items.at(i));
			if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				mSynapse = item->getSynapse();
				if(mSynapse != 0) {
					updateStatusMessage("Reconnect Synapse [Choose previous source / target]");
					return;
				}
			}
		}
		emit done();
		return;
	}
	else if(mPreviousSynapse == 0 && mPreviousNeuron == 0) {
		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			PaintItem *item = items.at(i);
			if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				
				NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
				if(neuronItem != 0) {
					Neuron *neuron = neuronItem->getNeuron();
					if(mSynapse->getTarget() == neuron || mSynapse->getSource() == neuron) {
						mPreviousNeuron = neuron;
						updateStatusMessage("Reconnect Synapse [Choose new source / target]");
						return;
					}
					continue;
				}
				SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
				if(synapseItem != 0) {
					Synapse *synapse = synapseItem->getSynapse();
					if(mSynapse->getTarget() == synapse) {
						mPreviousSynapse = synapse;
						updateStatusMessage("Reconnect Synapse [Choose new source / target]");
						return;
					}
					continue;
				}
			}
		}
		emit done();
		return;
	}
	else {
		Neuron *currentNeuron = 0;
		Synapse *currentSynapse = 0;

		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			PaintItem *item = items.at(i);
			if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				
				NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
				if(neuronItem != 0) {
					currentNeuron = neuronItem->getNeuron();;
					break;
				}
				SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
				if(synapseItem != 0) {
					currentSynapse = synapseItem->getSynapse();
					break;
				}
			}
		}

		if(currentNeuron == 0 && currentSynapse == 0) {
			emit done();
			return;
		}

		//check if the choice is valid
		NeuralNetworkElement *sourceElement = mPreviousNeuron;
		if(sourceElement == 0) {
			sourceElement = mPreviousSynapse;
		}
		SynapseTarget *target = currentNeuron;
		NeuralNetworkElement *targetElement = currentNeuron;
		if(targetElement == 0) {
			target = currentSynapse;
			targetElement = currentSynapse;
		}
	
		if(sourceElement == targetElement) {
			emit done();
			return;
		}

		QList<Synapse*> allSynapses = network->getSynapses();

		if(mSynapse->getSource() == mPreviousNeuron) {
			if(currentSynapse != 0) {
				//synapse can not be a source of another synapse.
				emit done();
				return;
			}

			for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
				Synapse *synapse = i.next();
				if(synapse->getSource() == currentNeuron 
						&& synapse->getTarget() == mSynapse->getTarget()) 
				{
					//there is already a synapse from currentNeuron to target.
					emit done();
					return;
				}
			}
		}
		if(mSynapse->getTarget() == mPreviousSynapse || mSynapse->getTarget() == mPreviousNeuron) {
			for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
				Synapse *synapse = i.next();
				if(synapse->getSource() == mSynapse->getSource() 
						&& synapse->getTarget() == target) 
				{
					//there is already a synapse from source to currentTarget.
					emit done();
					return;
				}
			}
		}


		//create command.
		ReconnectSynapseCommand *command = new ReconnectSynapseCommand(mVisuContext,
					mSynapse, sourceElement, targetElement);
		mVisuContext->getCommandExecutor()->executeCommand(command);

		emit done();
	}

	emit done();
}


void ReconnectSynapseNetworkTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
	TRACE("ReconnectSynapseNetworkTool::mouseButtonReleased");
}


void ReconnectSynapseNetworkTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}

void ReconnectSynapseNetworkTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
	TRACE("ReconnectSynapseNetworkTool::mouseDragged");
}


}



