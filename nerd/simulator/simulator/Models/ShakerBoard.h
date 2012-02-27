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

#ifndef ShakerBoard_H_
#define ShakerBoard_H_

#include <QList>
#include "ModelInterface.h"
#include "Signal/SignalSinus.h"
#include "Signal/SignalRandom.h"
#include "Value/BoolValue.h"

namespace nerd {

/**
 * ShakerBoard.
 * ShakerBoard is a board which can be shaked in the X and Z-dimension and rotation.
 * It is driven by linear motors which are driven by either Random or Sinus signals.
 * The signals can be enabled and modified via parameters.
 */
class ShakerBoard : public ModelInterface {

public:
	ShakerBoard(const QString &groupName = "");
	ShakerBoard(const ShakerBoard &model);
	~ShakerBoard();

	virtual void createModel();
	virtual SimObject* createCopy() const;

private:
	InterfaceValue *mDesiredMotorSettingX;
	InterfaceValue *mDesiredMotorSettingY;
	InterfaceValue *mDesiredMotorSettingPitch;
	InterfaceValue *mDesiredMotorSettingRoll;

	SimObject *mSliderMotorX;
	SimObject *mSliderMotorZ;
	SimObject *mDynamixelPitch;
	SimObject *mDynamixelRoll;

	SignalRandom *mRandomSignalX;
	SignalRandom *mRandomSignalZ;
	SignalSinus *mSinusSignalX;
	SignalSinus *mSinusSignalZ;
	SignalRandom *mRandomSignalPitch;
	SignalRandom *mRandomSignalRoll;
	SignalSinus *mSinusSignalPitch;
	SignalSinus *mSinusSignalRoll;
};

}
#endif

