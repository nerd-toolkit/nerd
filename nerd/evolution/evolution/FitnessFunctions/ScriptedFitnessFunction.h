/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Project homepage: nerd.x-bot.org                                      *
 *                                                                         *
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/



#ifndef NERDScriptedFitnessFunction_H
#define NERDScriptedFitnessFunction_H

#include <QString>
#include <QHash>
#include <QScriptEngine>
#include <QObject>
#include "PlugIns/CommandLineArgument.h"
#include <QVariantList>
#include "Script/ScriptingContext.h"
#include "Fitness/ControllerFitnessFunction.h"


namespace nerd {

	/**
	 * ScriptedFitnessFunction.
	 *
	 */
	class ScriptedFitnessFunction : public ScriptingContext, public ControllerFitnessFunction
	{
	Q_OBJECT

	Q_PROPERTY(double fitness WRITE setFitnessBuffer READ getFitnessBuffer);

	public:
		ScriptedFitnessFunction(const QString &name);
		ScriptedFitnessFunction(const ScriptedFitnessFunction &other);
		virtual ~ScriptedFitnessFunction();

		virtual FitnessFunction* createCopy() const;

		virtual QString getName() const;

		virtual void attachToSystem();
		virtual void detachFromSystem();

		virtual void setControlInterface(ControlInterface *controlInterface);

		virtual void valueChanged(Value *value);	
		virtual void eventOccured(Event *event);

		virtual void resetScriptContext();
		virtual double calculateCurrentFitness();

		virtual void prepareNextTry();

	public slots:
		void terminateCurrentTry();
		double getDoubleValue(const QString &valueName);
		QString getStringValue(const QString &valueName);

		void setFitnessBuffer(double currentFitness);
		double getFitnessBuffer() const;

	protected:
		virtual void reportError(const QString &message);
		virtual void addCustomScriptContextStructures();

	protected:
		double mFitnessBuffer;
		CommandLineArgument *mScriptFileNameArg;
		Event *mSimEnvironmentChangedEvent;
		Event *mBindPhaseEvent;
	};

}

#endif
