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

#include "NeuralNetworkManipulationOperator.h"
#include "NeuralNetworkManipulationChain/NeuralNetworkManipulationChainAlgorithm.h"
#include <QTime>
#include <limits>
#include "Core/Core.h"


using namespace std;

namespace nerd {

NeuralNetworkManipulationOperator::NeuralNetworkManipulationOperator(const QString &name)
	: ParameterizedObject(name), mOwner(0), mCumulatedTime(0), mMaxTime(0), mMinTime(0),
	  mExecCounter(0)
{
	mMaximalNumberOfApplications = new IntValue(15);
	mOperatorIndex = new IntValue(10);
	mEnableOpeatorValue = new BoolValue(true);
	mDocumentation = new StringValue("No documentation available.");
	mCumulatedExecutionTime = new IntValue(0);
	mLastSingleExecutionTime = new IntValue(0);
	mMaxExecutionTime = new IntValue(0);
	mMinExecutionTime = new IntValue(0);
	mExecutionCount = new IntValue(0);
	
	addParameter("Config/MaximalNumberOfApplications", mMaximalNumberOfApplications);
	addParameter("Config/OperatorIndex", mOperatorIndex);
	addParameter("Config/Enable", mEnableOpeatorValue);
	addParameter("Config/Doc", mDocumentation);
	addParameter("Performance/CumulatedTime", mCumulatedExecutionTime);
	addParameter("Performance/LastExecutionTime", mLastSingleExecutionTime);
	addParameter("Performance/MaxTime", mMaxExecutionTime);
	addParameter("Performance/MinTime", mMinExecutionTime);
	addParameter("Performance/ExecutionCount", mExecutionCount);

	Core::log(QString("Added operator: ") + name);

}

NeuralNetworkManipulationOperator::NeuralNetworkManipulationOperator(
			const NeuralNetworkManipulationOperator &other) 
	: ParameterizedObject(other), mOwner(0), mCumulatedTime(0), mMaxTime(0), mMinTime(0),
	  mExecCounter(0)
{
	mMaximalNumberOfApplications = dynamic_cast<IntValue*>(getParameter("Config/MaximalNumberOfApplications"));
	mOperatorIndex = dynamic_cast<IntValue*>(getParameter("Config/OperatorIndex"));
	mEnableOpeatorValue = dynamic_cast<BoolValue*>(getParameter("Config/Enable"));
	mDocumentation = dynamic_cast<StringValue*>(getParameter("Config/Doc"));
	mCumulatedExecutionTime = dynamic_cast<IntValue*>(
						getParameter("Performance/CumulatedTime"));
	mLastSingleExecutionTime = dynamic_cast<IntValue*>(
						getParameter("Performance/LastExecutionTime"));
	mMaxExecutionTime = dynamic_cast<IntValue*>(getParameter("Performance/MaxTime"));
	mMinExecutionTime = dynamic_cast<IntValue*>(getParameter("Performance/MinTime"));
	mExecutionCount = dynamic_cast<IntValue*>(getParameter("Performance/ExecutionCount"));

	Core::log("Copied Operator");
}


NeuralNetworkManipulationOperator::~NeuralNetworkManipulationOperator() {
}


void NeuralNetworkManipulationOperator::resetOperator() {
	mCumulatedExecutionTime->set(mCumulatedTime);
	mMinExecutionTime->set(mMinTime);
	mMaxExecutionTime->set(mMaxTime);
	mExecutionCount->set(mExecCounter);

	mCumulatedTime = 0;
	mMinTime = numeric_limits<int>::max();
	mMaxTime = 0;
	mExecCounter = 0;
}


bool NeuralNetworkManipulationOperator::runOperator(Individual *individual, CommandExecutor *executor) {
	
	QTime time;
	time.start();

	bool status = applyOperator(individual, executor);

	int duration = time.elapsed();
	mCumulatedTime += duration;
	++mExecCounter;
	if(mMaxTime < duration) {
		mMaxTime = duration;
	}
	if(mMinTime > duration) {
		mMinTime = duration;
	}
	mLastSingleExecutionTime->set(duration);
	return status;
}

void NeuralNetworkManipulationOperator::setOwnerAlgorithm(NeuralNetworkManipulationChainAlgorithm *algorithm) {
	mOwner = algorithm;
}


NeuralNetworkManipulationChainAlgorithm* NeuralNetworkManipulationOperator::getOwnerAlgorithm() const {
	return mOwner;
}


IntValue* NeuralNetworkManipulationOperator::getMaximalNumberOfApplicationsValue() const {
	return mMaximalNumberOfApplications;
}


IntValue* NeuralNetworkManipulationOperator::getOperatorIndexValue() const {
	return mOperatorIndex;
}

BoolValue* NeuralNetworkManipulationOperator::getEnableOperatorValue() const {
	return mEnableOpeatorValue;
}

StringValue* NeuralNetworkManipulationOperator::getDocumentationValue() const {
	return mDocumentation;
}

IntValue* NeuralNetworkManipulationOperator::getLastExecutionTimeValue() const {
	return mLastSingleExecutionTime;
}

}


