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



#ifndef NERDSpringAdapter_H
#define NERDSpringAdapter_H

#include <QString>
#include <QHash>
#include "Physics/MotorAdapter.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "Physics/SimJoint.h"
#include "MotorModels/MotorModel.h"
#include "Physics/SimObject.h"

namespace nerd {

	/**
	 * SpringAdapter.
	 *
	 */
	class SpringAdapter : public MotorAdapter, public SimObject {
	public:
		SpringAdapter(MotorModel::JointType type, const QString &name);
		SpringAdapter(const SpringAdapter &other);
		virtual ~SpringAdapter();

		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa);
		virtual void setup();
		virtual void clear();

		MotorModel::JointType getType() const;
		SimJoint* getCurrentTargetJoint() const;
		DoubleValue* getCurrentTorqueValue() const;

		virtual SimObject* createCopy() const;

		virtual QString getAbsoluteName() const;

		virtual bool addParameter(const QString &name, Value *value);
		virtual Value* getParameter(const QString &name) const;
		

		virtual QList<InterfaceValue*> getInputValues() const;
		virtual QList<InterfaceValue*> getOutputValues() const;

		

	protected:		
		virtual bool isValidMotorModel(MotorModel *model) const;

	protected:
		MotorModel::JointType mType;
		DoubleValue *mCurrentTorque;
		StringValue *mTargetJointName;
		SimJoint *mTargetJoint;

	};

}

#endif



