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


#ifndef NERDTestActuator_H
#define NERDTestActuator_H

#include "Physics/SimObject.h"
#include <iostream>

namespace nerd {

	/**
	 * TestActuator
	 */
	class TestActuator : public SimObject {
	public:
		TestActuator(const QString &name) : SimObject(name) { };
		virtual ~TestActuator() { }; 
		
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm*) {
			std::cout << getName().toStdString().c_str() << " Input: ";
			for(int i = 0; i < mInputValues.size(); ++i) {
				std::cout << mInputValues.at(i)->getNormalized() << ",";
			}
			std::cout << std::endl;
			std::cout << "Output: ";
			for(int i = 0; i < mOutputValues.size(); ++i) {
				std::cout << mOutputValues.at(i)->getNormalized() << ",";
				//count up in outputs.
				mOutputValues.at(i)->setNormalized(mOutputValues.at(i)->getNormalized() + 0.001);
			}
		};
		virtual void setup() {};
		virtual void clear() {};
		

	public:	

	};

}

#endif


