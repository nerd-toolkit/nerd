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



#ifndef NERDUniversalJointMotorAdapter_H
#define NERDUniversalJointMotorAdapter_H

#include <QString>
#include <QHash>
#include "Physics/MotorAdapter.h"
#include "Physics/UniversalJoint.h"
#include "Value/BoolValue.h"
#include "Value/StringValue.h"


namespace nerd {

	class MotorModel;

	/**
	 * UniversalJointMotorAdapter.
	 *
	 */
	class UniversalJointMotorAdapter : public virtual MotorAdapter, 
									   public virtual UniversalJoint 
	{
	public:
		UniversalJointMotorAdapter(const QString &name);
		UniversalJointMotorAdapter(const UniversalJointMotorAdapter &other);
		virtual ~UniversalJointMotorAdapter();

		virtual SimObject* createCopy() const;

		virtual void setup();
		virtual void clear();

		virtual QString getAbsoluteName() const;

		virtual bool addParameter(const QString &name, Value *value);
		virtual Value* getParameter(const QString &name) const;

		virtual const QList<InterfaceValue*>& getInputValues() const;
		virtual const QList<InterfaceValue*>& getOutputValues() const;

		virtual void valueChanged(Value *value);

	protected:		
		virtual bool isValidMotorModel(MotorModel *model) const;

	private:

		DoubleValue *mAxis1MinAngle;
		DoubleValue *mAxis1MaxAngle;
		DoubleValue *mAxis2MinAngle;
		DoubleValue *mAxis2MaxAngle;

		InterfaceValue *mDesiredMotorTorqueValue;
		InterfaceValue *mDesiredMotorAngleValue;
		InterfaceValue *mAngleAxis1Value;
		InterfaceValue *mAngleAxis2Value;
		
	};

}

#endif



