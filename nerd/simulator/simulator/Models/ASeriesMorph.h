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


#ifndef ASeriesMorph_H_
#define ASeriesMorph_H_

#include <QList>
#include "ModelInterface.h"
#include "Physics/SimObject.h"
#include "Collision/CollisionObject.h"

namespace nerd {

/**
* Model of the A-series humanoid robot.
**/

class ASeriesMorph : public ModelInterface {

public:
	ASeriesMorph(const QString &groupName = "");
	ASeriesMorph(const ASeriesMorph &model);
	~ASeriesMorph();

	virtual void createModel();
	virtual SimObject* createCopy() const;

protected:
	virtual void layoutObjects();

private:
	bool mUseDynamixels;
	bool mIncludeAccus;

	QString mGroupName;
	QString mNamePrefix;

	SimObject *mRightFoot;
	CollisionObject *mRightSoleOfFootFront;
	CollisionObject *mRightSoleOfFootFrontOut;
	CollisionObject *mRightSoleOfFootBack;
	SimObject *mRightAnkleBox;
	SimObject *mRightTibia;
	SimObject *mRightFemur;
	SimObject *mRightUpperFemur;
	CollisionObject *mRightAccuCase;
	
	SimObject *mLeftFoot;
	CollisionObject *mLeftSoleOfFootFront;
	CollisionObject *mLeftSoleOfFootFrontOut;
	CollisionObject *mLeftSoleOfFootBack;
	SimObject *mLeftAnkleBox;
	SimObject *mLeftTibia;
	SimObject *mLeftFemur;
	SimObject *mLeftUpperFemur;
	CollisionObject *mLeftAccuCase;

	SimObject *mHipPlate;
	SimObject *mLeftHipPart;
	SimObject *mMiddleInvisiblePart;
	CollisionObject *mHipMiddleObject;
	CollisionObject *mRightHipPartObject;
	CollisionObject *mLeftHipPartObject;
	CollisionObject *mHipPlateObject;
	CollisionObject *mAccuCaseObject;
	
	SimObject *mMiddlePda;
	SimObject *mMiddleUpperBody;
	SimObject *mMiddleNeck;
	SimObject *mMiddleHeadBase;
	SimObject *mMiddleHead;

	SimObject *mRightShoulder;
	SimObject *mRightUpperArm;
	SimObject *mRightLowerArm;
	CollisionObject *mRightHandObject;
	
	SimObject *mLeftShoulder;
	SimObject *mLeftUpperArm;
	SimObject *mLeftLowerArm;
	CollisionObject *mLeftHandObject;

	SimObject *mHipMotor;
	SimObject *mRightShoulderMotorRoll;
	SimObject *mLeftShoulderMotorRoll;
	SimObject *mRightShoulderMotor;
	SimObject *mLeftShoulderMotor;
	SimObject *mRightElbowMotor;
	SimObject *mLeftElbowMotor;
	SimObject *mRightHipRoll;
	SimObject *mLeftHipRoll;
	SimObject *mRightHipSide;
	SimObject *mLeftHipSide;
	SimObject *mRightHipFB;
	SimObject *mLeftHipFB;
	SimObject *mRightKnee;
	SimObject *mLeftKnee;
	SimObject *mRightFootUD;
	SimObject *mLeftFootUD;
	SimObject *mRightFootLR;
	SimObject *mLeftFootLR;
	SimObject *mHeadSide;
	SimObject *mHeadUp;

	SimObject *mAbfr;
	SimObject *mAbhr;
	SimObject *mAbsr;
	SimObject *mAbar;
	SimObject *mAbfl;
	SimObject *mAbhl;
	SimObject *mAbsl;
	SimObject *mAbal;
	
};
}
#endif

