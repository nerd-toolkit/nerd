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

#include "StandardConstraintCollection.h"
#include "Constraints/Constraints.h"
#include "Constraints/ConstraintManager.h"
#include "Constraints/NumberOfNeuronsConstraint.h"
#include "Constraints/NumberOfReadNeuronsConstraint.h"
#include "Constraints/SymmetryConstraint.h"
#include "Constraints/PreventMutualConnectionsConstraint.h"
#include "Constraints/ConnectionSymmetryConstraint.h"
#include "Constraints/WeightAndBiasCalculatorConstraint.h"
#include "Constraints/FeedForwardConstraint.h"
#include "Constraints/BackpropagationConstraint.h"
#include "Constraints/RestrictWeightAndBiasRangeConstraint.h"
#include "Constraints/SynchronizeTagsConstraint.h"

namespace nerd {

StandardConstraintCollection::StandardConstraintCollection()
{

	ConstraintManager *cm = Constraints::getConstraintManager();

	cm->addConstraintPrototype(new NumberOfNeuronsConstraint());
	cm->addConstraintPrototype(new NumberOfReadNeuronsConstraint());
	cm->addConstraintPrototype(new SymmetryConstraint(0, "Clone", true));
	cm->addConstraintPrototype(new SymmetryConstraint());
	cm->addConstraintPrototype(new PreventMutualConnectionsConstraint());
	cm->addConstraintPrototype(new ConnectionSymmetryConstraint());
	cm->addConstraintPrototype(new WeightAndBiasCalculatorConstraint());
	cm->addConstraintPrototype(new FeedForwardConstraint());
	cm->addConstraintPrototype(new BackpropagationConstraint());
	cm->addConstraintPrototype(new RestrictWeightAndBiasRangeConstraint());
	cm->addConstraintPrototype(new SynchronizeTagsConstraint());
}

StandardConstraintCollection::~StandardConstraintCollection() {
}

}


