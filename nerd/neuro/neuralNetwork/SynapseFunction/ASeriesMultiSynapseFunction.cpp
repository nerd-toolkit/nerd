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


#include "ASeriesMultiSynapseFunction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/ASeriesFunctions.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ASeriesMultiSynapseFunction.
 */
ASeriesMultiSynapseFunction::ASeriesMultiSynapseFunction()
	: SynapseFunction("ASeriesMulti")
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ASeriesMultiSynapseFunction object to copy.
 */
ASeriesMultiSynapseFunction::ASeriesMultiSynapseFunction(const ASeriesMultiSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other)
{
}

/**
 * Destructor.
 */
ASeriesMultiSynapseFunction::~ASeriesMultiSynapseFunction() {
}

SynapseFunction* ASeriesMultiSynapseFunction::createCopy() const {
	return new ASeriesMultiSynapseFunction(*this);
}

/**
 * Does nothing in this implementation.
 */
void ASeriesMultiSynapseFunction::reset(Synapse*) {
}

/**
 * Returns the strength of the owner synapse truncated to the
 * precision of the A-Series weight value.
 * 
 * @param owner the owner of this SynapseFunction.
 * @return the truncated strength of the owner.
 */
double ASeriesMultiSynapseFunction::calculate(Synapse *owner) {
	if(owner == 0) {
		return 0.0;
	}
	double strength = owner->getStrengthValue().get();
	return ASeriesFunctions::mapDoubleToFixedPoint_4_9_MultipartPrecision(strength);
}


bool ASeriesMultiSynapseFunction::equals(SynapseFunction *synapseFunction) const {
	if(SynapseFunction::equals(synapseFunction) == false) {
		return false;
	}

	ASeriesMultiSynapseFunction *af = dynamic_cast<ASeriesMultiSynapseFunction*>(synapseFunction);

	if(af == 0) {
		return false;
	}
	return true;
}


}



