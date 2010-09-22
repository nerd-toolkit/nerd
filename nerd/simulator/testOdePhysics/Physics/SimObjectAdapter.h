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

#ifndef NERDSimObjectAdapter_H
#define NERDSimObjectAdapter_H

#include "Physics/SimObject.h"

namespace nerd {

/**
	* SimObjectAdapter
	*/
class SimObjectAdapter : public SimObject {

	public:
		SimObjectAdapter(const QString &name, const QString &prefix)
				: SimObject(name, prefix), mCountValueChanged(0), 
					mLastChangedValue(0) {}
					
		SimObjectAdapter(const SimObjectAdapter &other) : Object(), ValueChangedListener(), 
								SimObject(other) 
		{
		}
		
		virtual void valueChanged(Value *value) {
			mCountValueChanged++;
			mLastChangedValue = value;
		}
		
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa) {
			mCountSynchronizeWithPhysicalModel++;
			mLastSimulationAlgorithm = psa;
		}
		
		void setInputValues(const QList<InterfaceValue*> &values) {
			mInputValues = values;
		}
		
		void setOutputValues(const QList<InterfaceValue*> &values) {
			mOutputValues = values;
		}
		
		void setInfoValues(const QList<InterfaceValue*> &values) {
			mInfoValues = values;
		}
		
	public:
		int mCountValueChanged;
		Value *mLastChangedValue;
		int mCountSynchronizeWithPhysicalModel;
		PhysicalSimulationAlgorithm* mLastSimulationAlgorithm;

};

}

#endif


