/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#ifndef NERDBifurcation_Calculator_H
#define NERDBifurcation_Calculator_H

#include <QString>
#include <QHash>
#include "DynamicsPlot/DynamicsPlotter.h"
#include "Value/IntValue.h"
#include "Value/MatrixValue.h"
#include "Value/ULongLongValue.h"

namespace nerd {

	/**
	 * The standard bifurcation plotter provides a tool for displaying changes 
	 * in the qualitative behavior of an neural network. Therefore a synapse
	 * strength or a bias is varied and the activation or output of a neuron
	 * is observed. The plotter consists of three parts, one to extract the data,
	 * one to export it and an external script to create the diagram.
	 */
	class Bifurcation_Calculator: public DynamicsPlotter {
		public:
			Bifurcation_Calculator();
			virtual ~Bifurcation_Calculator();

			virtual void calculateData();

		private:
			IntValue *mPlotPixelsX;
			
			StringValue *mIdsOfVariedNetworkElements;
			StringValue *mMinimaOfVariedNetworkElements;
			StringValue *mMaximaOfVariedNetworkElements;
			
			StringValue *mIdsOfObservedNeurons;
			DoubleValue *mMaxOutputRange;
			DoubleValue *mMinOutputRange;
			DoubleValue *mTolerance;
			IntValue *mPlotPixelsY;		
			BoolValue *mResetToInitState;
			IntValue *mMaxSteps;
			BoolValue *mBidirectional;
			DoubleValue *mPrerunSteps;

			QList<DoubleValue*> mObservedValues;
			
	};
	



}

#endif



