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


#include "NeuronActivityPlotter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Event/EventManager.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Util/Tracer.h"
#include "NerdConstants.h"
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QCheckBox>
#include "Gui/ValuePlotter/PlotterItemControlPanel.h"
#include <QVector>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QComboBox>
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "Gui/NetworkEditor/SynapseItem.h"
#include "ActivationFunction/ActivationFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "Network/ObservableNetworkElement.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuronActivityPlotter.
 */
NeuronActivityPlotter::NeuronActivityPlotter(const QString &name, int plotMode, NeuralNetworkEditor *editor,
											 int activeTab, QWidget *parent)
	: ValuePlotterWidget(name, activeTab, parent), mEditor(editor), mPlotMode(plotMode),
	  mShutDownEvent(0), mNetworksReplacedEvent(0), mNetworksModifiedEvent(0)
{
	setAttribute(Qt::WA_DeleteOnClose, true);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentTabChanged(int)));
	}

	EventManager *em = Core::getInstance()->getEventManager();
	
	//register for network events
	mNetworksReplacedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	mNetworksModifiedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);
	mShutDownEvent = em->registerForEvent(
				NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);


	currentTabChanged(0);


	//extend control area

	//create general tab
	QWidget *neuronTab = new QWidget(mControlArea);
	QVBoxLayout *neuronLayout = new QVBoxLayout(neuronTab);
	neuronLayout->setMargin(2);
	neuronTab->setLayout(neuronLayout);

	mAddSelectedButton = new QPushButton("Add Selected", neuronTab);
	neuronLayout->addWidget(mAddSelectedButton);

	mRemoveNeuronsButton = new QPushButton("Remove All", neuronTab);
	neuronLayout->addWidget(mRemoveNeuronsButton);

	neuronLayout->addStretch();

	mControlArea->insertTab(0, neuronTab, "Selection");
	mControlArea->setCurrentIndex(activeTab);
	

	connect(mAddSelectedButton, SIGNAL(pressed()),
			this, SLOT(addSelectedNeuronsButtonPressed()));
	connect(mRemoveNeuronsButton, SIGNAL(pressed()),
			this, SLOT(removeNeuronsButtonPressed()));
	
	if((mPlotMode == NeuronActivityPlotter::PLOT_PARAMETERS
			|| mPlotMode == NeuronActivityPlotter::PLOT_SYNAPSE_WEIGHTS)
		&& getValuePlotter() != 0 
		&& getValuePlotter()->getPlotterWidget() != 0)
	{
		getValuePlotter()->getPlotterWidget()->showValuesInLegend(true);
	}
	
}



/**
 * Destructor.
 */
NeuronActivityPlotter::~NeuronActivityPlotter() {
	TRACE("NeuronActivityPlotter::~NeuronActivityPlotter()");

	QMutexLocker guard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mValuePlotter != 0) {
		mValuePlotter->invalidateEvents();
	}

	removeAllNetworkElements();

	if(mEditor != 0) {
		disconnect(mEditor, SIGNAL(tabSelectionChanged(int)),
				   this, SLOT(currentTabChanged(int)));

		QList<NetworkVisualization*> visus = mEditor->getNetworkVisualizations();
		for(QListIterator<NetworkVisualization*> i(visus); i.hasNext();) {
			NetworkVisualization *visu = i.next();
			
			disconnect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
					   this, SLOT(neuralNetworkChanged(ModularNeuralNetwork*)));
			disconnect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
					   this, SLOT(neuralNetworkModified(ModularNeuralNetwork*)));
		}
	}

	if(mNetworksModifiedEvent != 0) {
		mNetworksModifiedEvent->removeEventListener(this);
	}
	if(mNetworksReplacedEvent != 0) {
		mNetworksReplacedEvent->removeEventListener(this);
	}	
	if(mShutDownEvent != 0) {
		mShutDownEvent->removeEventListener(this);
	}

}


void NeuronActivityPlotter::addPlottedNetworkElement(NeuralNetworkElement *element) {
	TRACE("NeuronActivityPlotter::addPlottedNeuron()");

	if(mEditor == 0 || element == 0 || mNetworksReplacedEvent == 0 
		|| mNetworksModifiedEvent == 0 || mShutDownEvent == 0) 
	{
		return;
	}

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		removeAllNetworkElements();
		return;
	}

	NeuralNetwork *net = visu->getNeuralNetwork();
	if(net == 0) {
		removeAllNetworkElements();
		return;
	}
	
	QList<NeuralNetworkElement*> availableElements;
	net->getNetworkElements(availableElements);
	if(!availableElements.contains(element)) {
		return;
	}
	
	if(mPlottedNetworkElements.contains(element)) {
		return;
	}
	
	if(mPlotMode == NeuronActivityPlotter::PLOT_PARAMETERS) {
		Neuron *neuron = dynamic_cast<Neuron*>(element);
		Synapse *synapse = dynamic_cast<Synapse*>(element);
		QString namePostfix = "";
		
		ObservableNetworkElement *one = 0;
		if(neuron != 0) {
			ActivationFunction *activationFunction = neuron->getActivationFunction();
			one = activationFunction;
			namePostfix = QString(" (").append(neuron->getNameValue().get()).append(",AF)");
		}
		else if(synapse != 0) {
			SynapseFunction *synapseFunction = synapse->getSynapseFunction();
			one = synapseFunction;
			namePostfix = QString(" (").append(QString::number(synapse->getId())).append(",SF)");
		}
		if(one == 0) {
			return;
		}
		QList<Value*> paramValues;
		QList<QString> paramNames = one->getObservableOutputNames();
		for(QListIterator<QString> i(paramNames); i.hasNext();) {
			QString name = i.next();
			Value *value = one->getObservableOutput(name);
			if(value != 0) {
				paramValues.append(value);
				getValuePlotter()->addValue(name + namePostfix, value);
			}
		}
		if(!paramValues.empty()) {
			mPlottedParameters.insert(element, paramValues);
			mPlottedNetworkElements.append(element);
		}
	}
	else {

		DoubleValue *valueToPlot = 0;
		QString name = "";
		if(mPlotMode == NeuronActivityPlotter::PLOT_NEURON_ACTIVATION ||
			mPlotMode == NeuronActivityPlotter::PLOT_NEURON_OUTPUT)
		{
			Neuron *neuron = dynamic_cast<Neuron*>(element);
			if(neuron != 0) {
				if(mPlotMode == NeuronActivityPlotter::PLOT_NEURON_ACTIVATION) {
					valueToPlot = &(neuron->getActivationValue());
				}
				else {
					valueToPlot = &(neuron->getOutputActivationValue());
				}
				name = neuron->getNameValue().get();
			}
		}
		else if(mPlotMode == PLOT_SYNAPSE_WEIGHTS) {
			Synapse *synapse = dynamic_cast<Synapse*>(element);
			if(synapse != 0) {
				valueToPlot = &(synapse->getStrengthValue());
				name = getSynapseName(synapse);
			}
		}

		if(valueToPlot != 0) {
			getValuePlotter()->addValue(name, valueToPlot);
			mPlottedNetworkElements.append(element);
		}
	}
}


QString NeuronActivityPlotter::getName() const {
	return "NeuronActivityPlotter";
}


void NeuronActivityPlotter::eventOccured(Event *event) {
	TRACE("NeuronActivityPlotter::eventOccured()");

	if(event == 0) {
		return;
	}
	else if(event == mNetworksReplacedEvent) {
		removeAllNetworkElements();
	}
	else if(event == mNetworksModifiedEvent) {
		neuralNetworkModified(0);
	}
	else if(event == mShutDownEvent) {
		removeAllNetworkElements();
	}
}


void NeuronActivityPlotter::currentTabChanged(int) {
	TRACE("NeuronActivityPlotter::currentTabChanged()");

	removeAllNetworkElements();
	
	if(mEditor == 0) {
		return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(neuralNetworkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(neuralNetworkModified(ModularNeuralNetwork*)));
	}
}


void NeuronActivityPlotter::neuralNetworkModified(ModularNeuralNetwork*) {
	TRACE("NeuronActivityPlotter::neuralNetworkModified()");

	//check if all neurons are still valid
	if(mEditor == 0) {
		removeAllNetworkElements();
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		removeAllNetworkElements();
		return;
	}

	NeuralNetwork *net = visu->getNeuralNetwork();
	if(net == 0) {
		removeAllNetworkElements();
		return;
	}
	
	if(mPlotMode == NeuronActivityPlotter::PLOT_PARAMETERS) {
		QList<NeuralNetworkElement*> allElements;
		net->getNetworkElements(allElements);
		
		QList<NeuralNetworkElement*> elements = mPlottedNetworkElements;
		for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
			NeuralNetworkElement *element = i.next();
			
			Neuron *neuron = dynamic_cast<Neuron*>(element);
			Synapse *synapse = dynamic_cast<Synapse*>(element);
			ObservableNetworkElement *one = 0;
			if(neuron != 0) {
				one = neuron->getActivationFunction();
			}
			else if(synapse != 0) {
				one = synapse->getSynapseFunction();
			}
			
			if(!allElements.contains(element) || one == 0) {
				QList<Value*> params = mPlottedParameters.value(element);
				mPlottedParameters.remove(element);
				
				for(QListIterator<Value*> j(params); j.hasNext();) {
					getValuePlotter()->removeValue(j.next());
				}
				mPlottedNetworkElements.removeAll(element);
			}
			else {
				QList<Value*> parametersOfObservableElement = one->getObservableOutputs();
				QList<Value*> params = mPlottedParameters.value(element);
				QList<Value*> currentParams = params;

				for(QListIterator<Value*> k(params); k.hasNext();) {
					Value *value = k.next();
					if(!parametersOfObservableElement.contains(value)) {
						getValuePlotter()->removeValue(value);
						currentParams.removeAll(value);
					}
				}
				if(currentParams.empty()) {
					mPlottedParameters.remove(element);
					mPlottedNetworkElements.removeAll(element);
				}
				else {
					mPlottedParameters.remove(element);
					mPlottedParameters.insert(element, currentParams);
				}
			}
		}
	}
	else {
	
		QList<Synapse*> synapses;
		if(mPlotMode == NeuronActivityPlotter::PLOT_SYNAPSE_WEIGHTS) {
			synapses = net->getSynapses();
		}

		QList<NeuralNetworkElement*> elements = mPlottedNetworkElements;
		for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
			NeuralNetworkElement *element = i.next();
			if(mPlotMode == NeuronActivityPlotter::PLOT_NEURON_ACTIVATION ||
				mPlotMode == NeuronActivityPlotter::PLOT_NEURON_OUTPUT)
			{
				Neuron *neuron = dynamic_cast<Neuron*>(element);
				if(neuron != 0 && !net->getNeurons().contains(neuron)) {
					getValuePlotter()->removeValue(&(neuron->getActivationValue()));
					getValuePlotter()->removeValue(&(neuron->getOutputActivationValue()));
					mPlottedNetworkElements.removeAll(element);
				}
			}
			else if(mPlotMode == NeuronActivityPlotter::PLOT_SYNAPSE_WEIGHTS) {
				Synapse *synapse = dynamic_cast<Synapse*>(element);
				if(synapse != 0 && !synapses.contains(synapse)) {
					getValuePlotter()->removeValue(&(synapse->getStrengthValue()));
					mPlottedNetworkElements.removeAll(synapse);
				}
			}
		}
	}
}

void NeuronActivityPlotter::neuralNetworkChanged(ModularNeuralNetwork*) {
	TRACE("NeuronActivityPlotter::neuralNetworkChanged()");

	removeAllNetworkElements();
}

void NeuronActivityPlotter::removeAllNetworkElements() {
	TRACE("NeuronActivityPlotter::removeAllNetworkElements()");

	QList<Value*> values = getValuePlotter()->getPlottedValues();
	for(QListIterator<Value*> i(values); i.hasNext();) {
		getValuePlotter()->removeValue(i.next());
	}
	mPlottedNetworkElements.clear();
	mPlottedParameters.clear();
}

QString NeuronActivityPlotter::getSynapseName(Synapse *synapse) const {
	if(synapse == 0) {
		return "";
	}
	return QString("S ").append(QString::number(synapse->getId()));
}


void NeuronActivityPlotter::addSelectedNeuronsButtonPressed() {
	TRACE("NeuronActivityPlotter::addSelectedNeuronsButtonPressed()");
	
	if(mEditor == 0) {
		return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	
	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		
		addPlottedNetworkElement(i.next()->getNetworkElement());
// 		if(mPlotMode == NeuronActivityPlotter::PLOT_NEURON_ACTIVATION ||
// 			mPlotMode == NeuronActivityPlotter::PLOT_NEURON_OUTPUT)
// 		{
// 			NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
// 
// 			if(item != 0 && !mPlottedNetworkElements.contains(item->getNeuron())) {
// 				Neuron *neuron = item->getNeuron();
// 				mPlottedNetworkElements.append(neuron);
// 				if(mPlotMode == NeuronActivityPlotter::PLOT_NEURON_ACTIVATION) {
// 					getValuePlotter()->addValue(neuron->getNameValue().get(), 
// 												&(neuron->getActivationValue()));
// 				}
// 				else {
// 					getValuePlotter()->addValue(neuron->getNameValue().get(),
// 												&(neuron->getOutputActivationValue()));
// 				}
// 			}
// 		}
// 		else if(mPlotMode == NeuronActivityPlotter::PLOT_SYNAPSE_WEIGHTS) {
// 			SynapseItem *item = dynamic_cast<SynapseItem*>(i.next());
// 			if(item != 0 && !mPlottedNetworkElements.contains(item->getSynapse())) {
// 				Synapse *synapse = item->getSynapse();
// 				
// 				mPlottedNetworkElements.append(synapse);
// 				getValuePlotter()->addValue(getSynapseName(synapse), &(synapse->getStrengthValue()));
// 			}
// 		}
	}
}




void NeuronActivityPlotter::removeNeuronsButtonPressed() {
	TRACE("NeuronActivityPlotter::removeNeuronsButtonPressed()");

	removeAllNetworkElements();
}


}



