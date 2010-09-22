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



#ifndef NERDNeatGenome_H
#define NERDNeatGenome_H

#include <QString>
#include <QHash>
#include "Neat/NeatSpeciesOrganism.h"
#include "Value/DoubleValue.h"
#include "Core/Object.h"

namespace nerd {

	struct NeatNodeGene {
		enum {SENSOR, OUTPUT, HIDDEN, BIAS};
		NeatNodeGene(long id, int type);
		NeatNodeGene(const NeatNodeGene &other);

		long mId;
		int mType;
	};

	struct NeatConnectionGene {
		NeatConnectionGene(long id, long inNode, long outNode, double weight);
		NeatConnectionGene(const NeatConnectionGene &other);
		long mId;
		long mInputNode;
		long mOutputNode;
		double mWeight;
		bool mEnabled;
	};

	/**
	 * NeatGenome.
	 *
	 */
	class NeatGenome : public virtual NeatSpeciesOrganism, public virtual Object {
	public:
		NeatGenome();
		NeatGenome(const NeatGenome &other);
		virtual ~NeatGenome();

		virtual QString getName() const;

		void setId(long id);
		long getId() const;
		QList<NeatNodeGene*>& getNodeGenes();
		QList<NeatConnectionGene*>& getConnectionGenes();

		QString getGenomeAsString() const;
		bool setFromString(const QString &genomeString);

		virtual bool isCompatible(NeatSpeciesOrganism *organism, double compatibilityThreshold);

		static long generateUniqueId();

		bool addConnectionGene(NeatConnectionGene *link);
		bool addNodeGene(NeatNodeGene *node);


	private:	
		static long mIdCounter;
		long mId;
		QList<NeatNodeGene*> mNodes;
		QList<NeatConnectionGene*> mConnectionGenes;
		DoubleValue *mDisjointFactor;
		DoubleValue *mExcessFactor;
		DoubleValue *mMutationDifferenceFactor;
	};

}

#endif



