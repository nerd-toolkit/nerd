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


#include "InsertModuleNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/InsertModuleCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new InsertModuleNetworkTool.
 */
InsertModuleNetworkTool::InsertModuleNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner)
{
	TRACE("InsertModuleNetworkTool::InsertModuleNetworkTool");
}



/**
 * Destructor.
 */
InsertModuleNetworkTool::~InsertModuleNetworkTool() {
	TRACE("InsertModuleNetworkTool::~InsertModuleNetworkTool");
}

void InsertModuleNetworkTool::clear() {
	TRACE("InsertModuleNetworkTool::clear");

	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}

	NetworkManipulationTool::clear();
}


void InsertModuleNetworkTool::activate(NetworkVisualization *visu) {
	TRACE("InsertModuleNetworkTool::activate");

	NetworkManipulationTool::activate(visu);

	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
}

QString InsertModuleNetworkTool::getStatusMessage() {
	TRACE("InsertModuleNetworkTool::getStatusMessage");

	return "Insert Module";
}

void InsertModuleNetworkTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{	
	TRACE("InsertModuleNetworkTool::mouseButtonPressed");

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

	NeuroModule *newModule = new NeuroModule("Module");

	InsertModuleCommand *command = new InsertModuleCommand(mVisuContext, newModule, pos, module, "");

	mVisuContext->getCommandExecutor()->executeCommand(command);

	emit done();
}


void InsertModuleNetworkTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
	TRACE("InsertModuleNetworkTool::mouseButtonReleased");
}

void InsertModuleNetworkTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}

void InsertModuleNetworkTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
	TRACE("InsertModuleNetworkTool::mouseDragged");
}




}




