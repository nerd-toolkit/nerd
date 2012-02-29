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

#include "CloneNetworkOperator.h"
#include "Network/NeuralNetwork.h"
#include <iostream>
#include "Util/Tracer.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {

CloneNetworkOperator::CloneNetworkOperator(const QString &name)
	: NeuralNetworkManipulationOperator(name, false)
{
	TRACE("CloneNetworkOperator::CloneNetworkOperator");
	getHiddenValue()->set(true);
}

CloneNetworkOperator::CloneNetworkOperator(const CloneNetworkOperator &other)
	: Object(), ValueChangedListener(), NeuralNetworkManipulationOperator(other)
{
	TRACE("CloneNetworkOperator::CloneNetworkOperatorCopy");
}


CloneNetworkOperator::~CloneNetworkOperator() {
	TRACE("CloneNetworkOperator::~CloneNetworkOperator");
}

NeuralNetworkManipulationOperator* CloneNetworkOperator::createCopy() const {
	TRACE("CloneNetworkOperator::createCopy");

	return new CloneNetworkOperator(*this);
}

bool CloneNetworkOperator::applyOperator(Individual *individual, CommandExecutor*) {
	TRACE("CloneNetworkOperator::applyOperator");

	if(individual == 0) {
		return false;
	}

	//do nothing if there is no parent to copy from (is ok)
	if(individual->getParents().empty() || individual->getGenome() != 0) {
		return true;
	}

	Individual *parent = individual->getParents().value(0);

	if(parent == 0) {
		return true;
	}

	NeuralNetwork *parentNet = dynamic_cast<NeuralNetwork*>(parent->getGenome());

	//if the parent individual does not have a NeuralNetwork as genome, 
	//stop processing (not ok).
	if(parentNet == 0) {
		return false;
	}

// 	if(individual->getGenome() != 0) {
// 		delete individual->getGenome();
// 		individual->setGenome(0);
// 	}

	individual->setGenome(parentNet->createCopy());

	return true;
}


}


