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



#include "TransferFunctionSigmoid.h"
#include <iostream>
#include <QList>
#include <math.h>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new TransferFunctionSigmoid.
 */
TransferFunctionSigmoid::TransferFunctionSigmoid()
	: TransferFunction("sigmoid", 0.0, 1.0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the TransferFunctionSigmoid object to copy.
 */
TransferFunctionSigmoid::TransferFunctionSigmoid(const TransferFunctionSigmoid &other) 
	: Object(), ValueChangedListener(), ObservableNetworkElement(other), TransferFunction(other)
{
}

/**
 * Destructor.
 */
TransferFunctionSigmoid::~TransferFunctionSigmoid() {
}


TransferFunction* TransferFunctionSigmoid::createCopy() const {
	return new TransferFunctionSigmoid(*this);
}


void TransferFunctionSigmoid::reset(Neuron*) {
}


double TransferFunctionSigmoid::transferActivation(double activation, Neuron*) {
	return (1.0 / (1 + ::exp(-1 * activation)));
}

bool TransferFunctionSigmoid::equals(TransferFunction *transferFunction) const {
	if(TransferFunction::equals(transferFunction) == false) {
		return false;
	}

	TransferFunctionSigmoid *tf = 
			dynamic_cast<TransferFunctionSigmoid*>(transferFunction);

	if(tf == 0) {
		return false;
	}
	return true;
}

}



