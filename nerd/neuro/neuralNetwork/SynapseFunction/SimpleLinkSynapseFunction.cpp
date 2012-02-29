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

#include "SimpleLinkSynapseFunction.h"
#include <iostream>

using namespace std;

namespace nerd {


/**
 * Constructor.
 */
SimpleLinkSynapseFunction::SimpleLinkSynapseFunction()
	: SynapseFunction("SimpleLink")
{
}


/**
 * Copy constructor.
 */
SimpleLinkSynapseFunction::SimpleLinkSynapseFunction(const SimpleLinkSynapseFunction &other)
	: Object(), ValueChangedListener(), SynapseFunction(other)
{
}


/**
 * Destructor.
 */
SimpleLinkSynapseFunction::~SimpleLinkSynapseFunction() {
}

SynapseFunction* SimpleLinkSynapseFunction::createCopy() const {
	return new SimpleLinkSynapseFunction(*this);
}


/**
 * Does nothing in this implementation.
 */
void SimpleLinkSynapseFunction::reset(Synapse*) {
}


/**
 * Returns the strength of the owner synapse.
 * 
 * @param owner the owner of this SynapseFunction.
 * @return the strength of the owner.
 */
double SimpleLinkSynapseFunction::calculate(Synapse *owner) {
	return 0.0;
}

bool SimpleLinkSynapseFunction::equals(SynapseFunction *synapseFunction) const {
	if(SynapseFunction::equals(synapseFunction) == false) {
		return false;
	}

	SimpleLinkSynapseFunction *sf = dynamic_cast<SimpleLinkSynapseFunction*>(synapseFunction);

	if(sf == 0) {
		return false;
	}
	return true;
}


}

