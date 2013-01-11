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

#include "StandardTransferFunctions.h"
#include "Network/Neuro.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "TransferFunction/TransferFunctionASeriesTanh.h"
#include "TransferFunction/TransferFunctionParameterizedSigmoid.h"
#include "TransferFunction/TransferFunctionSigmoid.h"
#include "TransferFunction/TransferFunctionTanh01.h"
#include "TransferFunction/TransferFunctionMSeriesTanh.h"
#include "TransferFunction/TransferFunctionGauss.h"
#include "TransferFunction/TransferFunctionStep.h"
#include "TransferFunction/ScriptableTransferFunction.h"
#include "TransferFunction/TransferFunctionThreshold.h"



namespace nerd {

StandardTransferFunctions::StandardTransferFunctions()
{
	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();
	//Tanh
	nnm->addTransferFunctionPrototype(TransferFunctionTanh());
	nnm->addTransferFunctionPrototype(TransferFunctionTanh01());
	nnm->addTransferFunctionPrototype(TransferFunctionASeriesTanh());
	nnm->addTransferFunctionPrototype(TransferFunctionMSeriesTanh());
	//Ramp
	nnm->addTransferFunctionPrototype(TransferFunctionRamp("ramp[-1,1]", -1.0, 1.0));
	nnm->addTransferFunctionPrototype(TransferFunctionRamp("ramp[0,1]", 0.0, 1.0));
	nnm->addTransferFunctionPrototype(TransferFunctionRamp("ramp[-u,u]", -1000000.0, 100000.0));
	nnm->addTransferFunctionPrototype(TransferFunctionRamp("ramp[n,m]", -1.0, 1.0, true));
	//Sigmoids
	nnm->addTransferFunctionPrototype(TransferFunctionSigmoid());
	nnm->addTransferFunctionPrototype(TransferFunctionParameterizedSigmoid(5.0, 10.0));
	nnm->addTransferFunctionPrototype(TransferFunctionGauss());
	//Step
	nnm->addTransferFunctionPrototype(TransferFunctionStep());
	//scriptable
	nnm->addTransferFunctionPrototype(ScriptableTransferFunction());
	nnm->addTransferFunctionPrototype(TransferFunctionThreshold());
	
}

}


