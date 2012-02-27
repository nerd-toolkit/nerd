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



#include "MotorControlManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Physics/Physics.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimObjectGroup.h"
namespace nerd{

MotorControlManager::MotorControlManager():QWidget(0), mInitialized(false) {
	setWindowTitle("Motor Control Manager");
	mXPos = 0;
	mYPos = 0;
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *windowLayout = new QVBoxLayout;
	setLayout(windowLayout);

	mInterfaceListBox = new QGroupBox(this);
	mInterfaceListBox->setTitle("Available Agents");
	QHBoxLayout *interfaceListLayout = new QHBoxLayout;
	mInterfaceListBox->setLayout(interfaceListLayout);
	mInterfaceList = new QListWidget(mInterfaceListBox);
	mInterfaceList->setMinimumHeight(30);
// 	mInterfaceListBox->setMinimumHeight(40);
	mInterfaceList->setMaximumHeight(80);
// 	mInterfaceListBox->setMaximumHeight(60);
	mInterfaceList->setSelectionMode(QAbstractItemView::MultiSelection);
	interfaceListLayout->addWidget(mInterfaceList);
	interfaceListLayout->setAlignment(Qt::AlignCenter);
	windowLayout->addWidget(mInterfaceListBox);
	windowLayout->setAlignment(mInterfaceListBox, Qt::AlignTop);
	windowLayout->setStretchFactor(mInterfaceListBox, 1);
	
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	windowLayout->addLayout(buttonLayout);

	mDeleteButton = new QPushButton("Remove Controls", this);
	buttonLayout->addWidget(mDeleteButton);
	buttonLayout->setAlignment(mDeleteButton, Qt::AlignLeft);

	mControlButton = new QPushButton("Create Controls", this);
	buttonLayout->addWidget(mControlButton);
	buttonLayout->setAlignment(mControlButton, Qt::AlignRight);

	mControllerListBox = new QGroupBox(this);
	mControllerListBox->setTitle("Active Controls");
	QHBoxLayout *activeControlsLayout = new QHBoxLayout;
	mControllerListBox->setLayout(activeControlsLayout);
	mControllerList = new QListWidget(mControllerListBox);
	activeControlsLayout->addWidget(mControllerList);
	activeControlsLayout->setAlignment(Qt::AlignCenter);
	windowLayout->addWidget(mControllerListBox);
	windowLayout->setAlignment(mControllerListBox, Qt::AlignBottom);
	windowLayout->setStretchFactor(mControllerListBox, 1);

// 	mControllerList->hide();
// 	mControllerListBox->setMinimumHeight(30);
	mControllerList->setMinimumHeight(30);
// 	mControllerListBox->setMaximumHeight(100);
	mControllerList->setMaximumHeight(80);
	mControllerList->setSelectionMode(QAbstractItemView::MultiSelection);

	connect(mControlButton, SIGNAL(clicked()), this, SLOT(addSelectedInterfaceControllers()));
	connect(mDeleteButton, SIGNAL(clicked()), this, SLOT(removeSelectedInterfaceControllers()));
	connect(mControllerList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(changeControlVisibility(QListWidgetItem*)));
	connect(mInterfaceList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addSelectedControl(QListWidgetItem*)));
}


void MotorControlManager::addSelectedControl(QListWidgetItem *listItem) {
	QString name = listItem->text();
		if(mCurrentControls.contains(name)) {
			return;
		}
		MotorControlGui *tmp = new MotorControlGui(name);
		QListWidgetItem *item = new QListWidgetItem(name);
		mControllerList->addItem(item);
		mCurrentControls[name] = tmp;
		tmp->changeVisibility();

// 	if(mControllerList->isHidden() && (mControllerList->count() != 0)){
// 		mControllerList->show();
// 	}
// 	int size = mControllerList->count() * 25;
// 	if(size > 150) {
// 			size = 150;
// 	}
// 	mControllerListBox->setMaximumHeight(size + 10);
// 	mControllerList->setMaximumHeight(size);
// 	adjustSize();
	
}

void MotorControlManager::addSelectedInterfaceControllers() {
	QList<QListWidgetItem*> currentSelection = mInterfaceList->selectedItems();
	while(currentSelection.size() > 0) {
		QString name = currentSelection.at(0)->text();
		if(mCurrentControls.contains(name)) {
			currentSelection.pop_front();
			continue;
		}
		MotorControlGui *tmp = new MotorControlGui(name);
		QListWidgetItem *item = new QListWidgetItem(name);
		mControllerList->addItem(item);
		mCurrentControls[name] = tmp;
		currentSelection.pop_front();
		tmp->changeVisibility();
	}
// 	if(mControllerList->isHidden() && (mControllerList->count() != 0)){
// 		mControllerList->show();
// 	}
// 	int size = mControllerList->count() * 25;
// 	if(size > 150) {
// 			size = 150;
// 	}
// 	mControllerListBox->setMaximumHeight(size + 10);
// 	mControllerList->setMaximumHeight(size);
// 	adjustSize();
}


void MotorControlManager::changeControlVisibility(QListWidgetItem *item) {
	QString name = item->text();
	MotorControlGui *tmp = mCurrentControls.value(name);
	if(tmp != 0) {
		tmp->changeVisibility();
	}
}

void MotorControlManager::removeSelectedInterfaceControllers() {
	
	QList<QListWidgetItem*> currentSelection = mControllerList->selectedItems();
	while(currentSelection.size() > 0) {
		QString name = currentSelection.at(0)->text();
		MotorControlGui *tmp = mCurrentControls.value(name);
		mCurrentControls.remove(name);
		mControllerList->removeItemWidget(currentSelection.at(0));
		//mControllerList->repaint();
		mControllerList->update();
		delete tmp;
		QListWidgetItem *tmpItem = currentSelection.at(0);
		currentSelection.pop_front();
		delete tmpItem;
	}
// 	int size = mControllerList->count() * 25;
// 	if(size > 150) {
// 			size = 150;
// 	}
// 	if(mControllerList->count() == 0) {
// 		mControllerList->hide();
// 	}
// 	mControllerListBox->setMaximumHeight(size + 10);
// 	mControllerList->setMaximumHeight(size);
// 	adjustSize();
}

MotorControlManager::~MotorControlManager(){
	  while (!mCurrentControls.empty()) {
   	MotorControlGui *tmp = mCurrentControls.begin().value();
		mCurrentControls.erase(mCurrentControls.begin());
		delete tmp;
   }
}

void MotorControlManager::createInterfaceList() {
	PhysicsManager *pManager = Physics::getPhysicsManager();
	QList<SimObjectGroup*> groups = pManager->getSimObjectGroups();
	for(int i = 0; i < groups.size(); i++) {
		QListWidgetItem *item = new QListWidgetItem(groups.at(i)->getName());
		mInterfaceList->addItem(item);
	}
	mInitialized = true;
// 	int size = mInterfaceList->count() * 25;
// 	if(size > 150) {
// 		size = 150;
// 	}
// 	mInterfaceList->setMaximumHeight(size);
// 	mInterfaceListBox->setMaximumHeight(size + 10);
// 	repaint();
// 	adjustSize();
}

void MotorControlManager::changeVisibility() {
	if(isHidden()) {
		if(!mInitialized) {
			createInterfaceList();
		}
		move(mXPos, mYPos);
		show();
	} 
	else {
		hide();
		mXPos = x();
		mYPos = y();
	}
}

void MotorControlManager::closeEvent(QCloseEvent*) {
	mXPos = x();
	mYPos = y();
}



}
