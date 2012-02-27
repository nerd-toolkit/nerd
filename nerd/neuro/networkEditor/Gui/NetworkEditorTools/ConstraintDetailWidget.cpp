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


#include "ConstraintDetailWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/ChangeValueCommand.h"
#include <QGridLayout>
#include <QGroupBox>
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Constraints/GroupConstraint.h"
#include "Constraints/Constraints.h"
#include "Math/Math.h"
#include "Gui/NetworkEditorCommands/AddConstraintCommand.h"
#include "Gui/NetworkEditorCommands/RemoveConstraintCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");
// #define pTRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new ConstraintDetailWidget.
 */
ConstraintDetailWidget::ConstraintDetailWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false), mCurrentGroup(0),
	  mPreviousGroup(0)
{
	TRACE("ConstraintDetailWidget::ConstraintDetailWidget");

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	setLayout(layout);

	QLayout *groupInfoLayout = new QHBoxLayout();
	layout->addLayout(groupInfoLayout);

	QLayout *listLayout = new QHBoxLayout();
	layout->addLayout(listLayout);

	QLayout *propertyLayout = new QHBoxLayout();
	layout->addLayout(propertyLayout);

	QLayout *constraintSelectorLayout = new QHBoxLayout();
	layout->addLayout(constraintSelectorLayout);

	QLayout *buttonLayout = new QHBoxLayout();
	layout->addLayout(buttonLayout);

	mGroupInfoLabel = new QLabel("");
	groupInfoLayout->addWidget(mGroupInfoLabel);

	mConstraintList = new QListWidget(this);
	mConstraintList->setSelectionMode(QAbstractItemView::SingleSelection);
	mConstraintList->setMaximumHeight(100);
	listLayout->addWidget(mConstraintList);

	mConstraintParameterEditor = new ParameterEditor(this, mEditor);
	propertyLayout->addWidget(mConstraintParameterEditor);

	mConstraintPoolSelector = new QComboBox(this);
	constraintSelectorLayout->addWidget(mConstraintPoolSelector);

	mAddConstraintButton = new QPushButton("Add New", this);
	mAddConstraintButton->setContentsMargins(2, 2, 2, 2);
	mAddConstraintButton->setMinimumWidth(15);
	mRemoveConstraintButton = new QPushButton("Remove", this);
	mRemoveConstraintButton->setContentsMargins(2, 2, 2, 2);
	mRemoveConstraintButton->setMinimumWidth(15);
	mApplyToAllSelectedCheckBox = new QCheckBox("All Selected", this);
	mApplyToAllSelectedCheckBox->setChecked(false);
	buttonLayout->addWidget(mAddConstraintButton);
	buttonLayout->addWidget(mRemoveConstraintButton);
	buttonLayout->addWidget(mApplyToAllSelectedCheckBox);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
	connect(mConstraintList, SIGNAL(currentRowChanged(int)),
			this, SLOT(constraintListSelectionChanged()));
	connect(mAddConstraintButton, SIGNAL(pressed()),
			this, SLOT(addButtonPressed()));
	connect(mRemoveConstraintButton, SIGNAL(pressed()),
			this, SLOT(removeButtonPressed()));
	connect(mApplyToAllSelectedCheckBox, SIGNAL(toggled(bool)),
			this, SLOT(constraintListSelectionChanged()));
}



/**
 * Destructor.
 */
ConstraintDetailWidget::~ConstraintDetailWidget() {
	TRACE("ConstraintDetailWidget::~ConstraintDetailWidget");
}


void ConstraintDetailWidget::selectionChanged(QList<PaintItem*> selectedItems) {
	TRACE("ConstraintDetailWidget::selectionChanged");

	mCurrentGroup = 0;
	if(!mNetworkInvalid) {
		for(int i = selectedItems.size() - 1; i >= 0; --i) {
			PaintItem *item = selectedItems.at(i);
			if(dynamic_cast<ModuleItem*>(item) != 0) {
				mCurrentGroup = dynamic_cast<ModuleItem*>(item)->getNeuroModule();
				break;
			}
			if(dynamic_cast<GroupItem*>(item) != 0) {
				mCurrentGroup = dynamic_cast<GroupItem*>(item)->getNeuronGroup();
				break;
			}
		}
	}

	updateView();
}


void ConstraintDetailWidget::modificationPolicyChanged() {
	mConstraintParameterEditor->setEnabled(mModificationsEnabled);
	mConstraintPoolSelector->setEnabled(mModificationsEnabled);
	mAddConstraintButton->setEnabled(mModificationsEnabled);
	mRemoveConstraintButton->setEnabled(mModificationsEnabled);
}

void ConstraintDetailWidget::invalidateView() {
	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	EditorToolWidget::invalidateView();
	mCurrentGroup = 0;
}


QList<NeuronGroup*> ConstraintDetailWidget::getSelectedGroups() const {
	QList<NeuronGroup*> selectedGroups;

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	if(visu == 0) {
		return selectedGroups;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *item = i.next();
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			selectedGroups.append(moduleItem->getNeuroModule());
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			selectedGroups.append(groupItem->getNeuronGroup());
		}
	}
	return selectedGroups;
}

void ConstraintDetailWidget::currentEditorTabChanged(int) {
	TRACE("ConstraintDetailWidget::currentEditorTabChanged");

	if(mEditor == 0) {
			return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(networkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(networkModified(ModularNeuralNetwork*)));
		visu->addSelectionListener(this);
	}
}



void ConstraintDetailWidget::networkChanged(ModularNeuralNetwork*) {
	TRACE("ConstraintDetailWidget::networkChanged");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mNetworkInvalid = false;

	if(!mInitialized) {
		mInitialized = true;
		//fill in prototype names
		QList<GroupConstraint*> constraints =
 					Constraints::getConstraintManager()->getConstraintPrototypes();

		for(QListIterator<GroupConstraint*> i(constraints); i.hasNext();) {
			mConstraintPoolSelector->addItem(i.next()->getName());
		}
	}
	updateView();
}



void ConstraintDetailWidget::networkModified(ModularNeuralNetwork*) {
	TRACE("ConstraintDetailWidget::networkModified");

	updateView();
}



void ConstraintDetailWidget::constraintListSelectionChanged() {
	TRACE("ConstraintDetailWidget::constraintListSelectionChanged");

	if(mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	int row = mConstraintList->currentRow();

	if(row >= 0 && row < mCurrentConstraints.size()) {
		mCurrentlySelectedGroups = getSelectedGroups();
		QList<ParameterizedObject*> selectedObjects;
		if(mApplyToAllSelectedCheckBox->isChecked()) {
			GroupConstraint *currentConstraint = mCurrentConstraints.at(row);

			if(!currentConstraint == 0) {
				for(QListIterator<NeuronGroup*> i(mCurrentlySelectedGroups); i.hasNext();) {
					QList<GroupConstraint*> constraints = i.next()->getConstraints();
					for(QListIterator<GroupConstraint*> j(constraints); j.hasNext();) {
						GroupConstraint *constraint = j.next();
						if(constraint->getName() == currentConstraint->getName()) {
							selectedObjects.append(constraint);
						}
					} 
				}
			}
		}
		mConstraintParameterEditor->setParameterizedObject(mCurrentConstraints.at(row),	selectedObjects);
	}
	else {
		mConstraintParameterEditor->setParameterizedObject(0);
	}
}


void ConstraintDetailWidget::addButtonPressed() {
	TRACE("ConstraintDetailWidget::addButtonPressed");

	if(mEditor == 0 || mCurrentGroup == 0 || !mModificationsEnabled || mNetworkInvalid) {
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
	QString constraintName = mConstraintPoolSelector->currentText();

	GroupConstraint *constraint = Constraints::getConstraintManager()
					->getConstraintPrototype(constraintName);

	if(constraint == 0) {
		return;
	}

	Command *command = 0;

	if(mApplyToAllSelectedCheckBox->isChecked()) {
		command = new AddConstraintCommand(visu, getSelectedGroups(), *constraint);
	}
	else {
		command = new AddConstraintCommand(visu, mCurrentGroup, *constraint);
	}

	visu->getCommandExecutor()->executeCommand(command);
	
}


void ConstraintDetailWidget::removeButtonPressed() {
	TRACE("ConstraintDetailWidget::removeButtonPressed");

	if(mEditor == 0 || mCurrentGroup == 0 || !mModificationsEnabled || mNetworkInvalid) {
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
	
	int row = mConstraintList->currentRow();
	if(row < 0 || row >= mCurrentConstraints.size()) {
		return;
	}

	GroupConstraint *constraint = mCurrentConstraints.at(row);

	if(constraint == 0) {
		return;
	}	

	Command *command = 0;

	if(mApplyToAllSelectedCheckBox->isChecked()) {
		command = new RemoveConstraintCommand(visu, getSelectedGroups(), constraint);
	}
	else {
		command = new RemoveConstraintCommand(visu, mCurrentGroup, constraint);
	}

	visu->getCommandExecutor()->executeCommand(command);

}


void ConstraintDetailWidget::updateView() {
	TRACE("ConstraintDetailWidget::updateView");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
	
	if(mCurrentGroup == mPreviousGroup) {
		//check if constraints are still the same
		if(mCurrentGroup != 0) {

			QList<GroupConstraint*> constraints = mCurrentGroup->getConstraints();

			if(constraints.size() == mCurrentConstraints.size()) {

				bool equal = true;
				for(QListIterator<GroupConstraint*> i(constraints); i.hasNext();) {
					if(!mCurrentConstraints.contains(i.next())) {
						equal = false;	
						break;
					}
				}
				QList<NeuronGroup*> selectedGroups = getSelectedGroups();
				if(selectedGroups.size() != mCurrentlySelectedGroups.size()) {
					equal = false;
				}
				else {
					for(QListIterator<NeuronGroup*> k(selectedGroups); k.hasNext();) {
						if(!mCurrentlySelectedGroups.contains(k.next())) {
							equal = false;
							break;
						}
					}
				}
				if(equal) {
					return;
				}
			}
		}
		
	}
	mPreviousGroup = mCurrentGroup;
	if(mCurrentGroup == 0) {
		mCurrentConstraints.clear();
		mGroupInfoLabel->setText("");
		setEnabled(false);
		mConstraintParameterEditor->setParameterizedObject(0);
	}
	else {	
		QString type = (dynamic_cast<ModuleItem*>(mCurrentGroup) == 0) ? "Group" : "Module";
		QString name = mCurrentGroup->getName();
		if(name.size() > 20) {
			name = QString("..").append(name.mid(name.size() - 20));
		}
		mGroupInfoLabel->setText(type + ": " + QString::number(mCurrentGroup->getId()) 
								+ " ["	+ name + "]");

		mCurrentConstraints = mCurrentGroup->getConstraints();

		int currentRow = mConstraintList->currentRow();;

		while(mConstraintList->count() > 0) {
			QListWidgetItem *item = mConstraintList->takeItem(0);
			delete item;
		}

		for(QListIterator<GroupConstraint*> i(mCurrentConstraints); i.hasNext();) {
			new QListWidgetItem(i.next()->getName(), mConstraintList);
		}

		currentRow = Math::max(0, Math::min(currentRow, mConstraintList->count() - 1));
		mConstraintList->setCurrentRow(currentRow);

		setEnabled(true);
	}

}




}




