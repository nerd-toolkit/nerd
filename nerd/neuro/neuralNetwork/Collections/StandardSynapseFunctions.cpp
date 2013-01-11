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

#include "StandardSynapseFunctions.h"
#include "Network/Neuro.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "SynapseFunction/ASeriesSynapseFunction.h"
#include "SynapseFunction/AbsoluteValueSynapseFunction.h"
#include "SynapseFunction/MultiplicativeSynapseFunction.h"
#include "SynapseFunction/ASeriesMultiSynapseFunction.h"
#include "SynapseFunction/MSeriesSynapseFunction.h"
#include "SynapseFunction/MSeriesAdjustableSynapseFunction.h"
#include "SynapseFunction/SimpleLinkSynapseFunction.h"
#include "SynapseFunction/Learning/HebbSynapseFunction.h"
#include "SynapseFunction/Learning/TesauroSynapseFunction.h"
#include "SynapseFunction/ScriptableSynapseFunction.h"
#include "SynapseFunction/CloneSimpleSynapseFunction.h"
#include "SynapseFunction/Learning/SimpleModulatedRandomSearchSynapseFunction.h"

namespace nerd {

StandardSynapseFunctions::StandardSynapseFunctions()
{
	//Simple synapse function (plain unmodulated synapse strength)
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		SimpleSynapseFunction());

	//ASeries synapse function
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		ASeriesSynapseFunction());

	//ASeries multipart synapse function
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		ASeriesMultiSynapseFunction());
		
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		MSeriesSynapseFunction());

	//Absolute Value synapse function
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		AbsoluteValueSynapseFunction());

	//Multiplicative Synapse Function
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		MultiplicativeSynapseFunction());
	
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		MSeriesAdjustableSynapseFunction());
	
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		SimpleLinkSynapseFunction());
	
	//Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
	//	HebbSynapseFunction());
	
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		TesauroSynapseFunction()); //Hebb Variants (Hebb, Tesauro, Barto & Sutton)
		
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		CloneSimpleSynapseFunction());
		
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		ScriptableSynapseFunction());
	
	Neuro::getNeuralNetworkManager()->addSynapseFunctionPrototype(
		SimpleModulatedRandomSearchSynapseFunction());
	
}

}


