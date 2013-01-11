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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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


#ifndef NERDScriptableConstraint_H
#define NERDScriptableConstraint_H

#include "Constraints/GroupConstraint.h"
#include <QObject>
#include <QScriptEngine>
#include "Value/StringValue.h"
#include "Value/CodeValue.h"
#include "Script/ScriptedNetworkManipulator.h"
#include "Script/ScriptingContext.h"
#include "Value/DoubleValue.h"


namespace nerd {

	/**
	 * ScriptableConstraint
	 */
	class ScriptableConstraint : public virtual ScriptingContext, public GroupConstraint {
		Q_OBJECT

	public:
		ScriptableConstraint();
		ScriptableConstraint(const ScriptableConstraint &other);
		virtual ~ScriptableConstraint();

		virtual GroupConstraint* createCopy() const;
		virtual QString getName() const;
		
		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);
		
		virtual void reset();
		virtual void resetScriptContext();

		virtual bool isValid(NeuronGroup *owner);
		virtual bool applyConstraint(NeuronGroup *owner, CommandExecutor *executor, 
									 QList<NeuralNetworkElement*> &trashcan);
		
		virtual bool applyConstraint();
		
// 		virtual void reset(Neuron *owner);
// 		virtual double calculateActivation(Neuron *owner);

		virtual bool equals(GroupConstraint *constraint) const;
		
		virtual void setErrorMessage(const QString &message);
		virtual void setWarningMessage(const QString &message);
		
	protected:
		virtual void reportError(const QString &message);
		virtual void addCustomScriptContextStructures();
		virtual void importVariables();
		virtual void exportVariables();
		
	private:
		ScriptedNetworkManipulator *mNetworkManipulator;
		StringValue *mErrorState;
		NeuronGroup *mOwner;
		
		DoubleValue *mVar1;
		DoubleValue *mVar2;
		StringValue *mVar3;
		StringValue *mVar4;
		
		BoolValue mReturnValue;
		
		bool mFirstExecution;
		
		BoolValue *mActiveConstraint;
		StringValue *mNameValue;
		
		Event *mNextStepEvent;
	};

}

#endif


