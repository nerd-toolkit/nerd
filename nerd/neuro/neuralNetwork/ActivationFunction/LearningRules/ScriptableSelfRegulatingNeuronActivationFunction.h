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


#ifndef NERDScriptableSelfRegulatingNeuronActivationFunction_H
#define NERDScriptableSelfRegulatingNeuronActivationFunction_H

#include <QString>
#include <QHash>
#include "ActivationFunction/LearningRules/SelfRegulatingNeuronActivationFunction.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include <QScriptEngine>
#include <QObject>

namespace nerd {

	/**
	 * ScriptableSelfRegulatingNeuronActivationFunction.
	 *
	 */
	class ScriptableSelfRegulatingNeuronActivationFunction 
			:  public virtual QObject,
			   public virtual SelfRegulatingNeuronActivationFunction
	{
		Q_OBJECT
		
		Q_PROPERTY(QString stringBuffer WRITE setStringBuffer READ getStringBuffer);
		
	public:
		ScriptableSelfRegulatingNeuronActivationFunction();
		ScriptableSelfRegulatingNeuronActivationFunction(const ScriptableSelfRegulatingNeuronActivationFunction &other);
		virtual ~ScriptableSelfRegulatingNeuronActivationFunction();

		virtual ActivationFunction* createCopy() const;

		virtual void reset(Neuron *owner);
		virtual double calculateActivation(Neuron *owner);

		bool equals(ActivationFunction *activationFunction) const;
		
	public slots:
		
		double tf(double activation);
		
		void setStringBuffer(const QString &string);
		QString getStringBuffer() const;
		
	protected:
		virtual double getReceptorStrengthUpdate(double activation);
		virtual double getTransmitterStrengthUpdate(double activation);
		virtual void setupScriptingContext(double activation);

	protected:	
		StringValue *mReceptorEquation;
		StringValue *mTransmitterEquation;
		
		QScriptEngine *mEquationScript;
		QString mVariableBuffer;
	};

}

#endif











