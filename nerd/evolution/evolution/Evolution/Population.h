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


#ifndef NERDPopulation_H
#define NERDPopulation_H

#include "Core/ParameterizedObject.h"
#include "Core/Properties.h"
#include "Evolution/Individual.h"
#include "SelectionMethod/SelectionMethod.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Phenotype/GenotypePhenotypeMapper.h"
#include "Fitness/FitnessFunction.h"

namespace nerd {

	/**
	 * Population.
	 */
	class Population : public ParameterizedObject, public Properties {
	public:
		Population(const QString &name);
		virtual ~Population();
		
		QList<Individual*>& getIndividuals();

		bool addSelectionMethod(SelectionMethod *selectionMethod);
		bool removeSelectionMethod(SelectionMethod *selectionMethod);
		const QList<SelectionMethod*>& getSelectionMethods() const;

		bool addFitnessFunction(FitnessFunction *fitnessFunction);
		bool removeFitnessFunction(FitnessFunction *fitnessFunction);
		const QList<FitnessFunction*>& getFitnessFunctions() const;
		FitnessFunction* getFitnessFunction(const QString &name) const;

		void setGenotypePhenotypeMapper(GenotypePhenotypeMapper *mapper);
		GenotypePhenotypeMapper* getGenotypePhenotypeMapper() const;

		IntValue* getPopulationSizeValue() const;
		IntValue* getDesiredPopulationSizeValue() const;
		IntValue* getNumberOfPreservedParentsValue() const;

	private:
		GenotypePhenotypeMapper *mGenotypePhenotypeMapper;
		QList<Individual*> mIndividuals;
		QList<SelectionMethod*> mSelectionMethods;
		QList<FitnessFunction*> mFitnessFunctions;
		IntValue *mDesiredPopulationSizeValue;
		IntValue *mPopulationSizeValue;
		IntValue *mNumberOfPreservedParentsValue;
	};

}

#endif


