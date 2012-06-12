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
#include "DynamicsPlotConstants.h"
#include "NetworkEditorConstants.h"
#include <Util/DynamicsPlotterUtil.h>

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
	mProgressPercentage = new DoubleValue(0);

	//****Till****//
	mData = new MatrixValue(); //data matrix
	mOutputPath = new StringValue(); //string giving the path to the output file, if an external plotting program is used
	mXAxisDescription = new StringValue("X Parameters");
	mYAxisDescription = new StringValue("Y Parameters");	
	mData->setDescription("Matrix containing the output data, do not change.");
	mOutputPath->setDescription("Path to the output file for exported data.");
	mXAxisDescription->setDescription("Description of x-axis that appears in the plotters.");
	mYAxisDescription->setDescription("Description of y-axis that appears in the plotters.");
	//***/Till****//
	
	
	addParameter("Config/Activate", mActiveValue, true);
	addParameter("Performance/ExecutionTime", mExecutionTime, true);
	addParameter("Performance/ProgressPercentage", mProgressPercentage, true);
	
	//****Till****c//
	addParameter("Internal/Data", mData, true);
	addParameter("Config/OutputPath", mOutputPath, true);
	addParameter("Config/XAxisDescription", mXAxisDescription, true);
	addParameter("Config/YAxisDescription", mYAxisDescription, true);
	//***/Till****c//
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
	//****Till****c//
	EventManager *em = Core::getInstance()->getEventManager();

	//Obtain the events for calculation start and completed.
	//If not available, create them (only once).
	mStartEvent = em->getEvent(DynamicsPlotConstants::EVENT_CALCULATION_STARTED);
	mFinishEvent = em->getEvent(DynamicsPlotConstants::EVENT_CALCULATION_COMPLETED);
	mClearAllEditorSelections = em->getEvent(NetworkEditorConstants::VALUE_EDITOR_CLEAR_ALL_SELECTIONS);
	
	if(mStartEvent == 0){
		mStartEvent = em->createEvent(DynamicsPlotConstants::EVENT_CALCULATION_STARTED, 
						"Triggers the preparation of the exporters and inbuilt plotters, "
						"informing about the start of the calculation process.");
	}
	if(mFinishEvent == 0){
		mFinishEvent = em->createEvent(DynamicsPlotConstants::EVENT_CALCULATION_COMPLETED, 
						"Triggers the execution of the exporters and inbuilt plotters, "
						"informing about the completion of the calculation process.");
	}
	//***/Till****c//
	
	
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

	//****Till***c//
	mValueManager->getValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER)
						->setValueFromString(getName());
	Core::getInstance()->executePendingTasks();
	mStartEvent->trigger();
	//***/Till***c//
	
	if(!Core::getInstance()->isMainExecutionThread()) {
		return;
	}
	
	if(mClearAllEditorSelections != 0) {
		mClearAllEditorSelections->trigger();
	}
	

	bool previousStasisSetting = true;
	if(mStasisValue != 0) {
		previousStasisSetting = mStasisValue->get();
		mStasisValue->set(false);
	}	
	
	//clear list of neurons with changing avtivation values.
	mNeuronsWithActivationsToTransfer.clear();

	calculateData();

	if(mStasisValue != 0) {
		mStasisValue->set(previousStasisSetting);
	}
	
	//****Till***//
	//set calculator inactive after every run:
	
	//QString path = QString("/DynamicsPlotters/") + mValueManager->getValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER)->getValueAsString() + QString("/Config/Activate");
	//mValueManager->getValue(path)->setValueFromString("F"); 
	getActiveValue()->set(false);
	
	mValueManager->getValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER)->setValueFromString("");
	
	Core::getInstance()->executePendingTasks();
	
	mFinishEvent->trigger();
	
	
	//***/Till***c//
	
	mExecutionTime->set(currentTime.elapsed());
	
}

BoolValue* DynamicsPlotter::getActiveValue() const {
	return mActiveValue;
}

ModularNeuralNetwork* DynamicsPlotter::getCurrentNetwork() const {
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	QList<NeuralNetwork*> networks = nnm->getNeuralNetworks();

	if(networks.empty()) {
		Core::log("DynamicsPlotter: Could not find a neural network!", true);
		return 0;
	}

	ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(networks.at(0));

	if(network == 0) {
		Core::log("DynamicsPlotter: Could not find a neural network!", true);
	}
	return network;
}

void DynamicsPlotter::storeCurrentNetworkActivities() {
	mNetworkActivities.clear();
	mNetworkOutputs.clear();

	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		return;
	}

	mCurrentNeurons = network->getNeurons();
	for(QListIterator<Neuron*> i(mCurrentNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		mNetworkActivities.append(neuron->getActivationValue().get());
		mNetworkOutputs.append(neuron->getOutputActivationValue().get());
	}
}


void DynamicsPlotter::restoreCurrentNetworkActivites() {
	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0 
			|| mCurrentNeurons.size() != mNetworkActivities.size() 
			|| mCurrentNeurons.size() != mNetworkOutputs.size()) 
	{
		return;
	}

	for(int i = 0; i < mCurrentNeurons.size(); ++i) {
		Neuron *neuron = mCurrentNeurons.at(i);
		neuron->getActivationValue().set(mNetworkActivities.at(i));
		neuron->getOutputActivationValue().set(mNetworkOutputs.at(i));
	}
}

void DynamicsPlotter::storeNetworkConfiguration() {
	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		return;
	}
	mNetworkConfigurationValues.clear();

	QList<Neuron*> neurons = network->getNeurons();
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		mNetworkConfigurationValues.insert(&neuron->getBiasValue(), neuron->getBiasValue().get());
		
		QList<Value*> observables;
		
		{
			TransferFunction *tf = neuron->getTransferFunction();
			if(tf != 0) {
				observables << tf->getObservableOutputs();
			}
		}
		{
			ActivationFunction *af = neuron->getActivationFunction();
			if(af != 0) {
				observables << af->getObservableOutputs();
			}
		}
		
		QList<Synapse*> synapses = neuron->getSynapses();
		for(QListIterator<Synapse*> j(synapses); j.hasNext();) {
			Synapse *synapse = j.next();
			
			mNetworkConfigurationValues.insert(&synapse->getStrengthValue(),
								synapse->getStrengthValue().get());
			
			SynapseFunction *sf = synapse->getSynapseFunction();
			if(sf != 0) {
				observables << sf->getObservableOutputs();
			}
		}
		
		 
		for(QListIterator<Value*> j(observables); j.hasNext();) {
			DoubleValue *value = dynamic_cast<DoubleValue*>(j.next());
			if(value != 0) {
				mNetworkConfigurationValues.insert(value, value->get());
			}
		}
	}
}


void DynamicsPlotter::restoreNetworkConfiguration() {
	NeuralNetwork *network = getCurrentNetwork();

	if(network == 0) {
		return;
	}

	for(QHashIterator<DoubleValue*, double> i(mNetworkConfigurationValues); i.hasNext();) {
		i.next();
		DoubleValue *value = i.key();
		if(value != 0) {
			value->set(i.value());
		}
	}
}



void DynamicsPlotter::triggerNetworkStep() {

	if(mEvaluateNetworkEvent != 0) {
		mEvaluateNetworkEvent->trigger();
	}
}

void DynamicsPlotter::notifyNetworkParametersChanged(ModularNeuralNetwork *network) {
	
	if(!mNeuronsWithActivationsToTransfer.empty()) {
		DynamicsPlotterUtil::transferNeuronActivationToOutput(mNeuronsWithActivationsToTransfer);
	}
	
}


//****Till****//
	/**
	 * Get varied element. Returns a pointer on either a synapse or neuron, depending what element is found with the ID. 
	 * @param idOfVariedNetworkElement ID of the element, that is varied. 
	 * @return Neuron or synapse which is varied.
	 */
	NeuralNetworkElement* DynamicsPlotter::getVariedNetworkElement(qulonglong idOfVariedNetworkElement) {
		NeuralNetwork *network = getCurrentNetwork();
		
		//Get pointers to the relevant varied element:
		Synapse *synapse = NeuralNetwork::selectSynapseById(idOfVariedNetworkElement, 
															network->getSynapses());													
		if(synapse != 0) { // element is synapse
			return synapse;
		}
		
		Neuron *neuron = NeuralNetwork::selectNeuronById(idOfVariedNetworkElement, 
														 network->getNeurons());
		if(neuron != 0){ // element is neuron
			return neuron;
		}

		//if the object was neither a neuron, nor a synapse...
		Core::log("DynamicsPlotter::getVariedNetworkElement: Could not find neuron or synapse from given ID", true);
		return 0;
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
	 * @param idsString QString containing the user input. 
	 *                  This should be a list of network elements ids.
	 * @param minsString QString containing the user input. 
	 *                   This should be a list of the minima of the varied values.
	 * @param maxsString QString containing the user input. 
	 *                   This should be a list of the maxima of the varied values.
	 * @return Return True the number of parts of the string after splitting at | or , are the same.
	 */
	bool DynamicsPlotter::checkStringListsItemCount(const QString &idsString, 
													const QString &minsString, 
													const QString &maxsString)
	{	
		// allow | and , as seperators
		QString ids = idsString;
		ids.replace("|", ","); 
		
		QStringList idsList1 = ids.split(",", QString::SkipEmptyParts);
		QStringList minsList1 = minsString.split(",", QString::SkipEmptyParts);
		QStringList maxsList1 = maxsString.split(",", QString::SkipEmptyParts);
		
		if(idsList1.removeDuplicates() > 0){
			Core::log("DynamicsPlotter::checkStringListsItemCount: Please avoid double IDs.", true);
			return true;
		}
		
		//TODO (chris): What the heck is this?? Can be removed?
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
		}
		else {
			return false;
		}
	}
	
	
	
	/**
	 * Creates a QList of doubles from a string. Elements in string must be seperated by commas or '|'.
	 * @param idsString StringValue containing a list of elements seperated by commas or '|'. 
	 *                  All elements must be convertable to ULongLong. 
	 * @return List of ULongLongValues. 
	 */
	QList<qulonglong> DynamicsPlotter::createListOfIds(const QString &idsString){
		QList<qulonglong> idsList;
		
		//replace all | by , to have a comma-seperated list.
		QString ids = idsString;
		ids.replace("|", ","); 
		
		QStringList idsStringList = ids.split(",", QString::SkipEmptyParts);
		
		bool ok = false;
		for(int j = 0; j < idsStringList.size(); j++){
			qulonglong id = idsStringList[j].toULongLong(&ok);
			
			if(ok){
				idsList.append(id);
			}
			else {
				Core::log("DynamicsPlotter::createListOfIds: Could not convert string entry "
						  "to uLongLong.", true);
			}
		}
		
		return idsList;
	}

	/**
	 * Creates a QList of doubles from a string. Elements in string must be seperated by commas. 
	 * @param idsString String Value containing a list of elements seperated by commas. 
	 * @return List of doubles. 
	 */
	QList<double>  DynamicsPlotter::createListOfDoubles(const QString &doublesString){
		QList<double> doublesList;

		QStringList doublesStringList = doublesString.split(",", QString::SkipEmptyParts);
		
		bool ok = false;
		for(int j = 0; j < doublesStringList.size(); j++) {
			double value = doublesStringList[j].toDouble(&ok);
			
			if(ok) {
				doublesList.append(value);
			}
			else {
				Core::log("DynamicsPlotter::createListOfDoubles: Could not convert string "
						  "entry to double.", true);
			}
		}
		
		return doublesList;
	}

//***/Till****c//




}




