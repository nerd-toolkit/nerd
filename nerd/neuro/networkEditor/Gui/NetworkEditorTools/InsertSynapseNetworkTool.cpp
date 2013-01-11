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


#include "InsertSynapseNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/InsertSynapseCommand.h"
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
 * Constructs a new InsertSynapseNetworkTool.
 */
InsertSynapseNetworkTool::InsertSynapseNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner), mSourceNeuron(0)
{
	TRACE("InsertSynapseNetworkTool::InsertSynapseNetworkTool");
}


/**
 * Destructor.
 */
InsertSynapseNetworkTool::~InsertSynapseNetworkTool() {
	TRACE("InsertSynapseNetworkTool::~InsertSynapseNetworkTool");
}

void InsertSynapseNetworkTool::clear() {
	TRACE("InsertSynapseNetworkTool::clear");

	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}

	mSourceNeuron = 0;
	NetworkManipulationTool::clear();
}


void InsertSynapseNetworkTool::activate(NetworkVisualization *visu) {
	TRACE("InsertSynapseNetworkTool::activate");

	NetworkManipulationTool::activate(visu);

	mSourceNeuron = 0;
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
}

QString InsertSynapseNetworkTool::getStatusMessage() {
	TRACE("InsertSynapseNetworkTool::getStatusMessage");

	return "Insert Synapse";
}

void InsertSynapseNetworkTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	TRACE("InsertSynapseNetworkTool::mouseButtonPressed");

	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::LeftButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		return;
	}
	if(mSourceNeuron == 0) {
		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			NeuronItem *item = dynamic_cast<NeuronItem*>(items.at(i));
			if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				mSourceNeuron = item->getNeuron();
				if(mSourceNeuron != 0) {
					return;
				}
			}
		}
	}
	else {
		SynapseTarget *target = 0;
		QPointF targetPosition;
		QList<PaintItem*> items = mVisuContext->getPaintItems();

		for(int i = items.size() -1; i >= 0; --i) {
			{
				SynapseItem *item = dynamic_cast<SynapseItem*>(items.at(i));
				if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					target = item->getSynapse();
					if(target != 0) {
						targetPosition = item->getGlobalPosition();
						break;
					}
				}
			}
			if(target == 0) {
				NeuronItem *item = dynamic_cast<NeuronItem*>(items.at(i));
				if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					target = item->getNeuron();
					if(target != 0) {
						targetPosition = item->getGlobalPosition();
						break;
					}
				}
			}
		}

		if(target == 0) {
			emit done();
			return;
		}

		NeuronItem *neuronItem = handler->getNeuronItem(mSourceNeuron);

		NeuralNetwork *network = handler->getNeuralNetwork();
		if(network == 0) {
			return;
		}

		//check if target already has a synapse from mSourceNeuron
		QList<Synapse*> incommingSynapses = target->getSynapses();
		for(QListIterator<Synapse*> i(incommingSynapses); i.hasNext();) {
			if(i.next()->getSource() == mSourceNeuron) {
				emit done();
				return;
			}
		}
	
		Synapse *synapse = new Synapse(mSourceNeuron, target, 
						0.0, *(network->getDefaultSynapseFunction()));

		if(target != 0 && neuronItem != 0) {
			//calculate position
			QPointF position = Math::centerOfLine(neuronItem->getGlobalPosition(), targetPosition);
			if(mSourceNeuron == target) {
				position = neuronItem->getGlobalPosition() + QPointF(0.0, -70.0);
			}
// 			ModuleItem *parentModule = dynamic_cast<ModuleItem*>(neuronItem->getParent());

			InsertSynapseCommand *command = new InsertSynapseCommand(
					mVisuContext, synapse, mSourceNeuron, target, position);

			mVisuContext->getCommandExecutor()->executeCommand(command);
		}
	}

	emit done();
}


void InsertSynapseNetworkTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
	TRACE("InsertSynapseNetworkTool::mouseButtonReleased");
}


void InsertSynapseNetworkTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}

void InsertSynapseNetworkTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
	TRACE("InsertSynapseNetworkTool::mouseDragged");
}



}




