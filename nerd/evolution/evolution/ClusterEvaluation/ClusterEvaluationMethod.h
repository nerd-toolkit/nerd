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

#ifndef ClusterEvaluationMethod_H_
#define ClusterEvaluationMethod_H_
#include "Evaluation/EvaluationMethod.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"
#include "Core/Core.h"
#include <QList>
#include <QStringList>
#include "Value/BoolValue.h"

namespace nerd {


class QSubRunner : public QThread {

Q_OBJECT

public:
	QSubRunner(const QStringList &qSubCommand);

protected:
	virtual void run();

private:
	QStringList mQSubCommand;

};

/**
 * ClusterEvaluationMethod. Abstract EvaluationMethod-implementation that defines the basics for using the sun grid engine during evaluation. 
**/

class ClusterEvaluationMethod : public EvaluationMethod {
	
	public:
		ClusterEvaluationMethod(const QString &name);
		ClusterEvaluationMethod(const ClusterEvaluationMethod &other);
		virtual ~ClusterEvaluationMethod();
	
		virtual EvaluationMethod* createCopy() = 0;

		virtual bool evaluateIndividuals();
		virtual bool reset();

		void setCurrentIndividualStartScriptName(const QString &fullScriptPathName);
		QString getCurrentIndividualStartScriptName() const;
	
	protected:
		virtual bool prepareEvaluation() = 0;
		virtual bool createConfigList();
		virtual bool createJobScript() = 0;
		virtual bool readEvaluationResults() = 0;		
		bool submitJob(int startIndex, int endIndex);
		bool reSubmitJobs();

	private:
		bool saveConfigFile();	
		bool saveJobScript();

	protected:
		QString mEvalCurrentDirectory;
		QString mJobScriptLocation;
		IntValue *mQsubPriority;
		StringValue *mJobScriptName;
		StringValue *mConfigFileName;
		StringValue *mQSubScriptName;
		IntValue *mCurrentGenerationID;
		IntValue *mNumberOfRetries;

		StringValue *mStatusMessageValue;
		StringValue *mWorkingDir;

		Event *mNextTry;
		Event *mTryCompleted;
		Event *mNextIndividual;
		Event *mIndividualCompleted;
		Event *mClusterJobSubmitted;
		IntValue *mNumberOfTries;
		IntValue *mCurrentTry;
		BoolValue *mZipGenerations;
		StringValue *mCurrentGenerationDirectory;
		StringValue *mCurrentStartScriptFullFileName;
	
		QList<QString> mConfigValues;
		QString mJobScriptContent;
		QList<int> mOpenEvaluations;
		Core *mCore;
};
}
#endif

