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



#include "NetworkDetailWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include <QGridLayout>
#include <QGroupBox>
#include "Network/Neuro.h"
#include "Gui/NetworkEditorCommands/ChangeNetworkFunctionCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");
//#define pTRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");


using namespace std;

namespace nerd {


/**
 * Constructs a new NetworkDetailWidget.
 */
NetworkDetailWidget::NetworkDetailWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false), mNetwork(0)
{
	TRACE("NetworkDetailWidget::NetworkDetailWidget");

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	setLayout(layout);


	QGroupBox *transferFunctionBox = new QGroupBox("DefaultTransferFunction", this);
	QVBoxLayout *tfBoxLayout = new QVBoxLayout();
	tfBoxLayout->setContentsMargins(4, 4, 4, 4);
	transferFunctionBox->setLayout(tfBoxLayout);
	layout->addWidget(transferFunctionBox);
	mTransferFunctionSelector = new QComboBox(transferFunctionBox);
	tfBoxLayout->addWidget(mTransferFunctionSelector);

	mTransferFunctionParameterEditor = new ParameterEditor(transferFunctionBox, mEditor);
	tfBoxLayout->addWidget(mTransferFunctionParameterEditor);



	QGroupBox *activationFunctionBox = new QGroupBox("DefaultActivationFunction", this);
	QVBoxLayout *afBoxLayout = new QVBoxLayout();
	afBoxLayout->setContentsMargins(4, 4, 4, 4);
	activationFunctionBox->setLayout(afBoxLayout);
	layout->addWidget(activationFunctionBox);
	mActivationFunctionSelector = new QComboBox(activationFunctionBox);
	afBoxLayout->addWidget(mActivationFunctionSelector);

	mActivationFunctionParameterEditor = new ParameterEditor(activationFunctionBox, mEditor);
	afBoxLayout->addWidget(mActivationFunctionParameterEditor);

	
	QGroupBox *synapseFunctionBox = new QGroupBox("DefaultSynapseFunction", this);
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
	connect(mTransferFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(defaultTransferFunctionSelectionChanged()));
	connect(mActivationFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(defaultActivationFunctionSelectionChanged()));
	connect(mSynapseFunctionSelector, SIGNAL(currentIndexChanged(int)),
			this, SLOT(defaultSynapseFunctionSelectionChanged()));
}

/**
 * Destructor.
 */
NetworkDetailWidget::~NetworkDetailWidget() {
	TRACE("NetworkDetailWidget::NetworkDetailWidget");
}


void NetworkDetailWidget::modificationPolicyChanged() {
	TRACE("NetworkDetailWidget::modificationPolicyChanged");

	mTransferFunctionParameterEditor->setEnabled(mModificationsEnabled);
	mTransferFunctionSelector->setEnabled(mModificationsEnabled);
	mActivationFunctionParameterEditor->setEnabled(mModificationsEnabled);
	mActivationFunctionSelector->setEnabled(mModificationsEnabled);
	mSynapseFunctionParameterEditor->setEnabled(mModificationsEnabled);
	mSynapseFunctionSelector->setEnabled(mModificationsEnabled);
}


void NetworkDetailWidget::invalidateView() {
	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	EditorToolWidget::invalidateView();
	mNetwork = 0;
	mTransferFunctionParameterEditor->invalidateListeners();
}


void NetworkDetailWidget::currentEditorTabChanged(int) {
	TRACE("NetworkDetailWidget::currentEditorTabChanged");

	if(mEditor == 0) {
			return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(networkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(networkModified(ModularNeuralNetwork*)));
	}
}


void NetworkDetailWidget::networkChanged(ModularNeuralNetwork*) {
	TRACE("NetworkDetailWidget::networkChanged");

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
		QList<SynapseFunction*> sfs =
					Neuro::getNeuralNetworkManager()->getSynapseFunctionPrototypes();
		for(QListIterator<SynapseFunction*> i(sfs); i.hasNext();) {
			mSynapseFunctionSelector->addItem(i.next()->getName());
		}
	}
	updateView();
}


void NetworkDetailWidget::networkModified(ModularNeuralNetwork*) {
	TRACE("NetworkDetailWidget::networkModified");

	updateView();
}


void NetworkDetailWidget::defaultTransferFunctionSelectionChanged() {
	TRACE("NetworkDetailWidget::defaultTransferFunctionSelectionChanged");

	if(mEditor == 0 || mNetwork == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QString name = mTransferFunctionSelector->currentText();
	if(mNetwork->getDefaultTransferFunction()->getName() == name) {
		return;
	}

	QList<TransferFunction*> tfs = Neuro::getNeuralNetworkManager()
										->getTransferFunctionPrototypes();

	for(QListIterator<TransferFunction*> i(tfs); i.hasNext();) {
		TransferFunction *prototype = i.next();
		if(prototype->getName() == name) {
			
			TransferFunction *tf = prototype->createCopy();

			ChangeNetworkFunctionCommand *command = 
					new ChangeNetworkFunctionCommand(visu, mNetwork, tf);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}


void NetworkDetailWidget::defaultActivationFunctionSelectionChanged() {
	TRACE("NetworkDetailWidget::defaultActivationFunctionSelectionChanged");

	if(mEditor == 0 || mNetwork == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QString name = mActivationFunctionSelector->currentText();
	if(mNetwork->getDefaultActivationFunction()->getName() == name) {
		return;
	}

	QList<ActivationFunction*> afs = Neuro::getNeuralNetworkManager()
										->getActivationFunctionPrototypes();

	for(QListIterator<ActivationFunction*> i(afs); i.hasNext();) {
		ActivationFunction *prototype = i.next();
		if(prototype->getName() == name) {
			
			ActivationFunction *af = prototype->createCopy();

			ChangeNetworkFunctionCommand *command = 
					new ChangeNetworkFunctionCommand(visu, mNetwork, af);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}


void NetworkDetailWidget::defaultSynapseFunctionSelectionChanged() {	
	TRACE("NetworkDetailWidget::defaultSynapseFunctionSelectionChanged");

	if(mEditor == 0 || mNetwork == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QString name = mSynapseFunctionSelector->currentText();
	if(mNetwork->getDefaultSynapseFunction()->getName() == name) {
		return;
	}

	QList<SynapseFunction*> sfs = Neuro::getNeuralNetworkManager()
										->getSynapseFunctionPrototypes();

	for(QListIterator<SynapseFunction*> i(sfs); i.hasNext();) {
		SynapseFunction *prototype = i.next();
		if(prototype->getName() == name) {
			
			SynapseFunction *sf = prototype->createCopy();

			ChangeNetworkFunctionCommand *command = 
					new ChangeNetworkFunctionCommand(visu, mNetwork, sf);
			visu->getCommandExecutor()->executeCommand(command);
		}
	}
}



void NetworkDetailWidget::updateView() {
	TRACE("NetworkDetailWidget::updateView");
	
	if(mEditor == 0) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	mNetwork = visu->getNeuralNetwork();

	if(mNetwork == 0) {
		setEnabled(false);
	}
	else {
		setEnabled(true);
	}

	udpateFunctionViews();
}


void NetworkDetailWidget::udpateFunctionViews() {
	TRACE("NetworkDetailWidget::udpateFunctionViews");

	if(mEditor == 0 || mNetwork == 0 || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mTransferFunctionParameterEditor->setParameterizedObject(0);
	mActivationFunctionParameterEditor->setParameterizedObject(0);
	mSynapseFunctionParameterEditor->setParameterizedObject(0);

	if(mNetwork != 0 && !mNetworkInvalid) {
		TransferFunction *tf = mNetwork->getDefaultTransferFunction();
		ActivationFunction *af = mNetwork->getDefaultActivationFunction();
		SynapseFunction *sf = mNetwork->getDefaultSynapseFunction();

		mTransferFunctionParameterEditor->setParameterizedObject(tf);
		mTransferFunctionSelector->setCurrentIndex(
					mTransferFunctionSelector->findText(tf->getName()));
		mTransferFunctionParameterEditor->setParameterizedObject(tf);

		
		mActivationFunctionParameterEditor->setParameterizedObject(af);
		mActivationFunctionSelector->setCurrentIndex(
					mActivationFunctionSelector->findText(af->getName()));
		mActivationFunctionParameterEditor->setParameterizedObject(af);


		mSynapseFunctionParameterEditor->setParameterizedObject(sf);
		mSynapseFunctionSelector->setCurrentIndex(
					mSynapseFunctionSelector->findText(sf->getName()));
		mSynapseFunctionParameterEditor->setParameterizedObject(sf);
	}
}




}




