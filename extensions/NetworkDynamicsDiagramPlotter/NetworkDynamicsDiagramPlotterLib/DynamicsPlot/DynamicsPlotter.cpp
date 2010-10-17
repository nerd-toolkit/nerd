/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#include "DynamicsPlotter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "DynamicsPlot/DynamicsPlotManager.h"
#include "Event/EventManager.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "Value/ValueManager.h"
#include "Value/IntValue.h"
#include "Value/ULongLongValue.h"
#include <QTime>
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include <QStringList>

using namespace std;

namespace nerd {


/**
 * Constructs a new DynamicsPlotter.
 */
DynamicsPlotter::DynamicsPlotter(const QString &name)
	: ParameterizedObject(name, "/DynamicsPlotters/" + name + "/")
{
	Core::getInstance()->addSystemObject(this);
	DynamicsPlotManager::getInstance()->addDynamicsPlotter(this);

	mActiveValue = new BoolValue(false);
	mExecutionTime = new IntValue(0);
	//****Till****//
	mData = new MatrixValue(); //data matrix
	mOutputPath = new StringValue(); //string giving the path to the output file, if an external plotting program is used
// 	mPlotPixelsX = new IntValue(600); //accuracy of x-parameter variation, no. of pixels in x-dimension of diagram
// 	mPlotPixelsY = new IntValue(500); //same for y
	mData->setDescription("Matrix containing the output data, do not change.");
	mOutputPath->setDescription("Path to the output file for exported data.");
	//***/Till****//
	
	
	addParameter("Config/Activate", mActiveValue, true);
	addParameter("Performance/ExecutionTime", mExecutionTime, true);
	//****Till****//
	addParameter("Data", mData, true);
	addParameter("OutputPath", mOutputPath, true);
// 	addParameter("PlotPixelsX", mPlotPixelsX, true); 
// 	addParameter("PlotPixelsY", mPlotPixelsY, true); 
	//***/Till****//
}

/**
 * Destructor.
 */
DynamicsPlotter::~DynamicsPlotter() {
}


QString DynamicsPlotter::getName() const {
	return ParameterizedObject::getName();
}

bool DynamicsPlotter::init() {
	bool ok = true;
	//****Till****//
	EventManager *em = Core::getInstance()->getEventManager();
// 	mStartEvent = em->createEvent("calculatorStarts", "Triggers the preparation of the exporters and inbuilt plotters, informing about the start of the calculation process.");
// 	mFinishEvent = em->createEvent("calculatorFinishes", "Triggers the execution of the exporters and inbuilt plotters, informing about the completion of the calculation process.");
	mStartEvent = em->getEvent("calculatorStarts");
	mFinishEvent = em->getEvent("calculatorFinishes");
	if(mStartEvent == 0){
		mStartEvent = em->createEvent("calculatorStarts", "Triggers the preparation of the exporters and inbuilt plotters, informing about the start of the calculation process.");
	}
	if(mFinishEvent == 0){
		mFinishEvent = em->createEvent("calculatorFinishes", "Triggers the execution of the exporters and inbuilt plotters, informing about the completion of the calculation process.");
	}
	//***/Till****//
	
	
	return ok;
}

bool DynamicsPlotter::bind() {
	bool ok = true;

	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mResetEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_RESET);
	mEvaluateNetworkEvent = em->getEvent(NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED);

	mStasisValue = Core::getInstance()->getValueManager()->getBoolValue(
					NeuralNetworkConstants::VALUE_EVO_STASIS_MODE);

	if(mNextStepEvent == 0 || mResetEvent == 0 || mStasisValue == 0) {
		Core::log("DynamicsPlotter [" + getName() 
				+ "]: Could not find next step / reset events of stasis value", true);
		ok = false;
	}

	return ok;
}

bool DynamicsPlotter::cleanUp() {
	bool ok = true;

	return ok;
}

void DynamicsPlotter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
}


void DynamicsPlotter::execute() {
	QTime currentTime;
	currentTime.start();

	//****Till***//
	mValueManager = Core::getInstance()->getValueManager();
	mValueManager->getValue("/DynamicsPlotters/ActiveCalculator")->setValueFromString(getName());
	Core::getInstance()->executePendingTasks();
	mStartEvent->trigger();
	//***/Till***//
	
	if(!Core::getInstance()->isMainExecutionThread()) {
		return;
	}

	bool previousStasisSetting = true;
	if(mStasisValue != 0) {
		previousStasisSetting = mStasisValue->get();
		mStasisValue->set(false);
	}	

	calculateData();

	if(mStasisValue != 0) {
		mStasisValue->set(previousStasisSetting);
	}
	//****Till***//
	//set calculator inactive after every run:
	QString path = QString("/DynamicsPlotters/") + mValueManager->getValue("/DynamicsPlotters/ActiveCalculator")->getValueAsString() + QString("/Config/Activate");
	mValueManager->getValue(path)->setValueFromString("F"); 
	
	mValueManager = Core::getInstance()->getValueManager();
	mValueManager->getValue("/DynamicsPlotters/ActiveCalculator")->setValueFromString(""); //weg?
	Core::getInstance()->executePendingTasks();
	mFinishEvent->trigger();
	
	
	//***/Till***//
	mExecutionTime->set(currentTime.elapsed());
	
}

BoolValue* DynamicsPlotter::getActiveValue() const {
	return mActiveValue;
}

NeuralNetwork* DynamicsPlotter::getCurrentNetwork() const {
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	QList<NeuralNetwork*> networks = nnm->getNeuralNetworks();

	if(networks.empty()) {
		Core::log("ExampleDynamicsPlotter: Could not find a neural network!", true);
		return 0;
	}

	NeuralNetwork *network = networks.at(0);

	if(network == 0) {
		Core::log("DynamicsPlotter: Could not find a modular neural network!", true);
	}
	return network;
}

void DynamicsPlotter::storeCurrentNetworkActivities() {
	mNetworkActivities.clear();

	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		return;
	}

	QList<Neuron*> neurons = network->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		mNetworkActivities.insert(neuron->getId(), neuron->getOutputActivationValue().get());
	}
}


void DynamicsPlotter::restoreCurrentNetworkActivites() {
	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		return;
	}

	QList<Neuron*> neurons = network->getNeurons();

	for(QHashIterator<qulonglong, double> i(mNetworkActivities); i.hasNext();) {
		i.next();
		Neuron *neuron = NeuralNetwork::selectNeuronById(i.key(), neurons);
		if(neuron != 0) {
			neuron->getOutputActivationValue().set(i.value());
		}
		else {
			Core::log("DynamicsPlotter::restoreCurrentNetworkActivities: Could not find neuron "
						+ QString::number(i.key()), true);
		}
		mNetworkActivities.insert(neuron->getId(), neuron->getActivationValue().get());
	}
}


//****Till****//
	/**
	 * Get varied element. Returns a pointer on either a synapse or neuron, depending what element is found with the ID. 
	 * @param idOfVariedNetworkElement ID of the element, that is varied. 
	 * @return Neuron or synapse which is varied.
	 */
	NeuralNetworkElement* DynamicsPlotter::getVariedNetworkElement(ULongLongValue *idOfVariedNetworkElement) {
		NeuralNetwork *network = getCurrentNetwork();
	//Get pointers to the relevant varied element:
		if(NeuralNetwork::selectSynapseById(idOfVariedNetworkElement->get(), network->getSynapses()) != 0){ // element is synapse
			Synapse *variedElem = NeuralNetwork::selectSynapseById(idOfVariedNetworkElement->get(), network->getSynapses());
			return variedElem;
		}else if(NeuralNetwork::selectNeuronById(idOfVariedNetworkElement->get(), network->getNeurons()) != 0){ // element is neuron
			Neuron *variedElem = NeuralNetwork::selectNeuronById(idOfVariedNetworkElement->get(), network->getNeurons());
			return variedElem;
		}else{
			Core::log("DynamicsPlotter::getVariedNetworkElement: Could not find neuron or synapse from given ID", true);
			return 0;
		}
	
	}


	/**
	 * Set varied value of neuron bias or synapse strength
	 * @param *variedElem Neuron or synapse which is varied
	 * @param value New neuron bias or synapse strength
 	 */
	void DynamicsPlotter::setVariedNetworkElementValue(NeuralNetworkElement *variedElem, double value){
		if(variedElem == 0) {
			Core::log("DynamicsPlotter::setVariedNetworkElementValue: Could not find required neurons (varied / observed)!", true);
			return;
		}
		if(dynamic_cast<Neuron*>(variedElem) != 0){
			static_cast<Neuron*>(variedElem)->getBiasValue().set(value);
		}else if(dynamic_cast<Synapse*>(variedElem) != 0){
			static_cast<Synapse*>(variedElem)->getStrengthValue().set(value);
		}else{
			Core::log("DynamicsPlotter::setVariedNetworkElementValue: Could not find neuron or synapse.", true);
			return;
		}
	}


	/**
	 * Get varied value of neuron bias or synapse strength. 
	 * @param *variedElem Neuron or synapse which is varied
	 * @return Bias value or synapse strength depending on parameter
 	 */
	double DynamicsPlotter::getVariedNetworkElementValue(NeuralNetworkElement *variedElem){
		if(variedElem == 0) {
			Core::log("DynamicsPlotter::getVariedNetworkElementValue: Could not find required neurons (varied / observed)!", true);
			return 0;
		}
		if(dynamic_cast<Neuron*>(variedElem) != 0){
			return static_cast<Neuron*>(variedElem)->getBiasValue().get();
		}else if(dynamic_cast<Synapse*>(variedElem) != 0){
			return static_cast<Synapse*>(variedElem)->getStrengthValue().get();
		}else{
			Core::log("DynamicsPlotter::getVariedNetworkElementValue: Could not find neuron or synapse.", true);
			return 0;
		}
	}

	/**
	 * Checks if all strings contain the same amount of elements
	 * @param idsString StringValue containing the user input. This should be a list of network elements ids.
	 * @param minsString StringValue containing the user input. This should be a list of the minima of the varied values.
	 * @param maxsString StringValue containing the user input. This should be a list of the maxima of the varied values.
	 * @return Return True the number of parts of the string after splitting at | or , are the same.
	 */
	bool DynamicsPlotter::checkStringlistsItemCount(StringValue *idsString, StringValue *minsString, StringValue *maxsString){
		QString ids = idsString->get().replace("|", ","); // allow | and , as seperators
		QStringList idsList1 = ids.split(",", QString::SkipEmptyParts);
		QStringList minsList1 = minsString->get().split(",", QString::SkipEmptyParts);
		QStringList maxsList1 = maxsString->get().split(",", QString::SkipEmptyParts);
		if(idsList1.removeDuplicates() > 0){
			Core::log("DynamicsPlotter::checkStringlistsItemCount: Please avoid double IDs.", true);
			return true;
		}
		if(idsList1.size() == 1){ //if no separator could be found
			bool ok = false;
			QString str = idsList1[0];
			str.toULongLong (&ok);//try to convert to qulonglong, *ok is set false if not successful
			if(ok == false){
				Core::log("DynamicsPlotter::checkStringlistsItemCount: Could not find separators! Use \",\" or \"|\" please.", true);
				return false;
			}
		}
		if(idsList1.size() == minsList1.size() && idsList1.size() == maxsList1.size()){
			return true;
		}else{
			return false;
		}
	}
	
	/**
	 * Creates a QList of doubles from a string. Elements in string must be seperated by commas or '|'.
	 * @param idsString StringValue containing a list of elements seperated by commas or '|'. All elements must be convertable to ULongLong. 
	 * @return List of ULongLongValues. 
	 */
	QList<ULongLongValue*> DynamicsPlotter::createListOfIds(StringValue *idsString){
		QString ids = idsString->get().replace("|", ","); //replace all | by , to have a comma-seperated list.
		QList<ULongLongValue*> idsList;
		QStringList idsStringList = ids.split(",", QString::SkipEmptyParts);
		bool ok = false;
		for(int j = 0; j < idsStringList.size(); j++){
			idsList.append(new ULongLongValue(idsStringList[j].toULongLong(&ok)));
			if(ok == false){
				Core::log("DynamicsPlotter::createListOfIds: Could not convert string entry to uLongLong.", true);
			}else{
				ok = false;
			}
			
		}
		
		return idsList;
	}

	/**
	 * Creates a QList of doubles from a string. Elements in string must be seperated by commas. 
	 * @param idsString String Value containing a list of elements seperated by commas. 
	 * @return List of doubles. 
	 */
	QList<double>  DynamicsPlotter::createListOfDoubles(StringValue *doublesString){
		QList<double> doublesList;
		QStringList doublesStringList = doublesString->get().split(",", QString::SkipEmptyParts);
		bool ok = false;
		for(int j = 0; j < doublesStringList.size(); j++){
			doublesList.append(doublesStringList[j].toDouble(&ok));
			if(ok == false){
				Core::log("DynamicsPlotter::createListOfDoubles: Could not convert string entry to double.", true);
			}else{
				ok = false;
			}
			
		}
		
		return doublesList;
	}

//***/Till****//




}




