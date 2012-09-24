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


#ifndef NERDScriptableActivationFunction_H
#define NERDScriptableActivationFunction_H

#include "ActivationFunction/NeuroModulatorActivationFunction.h"
#include <QObject>
#include <QScriptEngine>
#include "Value/StringValue.h"
#include "Value/CodeValue.h"
#include "Script/ScriptedNetworkManipulator.h"
#include "Script/ScriptingContext.h"
#include "Value/DoubleValue.h"


namespace nerd {

	/**
	 * ScriptableActivationFunction
	 */
	class ScriptableActivationFunction : public ScriptingContext, 
										  public NeuroModulatorActivationFunction 
	{
		Q_OBJECT

	public:
		ScriptableActivationFunction();
		ScriptableActivationFunction(const ScriptableActivationFunction &other);
		virtual ~ScriptableActivationFunction();

		virtual ActivationFunction* createCopy() const;
		virtual QString getName() const;
		virtual void valueChanged(Value *value);
		
		virtual void resetScriptContext();

		virtual void reset(Neuron *owner);
		virtual double calculateActivation(Neuron *owner);

		virtual bool equals(ActivationFunction *activationFunction) const;
		
	public slots: 
		
	protected:
		virtual void reportError(const QString &message);
		virtual void addCustomScriptContextStructures();
		virtual void importVariables();
		virtual void exportVariables();
		
	private:
		//QString mVariableBuffer;
		ScriptedNetworkManipulator *mNetworkManipulator;
		StringValue *mErrorState;
		Neuron *mOwner;
		
		DoubleValue *mVar1;
		DoubleValue *mVar2;
		StringValue *mVar3;
		StringValue *mVar4;
		
		DoubleValue mActivation;
		
		bool mFirstExecution;
		
	};

}

#endif


