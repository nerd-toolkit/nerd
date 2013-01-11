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


#include "GroupSelectionWidget.h"
#include <iostream>
#include <QList> 
#include "Core/Core.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Constraints/GroupConstraint.h"
#include "Gui/NetworkEditorCommands/InsertGroupCommand.h"
#include "Gui/NetworkEditorCommands/RemoveNetworkObjectsCommand.h"
#include "Gui/NetworkEditorCommands/RenameGroupCommand.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/MoveMembersToModuleCommand.h"
#include <QGroupBox>
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include <QHashIterator>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");
//#define pTRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new GroupSelectionWidget.
 */
GroupSelectionWidget::GroupSelectionWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner)
{
	TRACE("GroupSelectionWidget::GroupSelectionWidget");
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	setLayout(layout);

	QLayout *listLayout = new QHBoxLayout();
	layout->addLayout(listLayout);

	QLayout *nameLayout = new QHBoxLayout();
	layout->addLayout(nameLayout);

	QLayout *groupButtonLayout = new QHBoxLayout();
	layout->addLayout(groupButtonLayout);
	
	QLayout *neuronButtonLayout = new QHBoxLayout();
	layout->addLayout(neuronButtonLayout);

	mGroupList = new QListWidget(this);
	mGroupList->setSelectionMode(QAbstractItemView::SingleSelection);
	listLayout->addWidget(mGroupList);

	mGroupNameField = new QLineEdit("", this);
	nameLayout->addWidget(mGroupNameField);

	mAddButton = new QPushButton("Create", this);
	mAddButton->setContentsMargins(2, 2, 2, 2);
	mAddButton->setMinimumWidth(15);
	groupButtonLayout->addWidget(mAddButton);
	mRenameButton = new QPushButton("Rename", this);
	mRenameButton->setContentsMargins(2, 2, 2, 2);
	mRenameButton->setMinimumWidth(15);
	groupButtonLayout->addWidget(mRenameButton);
	mRemoveButton = new QPushButton("Delete", this);
	mRemoveButton->setContentsMargins(2, 2, 2, 2);
	mRemoveButton->setMinimumWidth(15);
	groupButtonLayout->addWidget(mRemoveButton);

	QGroupBox *itemBox = new QGroupBox("Member Items", this);
	itemBox->setAlignment(Qt::AlignHCenter);
	itemBox->setFlat(false);
	QHBoxLayout *itemBoxLayout = new QHBoxLayout();
	itemBox->setLayout(itemBoxLayout);
	itemBoxLayout->setContentsMargins(4, 4, 4, 4);
	mAddItemsButton = new QPushButton("Add", itemBox);
	mAddItemsButton->setContentsMargins(2, 2, 2, 2);
	mAddItemsButton->setMinimumWidth(15);
	itemBoxLayout->addWidget(mAddItemsButton);
	mRemoveItemsButton = new QPushButton("Remove", itemBox);
	mRemoveItemsButton->setContentsMargins(2, 2, 2, 2);
	mRemoveItemsButton->setMinimumWidth(15);
	itemBoxLayout->addWidget(mRemoveItemsButton);
	mShowMembersButton = new QPushButton("Select", itemBox);
	mShowMembersButton->setContentsMargins(2, 2, 2, 2);
	mShowMembersButton->setMinimumWidth(15);
	itemBoxLayout->addWidget(mShowMembersButton);

	neuronButtonLayout->addWidget(itemBox);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
	connect(mGroupList, SIGNAL(itemSelectionChanged()),
			this, SLOT(groupSelectionChanged()));

	connect(mAddButton, SIGNAL(pressed()),
			this, SLOT(addButtonPressed()));
	connect(mRemoveButton, SIGNAL(pressed()),
			this, SLOT(removeButtonPressed()));
	connect(mRenameButton, SIGNAL(pressed()),
			this, SLOT(renameButtonPressed()));
	connect(mAddItemsButton, SIGNAL(pressed()),
			this, SLOT(addItemsButtonPressed()));
	connect(mRemoveItemsButton, SIGNAL(pressed()),
			this, SLOT(removeItemsButtonPressed()));
	connect(mShowMembersButton, SIGNAL(pressed()),
			this, SLOT(showMembersButtonPressed()));
	connect(mGroupNameField, SIGNAL(returnPressed()),
			this, SLOT(renameButtonPressed()));
}



/**
 * Destructor.
 */
GroupSelectionWidget::~GroupSelectionWidget() {
	TRACE("GroupSelectionWidget::~GroupSelectionWidget");
}


void GroupSelectionWidget::selectionChanged(QList<PaintItem*>)
{
	TRACE("GroupSelectionWidget::selectionChanged");
	//TODO
}


void GroupSelectionWidget::modificationPolicyChanged() {
	TRACE("GroupSelectionWidget::modificationPolicyChanged");

	mGroupNameField->setEnabled(mModificationsEnabled);
	mAddButton->setEnabled(mModificationsEnabled);
	mRenameButton->setEnabled(mModificationsEnabled);
	mRemoveButton->setEnabled(mModificationsEnabled);
	mAddItemsButton->setEnabled(mModificationsEnabled);
	mRemoveItemsButton->setEnabled(mModificationsEnabled);
}


void GroupSelectionWidget::invalidateView() {
	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
	EditorToolWidget::invalidateView();
}

void GroupSelectionWidget::mouseButtonPressed(NetworkVisualization*, QMouseEvent*, const QPointF&)
{
}


void GroupSelectionWidget::mouseButtonReleased(NetworkVisualization *, QMouseEvent*, const QPointF&)
{
}

void GroupSelectionWidget::mouseDoubleClicked(NetworkVisualization *source,
					QMouseEvent *event, const QPointF &globalPosition)
{
	if(source == 0 || !(event->buttons() & Qt::LeftButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = source->getVisualizationHander();
	if(handler == 0) {
		return;
	}
	NeuronGroup *clickedGroup = 0;

	QList<PaintItem*> items = source->getPaintItems();
	QListIterator<PaintItem*> i(items);
	i.toBack();
	for(; i.hasPrevious();) {
		PaintItem *item = i.previous();
		if(!item->isHit(globalPosition, event->buttons(), source->getScaling())) {
			continue;
		}
		ModuleItem *module = dynamic_cast<ModuleItem*>(item);
		if(module != 0) {
			clickedGroup = module->getNeuroModule();
			break;
		}
		GroupItem *group = dynamic_cast<GroupItem*>(item);
		if(group != 0) {
			clickedGroup = group->getNeuronGroup();
			break;
		}
		//if another item was hit, then do nothing.
		return;
	}

	if(clickedGroup == 0) {
		return;
	}
	
	//change the currently selected item
	for(QHashIterator<QListWidgetItem*, NeuronGroup*> i(mNetworkGroups); i.hasNext();) {
		i.next();
		if(i.value() == clickedGroup) {
			mGroupList->setCurrentItem(i.key());
			groupSelectionChanged();
			break;
		}
	}
	mEditor->bringToolWidgetToFront(this);
	mGroupNameField->setFocus();
}

void GroupSelectionWidget::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
}



void GroupSelectionWidget::currentEditorTabChanged(int) {
	TRACE("GroupSelectionWidget::currentEditorTabChanged");

	if(mEditor == 0) {
			return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(networkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(networkModified(ModularNeuralNetwork*)));

		visu->addMouseListener(this);
	}
}


void GroupSelectionWidget::networkChanged(ModularNeuralNetwork*) {
	TRACE("GroupSelectionWidget::networkChanged");

	mNetworkInvalid = false;

	updateGroupList();
}


void GroupSelectionWidget::networkModified(ModularNeuralNetwork*) {
	TRACE("GroupSelectionWidget::networkModified");

	updateGroupList();
}


/**
 * Creates a new group and adds it to the network.
 */
void GroupSelectionWidget::addButtonPressed() {
	TRACE("GroupSelectionWidget::addButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(network == 0) {
		return;
	}

	QString groupName = mGroupNameField->text().trimmed();

	if(groupName == "") {
		return;
	}
	
	//ensure that there is not already a group with this name
	QList<NeuronGroup*> groups = network->getNeuronGroups();
	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		NeuronGroup *group = i.next();
		if(group->getName() == groupName) {
			return;
		}
	}

	NeuronGroup *group = new NeuronGroup(groupName);

	//determine location for group (under the lowest other group)
	QList<PaintItem*> items = visu->getPaintItems();
	QPointF lowestPos = QPointF(0.0, 0.0);
	bool foundPos = false;
	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		GroupItem *group = dynamic_cast<GroupItem*>(i.next());
		if(group != 0) {
			QPointF pos = group->getGlobalPosition();
			if(!foundPos || pos.y() > lowestPos.y()) {
				foundPos = true;
				lowestPos = pos;
			}
		}
	}

	InsertGroupCommand *command = new InsertGroupCommand(visu, group, lowestPos + QPointF(0.0, 40.0));
	visu->getCommandExecutor()->executeCommand(command);

	updateGroupList();
}


void GroupSelectionWidget::removeButtonPressed() {
	TRACE("GroupSelectionWidget::removeButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}
	
	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(network == 0) {
		return;
	}

	QList<QListWidgetItem*> selectedItems = mGroupList->selectedItems();

	QList<PaintItem*> itemsToRemove;
	NeuronGroup *defaultGroup = network->getDefaultNeuronGroup();

	for(QListIterator<QListWidgetItem*> i(selectedItems); i.hasNext();) {
		QListWidgetItem *item = i.next();
		NeuronGroup *group = mNetworkGroups.value(item);
		if(group != 0 && group != defaultGroup) {
			PaintItem *paintItem = handler->getGroupItem(group);
			if(paintItem == 0) {
				paintItem = handler->getModuleItem(dynamic_cast<NeuroModule*>(group));
			}
			if(paintItem != 0) {
				itemsToRemove.append(paintItem);
			}
		}
	}

	if(!itemsToRemove.empty()) {
		RemoveNetworkObjectsCommand *command = 
				new RemoveNetworkObjectsCommand(visu, itemsToRemove);
		visu->getCommandExecutor()->executeCommand(command);
	}

	updateGroupList();
}


void GroupSelectionWidget::renameButtonPressed() {
	TRACE("GroupSelectionWidget::renameButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(network == 0) {
		return;
	}

	QList<QListWidgetItem*> selectedItems = mGroupList->selectedItems();

	if(selectedItems.size() != 1) {
		return;
	}

	QString newGroupName = mGroupNameField->text().trimmed();

// 	//ensure that there is not already a group with this name
// 	QList<NeuronGroup*> groups = network->getNeuronGroups();
// 	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
// 		NeuronGroup *group = i.next();
// 		if(group->getName() == newGroupName) {
// 			return;
// 		}
// 	}

	NeuronGroup *defaultGroup = network->getDefaultNeuronGroup();
	
	NeuronGroup *group = mNetworkGroups.value(selectedItems.at(0));
	if(group == 0 || group == defaultGroup) {
		return;
	}

	if(group->getName() == newGroupName) {
		return;
	}

	RenameGroupCommand *command = new RenameGroupCommand(visu, group, newGroupName);
	visu->getCommandExecutor()->executeCommand(command);
}





void GroupSelectionWidget::addItemsButtonPressed() {
	TRACE("GroupSelectionWidget::addItemsButtonPressed");


	addAndRemoveItems(true);
}


void GroupSelectionWidget::removeItemsButtonPressed() {
	TRACE("GroupSelectionWidget::removeItemsButtonPressed");

	addAndRemoveItems(false);
}

void GroupSelectionWidget::showMembersButtonPressed() {
	TRACE("GroupSelectionWidget::showMembersButtonPressed");


	if(mEditor == 0 || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		clearGroupList();
		return;
	}

	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	QList<Neuron*> memberNeurons;

	QList<QListWidgetItem*> selectedItems = mGroupList->selectedItems();

	bool firstItem = true;
	for(QListIterator<QListWidgetItem*> i(selectedItems); i.hasNext();) {
		QListWidgetItem *item = i.next();
		
		NeuronGroup *group = mNetworkGroups.value(item);
		if(group != 0) {
			if(firstItem) {
				firstItem = false;
				memberNeurons = group->getNeurons();
			}
			else {
				QList<Neuron*> neurons = group->getNeurons();
				QList<Neuron*> currentMembers = memberNeurons;
				for(QListIterator<Neuron*> j(currentMembers); j.hasNext();) {
					Neuron *neuron = j.next();
					if(!neurons.contains(neuron)) {
						memberNeurons.removeAll(neuron);
					}
				}
			}
		}	
	}

	QList<PaintItem*> selectedObjects;
	for(QListIterator<Neuron*> i(memberNeurons); i.hasNext();) {
		Neuron *neuron = i.next();

		if(neuron == 0) {
			continue;
		}
		PaintItem *item = handler->getNeuronItem(neuron);
		if(item != 0) {
			selectedObjects.append(item);
		}
	}
	if(selectedItems.size() == 1) {
		NeuronGroup *group = mNetworkGroups.value(selectedItems.front());
		if(group != 0) {
			QList<NeuroModule*> subModules = group->getSubModules();
			for(QListIterator<NeuroModule*> j(subModules); j.hasNext();) {
				PaintItem *item = handler->getModuleItem(j.next());
				if(item != 0) {
					selectedObjects.append(item);
				}
			}
		}
	}
	
	visu->setSelectedItems(selectedObjects);

}




void GroupSelectionWidget::groupSelectionChanged() {
	TRACE("GroupSelectionWidget::groupSelectionChanged");

	QListWidgetItem *item = mGroupList->currentItem();
	if(item == 0) {
		mGroupNameField->setText("");
	}
	else {
		mGroupNameField->setText(item->text());
	}	
}

void GroupSelectionWidget::addAndRemoveItems(bool addToTarget) {
	TRACE("GroupSelectionWidget::addAndRemoveItems");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(handler == 0 || network == 0) {
		return;
	}

	QList<QListWidgetItem*> selectedWidgetItems = mGroupList->selectedItems();
	if(selectedWidgetItems.size() != 1) {
		return;
	}
	NeuronGroup *selectedGroup = mNetworkGroups.value(selectedWidgetItems.front());
	
	//do not allow the removal of neurons from the default list.
	NeuronGroup *defaultGroup = network->getDefaultNeuronGroup();
	if(selectedGroup == defaultGroup) {
		return;
	}

	QList<PaintItem*> selectedObjects = visu->getSelectedItems();

	if(selectedObjects.empty()) {
		return;
	}

	QList<PaintItem*> itemsToMove;
	for(QListIterator<PaintItem*> i(selectedObjects); i.hasNext();) {
		PaintItem *item = i.next();
		
		if(dynamic_cast<NeuronItem*>(item) != 0 || dynamic_cast<ModuleItem*>(item) != 0) {
			itemsToMove.append(item);
		}
	}
	
	if(itemsToMove.empty()) {
		return;
	}

	NeuronGroup *source = 0; 
	NeuronGroup *target = 0;

	if(addToTarget) {
		target = selectedGroup;
	}
	else {
		source = selectedGroup;
	}

	MoveMembersToModuleCommand *command = new MoveMembersToModuleCommand(
			visu, itemsToMove, source, target);

	visu->getCommandExecutor()->executeCommand(command);
}

void GroupSelectionWidget::updateGroupList() {
	TRACE("GroupSelectionWidget::updateGroupList");

	if(mEditor == 0 || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		clearGroupList();
		return;
	}

	bool selectionChanged = false;

	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(network == 0) {
		clearGroupList();
		return;
	}

	QList<NeuronGroup*> groups = network->getNeuronGroups();

	//Remove all groups that are not part of the network any more.
	QList<NeuronGroup*> currentlyKnownGroups = mNetworkGroups.values();
	for(QListIterator<NeuronGroup*> i(currentlyKnownGroups); i.hasNext();) {
		NeuronGroup *group = i.next();
		if(!groups.contains(group)) {
			QListWidgetItem *item = mNetworkGroups.key(group);
			mNetworkGroups.erase(mNetworkGroups.find(item));
			mGroupList->takeItem(mGroupList->row(item));
			selectionChanged = true;
			delete item;
		}
	}	

	for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
		NeuronGroup *group = i.next();
		//TODO reenable when an own module add/remove neurons operator is available
// 		if(dynamic_cast<NeuroModule*>(group) != 0) {
// 			//ignore neuro modules	
// 			continue;
// 		}
		
		if(mNetworkGroups.values().contains(group)) {
			QListWidgetItem *item = mNetworkGroups.key(group);
			if(item != 0) {
				item->setText(group->getName());
			}
			continue;
		}
		QListWidgetItem *item = new QListWidgetItem(group->getName(), mGroupList);
		mNetworkGroups.insert(item, group);
		selectionChanged = true;
	}

	if(selectionChanged) {
		groupSelectionChanged();
	}
}

void GroupSelectionWidget::clearGroupList() {
	TRACE("GroupSelectionWidget::clearGroupList");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mGroupList->clear();
	mNetworkGroups.clear();
	groupSelectionChanged();
}

}




