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



#include "RunConstraintsAction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditorTools/EditorMessageWidget.h"
#include "Constraints/ConstraintManager.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new RunConstraintsAction.
 */
RunConstraintsAction::RunConstraintsAction(const QString &name, bool allGroups, int maxSteps,
					NeuralNetworkEditor *editor, QObject *parent)
	: QAction(name, parent), mExecuteAllGroupConstraints(allGroups), 
	  mMaxResolverSteps(maxSteps), mEditor(editor)
{
	TRACE("RunConstraintsAction::RunConstraintsAction");

	connect(this, SIGNAL(triggered()),
			this, SLOT(runConstraints()));
}


/**
 * Destructor.
 */
RunConstraintsAction::~RunConstraintsAction() {
	TRACE("RunConstraintsAction::~RunConstraintsAction");
}


void RunConstraintsAction::runConstraints() {
	TRACE("RunConstraintsAction::runConstraints");
	
	if(mEditor == 0) {
		return;
	}	

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	if(visu == 0) {
		return;
	}

	NetworkVisualizationHandler *handler = visu->getVisualizationHander();

	if(handler == 0) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
	
	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	CommandExecutor *executor = visu->getCommandExecutor();

	QList<NeuralNetworkElement*> trashcan;

	if(network == 0) {
		return;
	}
	
	

	EditorMessageWidget *messageBoard = mEditor->getMessageWidget();

	bool resolvedSucessfully = false;

	QStringList errors;
	if(mExecuteAllGroupConstraints) {
		messageBoard->setMessage("Running constraints for all NeuronGroups.");
		messageBoard->addMessage(QString("Maximal number of iterations: ") 
								 + QString::number(mMaxResolverSteps));
		resolvedSucessfully = ConstraintManager::runConstraints(network->getNeuronGroups(), 
									mMaxResolverSteps, executor, trashcan, errors);
	}
	else {
		QList<NeuronGroup*> groups;
		QList<PaintItem*> selectedItems = visu->getSelectedItems();

		for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
			PaintItem *item = i.next();
			if(dynamic_cast<GroupItem*>(item) != 0) {
				NeuronGroup *group = dynamic_cast<GroupItem*>(item)->getNeuronGroup();
				if(group != 0) {
					groups.append(group);
				}
			}
			else if(dynamic_cast<ModuleItem*>(item) != 0) {
				NeuronGroup *group = dynamic_cast<ModuleItem*>(item)->getNeuroModule();
				if(group != 0) {
					groups.append(group);
				}
			}
		}

		messageBoard->setMessage("Running constraints for " 
								 + QString::number(groups.size())
								 + " selected NeuronGroups.");
		messageBoard->addMessage("Maximal number of iterations: " 
								 + QString::number(mMaxResolverSteps));

		resolvedSucessfully = ConstraintManager::runConstraints(groups, mMaxResolverSteps, 
								executor, trashcan, errors);
	}
	
	//remove all temporary tags.
	{
		QList<NeuralNetworkElement*> elements;
		network->getNetworkElements(elements);
		for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
			i.next()->removePropertyByPattern("__.*__");
		}
	}

	for(QListIterator<QString> i(errors); i.hasNext();) {
		messageBoard->addMessage(i.next());
	}
	if(resolvedSucessfully) {
		messageBoard->addMessage("\nSuccess!");
	}
	else {
		
		messageBoard->addMessage("\nFailure!");
	}
	
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	visu->validateSelectedItems();
	handler->updateNetworkView();

	//delete objects that have been removed by constraints.
	while(!trashcan.empty()) {
		NeuralNetworkElement *elem = trashcan.at(0);
		trashcan.removeAll(elem);
		delete elem;
	}

	visu->getCommandExecutor()->clearCommandStacks();
}

}



