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


#ifndef ORCSODE_PID_PassiveActuatorModel_H
#define ORCSODE_PID_PassiveActuatorModel_H

#include <QString>
#include <QHash>
#include "Physics/ODE_Joint.h"
#include "MotorModels/PID_PassiveActuatorModel.h"

namespace nerd {

	/**
	 * ODE_PID_PassiveActuatorModel.
	 *
	 */
	class ODE_PID_PassiveActuatorModel : public PID_PassiveActuatorModel, public ODE_Joint {
	public:
		ODE_PID_PassiveActuatorModel(const QString &name);
		ODE_PID_PassiveActuatorModel(const ODE_PID_PassiveActuatorModel &other);
		virtual ~ODE_PID_PassiveActuatorModel();

		virtual SimObject* createCopy() const;

		virtual void valueChanged(Value *value);
		virtual void setup();
		virtual void clear();
		virtual dJointID createJoint(dBodyID body1, dBodyID body2);
	
		virtual void updateInputValues();
		virtual void updateOutputValues();
	
	protected:
		dJointID mHingeJoint;

	private:
	};

}

#endif



