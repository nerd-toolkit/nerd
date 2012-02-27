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


#include "InsertNeuronNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/InsertNeuronCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");


using namespace std;

namespace nerd {


/**
 * Constructs a new InsertNeuronNetworkTool.
 */
InsertNeuronNetworkTool::InsertNeuronNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner)
{
	TRACE("InsertNeuronNetworkTool::InsertNeuronNetworkTool");
}


/**
 * Destructor.
 */
InsertNeuronNetworkTool::~InsertNeuronNetworkTool() {	
	TRACE("InsertNeuronNetworkTool::~InsertNeuronNetworkTool");
}


void InsertNeuronNetworkTool::clear() {
	TRACE("InsertNeuronNetworkTool::clear");

	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}

	NetworkManipulationTool::clear();
}


void InsertNeuronNetworkTool::activate(NetworkVisualization *visu) {
	TRACE("InsertNeuronNetworkTool::activate");

	NetworkManipulationTool::activate(visu);

	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
}

QString InsertNeuronNetworkTool::getStatusMessage() {	
	TRACE("InsertNeuronNetworkTool::getStatusMessage");

	return "Insert Neuron";
}

void InsertNeuronNetworkTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	TRACE("InsertNeuronNetworkTool::mouseButtonPressed");

	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::LeftButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		return;
	}
	NeuroModule *module = 0;
	QPointF pos = globalPosition;

	QList<PaintItem*> items = mVisuContext->getPaintItems();
	for(int i = items.size() -1; i >= 0; --i) {
		ModuleItem *item = dynamic_cast<ModuleItem*>(items.at(i));
		if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
			module = item->getNeuroModule();
			break;
		}
	}

	NeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return;
	}

	Neuron *neuron = new Neuron("", *(network->getDefaultTransferFunction()),
								*(network->getDefaultActivationFunction()));

	InsertNeuronCommand *command = new InsertNeuronCommand(mVisuContext, neuron, pos, module);

	mVisuContext->getCommandExecutor()->executeCommand(command);

	emit done();
}


void InsertNeuronNetworkTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
	TRACE("InsertNeuronNetworkTool::mouseButtonReleased");
}

void InsertNeuronNetworkTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}

void InsertNeuronNetworkTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
	TRACE("InsertNeuronNetworkTool::mouseDragged");
}


}




