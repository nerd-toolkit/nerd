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

#include "MSeriesActivationFunction.h"
#include "Math/MSeriesFunctions.h"
#include <QListIterator>
#include "Network/Synapse.h"
#include "Network/Neuron.h"

namespace nerd {

MSeriesActivationFunction::MSeriesActivationFunction()
	: ActivationFunction("MSeries")
{
}

MSeriesActivationFunction::MSeriesActivationFunction(
			const MSeriesActivationFunction &other)
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), ActivationFunction(other)
{
}

MSeriesActivationFunction::~MSeriesActivationFunction() {
}

ActivationFunction* MSeriesActivationFunction::createCopy() const {
	return new MSeriesActivationFunction(*this);
}

void MSeriesActivationFunction::reset(Neuron*) {
}


double MSeriesActivationFunction::calculateActivation(Neuron *owner) {
	if(owner == 0) {
		return 0.0;
	}
	double activation = owner->getBiasValue().get();

	// Convert activation value to ACCU-Value of A-Series with 8 bits integer part 
	// and 24 bits fractional part
	int32_t fpActivation = MSeriesFunctions::doubleToFixedPoint_17_15(activation);

	// Sum up converted activation values of all synapses
	QList<Synapse*> synapses = owner->getSynapses();
	for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
		fpActivation += MSeriesFunctions::doubleToFixedPoint_17_15(i.next()->calculateActivation());
	}

	// Convert the final activation value back to double
	return MSeriesFunctions::fixedPoint_17_15_ToDouble(fpActivation);
}


bool MSeriesActivationFunction::equals(ActivationFunction *activationFunction) const {
	if(ActivationFunction::equals(activationFunction) == false) {
		return false;
	}
	MSeriesActivationFunction *af = 
			dynamic_cast<MSeriesActivationFunction*>(activationFunction);

	if(af == 0) {
		return false;
	}
	return true;
}

}


