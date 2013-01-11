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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "FirstParentsTestSelectionMethod.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Evolution/Population.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new FirstParentsTestSelectionMethod.
 */
FirstParentsTestSelectionMethod::FirstParentsTestSelectionMethod()
	: SelectionMethod("FirstParentsTestSelectionMethod"), mNumberOfParents(0)
{
	mNumberOfParents = new IntValue(2);
	mRequiredNumberOfParents = new IntValue(0);

	addParameter("NumberOfParents", mNumberOfParents);
	addParameter("RequiredNumberOfParents[R]", mRequiredNumberOfParents);
}


/**
 * Copy constructor. 
 * 
 * @param other the FirstParentsTestSelectionMethod object to copy.
 */
FirstParentsTestSelectionMethod::FirstParentsTestSelectionMethod(const FirstParentsTestSelectionMethod &other) 
	: Object(), ValueChangedListener(), SelectionMethod(other)
{
	mNumberOfParents = dynamic_cast<IntValue*>(getParameter("NumberOfParents"));
	mRequiredNumberOfParents = dynamic_cast<IntValue*>(getParameter("RequiredNumberOfParents"));
}

/**
 * Destructor.
 */
FirstParentsTestSelectionMethod::~FirstParentsTestSelectionMethod() {
}

SelectionMethod* FirstParentsTestSelectionMethod::createCopy() const {
	return new FirstParentsTestSelectionMethod(*this);
}


/**
 * Creates the given number of individuals as part of a new generation.
 * 
 * @param numberOfIndividuals the number of individuals to create.
 * @param numberOfPreservedParents hint to preserve up to this number of parents (unchanged parents)
 * @param numberOfParentsPerIndividual the number of parents per new individudal.
 */
QList<Individual*> FirstParentsTestSelectionMethod::createSeed(
		const QList<Individual*> &currentGeneration, int numberOfIndividuals, 
		int numberOfPreservedParents, int numberOfParentsPerIndividual)
{
	//disable warnings of missing variables
	numberOfPreservedParents = numberOfPreservedParents;


	mRequiredNumberOfParents->set(numberOfParentsPerIndividual);

	 QList<Individual*> newGeneration;

	if(mOwnerPopulation == 0) {
		return newGeneration;
	}
	
	const QList<Individual*> &individuals = currentGeneration;

	int numberOfParents = mNumberOfParents->get();

	if(individuals.size() < numberOfParents) {
		return newGeneration;
	}

	QList<Individual*> parents;
	for(int i = 0; i < numberOfParents && i < individuals.size(); ++i) {
		Individual *parent = individuals.at(i);
		parent->protectGenome(true);
		parents.append(parent);
		newGeneration.append(parent);
	}

	for(int i = 0; i < numberOfIndividuals - numberOfParents; ++i) {
		Individual *ind = new Individual();
		for(QListIterator<Individual*> j(parents); j.hasNext();) {
			ind->getParents().append(j.next());
		}
		newGeneration.append(ind);
	}

	return newGeneration;
}


}



