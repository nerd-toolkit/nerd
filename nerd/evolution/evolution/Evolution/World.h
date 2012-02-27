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


#ifndef NERDWorld_H
#define NERDWorld_H

#include "Core/ParameterizedObject.h"
#include "Evolution/Population.h"
#include <QList>
#include "Evolution/EvolutionAlgorithm.h"
#include "Evaluation/EvaluationMethod.h"

namespace nerd {

	/**
	 * World.
	 */
	class World : public ParameterizedObject {
	public:
		World(const QString &name);
		virtual ~World();

		bool addPopulation(Population *population);
		bool removePopulation(Population *population);
		QList<Population*> getPopulations() const;

		void setEvolutionAlgorithm(EvolutionAlgorithm *evolutionAlgorithm);
		EvolutionAlgorithm* getEvolutionAlgorithm() const;

		void setEvaluationMethod(EvaluationMethod *evaluationMethod);
		EvaluationMethod* getEvaluationMethod() const;

		StringValue* getControlleAgentValue() const;
		
	private:
		QList<Population*> mPopulations;
		EvolutionAlgorithm *mEvolutionAlgorithm;
		EvaluationMethod *mEvaluationMethod;
		bool mDeleteEvolutionAlgorithm;
		bool mDeleteEvaluationAlgorithm;
		StringValue *mControlledAgents;
	};

}

#endif


