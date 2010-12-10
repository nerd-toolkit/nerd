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


#ifndef NERDNeuralNetworkElement_H
#define NERDNeuralNetworkElement_H

#include "Math/Vector3D.h"
#include <QtGlobal>
#include "Core/Properties.h"
#include "Core/PropertyChangedListener.h"

namespace nerd {

	/**
	 * NeuralNetworkElement.
	 */
	class NeuralNetworkElement : public Properties, public virtual PropertyChangedListener {
		public: 
			NeuralNetworkElement();
			NeuralNetworkElement(const NeuralNetworkElement &other);
			virtual ~NeuralNetworkElement();

			virtual qulonglong getId() const = 0;	
			virtual void setId(qulonglong id) = 0;

			virtual void setPosition(const Vector3D &position);
			virtual Vector3D getPosition() const;
		
			virtual int getRequiredIterations() const;
			virtual void setRequiredIterations(int iterations);

			virtual int getStartIteration() const;
// 			virtual void setStartIteration(int startIteration);

			virtual void propertyChanged(Properties *owner, const QString &property);

		protected:
			void updateIterationProperty();

		protected:
			int mStartIteration;
			int mRequiredIterations;

		public:
			NeuralNetworkElement *mCopyPtr;
	};

}

#endif


