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


#ifndef NERDInternalStateValue_H
#define NERDInternalStateValue_H

#include "Physics/SimObject.h"
#include "Value/NormalizedDoubleValue.h"
#include "Value/RangeValue.h"


namespace nerd {
	
	/**
	 * InternalStateValue.
	 * Provides a NormalizedDoubleValue as part of the animat.
	 * This internal state can be manipulated by other processes and SimObjects, such as
	 * the ValueTransferControllers.
	 */
	class InternalStateValue : public SimObject {
	public:
		InternalStateValue(const QString &name);
		InternalStateValue(const InternalStateValue &other);
		virtual ~InternalStateValue();
		
		virtual SimObject* createCopy() const;
		
		virtual void setup();
		virtual void clear();
		virtual void valueChanged(Value *value);
		
		virtual NormalizedDoubleValue* getInternalStateValue() const;
		
	protected:
		NormalizedDoubleValue *mInternalStateValue;
		//DoubleValue *mInitialValue;
		RangeValue *mValueRange;
	};
	
}

#endif


