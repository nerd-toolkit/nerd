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



#include "CommandExecutor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QMutexLocker>

using namespace std;

namespace nerd {


/**
 * Constructs a new CommandExecutor.
 */
CommandExecutor::CommandExecutor()
	: mUndoAction(0), mRedoAction(0), mIsExecuting(false)
{
	mExecutionMutex = new QMutex(QMutex::Recursive);

	mUndoAction = new QAction("Undo", 0);
	mRedoAction = new QAction("Redo", 0);

	connect(mUndoAction, SIGNAL(triggered()),	
			this, SLOT(undoLastCommand()));
	connect(mRedoAction, SIGNAL(triggered()),	
			this, SLOT(doNextCommand()));

	updateActions();
}



/**
 * Destructor.
 */
CommandExecutor::~CommandExecutor() {
	clearCommandStacks();
	delete mUndoAction;
	delete mRedoAction;
	delete mExecutionMutex;
}


void CommandExecutor::clearCommandStacks() {

	QMutexLocker g(mExecutionMutex);

	destroyList(mDoList);
	destroyList(mUndoList);
	notifyExecutionListeners();
}


bool CommandExecutor::executeCommand(Command *command) {
	if(command == 0) {
		return false;
	}
	QMutexLocker g(mExecutionMutex);

	mIsExecuting = true;

	destroyList(mDoList);

	if(command->doCommand() == true && command->isUndoable()) {
		mUndoList.append(command);
		notifyExecutionListeners();
		mIsExecuting = false;
		return true;
	}
	else {
		destroyList(mUndoList);
		notifyExecutionListeners();
		mIsExecuting = false;
		return false;
	}
}



Command* CommandExecutor::getLastCommand() {
	if(mUndoList.empty()) {
		return 0;
	}
	return mUndoList.last();
}


Command* CommandExecutor::getNextCommand() {
	if(mDoList.empty()) {
		return 0;
	}
	return mDoList.last();
}



QList<Command*> CommandExecutor::getUndoList() const {
	return mUndoList;
}


QList<Command*> CommandExecutor::getDoList() const {
	return mDoList;
}

QAction* CommandExecutor::getUndoAction() const {
	return mUndoAction;
}


QAction* CommandExecutor::getRedoAction() const {
	return mRedoAction;
}


bool CommandExecutor::addExecutionListener(CommandExecutionListener *listener) {
	if(listener == 0 || mExecutionListeners.contains(listener)) {
		return false;
	}
	mExecutionListeners.append(listener);
	return true;
}


bool CommandExecutor::removeExecutionListener(CommandExecutionListener *listener) {
	if(listener == 0 || !mExecutionListeners.contains(listener)) {
		return false;
	}
	mExecutionListeners.removeAll(listener);
	return true;
}


void CommandExecutor::notifyExecutionListeners() {
	updateActions();

	for(QListIterator<CommandExecutionListener*> i(mExecutionListeners); i.hasNext();)	
	{
		i.next()->notifyCommandExecutionChange();
	}
}


QList<CommandExecutionListener*> CommandExecutor::getExecutionListeners() const {
	return mExecutionListeners;
}


bool CommandExecutor::isExecuting() const {
	return mIsExecuting;
}

bool CommandExecutor::undoLastCommand() {
	QMutexLocker g(mExecutionMutex);

	if(mUndoList.empty()) {
		notifyExecutionListeners();
		return false;
	}
	mIsExecuting = true;
	
	Command *command = mUndoList.takeLast();

	if(command->undoCommand() == true) {
		mDoList.append(command);
		notifyExecutionListeners();
		mIsExecuting = false;
		return true;
	}
	else {
		destroyList(mDoList);
		destroyList(mUndoList);
		delete command;
		notifyExecutionListeners();
		mIsExecuting = false;
		return false;
	}
	
}


bool CommandExecutor::doNextCommand() {
	QMutexLocker g(mExecutionMutex);

	if(mDoList.empty()) {
		notifyExecutionListeners();
		return false;
	}
	mIsExecuting = true;
	
	Command *command = mDoList.takeLast();
	
	if(command->doCommand() == true) {
		mUndoList.append(command);
		notifyExecutionListeners();
		mIsExecuting = false;
		return true;
	}
	else {
		destroyList(mDoList);
		destroyList(mUndoList);
		delete command;
		notifyExecutionListeners();
		mIsExecuting = false;
		return false;
	}
}


void CommandExecutor::destroyList(QList<Command*> &list) {
	QMutexLocker g(mExecutionMutex);

	while(!list.empty()) {
		Command *c = list.at(0);
		list.removeAll(c);
		delete c;
	}
	list.clear();
}


void CommandExecutor::updateActions() {
	if(mUndoList.empty()) {
		mUndoAction->setText("Undo");
		mUndoAction->setEnabled(false);
	}
	else {
		mUndoAction->setText("Undo [" + mUndoList.last()->getLabel() + "]");
		mUndoAction->setEnabled(true);
	}
	if(mDoList.empty()) {
		mRedoAction->setText("Redo");
		mRedoAction->setEnabled(false);
	}
	else {
		mRedoAction->setText("Redo [" + mDoList.last()->getLabel() + "]");
		mRedoAction->setEnabled(true);
	}
}


}



