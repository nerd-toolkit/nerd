/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/

#include "GrabNetworkElementIdTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Network/NeuralNetworkElement.h"
#include <QClipboard>
#include <QApplication>
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>

using namespace std;

namespace nerd {


/**
 * Constructs a new GrabNetworkElementIdTool.
 */
GrabNetworkElementIdTool::GrabNetworkElementIdTool(QObject *owner)
	: NetworkManipulationTool(owner), mCurrentState(0)
{
}


/**
 * Destructor.
 */
GrabNetworkElementIdTool::~GrabNetworkElementIdTool() {
}

void GrabNetworkElementIdTool::clear() {
	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}
	mCurrentState = 0;
	NetworkManipulationTool::clear();
}


void GrabNetworkElementIdTool::activate(NetworkVisualization *visu) {
	NetworkManipulationTool::activate(visu);

	mCurrentState = 0;
	mIdString = "";
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
	
}

QString GrabNetworkElementIdTool::getStatusMessage() {
	return QString("Grab Object IDs:");
}

void GrabNetworkElementIdTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::RightButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHandler();
	if(handler == 0) {
		return;
	}

	NeuralNetworkElement *netElem = 0;

	QList<PaintItem*> items = mVisuContext->getPaintItems();
	for(int i = items.size() -1; i >= 0; --i) {
		PaintItem *item = items.at(i);
		ModuleItem *module = dynamic_cast<ModuleItem*>(item);
		if(module != 0 && module->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
			netElem = module->getNeuroModule();
			break;
		}
		GroupItem *group = dynamic_cast<GroupItem*>(item);
		if(group != 0 && group->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
			netElem = group->getNeuronGroup();
			break;
		}
		NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
		if(neuron != 0 && neuron->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
			netElem = neuron->getNeuron();
			break;
		}
		SynapseItem *synapse = dynamic_cast<SynapseItem*>(item);
		if(synapse != 0 && synapse->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
			netElem = synapse->getSynapse();
			break;
		}
	}

	QString text = "";
	if(netElem != 0) {
		text = QString::number(netElem->getId());

		switch(mCurrentState) {
			case 0:
				mCurrentState = 1;
				mIdString = text;
				break;
			case 1:
				mCurrentState = 2;
				mIdString = mIdString.append(",").append(text);
				break;
			case 2:
				mCurrentState = 1;
				mIdString = mIdString.append("|").append(text);
				break;
			default:
				;
		}
	
		QApplication::clipboard()->setText(mIdString);
		updateStatusMessage(QString("Grab Object IDs: [").append(mIdString).append("]"));
	}
	else {
		emit done();
	}
}


void GrabNetworkElementIdTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
}

void GrabNetworkElementIdTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}

void GrabNetworkElementIdTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
}



}




