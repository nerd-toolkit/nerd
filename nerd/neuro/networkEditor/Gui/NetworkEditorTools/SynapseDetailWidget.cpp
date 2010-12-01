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



#include "SynapseDetailWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/ChangeValueCommand.h"
#include <QGridLayout>
#include <QGroupBox>
#include "Network/Neuro.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditorCommands/ChangeSynapseFunctionCommand.h"
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
 * Constructs a new SynapseDetailWidget.
 */
SynapseDetailWidget::SynapseDetailWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false), mSynapse(0),
	  mStrengthValue(0), mEnabledValue(0), mUpdatingChangedSelection(false)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	setLayout(layout);

	QGridLayout *synapseLayout = new QGridLayout();
	layout->addLayout(synapseLayout);

	QWidget *strengthArea = new QWidget(this);
	mObserveStrengthField = new QCheckBox(strengthArea);
	mObserveStrengthField->setChecked(true);
	QGridLayout *strengthFieldLayout = new QGridLayout(strengthArea);
	strengthFieldLayout->setContentsMargins(0, 0, 0, 0);
	strengthFieldLayout->addWidget(mObserveStrengthField, 0, 1);
	mSynapseStrengthField = new QLineEdit("", strengthArea);
	strengthFieldLayout->addWidget(mSynapseStrengthField, 0, 0);
	synapseLayout->addWidget(new QLabel("Weight:", this), 0, 0);
	synapseLayout->addWidget(strengthArea, 0, 1);


	mEnableSynapseCheckBox = new QCheckBox(this);
	synapseLayout->addWidget(new QLabel("Disable:", this), 1, 0);
	synapseLayout->addWidget(mEnableSynapseCheckBox, 1, 1);


	QGroupBox *synapseFunctionBox = new QGroupBox("SynapseFunction", this);
	QVBoxLayout *sfBoxLayout = new QVBoxLayout();
	sfBoxLayout->setContentsMargins(4, 4, 4, 4);
	synapseFunctionBox->setLayout(sfBoxLayout);
	layout->addWidget(synapseFunctionBox);
	mSynapseFunctionSelector = new QComboBox(synapseFunctionBox);
	sfBoxLayout->addWidget(mSynapseFunctionSelector);

	mSynapseFunctionParameterEditor = new ParameterEditor(synapseFunctionBox, mEditor);
	sfBoxLayout->addWidget(mSynapseFunctionParameterEditor);


	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}

	connect(mObserveStrengthField, SIGNAL(stateChanged(int)),
			this, SLOT(enableStrengthObservation(int)));
	connect(mSynapseFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(synapseFunctionSelectionChanged()));
	connect(mSynapseStrengthField, SIGNAL(returnPressed()),
			this, SLOT(changeSynapseStrength()));
    connect(mSynapseStrengthField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markAsValueEdited()));
	connect(mEnableSynapseCheckBox, SIGNAL(clicked()),
			this, SLOT(enableSynapse()));
	connect(this, SIGNAL(setSynapseStrengthText(const QString&)),
			mSynapseStrengthField, SLOT(setText(const QString&)));
}


/**
 * Destructor.
 */
SynapseDetailWidget::~SynapseDetailWidget() {
	TRACE("SynapseDetailWidget::~SynapseDetailWidget");

	if(mSynapse != 0) {
		mSynapse->getStrengthValue().removeValueChangedListener(this);
	}
}


void SynapseDetailWidget::selectionChanged(QList<PaintItem*>) {
	TRACE("SynapseDetailWidget::selectionChanged");

	updateSynapseView();
}


void SynapseDetailWidget::modificationPolicyChanged() {
	TRACE("SynapseDetailWidget::modificationPolicyChanged");

	mSynapseStrengthField->setReadOnly(!mModificationsEnabled);
	mSynapseFunctionSelector->setEnabled(mModificationsEnabled);
	mSynapseFunctionParameterEditor->setEnabled(mModificationsEnabled);
	mEnableSynapseCheckBox->setEnabled(mModificationsEnabled);
}


void SynapseDetailWidget::valueChanged(Value *value) {
	TRACE("SynapseDetailWidget::valueChanged");

	if(value == 0) {
		return;
	}
	if(value == mStrengthValue) {
		mSynapseStrengthField->setText(mStrengthValue->getValueAsString());
	}
	if(value == mEnabledValue) {
		mEnableSynapseCheckBox->setChecked(!mEnabledValue->get());
	}
}


QString SynapseDetailWidget::getName() const {
	return "SynapseDetailWidget";
}


void SynapseDetailWidget::invalidateView() {

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	EditorToolWidget::invalidateView();
	mSynapseFunctionParameterEditor->invalidateListeners();

	mSynapse = 0;
	if(mStrengthValue != 0) {
		mStrengthValue->removeValueChangedListener(this);
		mStrengthValue = 0;
	}
	if(mEnabledValue != 0) {
		mEnabledValue->removeValueChangedListener(this);
		mEnabledValue = 0;
	}
}


void SynapseDetailWidget::mouseButtonPressed(NetworkVisualization*, QMouseEvent*, const QPointF&)
{
}


void SynapseDetailWidget::mouseButtonReleased(NetworkVisualization *, QMouseEvent*, const QPointF&)
{
}

void SynapseDetailWidget::mouseDoubleClicked(NetworkVisualization *source,
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
	SynapseItem *clickedSynapse = 0;

	QList<PaintItem*> items = source->getPaintItems();
	QListIterator<PaintItem*> i(items);
	i.toBack();
	for(; i.hasPrevious();) {
		PaintItem *item = i.previous();
		if(!item->isHit(globalPosition, event->buttons(), source->getScaling())) {
			continue;
		}
		SynapseItem *synapse = dynamic_cast<SynapseItem*>(item);
		if(synapse != 0) {
			clickedSynapse = synapse;
			break;
		}
		//if another item was hit, then do nothing.
		return;
	}

	if(clickedSynapse == 0) {
		return;
	}

	mEditor->bringToolWidgetToFront(this);
	mSynapseStrengthField->setFocus();
}

void SynapseDetailWidget::mouseDragged(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}


void SynapseDetailWidget::currentEditorTabChanged(int) {
	TRACE("SynapseDetailWidget::currentEditorTabChanged");

	if(mEditor == 0) {
			return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->addSelectionListener(this);
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(networkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(networkModified(ModularNeuralNetwork*)));

		visu->addMouseListener(this);
	}
}


void SynapseDetailWidget::networkChanged(ModularNeuralNetwork*) {
	TRACE("SynapseDetailWidget::networkChanged");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(!mInitialized) {
		mInitialized = true;
		//fill in prototype names
		QList<SynapseFunction*> sfs =
 					Neuro::getNeuralNetworkManager()->getSynapseFunctionPrototypes();
		for(QListIterator<SynapseFunction*> i(sfs); i.hasNext();) {
			mSynapseFunctionSelector->addItem(i.next()->getName());
		}
	}
	updateSynapseView();
}


void SynapseDetailWidget::networkModified(ModularNeuralNetwork*)  {
	TRACE("SynapseDetailWidget::networkModified");

	updateSynapseView();
}


void SynapseDetailWidget::changeSynapseStrength() {
	TRACE("SynapseDetailWidget::changeSynapseStrength");

	emit markAsValueUpdated();

	if(mEditor == 0 || !mModificationsEnabled) {
			return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0 || mSynapse == 0) {
		return;
	}
	bool changeRequired = false;
	QString content = mSynapseStrengthField->text().trimmed();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Value*> selectedValues;
	QList<QString> contents;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		SynapseItem *item = dynamic_cast<SynapseItem*>(i.next());
		if(item != 0) {
			Synapse *synapse = item->getSynapse();
			if(synapse != 0) {
				selectedValues.append(&(synapse->getStrengthValue()));
				contents.append(content);
				if(content != synapse->getStrengthValue().getValueAsString()) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	ChangeValueCommand *command = new ChangeValueCommand(visu, selectedValues, contents,
														 "Change Weight");

	visu->getCommandExecutor()->executeCommand(command);
}


void SynapseDetailWidget::enableSynapse() {
	TRACE("SynapseDetailWidget::enableSynapse");

	if(mSynapse == 0 || mEditor == 0 || !mModificationsEnabled) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	bool changeRequired = false;
	bool isDisabled = mEnableSynapseCheckBox->isChecked();
	QString content = isDisabled ? "F" : "T";

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Value*> selectedValues;
	QList<QString> contents;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		SynapseItem *item = dynamic_cast<SynapseItem*>(i.next());
		if(item != 0) {
			Synapse *synapse = item->getSynapse();
			if(synapse != 0) {
				selectedValues.append(&(synapse->getEnabledValue()));
				contents.append(content);
				if(synapse->getEnabledValue().get() != !isDisabled) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	QString label = isDisabled ? "Disable Synapse" : "Enable Synapse";

	Command *command = new ChangeValueCommand(visu, selectedValues, contents, label);
	visu->getCommandExecutor()->executeCommand(command);

}


void SynapseDetailWidget::synapseFunctionSelectionChanged() {
	TRACE("SynapseDetailWidget::synapseFunctionSelectionChanged");

	if(mSynapse == 0 || mEditor == 0 || !mModificationsEnabled || mUpdatingChangedSelection) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	bool changeRequired = false;
	QString name = mSynapseFunctionSelector->currentText();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Synapse*> selectedSynapses;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		SynapseItem *item = dynamic_cast<SynapseItem*>(i.next());
		if(item != 0) {
			Synapse *synapse = item->getSynapse();
			if(synapse != 0) {
				selectedSynapses.append(synapse);
				if(synapse->getSynapseFunction()->getName() != name) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	QList<SynapseFunction*> sfs = Neuro::getNeuralNetworkManager()
										->getSynapseFunctionPrototypes();
	for(QListIterator<SynapseFunction*> i(sfs); i.hasNext();) {
		SynapseFunction *prototype = i.next();
		if(prototype->getName() == name) {

			SynapseFunction *sf = prototype->createCopy();

			ChangeSynapseFunctionCommand *command =
					new ChangeSynapseFunctionCommand(visu, selectedSynapses, sf);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}


void SynapseDetailWidget::enableStrengthObservation(int) {
	TRACE("SynapseDetailWidget::enableStrengthObservation");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mSynapse != 0) {
		mStrengthValue = &(mSynapse->getStrengthValue());
		if(mObserveStrengthField->isChecked()) {
			mStrengthValue->addValueChangedListener(this);
			emit setSynapseStrengthText(mStrengthValue->getValueAsString());
		}
		else {
			mStrengthValue->removeValueChangedListener(this);
		}
	}
}

/**
 * Marks the background color of the text label red to show that the content
 * has been changed manually.
 */
void SynapseDetailWidget::markAsValueEdited() {
	QPalette p = mSynapseStrengthField->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	mSynapseStrengthField->setPalette(p);
}


/**
 * Marks the background color of the text label white to show that the content
 * originates from the observed Value.
 */
void SynapseDetailWidget::markAsValueUpdated() {
	QPalette p = mSynapseStrengthField->palette();
	p.setColor(QPalette::Base, Qt::white);
	mSynapseStrengthField->setPalette(p);
}



void SynapseDetailWidget::updateSynapseView() {
	TRACE("SynapseDetailWidget::updateSynapseView");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mSynapse = 0;
	if(mStrengthValue != 0) {
		mStrengthValue->removeValueChangedListener(this);
	}
	mStrengthValue = 0;
	if(mEnabledValue != 0) {
		mEnabledValue->removeValueChangedListener(this);
	}
	mEnabledValue = 0;

	if(mEditor == 0) {
		return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	mUpdatingChangedSelection = true;

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	for(int i = selectedItems.size() - 1; i >= 0; --i) {
		SynapseItem *item = dynamic_cast<SynapseItem*>(selectedItems.at(i));
		if(item != 0) {
			mSynapse = item->getSynapse();
			enableStrengthObservation(0);

			if(mSynapse != 0) {
				mSynapseStrengthField->setText(
						mSynapse->getStrengthValue().getValueAsString());
			}
			mEnabledValue = &(mSynapse->getEnabledValue());
			mEnabledValue->addValueChangedListener(this);
			mEnableSynapseCheckBox->setChecked(!mEnabledValue->get());
			break;
		}
	}

	if(mSynapse == 0) {
		mSynapseStrengthField->setText("");
		setEnabled(false);
	}
	else {
		setEnabled(true);
	}

	emit markAsValueUpdated();

	udpateFunctionViews();

	mUpdatingChangedSelection = false;
}


void SynapseDetailWidget::udpateFunctionViews() {
	TRACE("SynapseDetailWidget::udpateFunctionViews");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mSynapse == 0) {
		mSynapseFunctionParameterEditor->setParameterizedObject(0);
	}
	else {
		SynapseFunction *sf = mSynapse->getSynapseFunction();

		mSynapseFunctionParameterEditor->setParameterizedObject(sf);

		mSynapseFunctionSelector->setCurrentIndex(
					mSynapseFunctionSelector->findText(sf->getName()));
	}
}


}




