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


#include "RemoveSelectedObjectsNetworkTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/RemoveNetworkObjectsCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new RemoveSelectedObjectsNetworkTool.
 */
RemoveSelectedObjectsNetworkTool::RemoveSelectedObjectsNetworkTool(QObject *owner)
	: NetworkManipulationTool(owner)
{
	TRACE("RemoveSelectedObjectsNetworkTool::RemoveSelectedObjectsNetworkTool");
}


/**
 * Destructor.
 */
RemoveSelectedObjectsNetworkTool::~RemoveSelectedObjectsNetworkTool() {
	TRACE("RemoveSelectedObjectsNetworkTool::~RemoveSelectedObjectsNetworkTool");
}

void RemoveSelectedObjectsNetworkTool::clear() {
	TRACE("RemoveSelectedObjectsNetworkTool::clear");

	NetworkManipulationTool::clear();
}


void RemoveSelectedObjectsNetworkTool::activate(NetworkVisualization *visu) {	
	TRACE("RemoveSelectedObjectsNetworkTool::activate");

	NetworkManipulationTool::activate(visu);

	if(mVisuContext == 0 || mVisuContext != visu) {
		emit done();
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<PaintItem*> removedItems = mVisuContext->getSelectedItems();

	if(!removedItems.empty()) {
		RemoveNetworkObjectsCommand *command = 
				new RemoveNetworkObjectsCommand(mVisuContext, removedItems);

		mVisuContext->getCommandExecutor()->executeCommand(command);
	}

	emit done();
}

QString RemoveSelectedObjectsNetworkTool::getStatusMessage() {	
	TRACE("RemoveSelectedObjectsNetworkTool::getStatusMessage");

	return "Remove Selected Objects";
}





}




