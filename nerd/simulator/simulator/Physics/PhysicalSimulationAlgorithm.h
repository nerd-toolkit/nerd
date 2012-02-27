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

#ifndef NERDPhysicalSimulationAlgorithm_H
#define NERDPhysicalSimulationAlgorithm_H

#include "Core/ParameterizedObject.h"
#include <QString>
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

namespace nerd {

	class PhysicsManager;

	/**
	 * PhysicalSimulationAlgorithm: Base class for simulation algorithms of the used physics-engine. The three abstract methods need to implement how to reset the physic, how to execute a single simulation step (including collision handling and creation of the physics independent collision information) and how to finalize the setup after a reset. The method finalizeReset implements initializations of the physcis, which need to be done, after setup for all SimObjects was called.
	 */
	class PhysicalSimulationAlgorithm : public ParameterizedObject {
	public:
		PhysicalSimulationAlgorithm(const QString &name);
		virtual ~PhysicalSimulationAlgorithm();
		
		virtual bool resetPhysics() = 0;
		virtual bool executeSimulationStep(PhysicsManager *pmanager) = 0;
		virtual bool finalizeSetup() = 0;

		int getIterationsPerStep() const;
		double getTimeStepSize() const;

	protected:
		IntValue *mIterationsPerStepValue;
		DoubleValue *mGravitationValue;
		DoubleValue *mTimeStepSizeValue;

		Event *mUpdateSensorsEvent;
		Event *mUpdateMotorsEvent;

	};

}

#endif


