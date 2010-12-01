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



#include "ModuleManagementWidget.h"
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
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditorCommands/ChangeValueCommand.h"
#include <QGridLayout>
#include <QGroupBox>
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Math/Math.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "ModularNeuralNetwork/NeuroModuleManager.h"
#include <QListWidgetItem>
#include "Util/InsertSubnetworkIntoNetwork.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "Gui/NetworkEditorCommands/InsertSubnetworkCommand.h"
#include "NeuralNetworkConstants.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");

using namespace std;

namespace nerd {


/**
 * Constructs a new ModuleManagementWidget.
 */
ModuleManagementWidget::ModuleManagementWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mModuleList(0), mModuleListScrollArea(0), mModuleNameField(0), 
	  mAddButton(0), mCreateButton(0), mLoadButton(0),
	  mInitialized(false)
{

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	setLayout(layout);

	QLayout *listLayout = new QHBoxLayout();
	layout->addLayout(listLayout);

	QLayout *nameLayout = new QHBoxLayout();
	layout->addLayout(nameLayout);

	QLayout *buttonLayout = new QHBoxLayout();
	layout->addLayout(buttonLayout);

	mModuleList = new QListWidget(this);
	mModuleList->setSelectionMode(QAbstractItemView::SingleSelection);
	mModuleListScrollArea = new QScrollArea(this);
	mModuleListScrollArea->setWidget(mModuleList);
	mModuleListScrollArea->setWidgetResizable(true);
	listLayout->addWidget(mModuleListScrollArea);

	mModuleNameField = new QLineEdit("", this);
	nameLayout->addWidget(mModuleNameField);

	mAddButton = new QPushButton("Add", this);
	mAddButton->setContentsMargins(2, 2, 2, 2);
	mAddButton->setMinimumWidth(15);
	mAddButton->setCheckable(true);
	buttonLayout->addWidget(mAddButton);
	mCreateButton = new QPushButton("Create", this);
	mCreateButton->setContentsMargins(2, 2, 2, 2);
	mCreateButton->setMinimumWidth(15);
	buttonLayout->addWidget(mCreateButton);
	mLoadButton = new QPushButton("Reload", this);
	mLoadButton->setContentsMargins(2, 2, 2, 2);
	mLoadButton->setMinimumWidth(15);
	buttonLayout->addWidget(mLoadButton);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
	connect(mCreateButton, SIGNAL(pressed()),
			this, SLOT(createModule()));
	connect(mLoadButton, SIGNAL(pressed()),
			this, SLOT(reloadModules()));

	updateView();
}



/**
 * Destructor.
 */
ModuleManagementWidget::~ModuleManagementWidget() {
}


void ModuleManagementWidget::modificationPolicyChanged() {
	mAddButton->setEnabled(mModificationsEnabled);
	mCreateButton->setEnabled(mModificationsEnabled);
	mLoadButton->setEnabled(mModificationsEnabled);
}


void ModuleManagementWidget::invalidateView() {
	if(!mInitialized) {
		mInitialized = true;
		updateView();
	}
}


void ModuleManagementWidget::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	if(source == 0 || source->getVisualizationHander() == 0 || event == 0) {
		return;
	}
	if(mAddButton->isChecked() && (event->buttons() & Qt::RightButton)) {
		mAddButton->setChecked(false);

		QListWidgetItem *item = mModuleList->currentItem();
		if(item == 0) {
			return;
		}
		QString name = item->text();

		NeuroModulePrototype *prototype = NeuroModuleManager::getInstance()->getNeuroModuleByName(name);
	
		if(prototype == 0 || prototype->mNetwork == 0) {
			return;
		}

		InsertSubnetworkCommand *command = new InsertSubnetworkCommand(
					source, prototype->mNetwork, globalPosition, true);

		source->getCommandExecutor()->executeCommand(command);

	}
}


void ModuleManagementWidget::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{

}

void ModuleManagementWidget::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{
}


void ModuleManagementWidget::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{

}


void ModuleManagementWidget::currentEditorTabChanged(int) {
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->addMouseListener(this);
	}
}


void ModuleManagementWidget::createModule() {

	NeuroModuleManager *nmm = NeuroModuleManager::getInstance();

	//prevent modules with existing names (module names must be unique).
	QString name = mModuleNameField->text();
	if(name == "") {
		return;
	}

	QList<NeuroModulePrototype*> prototypes = nmm->getNeuroModulePrototpyes();

	for(QListIterator<NeuroModulePrototype*> i(prototypes); i.hasNext();) {
		NeuroModulePrototype *p = i.next();
		if(p->mName == name) {
			return;
		}
	}



	ModularNeuralNetwork *network = 0;
	NetworkVisualization *visu = 0;
	NetworkVisualizationHandler *handler = 0;

	if(mEditor != 0) {
		visu = mEditor->getCurrentNetworkVisualization();
		if(visu != 0) {
			handler = visu->getVisualizationHander();
			if(handler != 0) {
				network = handler->getNeuralNetwork();
			}
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	{
		QMutexLocker guard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
		QMutexLocker selectionGuard(visu->getSelectionMutex());
	
		QList<PaintItem*> selectedItems = visu->getSelectedItems();
		ModularNeuralNetwork *net = new ModularNeuralNetwork(AdditiveTimeDiscreteActivationFunction(),
											TransferFunctionTanh(), SimpleSynapseFunction());
	
		for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
			PaintItem *item = i.next();
			NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
			if(neuron != 0) {
				net->addNeuron(neuron->getNeuron());
				continue;
			}
			ModuleItem *module = dynamic_cast<ModuleItem*>(item);
			if(module != 0) {
				net->addNeuronGroup(module->getNeuroModule());
				continue;
			}
			GroupItem *group = dynamic_cast<GroupItem*>(item);
			if(group != 0) {
				net->addNeuronGroup(group->getNeuronGroup());
				continue;
			}
		}

		ModularNeuralNetwork *netCopy = dynamic_cast<ModularNeuralNetwork*>(net->createCopy());

		netCopy->validateSynapseConnections();
		
		//make sure that the subnetwork does not contain input or output neurons.
		QList<Neuron*> neurons = netCopy->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			neuron->removeProperty(Neuron::NEURON_TYPE_INPUT);
			neuron->removeProperty(Neuron::NEURON_TYPE_OUTPUT);
		}

		netCopy->setProperty(NeuralNetworkConstants::TAG_PROTOTYPE_TYPE_LIST);

		nmm->addNeuroModulePrototype(name, netCopy);
		net->freeElements(false);
		delete net;
		storeModules();
	}

	reloadModules();
}



void ModuleManagementWidget::reloadModules() {
	NeuroModuleManager *nmm = NeuroModuleManager::getInstance();
	nmm->loadNeuroModulePrototypes(nmm->getOptionalModuleDirectoryValue()->get());
	updateView();
}


void ModuleManagementWidget::storeModules() {
	NeuroModuleManager *nmm = NeuroModuleManager::getInstance();
	nmm->saveNeuroModulePrototypes(nmm->getOptionalModuleDirectoryValue()->get());
}


void ModuleManagementWidget::checkBoxChanged() {
	
}

void ModuleManagementWidget::updateView() {
	mModuleList->clear();

	NeuroModuleManager::getInstance();

	QList<NeuroModulePrototype*> prototypes = NeuroModuleManager::getInstance()->getNeuroModulePrototpyes();

	for(QListIterator<NeuroModulePrototype*> i(prototypes); i.hasNext();) {
		NeuroModulePrototype *p = i.next();
		QListWidgetItem *item = new QListWidgetItem(p->mName, mModuleList);
		item->setCheckState(p->mUseDuringEvolution ? Qt::Checked : Qt::Unchecked);
		mModuleList->addItem(item);
	}
	mAddButton->setChecked(false);
}


}



