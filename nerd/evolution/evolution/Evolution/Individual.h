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


#ifndef NERDIndividual_H
#define NERDIndividual_H

#include "Core/Object.h"
#include <QList>
#include "Core/Properties.h"

namespace nerd {

	class FitnessFunction;

	/**
	 * Individual.
	 * This class is a container for all Individuals that can be used for 
	 * artificial evolution. The genome of the Individual can be any subclass
	 * of Object and has to match the requirements posted by the EvolutionAlgorithm,
	 * EvaluationMethod and GenotypePhenotype Mapper of the current World / Population.
	 */
	class Individual : public Properties {
	public:
		Individual();
		Individual(int id);
		virtual ~Individual();

		int getId() const;

		Object* getGenome() const;
		void setGenome(Object *genome);

		Object* getPhenotype() const;
		void setPhenotype(Object *phenotype);

		double getFitness(FitnessFunction *key) const;
		void setFitness(FitnessFunction *key, double fitness);
		void clearFitness();
		QList<FitnessFunction*> getFitnessFunctions() const;

		QList<Individual*>& getParents();

		void protectGenome(bool protect);
		bool isGenomeProtected() const;

	private:
		int mId;
		QList<Individual*> mParents;	
		Object *mGenome;
		Object *mPhenotype;
		static int mIdCounter;
		QHash<FitnessFunction*, double> mFitness;
		bool mProtectGenome;
	};

}

#endif


