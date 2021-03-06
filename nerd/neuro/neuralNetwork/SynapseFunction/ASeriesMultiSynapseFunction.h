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



#ifndef NERDASeriesMultiSynapseFunction_H
#define NERDASeriesMultiSynapseFunction_H

#include <QString>
#include <QHash>
#include "SynapseFunction/SynapseFunction.h"

namespace nerd {

	/**
	 * ASeriesSynapseFunction.
	 *
	 * The ASeriesSynapseFunction returns the strength of the
	 * owner Synapse truncated to the precision of the A-Series weight value.
	 * This transfer function however allows to divide a single synapse with a high
	 * strength into mutliple synapses which do not exceed the precision limit each.
	 */
	class ASeriesMultiSynapseFunction : public SynapseFunction {
	public:
		ASeriesMultiSynapseFunction();
		ASeriesMultiSynapseFunction(const ASeriesMultiSynapseFunction &other);
		virtual ~ASeriesMultiSynapseFunction();

		virtual SynapseFunction* createCopy() const;
		
		virtual void reset(Synapse *owner);
		virtual double calculate(Synapse *owner);

		bool equals(SynapseFunction *synapseFunction) const;

	private:
	};

}

#endif



