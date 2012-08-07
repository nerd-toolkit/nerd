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



#ifndef NERDWavefrontBody_H
#define NERDWavefrontBody_H

#include "SimBody.h"
#include "TriangleGeom.h"
#include "WavefrontGeom.h"
#include "Collision/CollisionObject.h"
#include "Value/FileNameValue.h"

namespace nerd {


/**
 * A WavefrontObject can represent shapes encoded in in the Wavefront format.
 */
class WavefrontBody : public SimBody {

	public:
		WavefrontBody(const QString &name, const QString& filename = "", double scale = 1.0);
		WavefrontBody(const WavefrontBody &body);
		virtual ~WavefrontBody();
    
		virtual SimBody* createCopy() const;
		
		virtual void setup();
		virtual void clear();
		
		virtual void valueChanged(Value *value);

	private:		
		void createWavefrontGeom();
		
	protected:
		DoubleValue *mScale;
		FileNameValue *mFilename;
		StringValue *mReferenceObjectName;
		Vector3DValue *mLocalPosition;
		Vector3DValue *mLocalOrientation;
		SimBody *mReferenceObject;
	
};

}

#endif
