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

#include "SliderJoint.h"
#include <limits>


namespace nerd {

/**
 * Constructor. Initializes the axes as (0,0,0) and the min and max position on the minium and maximum.
 * @param name 
 */
SliderJoint::SliderJoint(const QString &name) : SimJoint(name) {

	mFirstAxisPoint = new Vector3DValue();
	mSecondAxisPoint = new Vector3DValue();
	mMinPositionValue = new DoubleValue(-1 * std::numeric_limits<double>::max());
	mMaxPositionValue = new DoubleValue(std::numeric_limits<double>::max());
	
	addParameter("AxisPoint1", mFirstAxisPoint);
	addParameter("AxisPoint2", mSecondAxisPoint);

	mAxisPoints.push_back(mFirstAxisPoint);
	mAxisPoints.push_back(mSecondAxisPoint);
	
	addParameter("MinPosition", mMinPositionValue);
	addParameter("MaxPosition", mMaxPositionValue);

	mFrictionValue = new DoubleValue();
	addParameter("FrictionFMax", mFrictionValue);
}

SliderJoint::SliderJoint(const SliderJoint &copy) : Object(), ValueChangedListener(), 
		SimJoint(copy) 
{
	mFirstAxisPoint = dynamic_cast<Vector3DValue*>(getParameter("AxisPoint1"));
	mSecondAxisPoint = dynamic_cast<Vector3DValue*>(getParameter("AxisPoint2"));
	mMinPositionValue = dynamic_cast<DoubleValue*>(getParameter("MinPosition"));
	mMaxPositionValue = dynamic_cast<DoubleValue*>(getParameter("MaxPosition"));

	mAxisPoints.push_back(mFirstAxisPoint);
	mAxisPoints.push_back(mSecondAxisPoint);

	mFrictionValue = dynamic_cast<DoubleValue*>(getParameter("FrictionFMax"));
}

SliderJoint::~SliderJoint() {
}

SimObject* SliderJoint::createCopy() const {
	return new SliderJoint(*this);
}
		
}
