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




#include "TestPopulation.h"
#include <iostream>
#include "Evolution/Population.h"
#include "SelectionMethod/SelectionMethodAdapter.h"
#include "Evolution/IndividualAdapter.h"
#include "Phenotype/GenotypePhenotypeMapperAdapter.h"

using namespace std;
using namespace nerd;

void TestPopulation::initTestCase() {
}

void TestPopulation::cleanUpTestCase() {
}


//chris
void TestPopulation::testConstruction() {

	bool destroyedIndividual = false;
	IndividualAdapter *i1 = new IndividualAdapter(&destroyedIndividual);

	bool destroyedMapper = false;
	GenotypePhenotypeMapperAdapter *mapper = new GenotypePhenotypeMapperAdapter(
				"Mapper1", &destroyedMapper);

	{
		Population pop1("Pop1");
		
		QVERIFY(pop1.getName() == "Pop1");
		QVERIFY(pop1.getIndividuals().size() == 0);
	
		pop1.getIndividuals().append(i1);
	
		QVERIFY(pop1.getIndividuals().size() == 1);
		QVERIFY(pop1.getIndividuals().at(0) == i1);

		QVERIFY(pop1.getGenotypePhenotypeMapper() == 0);
		pop1.setGenotypePhenotypeMapper(mapper);
		QVERIFY(pop1.getGenotypePhenotypeMapper() == mapper);
		pop1.setGenotypePhenotypeMapper(0); //allowed
		QVERIFY(pop1.getGenotypePhenotypeMapper() == 0);
		pop1.setGenotypePhenotypeMapper(mapper);
		QVERIFY(pop1.getGenotypePhenotypeMapper() == mapper);

		QVERIFY(pop1.getPopulationSizeValue()->get() == 0);
		QVERIFY(pop1.getDesiredPopulationSizeValue()->get() == 1);
		QVERIFY(pop1.getNumberOfPreservedParentsValue()->get() == 1);

		QVERIFY(pop1.getParameter("PopulationSize") == pop1.getPopulationSizeValue());
		QVERIFY(pop1.getParameter("DesiredPopulationSize") == pop1.getDesiredPopulationSizeValue());
		QVERIFY(pop1.getParameter("NumberOfPreservedParents") 
					== pop1.getNumberOfPreservedParentsValue());

	}

	//individual was destroyed with Population.
	QVERIFY(destroyedIndividual == true);
	QVERIFY(destroyedMapper == true);
	
	
	
}


//chris
void TestPopulation::addAndRemoveSelectionMethods() {

	bool destroyedSelection = false;

	{
		Population pop1("Pop1");
	
		SelectionMethodAdapter *selection1 = new SelectionMethodAdapter("Selection1", 0);
		SelectionMethodAdapter *selection2 = new SelectionMethodAdapter("Selection1", 0);
		selection2->mDestroyFlag = &destroyedSelection;

		QVERIFY(pop1.addSelectionMethod(0) == false);
		QVERIFY(pop1.getSelectionMethods().size() == 0);

		QVERIFY(pop1.addSelectionMethod(selection1) == true);
		QVERIFY(pop1.getSelectionMethods().size() == 1);
		QVERIFY(pop1.getSelectionMethods().at(0) == selection1);

		QVERIFY(pop1.addSelectionMethod(selection1) == false);	//can not add twice
		QVERIFY(pop1.getSelectionMethods().size() == 1);

		QVERIFY(pop1.addSelectionMethod(selection2) == true);
		QVERIFY(pop1.getSelectionMethods().size() == 2);
		QVERIFY(pop1.getSelectionMethods().contains(selection1));
		QVERIFY(pop1.getSelectionMethods().contains(selection2));

		//remove
		QVERIFY(pop1.removeSelectionMethod(0) == false);
		QVERIFY(pop1.getSelectionMethods().size() == 2);

		QVERIFY(pop1.removeSelectionMethod(selection1) == true);
		QVERIFY(pop1.getSelectionMethods().size() == 1);
		QVERIFY(pop1.getSelectionMethods().at(0) == selection2);

		QVERIFY(pop1.removeSelectionMethod(selection1) == false); //not contained any more
		QVERIFY(pop1.getSelectionMethods().size() == 1);

		delete selection1;
	}

	QVERIFY(destroyedSelection == true); //is destroyed because it was not removed.
	
}


