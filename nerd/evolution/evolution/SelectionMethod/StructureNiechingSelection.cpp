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



#include "StructureNiechingSelection.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Random.h"
#include "Evolution/Population.h"
#include "Math/Math.h"
#include <QListIterator>
#include "Fitness/FitnessFunction.h"
#include "Core/Core.h"
#include <iostream>
#include "Math/IndividualSorter.h"
#include "Util/Tracer.h"
#include <QStringList>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

using namespace std;

namespace nerd {


/**
 * Constructs a new StructureNiechingSelection.
 */
StructureNiechingSelection::StructureNiechingSelection(const SelectionMethod &selectionMethod)
	: SelectionMethod("StructureNiechingSelection"), mSelectionMethod(selectionMethod.createCopy()),
	  mNiecheCounter(0)
{
	mNumberOfSeparateNieches = new IntValue(0);
	mNumberOfCompetingNieches = new IntValue(1);
	mQuarantineTime = new IntValue(5);

	addParameter("NumberOfSeparateNieches", mNumberOfSeparateNieches);
	addParameter("NumberOfCompetingNieches", mNumberOfCompetingNieches);
	addParameter("QuarantineTime", mQuarantineTime);
}


/**
 * Copy constructor. 
 * 
 * @param other the StructureNiechingSelection object to copy.
 */
StructureNiechingSelection::StructureNiechingSelection(const StructureNiechingSelection &other)
	: Object(), ValueChangedListener(), SelectionMethod(other),
	  mSelectionMethod(other.mSelectionMethod->createCopy()), mNiecheCounter(0)
{
	mNumberOfSeparateNieches = dynamic_cast<IntValue*>(getParameter("NumberOfSeparateNieches"));
	mNumberOfCompetingNieches = dynamic_cast<IntValue*>(getParameter("NumberOfCompetingNieches"));
	mQuarantineTime = dynamic_cast<IntValue*>(getParameter("QuarantineTime"));
}

/**
 * Destructor.
 */
StructureNiechingSelection::~StructureNiechingSelection() {
}

SelectionMethod* StructureNiechingSelection::createCopy() const {
	return new StructureNiechingSelection(*this);
}



QList<Individual*> StructureNiechingSelection::createSeed(QList<Individual*> currentGeneration,
								int numberOfIndividuals, 
								int numberOfPreservedParents,
								int numberOfParentsPerIndividual)
{
	//TODO
	currentGeneration.size();
	numberOfIndividuals = numberOfIndividuals;
	numberOfPreservedParents = numberOfPreservedParents;
	numberOfParentsPerIndividual = numberOfParentsPerIndividual;


	QList<Individual*> newGeneration;


	int numberOfSeparateNieches = mNumberOfSeparateNieches->get();

	QList<QList<Individual*>*> nieches;
	for(int i = 0; i < numberOfSeparateNieches; ++i) {
		nieches.append(new QList<Individual*>());
	}
	while(mNiecheBestFitness.size() < nieches.size()) {
		mNiecheBestFitness.append(0.0);
	}

	QList<Individual*> modifiedIndividuals;
	QList<Individual*> freeIndividuals;
	QList<Individual*> removedIndividuals;

	for(QListIterator<Individual*> i(currentGeneration); i.hasNext();) {
		Individual *ind = i.next();
		if(ind->hasProperty("Nieche")) {
			QStringList niecheList = ind->getProperty("Nieche").split(",");
			for(QListIterator<QString> j(niecheList); j.hasNext();) {
// 				int niecheid = j.next().toInt();
				
// 				QList<Individual*> *nieche = nieches.value(niecheName);
// 				if(nieche == 0) {
// 					removedIndividuals.append(ind);
// 				}
// 				else {
// 					if(!nieche->contains(ind)) {
// 						nieche->append(ind);
// 					}
// 				}
			}
		}
		else {
			freeIndividuals.append(ind);
		}
	}

	//distribute the free individuals to the nieches.
	
	
	

	return newGeneration;
}


}



