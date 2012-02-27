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


#ifndef NERDSimObjectAdapter_H
#define NERDSimObjectAdapter_H

#include "Physics/SimObject.h"

namespace nerd {

/**
 * SimObjectAdapter
 */
class SimObjectAdapter : public SimObject {

	public:
		SimObjectAdapter(const QString &name, const QString &prefix, bool *destroyFlag = 0)
				: SimObject(name, prefix), mCountValueChanged(0), 
					mLastChangedValue(0), mDestroyFlag(destroyFlag) {}
					
		SimObjectAdapter(const SimObjectAdapter &other) 
			: Object(), ValueChangedListener(), SimObject(other),
			  mCountValueChanged(0), mLastChangedValue(0), mDestroyFlag(0)
		{
		}
		virtual ~SimObjectAdapter() {
			if(mDestroyFlag != 0) {
				*mDestroyFlag = true;
			}
		}

		virtual SimObject* createCopy() const {
			return new SimObjectAdapter(*this);
		}
		
		virtual void valueChanged(Value *value) {
			SimObject::valueChanged(value);
			mCountValueChanged++;
			mLastChangedValue = value;
		}
		
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
			mCountSynchronizeWithPhysicalModel++;
			mLastSimulationAlgorithm = psa;
		}
		
		void setInputValues(QList<InterfaceValue*> values) {
			mInputValues.clear();
			mInputValues += QList<InterfaceValue*>(values);
			updateInterfaceNames();
		}
		
		void setOutputValues(QList<InterfaceValue*> values) {
			mOutputValues.clear();
			mOutputValues += QList<InterfaceValue*>(values);
			updateInterfaceNames();
		}
		
		void setInfoValues(QList<InterfaceValue*> values) {
			mInfoValues.clear();
			mInfoValues += QList<InterfaceValue*>(values);
			updateInterfaceNames();
		}
		
	public:
		int mCountValueChanged;
		Value *mLastChangedValue;
		int mCountSynchronizeWithPhysicalModel;
		PhysicalSimulationAlgorithm* mLastSimulationAlgorithm;
		bool *mDestroyFlag;

};

}

#endif


