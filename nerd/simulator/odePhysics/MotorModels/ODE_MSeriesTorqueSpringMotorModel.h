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

#ifndef NERDODE_MSeriesTorqueSpringMotorModel_H
#define NERDODE_MSeriesTorqueSpringMotorModel_H

#include <QString>
#include <QHash>
#include "MotorModels/MotorModel.h"
#include "MotorModels/H_MSeriesTorqueSpringMotorModel.h"
#include "Physics/ODE_Joint.h"
#include "Value/Vector3DValue.h"
#include "Value/InterfaceValue.h"

namespace nerd {

	/**
	 * ODE_MSeriesTorqueSpringMotorModel.
	 *
	 */
	class ODE_H_MSeriesTorqueSpringMotorModel : public H_MSeriesTorqueSpringMotorModel,
										   public ODE_Joint 
	{
	public:
		ODE_H_MSeriesTorqueSpringMotorModel(const QString &name);
		ODE_H_MSeriesTorqueSpringMotorModel(const ODE_H_MSeriesTorqueSpringMotorModel &other);
		virtual ~ODE_H_MSeriesTorqueSpringMotorModel();

		virtual SimObject* createCopy() const;

		virtual void setup(); 
		virtual void clear();
	
		virtual void updateInputValues();
		virtual void updateOutputValues();
	
		virtual dJointID createJoint(dBodyID body1, dBodyID body2);

	};

}

#endif



