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



#include "NeuronDetailWidget.h"
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
#include "Gui/NetworkEditorCommands/ChangeTransferFunctionCommand.h"
#include "Gui/NetworkEditorCommands/ChangeActivationFunctionCommand.h"
#include "Util/Tracer.h"
#include <QThread>
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include <typeinfo>
#include <qsplitter.h>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");


using namespace std;

namespace nerd {


/**
 * Constructs a new NeuronDetailWidget.
 */
NeuronDetailWidget::NeuronDetailWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false), mNeuron(0),
	  mActivationValue(0), mBiasValue(0), mOutputValue(0)
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	setLayout(layout);

	QGridLayout *neuronLayout = new QGridLayout();
	layout->addLayout(neuronLayout);

	mNeuronNameField = new QLineEdit("", this);
	neuronLayout->addWidget(new QLabel("Name:", this), 0, 0);
	neuronLayout->addWidget(mNeuronNameField, 0, 1);

	QWidget *activityArea = new QWidget(this);
	mObserveActivityField = new QCheckBox(activityArea);
	mObserveActivityField->setChecked(true);
	QGridLayout *activityFieldLayout = new QGridLayout(activityArea);
	activityFieldLayout->setContentsMargins(0, 0, 0, 0);
	activityFieldLayout->addWidget(mObserveActivityField, 0, 1);
	mActivityField = new QLineEdit("", activityArea);
	activityFieldLayout->addWidget(mActivityField, 0, 0);
	neuronLayout->addWidget(new QLabel("Activity:", this), 1, 0);
	neuronLayout->addWidget(activityArea, 1, 1);


	QWidget *outputArea = new QWidget(this);
	mObserveOutputField = new QCheckBox(outputArea);
	mObserveOutputField->setChecked(true);
	QGridLayout *outputFieldLayout = new QGridLayout(outputArea);
	outputFieldLayout->setContentsMargins(0, 0, 0, 0);
	outputFieldLayout->addWidget(mObserveOutputField, 0, 1);
	mOutputField = new QLineEdit("", outputArea);
	outputFieldLayout->addWidget(mOutputField, 0, 0);
	neuronLayout->addWidget(new QLabel("Output:", this), 2, 0);
	neuronLayout->addWidget(outputArea, 2, 1);


	QWidget *biasArea = new QWidget(this);
	mObserveBiasField= new QCheckBox(biasArea);
	mObserveBiasField->setChecked(true);
	QGridLayout *biasFieldLayout = new QGridLayout(biasArea);
	biasFieldLayout->setContentsMargins(0, 0, 0, 0);
	biasFieldLayout->addWidget(mObserveBiasField, 0, 1);
	mBiasField = new QLineEdit("", biasArea);
	biasFieldLayout->addWidget(mBiasField, 0, 0);
	neuronLayout->addWidget(new QLabel("Bias:", this), 3, 0);
	neuronLayout->addWidget(biasArea, 3, 1);
	
	QSplitter *splitter = new QSplitter(Qt::Vertical);

	QGroupBox *transferFunctionBox = new QGroupBox("TransferFunction", this);
	QVBoxLayout *tfBoxLayout = new QVBoxLayout();
	tfBoxLayout->setContentsMargins(4, 4, 4, 4);
	transferFunctionBox->setLayout(tfBoxLayout);
	
	mTransferFunctionSelector = new QComboBox(transferFunctionBox);
	tfBoxLayout->addWidget(mTransferFunctionSelector);

	mTransferFunctionParameterEditor = new ParameterEditor(transferFunctionBox, mEditor);
	tfBoxLayout->addWidget(mTransferFunctionParameterEditor);

	splitter->addWidget(transferFunctionBox);


	QGroupBox *activationFunctionBox = new QGroupBox("ActivationFunction", this);
	QVBoxLayout *afBoxLayout = new QVBoxLayout();
	afBoxLayout->setContentsMargins(4, 4, 4, 4);
	activationFunctionBox->setLayout(afBoxLayout);
	//layout->addWidget(activationFunctionBox);
	mActivationFunctionSelector = new QComboBox(activationFunctionBox);
	afBoxLayout->addWidget(mActivationFunctionSelector);

	mActivationFunctionParameterEditor = new ParameterEditor(activationFunctionBox, mEditor);
	afBoxLayout->addWidget(mActivationFunctionParameterEditor);

	splitter->addWidget(activationFunctionBox);
	
	layout->addWidget(splitter);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
	connect(mObserveActivityField, SIGNAL(stateChanged(int)),
			this, SLOT(enableActivityObservation(int)));
	connect(mObserveBiasField, SIGNAL(stateChanged(int)),
			this, SLOT(enableBiasObservation(int)));
	connect(mObserveOutputField, SIGNAL(stateChanged(int)),
			this, SLOT(enableOutputObservation(int)));
	connect(mTransferFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(transferFunctionSelectionChanged()));
	connect(mActivationFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(activationFunctionSelectionChanged()));
	connect(mNeuronNameField, SIGNAL(returnPressed()),
			this, SLOT(changeNeuronName()));
    connect(mNeuronNameField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markNameFieldAsEdited()));
	connect(mBiasField, SIGNAL(returnPressed()),
			this, SLOT(changeBias()));
    connect(mBiasField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markBiasFieldAsEdited()));
	connect(mActivityField, SIGNAL(returnPressed()),
			this, SLOT(changeActivity()));
    connect(mActivityField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markActivationFieldAsEdited()));
	connect(this, SIGNAL(setActivationText(const QString&)),
			mActivityField, SLOT(setText(const QString&)));
	connect(this, SIGNAL(setBiasText(const QString&)),
			mBiasField, SLOT(setText(const QString&)));
	connect(mOutputField, SIGNAL(returnPressed()),
			this, SLOT(changeOutput()));
    connect(mOutputField, SIGNAL(textEdited(const QString&)),
			this, SLOT(markOutputFieldAsEdited()));
	connect(this, SIGNAL(setOutputText(const QString&)),
			mOutputField, SLOT(setText(const QString&)));

	resize(100, 200);
	setMinimumWidth(250);
}


/**
 * Destructor.
 */
NeuronDetailWidget::~NeuronDetailWidget() {
	TRACE("NeuronDetailWidget::~NeuronDetailWidget");
	if(mNeuron != 0) {
		mNeuron->getBiasValue().removeValueChangedListener(this);
		mNeuron->getActivationValue().removeValueChangedListener(this);
	}
	if(mEditor != 0) {
		disconnect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
}

void NeuronDetailWidget::selectionChanged(QList<PaintItem*>) {
	TRACE("NeuronDetailWidget::selectionChanged");

	updateNeuronView();
}


void NeuronDetailWidget::valueChanged(Value *value) {
	TRACE("NeuronDetailWidget::valueChanged");

	if(value == 0) {
		return;
	}
	if(value == mOutputValue) {
		emit setOutputText(mOutputValue->getValueAsString());
		emit markAsValueUpdated(mOutputField);
	}
	else if(value == mActivationValue) {
		emit setActivationText(mActivationValue->getValueAsString());
		emit markAsValueUpdated(mActivityField);
	}
	else if(value == mBiasValue) {
		emit setBiasText(mBiasValue->getValueAsString());
		emit markAsValueUpdated(mBiasField);
	}
}


QString NeuronDetailWidget::getName() const {
	return "NeuronDetailWidget";
}


void NeuronDetailWidget::modificationPolicyChanged() {
	mNeuronNameField->setReadOnly(!mModificationsEnabled);
	mActivityField->setReadOnly(!mModificationsEnabled);
	mObserveActivityField->setEnabled(mModificationsEnabled);
	mOutputField->setReadOnly(!mModificationsEnabled);
	mObserveOutputField->setEnabled(mModificationsEnabled);
	mBiasField->setReadOnly(!mModificationsEnabled);
	mObserveBiasField->setEnabled(mModificationsEnabled);
	mTransferFunctionSelector->setEnabled(mModificationsEnabled);
	mActivationFunctionSelector->setEnabled(mModificationsEnabled);
	mTransferFunctionParameterEditor->setEnabled(mModificationsEnabled);
	mActivationFunctionParameterEditor->setEnabled(mModificationsEnabled);
}

void NeuronDetailWidget::invalidateView() {

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	EditorToolWidget::invalidateView();
	mTransferFunctionParameterEditor->invalidateListeners();
	mActivationFunctionParameterEditor->invalidateListeners();

	mNeuron = 0;
	if(mOutputValue != 0) {
		mOutputValue->removeValueChangedListener(this);
		mOutputValue = 0;
	}
	if(mActivationValue != 0) {
		mActivationValue->removeValueChangedListener(this);
		mActivationValue = 0;
	}
	if(mBiasValue != 0) {
		mBiasValue->removeValueChangedListener(this);
		mBiasValue = 0;
	}
	mSlaveNeurons.clear();
}


void NeuronDetailWidget::mouseButtonPressed(NetworkVisualization*, QMouseEvent*, const QPointF&)
{
}


void NeuronDetailWidget::mouseButtonReleased(NetworkVisualization *, QMouseEvent*, const QPointF&)
{
}

void NeuronDetailWidget::mouseDoubleClicked(NetworkVisualization *source,
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
	NeuronItem *clickedItem = 0;

	QList<PaintItem*> items = source->getPaintItems();
	QListIterator<PaintItem*> i(items);
	i.toBack();
	for(; i.hasPrevious();) {
		PaintItem *item = i.previous();
		if(!item->isHit(globalPosition, event->buttons(), source->getScaling())) {
			continue;
		}
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			clickedItem = neuronItem;
			break;
		}
		//if another item was hit, then do nothing.
		return;
	}
	if(clickedItem == 0) {
		return;
	}

	mEditor->bringToolWidgetToFront(this);
	mBiasField->setFocus();
}

void NeuronDetailWidget::mouseDragged(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}

void NeuronDetailWidget::currentEditorTabChanged(int) {
	TRACE("NeuronDetailWidget::currentEditorTabChanged");

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
	updateNeuronView();
}


void NeuronDetailWidget::networkChanged(ModularNeuralNetwork *) {
	TRACE("NeuronDetailWidget::networkChanged");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mNetworkInvalid = false;

	if(!mInitialized) {
		mInitialized = true;
		//fill in prototype names
		QList<TransferFunction*> tfs =
 					Neuro::getNeuralNetworkManager()->getTransferFunctionPrototypes();
		for(QListIterator<TransferFunction*> i(tfs); i.hasNext();) {
			mTransferFunctionSelector->addItem(i.next()->getName());
		}
		QList<ActivationFunction*> afs =
					Neuro::getNeuralNetworkManager()->getActivationFunctionPrototypes();
		for(QListIterator<ActivationFunction*> i(afs); i.hasNext();) {
			mActivationFunctionSelector->addItem(i.next()->getName());
		}
	}
	updateNeuronView();
}


void NeuronDetailWidget::networkModified(ModularNeuralNetwork*) {
	TRACE("NeuronDetailWidget::networkModified");

	updateNeuronView();
}


void NeuronDetailWidget::changeNeuronName() {
	TRACE("NeuronDetailWidget::changeNeuronName");

	emit markAsValueUpdated(mNeuronNameField);

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
			return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0 || mNeuron == 0) {
		return;
	}

	QString content = mNeuronNameField->text().trimmed();

	//changes only the currently selected neuron name
	if(content == mNeuron->getNameValue().get()) {
		return;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Value*> selectedValues;
	QList<QString> contents;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				selectedValues.append(&(neuron->getNameValue()));
				contents.append(content);
			}
		}
	}

	ChangeValueCommand *command = new ChangeValueCommand(visu, selectedValues, contents,
														 "Change Neuron Name");

	visu->getCommandExecutor()->executeCommand(command);
}

void NeuronDetailWidget::changeBias() {
	TRACE("NeuronDetailWidget::changeBias");

	emit markAsValueUpdated(mBiasField);

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0 || mNeuron == 0) {
		return;
	}
	bool changeRequired = false;
	QString content = mBiasField->text().trimmed();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Value*> selectedBiases;
	QList<QString> contents;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				selectedBiases.append(&(neuron->getBiasValue()));
				contents.append(content);
				if(content != neuron->getBiasValue().getValueAsString()) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	ChangeValueCommand *command = new ChangeValueCommand(visu, selectedBiases, contents,
														 "Change Bias");

	visu->getCommandExecutor()->executeCommand(command);
}


void NeuronDetailWidget::changeActivity() {
	TRACE("NeuronDetailWidget::changeActivity");

    emit markAsValueUpdated(mActivityField);

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
			return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0 || mNeuron == 0) {
		return;
	}

	QString content = mActivityField->text().trimmed();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				neuron->getActivationValue().setValueFromString(content);
				neuron->prepare();
			}
		}
	}
}

void NeuronDetailWidget::changeOutput() {
	TRACE("NeuronDetailWidget::changeOutput");

    emit markAsValueUpdated(mOutputField);

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
			return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0 || mNeuron == 0) {
		return;
	}
	QString content = mOutputField->text().trimmed();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<QString> contents;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				neuron->getOutputActivationValue().setValueFromString(content);
				neuron->prepare();
			}
		}
	}
}


void NeuronDetailWidget::enableActivityObservation(int) {
	TRACE("NeuronDetailWidget::enableActivityObservation");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mNeuron != 0 || mNetworkInvalid) {
		mActivationValue = &(mNeuron->getActivationValue());
		if(mActivationValue != 0) {
            if(mObserveActivityField->isChecked()) {
                mActivationValue->addValueChangedListener(this);
                emit setActivationText(mActivationValue->getValueAsString());
            }
            else {
                mActivationValue->removeValueChangedListener(this);
            }
		}
	}
}


void NeuronDetailWidget::enableBiasObservation(int) {
	TRACE("NeuronDetailWidget::enableBiasObservation");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mNeuron != 0 || mNetworkInvalid) {
		mBiasValue = &(mNeuron->getBiasValue());
		if(mBiasValue != 0) {
            if(mObserveBiasField->isChecked()) {
                mBiasValue->addValueChangedListener(this);
                emit setBiasText(mBiasValue->getValueAsString());
            }
            else {
                mBiasValue->removeValueChangedListener(this);
            }
		}
	}
}

void NeuronDetailWidget::enableOutputObservation(int) {
	TRACE("NeuronDetailWidget::enableOutputObservation");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mNeuron != 0 || mNetworkInvalid) {
		mOutputValue = &(mNeuron->getOutputActivationValue());
		if(mOutputValue != 0) {
            if(mObserveOutputField->isChecked()) {
                mOutputValue->addValueChangedListener(this);
                emit setOutputText(mOutputValue->getValueAsString());
            }
            else {
                mOutputValue->removeValueChangedListener(this);
            }
		}
	}
}

void NeuronDetailWidget::transferFunctionSelectionChanged() {
	TRACE("NeuronDetailWidget::transferFunctionSelectionChanged");

	if(mNeuron == 0 || mEditor == 0 || mUpdatingChangedSelection || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	bool changeRequired = false;
	QString name = mTransferFunctionSelector->currentText();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Neuron*> selectedNeurons;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				selectedNeurons.append(neuron);
				if(neuron->getTransferFunction()->getName() != name) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	QList<TransferFunction*> tfs = Neuro::getNeuralNetworkManager()
										->getTransferFunctionPrototypes();

	for(QListIterator<TransferFunction*> i(tfs); i.hasNext();) {
		TransferFunction *prototype = i.next();
		if(prototype->getName() == name) {

			TransferFunction *tf = prototype->createCopy();

			ChangeTransferFunctionCommand *command =
					new ChangeTransferFunctionCommand(visu, selectedNeurons, tf);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}

void NeuronDetailWidget::activationFunctionSelectionChanged() {
	TRACE("NeuronDetailWidget::activationFunctionSelectionChanged");

	if(mNeuron == 0 || mEditor == 0 || mUpdatingChangedSelection || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	bool changeRequired = false;
	QString name = mActivationFunctionSelector->currentText();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Neuron*> selectedNeurons;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
		if(item != 0) {
			Neuron *neuron = item->getNeuron();
			if(neuron != 0) {
				selectedNeurons.append(neuron);
				if(neuron->getActivationFunction()->getName() != name) {
					changeRequired = true;
				}
			}
		}
	}

	if(!changeRequired) {
		return;
	}

	QList<ActivationFunction*> afs = Neuro::getNeuralNetworkManager()
										->getActivationFunctionPrototypes();
	for(QListIterator<ActivationFunction*> i(afs); i.hasNext();) {
		ActivationFunction *prototype = i.next();
		if(prototype->getName() == name) {

			ActivationFunction *af = prototype->createCopy();

			ChangeActivationFunctionCommand *command =
					new ChangeActivationFunctionCommand(visu, selectedNeurons, af);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}


void NeuronDetailWidget::markNameFieldAsEdited() {
   markAsValueEdited(mNeuronNameField);
}

void NeuronDetailWidget::markBiasFieldAsEdited() {
    markAsValueEdited(mBiasField);
}

void NeuronDetailWidget::markActivationFieldAsEdited() {
    markAsValueEdited(mActivityField);
}

void NeuronDetailWidget::markOutputFieldAsEdited() {
    markAsValueEdited(mOutputField);
}


void NeuronDetailWidget::markAsValueEdited(QLineEdit *edit) {
    if(edit == 0) {
        return;
    }
    QPalette p = edit->palette();
	p.setColor(QPalette::Base, QColor(255,120,120));
	edit->setPalette(p);
}


void NeuronDetailWidget::markAsValueUpdated(QLineEdit *edit) {
    if(edit == 0) {
        return;
    }
    QPalette p = edit->palette();
	p.setColor(QPalette::Base, Qt::white);
	edit->setPalette(p);
}


void NeuronDetailWidget::updateNeuronView() {
	TRACE("NeuronDetailWidget::updateNeuronView");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mNeuron = 0;
	if(mActivationValue != 0) {
		mActivationValue->removeValueChangedListener(this);
	}
	mActivationValue = 0;
	if(mBiasValue != 0) {
		mBiasValue->removeValueChangedListener(this);
	}
	mBiasValue = 0;
	if(mOutputValue != 0) {
		mOutputValue->removeValueChangedListener(this);
	}
	mOutputValue = 0;
	mSlaveNeurons.clear();

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
		NeuronItem*item = dynamic_cast<NeuronItem*>(selectedItems.at(i));
		
		if(item != 0 && item->getNeuron() != 0) {
			if(mNeuron == 0) {
				//first neuron is the primary neuron
				mNeuron = item->getNeuron();
				enableActivityObservation(0);
				enableBiasObservation(0);
				enableOutputObservation(0);
				if(mNeuron != 0) {
					mNeuronNameField->setText(mNeuron->getNameValue().get());
				}
			}
			else {
				//all others are treated as slaves
				Neuron *neuron = item->getNeuron();
				if(!mSlaveNeurons.contains(neuron)) {
					mSlaveNeurons.append(neuron);
				}
			}
		} 
	}

	if(mNeuron == 0) {
		mNeuronNameField->setText("");
		setEnabled(false);
	}
	else {
		setEnabled(true);
	}

	emit markAsValueUpdated(mNeuronNameField);
	emit markAsValueUpdated(mActivityField);
	emit markAsValueUpdated(mOutputField);
	emit markAsValueUpdated(mBiasField);

	udpateFunctionViews();

	mUpdatingChangedSelection = false;
}


void NeuronDetailWidget::udpateFunctionViews() {
	TRACE("NeuronDetailWidget::udpateFunctionViews");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mNeuron == 0) {
		mTransferFunctionParameterEditor->setParameterizedObject(0);
		mActivationFunctionParameterEditor->setParameterizedObject(0);
	}
	else {
		TransferFunction *tf = mNeuron->getTransferFunction();
		ActivationFunction *af = mNeuron->getActivationFunction();
		
		
		QList<ParameterizedObject*> slaveTransferFunctions;
		QList<ParameterizedObject*> slaveActivationFunctions;
		for(QListIterator<Neuron*> i(mSlaveNeurons); i.hasNext();) {
			Neuron *neuron = i.next();
			TransferFunction *neuronTF = neuron->getTransferFunction();
			ActivationFunction *neuronAF = neuron->getActivationFunction();
			
			if(typeid(*neuronTF) == typeid(*tf)) {
				slaveTransferFunctions.append(neuronTF);
			}
			if(typeid(*neuronAF) == typeid(*af)) {
				slaveActivationFunctions.append(neuronAF);
			}
		}

		mTransferFunctionParameterEditor->setParameterizedObject(tf, slaveTransferFunctions);

		mTransferFunctionSelector->setCurrentIndex(
					mTransferFunctionSelector->findText(tf->getName()));

		

		mActivationFunctionParameterEditor->setParameterizedObject(af, slaveActivationFunctions);
		mActivationFunctionSelector->setCurrentIndex(
					mActivationFunctionSelector->findText(af->getName()));
	}
}

}




