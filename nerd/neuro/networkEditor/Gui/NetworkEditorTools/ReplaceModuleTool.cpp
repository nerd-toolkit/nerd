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



#include "ReplaceModuleTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/ReplaceModuleCommand.h"
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
 * Constructs a new ReplaceModuleTool.
 */
ReplaceModuleTool::ReplaceModuleTool(QObject *owner)
	: NetworkManipulationTool(owner), mOldModule(0)
{
	TRACE("ReplaceModuleTool::ReplaceModuleTool");
}


/**
 * Destructor.
 */
ReplaceModuleTool::~ReplaceModuleTool() {
}



void ReplaceModuleTool::clear() {
	TRACE("ReplaceModuleTool::clear");

	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}

	mOldModule = 0;
	NetworkManipulationTool::clear();
}


void ReplaceModuleTool::activate(NetworkVisualization *visu) {
	TRACE("ReplaceModuleTool::activate");

	NetworkManipulationTool::activate(visu);

	mOldModule = 0;
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
}

QString ReplaceModuleTool::getStatusMessage() {
	TRACE("ReplaceModuleTool::getStatusMessage");

	return "Replace Module";
}

void ReplaceModuleTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	TRACE("ReplaceModuleTool::mouseButtonPressed");

	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::LeftButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		return;
	}
	if(mOldModule == 0) {
		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			ModuleItem *item = dynamic_cast<ModuleItem*>(items.at(i));
			if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				mOldModule = item->getNeuroModule();
				if(mOldModule != 0) {
					return;
				}
			}
		}
	}
	else {
		NeuroModule *newModule = 0;
		QList<PaintItem*> items = mVisuContext->getPaintItems();

		for(int i = items.size() -1; i >= 0; --i) {
			{
				ModuleItem *item = dynamic_cast<ModuleItem*>(items.at(i));
				if(item != 0 && item->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					newModule = item->getNeuroModule();
					if(newModule != 0) {
						break;
					}
				}
			}
		}

		if(newModule == 0) {
			emit done();
			return;
		}

		NeuralNetwork *network = handler->getNeuralNetwork();
		if(network == 0) {
			return;
		}


		ReplaceModuleCommand *command = new ReplaceModuleCommand(
					mVisuContext, mOldModule, newModule);

		mVisuContext->getCommandExecutor()->executeCommand(command);

		
	}

	emit done();
}


void ReplaceModuleTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
	TRACE("ReplaceModuleTool::mouseButtonReleased");
}

void ReplaceModuleTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}

void ReplaceModuleTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
	TRACE("ReplaceModuleTool::mouseDragged");
}

}
