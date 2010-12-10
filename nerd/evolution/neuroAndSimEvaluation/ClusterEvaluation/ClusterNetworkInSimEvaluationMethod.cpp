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

#include "ClusterNetworkInSimEvaluationMethod.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "EvolutionConstants.h"
#include "Evolution/Population.h"
#include "Evolution/World.h"
#include "Evolution/Individual.h"
#include "Evolution/Evolution.h"
#include "Network/NeuralNetwork.h"
#include "Fitness/ControllerFitnessFunction.h"
#include "IO/NeuralNetworkIO.h"
#include "IO/NeuralNetworkIONerdV1Xml.h"
#include "NerdConstants.h"
#include "Math/Random.h"
#include <QTextStream>
#include <iostream>
#include "Util/Tracer.h"
#include <QCoreApplication>
#include <QDir>

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

ClusterNetworkInSimEvaluationMethod::ClusterNetworkInSimEvaluationMethod(const QString &name)
		: ClusterEvaluationMethod(name), mRandomizeSeed(0) 
{
	mNumberOfSteps = new IntValue(1000);
	mCurrentStep = new IntValue(0);
	mGenerationSimulationSeed = new IntValue(0);
	mPauseSimulation = new BoolValue(false);
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	if(!vm->addValue(EvolutionConstants::VALUE_EXECUTION_PAUSE, mPauseSimulation)) {
		mPauseSimulation = vm->getBoolValue(EvolutionConstants::VALUE_EXECUTION_PAUSE);
	}
	if(mPauseSimulation == 0) {
		Core::log("EvaluationLoop: Required Value could not be found.");
	}
	
	if(vm->addValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS, mNumberOfSteps) 
		== false) 
	{
		Core::log(QString("EvaluationLoop: Required Value [")
			.append(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS)
			.append("] could not be added to the ValueManager."));
	}

	vm->addValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_STEP, mCurrentStep);
	vm->addValue(NerdConstants::VALUE_RANDOMIZATION_SIMULATION_SEED, 
		mGenerationSimulationSeed);

	mAgentInterfaceNames = new StringValue("");
	mApplication = new StringValue("");
	mHostWithGuiName = new StringValue("Default");
	mRandomizeSeed = new BoolValue(true);

	addParameter("AgentInterfaces", mAgentInterfaceNames, true);	
	addParameter("Application", mApplication, true);
	addParameter("GuiHost", mHostWithGuiName, true);
	addParameter("RandomizeSeed", mRandomizeSeed, true);
	mFitnessFileName = "fitness.txt";

	mNextStep = mCore->getEventManager()->createEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP);
	mStepCompleted = mCore->getEventManager()->createEvent(
		NerdConstants::EVENT_EXECUTION_STEP_COMPLETED);

	mGenerationStartedEvent = mCore->getEventManager()->getEvent(
		EvolutionConstants::EVENT_EVO_GENERATION_STARTED, true);

	if(mNextStep == 0 || mStepCompleted == 0 || mGenerationStartedEvent == 0) {
		Core::log("ClusterNetworkInSimEvaluationMethod: Could not create required events!");
	}
	mGenerationStartedEvent->addEventListener(this);
}


ClusterNetworkInSimEvaluationMethod::ClusterNetworkInSimEvaluationMethod(
		const ClusterNetworkInSimEvaluationMethod &other) : Object(), 
		ValueChangedListener(), EventListener(), ClusterEvaluationMethod(other)
{
	mRandomizeSeed = dynamic_cast<BoolValue*>(getParameter("RandomizeSeed"));
	mApplication = dynamic_cast<StringValue*>(getParameter("Application"));
	mHostWithGuiName = dynamic_cast<StringValue*>(getParameter("GuiHost"));
	mAgentInterfaceNames = dynamic_cast<StringValue*>(getParameter("AgentInterfaces"));

}

ClusterNetworkInSimEvaluationMethod::~ClusterNetworkInSimEvaluationMethod() {
}

EvaluationMethod* ClusterNetworkInSimEvaluationMethod::createCopy() {
	return new ClusterNetworkInSimEvaluationMethod(*this);
}

void ClusterNetworkInSimEvaluationMethod::stopEvaluation() {

}


/**
 * This method collects all fitness-files that were created during the evaluation. The fitness-results of the used fitness functions are read from the file and stored with the according individual of the generation. If for some individuals no fitness-file was found, the indizes are stored and if the number of resubmit-tries is not exceeded yet, the according jobs are submitted again.
 * @return whether reading the results was successful
 */
bool ClusterNetworkInSimEvaluationMethod::readEvaluationResults() {
	TRACE("EvolutionManager::readEvaluationResults");

	bool ok = true;

	mOpenEvaluations.clear();
	QList<QList<Individual*> > groups = mEvaluationGroupsBuilder->getEvaluationGroups();
	mStatusMessageValue->set("Start loading fitness results.");
	for(int i = 0; i < groups.size(); i++) {
		QMap<QString, double> fitnessResults;
		QString filePath = mEvalCurrentDirectory;
		if(!filePath.endsWith("/")) {
			filePath.append("/");
		}
		filePath.append(QString::number(i+1)).append("/").append(mFitnessFileName);
		QFile file(filePath);
		bool fileOpend = file.open(QIODevice::ReadOnly | QIODevice::Text);
		if(!fileOpend) {
			mOpenEvaluations.push_back(i+1);
			Core::log(QString("ClusterNetworkInSimEvaluationMethod: Could not load file ").append(filePath), true);
			if(mNumberOfRetries->get() == 0) {
					mStatusMessageValue->set(QString("Could not load fitness file for "
						"evaluation group %1! Skipping individual!").arg(i+1));
			}
			file.close();
			//relieve the system to recover ressources (like file handles)
			QCoreApplication::instance()->thread()->wait(100);
			continue;
		}
	
		QTextStream input(&file);
		while (!input.atEnd()) {
			QString line = input.readLine();
			line = line.trimmed();
	
			if(line.startsWith("#")) {
				continue;
			}
			int sepIndex = line.indexOf("=");
			if(sepIndex == -1) {
				continue;
			}
			QString name = line.left(sepIndex);
			QString valueContent = line.right(line.length() - sepIndex - 1);
			fitnessResults[name] = valueContent.toDouble();
		}
		file.close();

		QMap<QString, double>::iterator index;
 		for (index = fitnessResults.begin(); 
				index != fitnessResults.end() && !Core::getInstance()->isShuttingDown(); 
				++index) 
		{
			mNextIndividual->trigger();
			bool found = false;
			for(int k = 0; k < mOwnerWorld->getPopulations().size(); k++) {
				Population *population  = mOwnerWorld->getPopulations().at(k); 
				FitnessFunction *fitness = population->getFitnessFunction(index.key());
				if(fitness == 0) {
					continue;
				}
				if(groups.size() < i || groups.at(i).size() < k) {
					Core::log("ClusterNetworkInSimEvaluationMethod: List with EvaluationGroups was "
							  "did not have the correct size! ");
					continue;
				}
				Individual *individual = groups.at(i).at(k);
				if(!population->getIndividuals().contains(individual)){
					Core::log("ClusterNetworkInSimEvaluationMethod: Error while reading "
						"evaluation results. Individual is not part of the population!");
					continue;
				}
				individual->setFitness(fitness, index.value());

				FitnessFunction *fitnessFunction = population->getFitnessFunction(fitness->getName());

				if(fitnessFunction == 0) {
					Core::log(QString("ClusterNetworkInSimEvaluationMethod: FitnessFunction with name ["
							+ fitness->getName() + "] could not be found!!"));	
					continue;
				}

				DoubleValue *fitnessValue = dynamic_cast<DoubleValue*>(
							fitnessFunction->getParameter("Fitness/Fitness"));

				if(fitnessValue == 0) {
					Core::log(QString("ClusterNetworkInSimEvaluationMethod: Fitness Value of FitnessFunction"
							" was NULL!"));
					continue;
				}
				fitnessValue->set(index.value());
				found = true;
			}
			if(!found) {
				Core::log("ClusterNetworkInSimEvaluationMethod: Found a fitness result "
					"that doesn't belong to any population of the current evolution.");
			}
			mIndividualCompleted->trigger();
		}
	}
	if(mOpenEvaluations.size() != 0) {
		performNeccessaryReSubmits();
	}
	
	mStatusMessageValue->set("Loaded fitness results from files.");
	return ok;
}


/**
 * Creates the data-structure for evaluating all individuals of all populations that are evaulated by this EvaluationMethod. Therefore all required directories are created and the neural networks of each group are stored in the according directories.
 * @return evaluation was prepared successfully
 */
bool ClusterNetworkInSimEvaluationMethod::prepareEvaluation() {
	TRACE("EvolutionManager::prepareEvaluation");

	bool ok = true;
	mAgentInterfaces = mAgentInterfaceNames->get().split(",", QString::SkipEmptyParts);

	if(mAgentInterfaces.empty()) {
		mAgentInterfaces.append("");
	}
	
	if(mAgentInterfaces.size() != mOwnerWorld->getPopulations().size()) {
		mStatusMessageValue->set("ClusterNetworkInSimEvaluationMethod: Wrong number of "
			"agent interfaces!");
		Core::log("ClusterNetworkInSimEvaluationMethod: Wrong number of agent interfaces!");
		return false;
	}
	QString configFilePath = mEvalCurrentDirectory;
	if(!configFilePath.endsWith("/")) {
		configFilePath.append("/");
	}
	configFilePath.append(mConfigFileName->get());

	mApplicationParameter.append(" -logFit $EVAL_DIR/").append(mFitnessFileName).append(" ");
	mApplicationParameter.append(" -val ").append(configFilePath).append(" ");

	QList<QList<Individual*> > groups = mEvaluationGroupsBuilder->getEvaluationGroups();

	for(int i = 0; i < groups.size(); i++) {
		QString currentDirectory = mEvalCurrentDirectory;
		if(!currentDirectory.endsWith("/")) {
			currentDirectory.append("/");
		}
		currentDirectory.append(QString::number(i+1)).append("/");
		if(!mCore->enforceDirectoryPath(currentDirectory)) {
				Core::log(QString("ClusterNetworkInSimEvaluationMethod: Could not "
					"create/find directory \"").append(currentDirectory).append("\". "
					"Skipping group!"));
			continue;
		}
		for(int j = 0; j < groups.at(i).size(); j++) {
			QString networkName = "network";
			networkName.append(QString::number(j)).append(".onn");
			
			Individual *individual = groups.at(i).at(j);
			Population *population = mOwnerWorld->getPopulations().at(j);
			GenotypePhenotypeMapper *mapper = population->getGenotypePhenotypeMapper();
			if(mapper == 0) {
				Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
  					" Could not apply GenotypePhenotypeMapper [")
				  	.append(mapper->getName()).append("]! [SKIPPING INDIVIDUAL]"));
			continue;
			}
			if(!mapper->createPhenotype(individual)) {
			Core::log(QString("LocalNetworkInSimulationEvaluationMethod: "
				  " Could not apply GenotypePhenotypeMapper [")
				  .append(mapper->getName()).append("]! [SKIPPING INDIVIDUAL]"));
			continue;
			}
			NeuralNetwork *network =  dynamic_cast<NeuralNetwork*>(individual->getPhenotype());
			if(network == 0) {
				Core::log("ClusterNetworkInSimEvaluationMethod: "
					"Found an individual that is not a neural network! "
					"Skipping group!");
				continue;
			}
			QString errorMsg;
			QString filePath = currentDirectory + networkName;
			QString netXml = NeuralNetworkIONerdV1Xml::createXmlFromNetwork(network);
			
			QFile file(filePath);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
				if(errorMsg != NULL) {
					mStatusMessageValue->set(QString("Cannot create file ").append(filePath).append("."));
					Core::log(QString("Cannot create file ").append(filePath).append("."));
				}
				file.close();
				continue;
			}
			QTextStream out(&file);
			out << netXml << "\n";
			file.close();	

			//set file property
			individual->setProperty("FileName", filePath);

			if(i == 0) {
				mApplicationParameter.append(" -net ").append(mAgentInterfaces.at(j)).append(" ")
					.append("$EVAL_DIR/").append(networkName).append(" ");
			}
		}
		mStatusMessageValue->set("Saved individuals to file.");
	}
	createFitnessInformation();
	return ok;
}


/**
 * Adds all parameters of the fitness functions, the randomization seed and the number of evaluation steps to the config-file that will be used by all evaluation-jobs of this generation.
 * @return 
 */
bool ClusterNetworkInSimEvaluationMethod::createConfigList() {
	TRACE("EvolutionManager::createConfigList");

	ClusterEvaluationMethod::createConfigList();
	bool ok = true;
	mConfigValues += mCore->getValueManager()
			->getValueNamesMatchingPattern("(?!.*/Config/)(?!.*/Fitness/.*/Fitness/)/Evo/Pop/.*/Fitness/.*");
	mConfigValues += mCore->getValueManager()
			->getValueNamesMatchingPattern("/Evo/Pop/.*/Fitness/.*/Fitness/CalculationMode");
	mConfigValues.push_back(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS);
	mConfigValues.push_back(NerdConstants::VALUE_RANDOMIZATION_SIMULATION_SEED);
	return ok;
}

/**
 * Prepares the commandline information for all used fitness functions (AgentInterface, PrototypeName, Name).
 * @return 
 */
bool ClusterNetworkInSimEvaluationMethod::createFitnessInformation() {
	TRACE("EvolutionManager::createFitnessInformation");

	bool ok = true;
	mFitnessParameter = "";
	for(int i = 0; i < mOwnerWorld->getPopulations().size(); i++) {
		Population *population = mOwnerWorld->getPopulations().at(i);
		QString interfaceName = mAgentInterfaces.at(i);
		for(int j = 0; j < population->getFitnessFunctions().size(); j++) {
			FitnessFunction *fit = population->getFitnessFunctions().at(j);
			if(dynamic_cast<ControllerFitnessFunction*>(fit) == 0) {
				Core::log(QString("ClusterNetworkInSimEvaluationMethod: Population contains "
					"a fitness function that is no ControllerFitnessFunction: [")
					.append(fit->getName()).append("]!"));
				return false;
			}
			mFitnessParameter.append(" -fit ");
			mFitnessParameter.append(mAgentInterfaces.at(i));
			mFitnessParameter.append(" ").append(fit->getPrototypeName());
			mFitnessParameter.append(" ").append(fit->getName());
			mFitnessParameter.append(" ").append(population->getPrefix() + "Fitness/ ");;
		}
	}
	mApplicationParameter.append(" ").append(mFitnessParameter).append(" ");
	return ok;
}

bool ClusterNetworkInSimEvaluationMethod::reset() {
	TRACE("EvolutionManager::reset");

	bool ok = ClusterEvaluationMethod::reset();
	mFitnessParameter = "";
	mAgentInterfaces.clear();
	mApplicationParameter = "";
	return ok;
}

/**
 * Creates the job-script for the sun grid engine.
 * @return 
 */
bool ClusterNetworkInSimEvaluationMethod::createJobScript() {
	TRACE("EvolutionManager::createJobScript");

	bool ok = true;
	mJobScriptContent = "";
	QTextStream script(&mJobScriptContent, QIODevice::WriteOnly | QIODevice::Truncate);

	QString applicationCall = mApplication->get();
	applicationCall = applicationCall.replace("$HOME$", QDir::currentPath());

	script << "#!/bin/sh" << endl << endl;
	script << "TASK_ID=$1" << endl;
	script << "ARGUMENT_POSTFIX=" << endl << endl;
	script << "if [ \"${SGE_TASK_ID}\" != \"undefined\" -a \"x${SGE_TASK_ID}\" != \"x\" ]" << endl;
	script << "then TASK_ID=$SGE_TASK_ID" << endl;
	script << "else" << endl;
	script << "  if [ $# -eq 0 ]" << endl;
	script << "  then ARGUMENT_POSTFIX=\"-gui -test -toggle\"" << endl;
	script << "    TASK_ID=1" << endl;
	script << "  fi" << endl << endl;
	script << "fi" << endl;
	script << "EVAL_DIR=" << mEvalCurrentDirectory << "/$TASK_ID" << endl << endl;
	script << "if [ \"x$TASK_ID\" = \"x\" ]" << endl;
	script << "then exit 1" << endl;
	script << "fi" << endl << endl;
	script << "i=1" << endl;
	script << "MAX_RETRIES=10" << endl;
	script << "while [ ! -d $EVAL_DIR -a $i -le $MAX_RETRIES ]" << endl;
	script << "do" << endl;
	script << "#Wait for one second" << endl;
	script << "sleep 1" << endl;
	script << "i='expr $i + 1'" << endl;
	script << "done" << endl;
	script << "if [ ! -d $EVAL_DIR ]" << endl;
	script << "then exit 1" << endl;
	script << "fi" << endl << endl;
	script << "if [ $# -gt 0 ]" << endl;
	script << "then shift" << endl;
	script << "fi" << endl;
	script << "cd $EVAL_DIR" << endl;
	script << applicationCall << mApplicationParameter << " -nogui -disableLogging $ARGUMENT_POSTFIX $* " << endl << endl;
	script << "exit 0" << endl;
	return ok;
}

QString ClusterNetworkInSimEvaluationMethod::getName() const {
	return "ClusterNetworkInSimEvaluationMethod";
}

void ClusterNetworkInSimEvaluationMethod::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mGenerationStartedEvent && mRandomizeSeed->get()) {
		int seed = Random::nextInt();
		mGenerationSimulationSeed->set(seed);
	}
}



/**
 * Submits jobs for all individuals, where no fitness-results were returned. The evaluation-jobs are resubmitted maximum "NumberOfResubmits" times (including the initial submit).
 */
void ClusterNetworkInSimEvaluationMethod::performNeccessaryReSubmits() {
	TRACE("EvolutionManager::performNeccessaryReSubmits");

	//TODO Merge this method with the other submit method!!!!!

	int currentTry = 0;
	QList<int> openEvaluations;
	while(mOpenEvaluations.size() != 0 && currentTry < mNumberOfRetries->get() 
			&& !Core::getInstance()->isShuttingDown()) 
	{
		openEvaluations.clear();
		reSubmitJobs();
	
		QList<QList<Individual*> > groups = mEvaluationGroupsBuilder->getEvaluationGroups();
		mStatusMessageValue->set("Start loading fitness results.");
		for(int i = 0; i < mOpenEvaluations.size() && !Core::getInstance()->isShuttingDown(); i++) {
			QMap<QString, double> fitnessResults;
			QString filePath = mEvalCurrentDirectory;
			if(!filePath.endsWith("/")) {
				filePath.append("/");
			}
			filePath.append(QString::number(mOpenEvaluations.at(i)))
				.append("/")
				.append(mFitnessFileName);
			QFile file(filePath);
		
			bool fileOpend = file.open(QIODevice::ReadOnly | QIODevice::Text);
			if(!fileOpend) {
				file.close();
				Core::log(QString("ClusterNetworkInSimEvaluationMethod: Could not load file ")
						.append(filePath), true);
				if(currentTry == mNumberOfRetries->get() - 1) {
					mStatusMessageValue->set(QString("Could not load fitness file for "
						"evaluation group %1! Skipping individual!").arg(mOpenEvaluations.at(i)));
				} else {
					openEvaluations.push_back(mOpenEvaluations.at(i));
				}
				//relieve the system to recover ressources (like file handles)
				QCoreApplication::instance()->thread()->wait(100);
				continue;
			}
		
			QTextStream input(&file);
			while (!input.atEnd()) {
				QString line = input.readLine();
				line = line.trimmed();
		
				if(line.startsWith("#")) {
					continue;
				}
				int sepIndex = line.indexOf("=");
				if(sepIndex == -1) {
					continue;
				}
				QString name = line.left(sepIndex);
				QString valueContent = line.right(line.length() - sepIndex - 1);
				fitnessResults[name] = valueContent.toDouble();
			}
			file.close();
	
			QMap<QString, double>::iterator index;
			for (index = fitnessResults.begin(); index != fitnessResults.end(); ++index) {
				mNextIndividual->trigger();
				bool found = false;
				for(int k = 0; k < mOwnerWorld->getPopulations().size(); k++) {
					Population *population  = 
							mOwnerWorld->getPopulations().at(k); 
					FitnessFunction *fitness =
							population->getFitnessFunction(index.key());
					if(fitness == 0) {
						continue;
					}
					Individual *individual = groups.at(i).at(k);
					if(!population->getIndividuals().contains(individual)){
						Core::log("ClusterNetworkInSimEvaluationMethod: Error while "
							"reading evaluation results. Individual is not part of the "
							"population!");
						continue;
					}
					individual->setFitness(fitness, index.value());
					
					FitnessFunction *fitnessFunction = population->getFitnessFunction(fitness->getName());

					if(fitnessFunction != 0) {
						Core::log("ClusterNetworkInSimEvaluationMethod: FitnessFunction was not found.");
						continue;
					}

					DoubleValue *fitnessValue = dynamic_cast<DoubleValue*>(
									fitnessFunction->getParameter("Fitness/Fitness"));

					if(fitnessValue == 0) {
						Core::log("ClusterNetworkInSimEvaluationMethod: Fitness Value of FF not found!");
						continue;
					}

					fitnessValue->set(index.value());
					found = true;
				}
				if(!found) {
					Core::log("ClusterNetworkInSimEvaluationMethod: Found a fitness "
						"results that doesn't belong to any population of the current "
						"evolution.");
				}
				mIndividualCompleted->trigger();
			}
		}		

		mOpenEvaluations = openEvaluations;
		currentTry++;
	}
}
}
