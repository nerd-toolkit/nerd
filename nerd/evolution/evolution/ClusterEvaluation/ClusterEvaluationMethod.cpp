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

#include "ClusterEvaluationMethod.h"
#include "NerdConstants.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <iostream>
#include <QCoreApplication>
#include "Evolution/Evolution.h"
#include "EvolutionConstants.h"
#include "Util/Tracer.h"

using namespace std;

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

namespace nerd {


QSubRunner::QSubRunner(const QStringList &qSubCommand) {
	mQSubCommand = qSubCommand;
}

void QSubRunner::run() {
	QProcess *evalProcess = new QProcess();
	evalProcess->start("/bin/bash", mQSubCommand);
	while(!evalProcess->waitForFinished(100)) {
	}
	if(evalProcess->exitCode() != 0) {
		Core::log("ClusterEvaluationMethod: Error while performing evaluation."
			" Submissing job was not successful!");
	}
	delete evalProcess;
}


ClusterEvaluationMethod::ClusterEvaluationMethod(const QString &name)
		: EvaluationMethod(name), mQSubScriptName(0), mClusterJobSubmitted(0),
		  mCurrentGenerationDirectory(0), mCurrentStartScriptFullFileName(0)
{
	mCore = Core::getInstance();
	ValueManager *vm = Core::getInstance()->getValueManager();
	mCurrentTry = new IntValue(0);
	mNumberOfTries = new IntValue(1);
	if(vm->addValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES, mNumberOfTries)
		 == false)
	{
		Core::log(QString("ClusterEvaluationMethod: Required Value [")
			.append(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES)
			.append("] could not be added to the ValueManager."));
	}
	vm->addValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_TRY, mCurrentTry);

	mWorkingDir = vm->getStringValue(EvolutionConstants::VALUE_EVO_WORKING_DIRECTORY);
	if(mWorkingDir == 0) {
		Core::log(QString("ClusterEvaluationMethod: Required value [")
			.append(EvolutionConstants::VALUE_EVO_WORKING_DIRECTORY)
			.append("] could not be found."));
	}
	mWorkingDir->useAsFileName(true);

	mNextTry = mCore->getEventManager()->createEvent(NerdConstants::
		EVENT_EXECUTION_NEXT_TRY);
	mTryCompleted = mCore->getEventManager()->createEvent(NerdConstants::
		EVENT_EXECUTION_TRY_COMPLETED);

	mNextIndividual = mCore->getEventManager()->createEvent(NerdConstants::
		EVENT_EXECUTION_NEXT_INDIVIDUAL);
	mIndividualCompleted = mCore->getEventManager()->createEvent(NerdConstants::
		EVENT_EXECUTION_INDIVIDUAL_COMPLETED);

	mClusterJobSubmitted = mCore->getEventManager()->createEvent(
		EvolutionConstants::EVENT_EVO_CLUSTER_JOB_SUBMITTED);

	if(mNextIndividual == 0
		|| mIndividualCompleted == 0
		|| mNextTry == 0
		|| mTryCompleted == 0
		|| mClusterJobSubmitted == 0) 
	{
		Core::log("ClusterEvaluationMethod: Required Event could not be found.");
	}

	mJobScriptContent = "";
	mCurrentGenerationID = mCore->getValueManager()->getIntValue(EvolutionConstants::
		VALUE_EVO_CURRENT_GENERATION_NUMBER);

	if(mCurrentGenerationID == 0) {
		Core::log("ClusterEvaluationMethod: Required value could not be found!");
	}

	QString qsubDirName = "";
	QStringList singleParts = mWorkingDir->get().split("/");

	int maxIndex = singleParts.size() - 1;
	if(mWorkingDir->get().endsWith("/")) {
		maxIndex = singleParts.size() - 2;
	}
	if(!mWorkingDir->get().startsWith("/")) {
		qsubDirName = "/";
	}

	for(int i = 0; i < maxIndex; i++) {
		qsubDirName.append(singleParts.at(i));
		qsubDirName.append("/");
	}

	mQsubPriority = new IntValue(0);
	mJobScriptName = new StringValue("evalJob");
	mJobScriptName->useAsFileName(true);
	mQSubScriptName = new StringValue(qsubDirName + "qsubCaller.sh");
	mQSubScriptName->useAsFileName(true);
	mConfigFileName = new StringValue("config.val");
	mConfigFileName->useAsFileName(true);
	mNumberOfRetries = new IntValue(5);
	mStatusMessageValue = new StringValue("");
	mZipGenerations = new BoolValue(false);
	mCurrentGenerationDirectory = new StringValue("");
	mCurrentStartScriptFullFileName = new StringValue("");

	setPrefix(getName() + "/");

	addParameter("Priority", mQsubPriority, true);
	addParameter("JobName", mJobScriptName, true);
	addParameter("QSubScript", mQSubScriptName, true);
	addParameter("ConfigFile", mConfigFileName, true);
	addParameter("NumberOfResubmits", mNumberOfRetries, true);
	addParameter("EvaluationStatus", mStatusMessageValue, true);
	addParameter("ZipGenerations", mZipGenerations, true);
	addParameter("CurrentGenerationDirectory", mCurrentGenerationDirectory, true);
	vm->addValue(EvolutionConstants::VALUE_CURRENT_GENERATION_DIRECTORY, mCurrentGenerationDirectory);
	addParameter("CurrentStartScript", mCurrentStartScriptFullFileName);
	vm->addValue(EvolutionConstants::VALUE_CURRENT_INDIVIDUAL_EVALUATION_START_SCRIPT, mCurrentStartScriptFullFileName);
}


ClusterEvaluationMethod::ClusterEvaluationMethod(const ClusterEvaluationMethod &other)
		: Object(), ValueChangedListener(), EvaluationMethod(other) 
{
	Core::log("ClusterEvaluationMethod: Warning! Copy Consructor is not implemented yet!!");
}


ClusterEvaluationMethod::~ClusterEvaluationMethod() {
}


/**
 * Main method that controls the course of an evaluation if using the sun grid engine.
 * @return 
 */
bool ClusterEvaluationMethod::evaluateIndividuals() {

	mOpenEvaluations.clear();
	bool ok = true;
	reset();
	mStatusMessageValue->set(QString("~Starting evaluation of generation ")
		.append(mCurrentGenerationID->getValueAsString()));
	mEvalCurrentDirectory = mWorkingDir->get();
	if(!mEvalCurrentDirectory.endsWith("/")) {
		mEvalCurrentDirectory.append("/");
	}
	mEvalCurrentDirectory.append(EvolutionConstants::GENERATION_DIRECTORY_PREFIX)
		.append(mCurrentGenerationID->getValueAsString());//.append("/eval");
	mCore->enforceDirectoryPath(mEvalCurrentDirectory);
	mCurrentGenerationDirectory->set(mEvalCurrentDirectory);

	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	if(!createConfigList()) {
		Core::log("ClusterEvaluationMethod: Creating config-file was not successful."
			"Quitting evaluation!");
		mStatusMessageValue->set("ClusterEvaluationMethod: Creating config-file was not "
			"successful. Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	if(!saveConfigFile()) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Saving config-file was not "
			"successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Saving config-file was not successful."
			"Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	mStatusMessageValue->set("Value set created and saved to file.");
	if(!prepareEvaluation()) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Preparing evaluation was not "
			"successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Preparing evaluation was not successful. "
			"Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	if(!createJobScript()) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Creating job-script was not "
			"successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Creating job-script was not successful."
			"Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	if(!saveJobScript()) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Saving job script was not"
			" successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Saving job script was not successful."
			" Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	mStatusMessageValue->set("Job script for grid engine created and saved.");
	mStatusMessageValue->set(QString("Submitting evaluation job for generation ")
		.append(mCurrentGenerationID->getValueAsString()).append("..."));
	int nrOfJobs = mEvaluationGroupsBuilder->getEvaluationGroups().size();
	if(!submitJob(1, nrOfJobs)) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Submitting and performing job"
			" was not successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Submitting and performing job was not"
			" successful. Quitting evaluation!");
		return false;
	}
	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	mStatusMessageValue->set("Evaluation terminated. Reading fitness results.");
	if(!readEvaluationResults()) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Reading evaluation results"
			" was not successful. Quitting evaluation!");
		Core::log("ClusterEvaluationMethod: Reading evaluation results was not"
			" successful. Quitting evaluation!");
		return false;
	}

	mCore->executePendingTasks();

	if(Core::getInstance()->isShuttingDown()) {
		return true;
	}

	mStatusMessageValue->set(QString("Evaluation of generation ")
		.append(mCurrentGenerationID->getValueAsString()).append(" finished!"));

	if(mZipGenerations->get()) {
		// zip last generation
		QString dir = "./";
		dir.append(EvolutionConstants::GENERATION_DIRECTORY_PREFIX)
			.append(QString::number(mCurrentGenerationID->get()));
	
		QStringList zipArguments;
		zipArguments.push_back("-r");
		zipArguments.push_back("-m");
		zipArguments.push_back("-l");
		zipArguments.push_back("-9");
		zipArguments.push_back(dir);
		zipArguments.push_back(dir);
	
		mStatusMessageValue->set(QString("Creating archiv for generation ")
			.append(QString::number(mCurrentGenerationID->get())).append("."));
		QProcess *zipProcess = new QProcess();
		zipProcess->setWorkingDirectory(mWorkingDir->get());
		zipProcess->start("zip", zipArguments);
		while(!zipProcess->waitForFinished(250)) {
			if(zipProcess->exitCode() != 0) {
				break;
			}
			Core::getInstance()->executePendingTasks();
		}
		if(zipProcess->exitCode() == 0) {
			mStatusMessageValue->set(QString("Archive for generation ")
				.append(QString::number(mCurrentGenerationID->get()))
				.append( " was created successfully."));
		}
		else {
			Core::log("ClusterEvaluationMethod: Could not compress generation data. Keeping "
				"data uncompressed.");
			mStatusMessageValue->set("ClusterEvaluationMethod: Could not compress generation data. "
				"Keeping data uncompressed.");
		}
		delete zipProcess;
	}
	return ok;
}

bool ClusterEvaluationMethod::reset() {
	bool ok = true;
	mConfigValues.clear();
	mEvalCurrentDirectory = "";
	mJobScriptContent = "";
	mJobScriptLocation = "";
	return ok;
}

void ClusterEvaluationMethod::setCurrentIndividualStartScriptName(const QString &fullScriptPathName) {
	mCurrentStartScriptFullFileName->set(fullScriptPathName);
}

QString ClusterEvaluationMethod::getCurrentIndividualStartScriptName() const {
	return mCurrentStartScriptFullFileName->get();
}

bool ClusterEvaluationMethod::createConfigList() {
	bool ok = true;
	mConfigValues.append(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES);
	return ok;
}

/**
 * Creates the execution command for submitting the evaluation job and submits the job to the sun grid engine.
 * @param startIndex: Index of first evaluation group to be evaluated.
 * @param endIndex: Index of last evaluation group to be evaluated.
 * @return 
 */
bool ClusterEvaluationMethod::submitJob(int startIndex, int endIndex) {
	bool ok = true;

	QString outputDirectory = mEvalCurrentDirectory;
	if(!outputDirectory.endsWith("/")) {
		outputDirectory.append("/");
	}
	outputDirectory.append("QOutput");
	if(!mCore->enforceDirectoryPath(outputDirectory)) {
		Core::log("ClusterEvaluationMethod: Directory to store Grid Engine output could "
			"not be created/found. Writing Grid Engine output to \"/dev/null\"!");
		outputDirectory = "/dev/null";
	}
	QString jobName = mJobScriptName->get();
	jobName.append(QString::number(mCurrentGenerationID->get()));
	
	QStringList parameter;
	parameter << mQSubScriptName->get();
	parameter << QString::number(startIndex);
	parameter << QString::number(endIndex);
	parameter << mJobScriptLocation;
	parameter << jobName;
	parameter << outputDirectory;
	parameter << QString::number(mQsubPriority->get());

	QSubRunner submitJob(parameter);

	//register to ensure that the thread is not destroyed while running.
	Core::getInstance()->registerThread(&submitJob);
	submitJob.start();


	//this event can be used to do some cleanup tasks while the main thread is waiting for 
 	//the evolution job to complete.
	if(mClusterJobSubmitted != 0) {
		mClusterJobSubmitted->trigger();
	}

	while(submitJob.isRunning()) {
		Core::getInstance()->executePendingTasks();

		if(Core::getInstance()->isShuttingDown()) {
			break;
		}
		QCoreApplication::instance()->thread()->wait(200);
	}

	Core::getInstance()->deregisterThread(&submitJob);
	return ok;
}

/**
 * Save the jobscript for this evaluation.
 * @return 
 */
bool ClusterEvaluationMethod::saveJobScript() {
	bool ok = true;
	QString directory = mEvalCurrentDirectory;
	if(!mCore->enforceDirectoryPath(directory)) {
		Core::log("ClusterEvaluationMethod: Could not save job script! Directory"
			" could not be created/found.");
		return false;
	}
	if(!directory.endsWith("/")) {
		directory.append("/");
	}
	QString fileName = directory + mJobScriptName->get();
	mJobScriptLocation = fileName;

	QFile file(fileName);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		Core::log(QString("Could not open file ").append(fileName).append(" to write JobScript."));
		file.close();
		return false;
	}
	QTextStream output(&file);
	output <<  mJobScriptContent << "\n";
	file.setPermissions(file.permissions() | QFile::ExeOwner | QFile::ExeGroup);
	file.close();

	mCurrentStartScriptFullFileName->set(fileName);
	//Core::log("Setting script: " + mCurrentStartScriptFullFileName->get(), true);

	return ok;
}

bool ClusterEvaluationMethod::saveConfigFile() {
	bool ok = true;
	QString directoryName = mEvalCurrentDirectory;
	if(!directoryName.endsWith("/")) {
		directoryName.append("/");
	}
	if(!mCore->enforceDirectoryPath(directoryName)) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Could not enforce config-file path!");
		Core::log("ClusterEvaluationMethod: Could not save config file!");
		return false;
	}
	QString fileName = directoryName + mConfigFileName->get();
	ok = mCore->getValueManager()->saveValues(fileName, mConfigValues,
		QString("Evaluation settings for Generation: ")
		.append(mCurrentGenerationID->getValueAsString())
		.append(" of Evolution: ")
		.append(mWorkingDir->get()).append("!"));
	if(!ok) {
		mStatusMessageValue->set("ClusterEvaluationMethod: Could not save config.val file.");
		Core::log("ClusterEvaluationMethod: Could not save config.val file.");
	}
	return ok;
}

/**
 * sequentially submits the evaulation jobs for all evaluation groups, where no fitness result was returned yet. 
 * @return 
 */
bool ClusterEvaluationMethod::reSubmitJobs() {
	TRACE("EvolutionManager::reSubmitJobs");

	QString outputDirectory = mEvalCurrentDirectory;
	if(!outputDirectory.endsWith("/")) {
		outputDirectory.append("/");
	}
	outputDirectory.append("QOutput");
	if(!mCore->enforceDirectoryPath(outputDirectory)) {
		Core::log("ClusterEvaluationMethod: Directory to store Grid Engine output could "
			"not be created/found. Writing Grid Engine output to \"/dev/null\"!");
		outputDirectory = "/dev/null";
	}
	QString jobName = mJobScriptName->get();
	jobName.append(QString::number(mCurrentGenerationID->get()));
	QList<QSubRunner*> runningThreads;

	for(int i = 0; i < mOpenEvaluations.size(); i++) {

		mStatusMessageValue->set(QString("Resubmitting evaluation group %1")
			.arg(QString::number(mOpenEvaluations.at(i))));
		int index = mOpenEvaluations.at(i);
		
		QStringList parameter;
		parameter << mQSubScriptName->get();
		parameter << QString::number(index);
		parameter << QString::number(index);
		parameter << mJobScriptLocation;
		parameter << jobName;
		parameter << outputDirectory;
		parameter << QString::number(mQsubPriority->get());
	
		QSubRunner *submitJob = new QSubRunner(parameter);
		submitJob->start();
		runningThreads.push_back(submitJob);
	}
	bool running = true;
	while(running) {
		running = false;
		
		Core::getInstance()->executePendingTasks();

		if(Core::getInstance()->isShuttingDown()) {
			break;
		}

		for(int i = 0; i < runningThreads.size(); i++) {
			if(runningThreads.at(i)->isRunning()) {
				running = true;
				break;
			}
		}

		QCoreApplication::instance()->thread()->wait(200);
	}
	return true;
}

}
