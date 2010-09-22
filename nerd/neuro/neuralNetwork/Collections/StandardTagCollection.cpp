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



#include "StandardTagCollection.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/NeuroTagManager.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new StandardTagCollection.
 */
StandardTagCollection::StandardTagCollection()
{

	NeuroTagManager *ntm = NeuroTagManager::getInstance();

	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED,
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT, 
						 "Protects a network element from changes (not including manual changes)"));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_ELEMENT_PROTECT_EXISTENCE,
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT,
						 "Protects a network element from being removed."));

	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_MIN_BIAS,
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Specifies the minimal bias for automatic bias changes."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_MAX_BIAS,
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Specifies the maximal bias for automatic bias changes."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH,
						 NeuralNetworkConstants::TAG_TYPE_SYNAPSE, 
						 "Specifies the maximal strength for automatic synapse changes."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH,
						 NeuralNetworkConstants::TAG_TYPE_SYNAPSE, 
						 "Specifies the maximal strength for automatic synapse changes."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_FAST_ITERATIONS,
						 NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT,
						 "Makes an element to be executed <n> times per network update."));


	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MAX_NUMBER_OF_SUBMODULES,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Sets the maximum number of submodules for a module."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH,
						 NeuralNetworkConstants::TAG_TYPE_SYNAPSE,
						 "Protects a synapse from changes of the synapse strength."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_PROBABILITY,
						 NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_PROBABILITY,
						 "Overwrites the global change probability, if the operator is switched on."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_DEVIATION,
						 NeuralNetworkConstants::TAG_SYNAPSE_CHANGE_DEVIATION,
						 "Overwrites the global deviation setting, if the operator is switched on."));

	

	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Contains the ids of all modules to which this module may connect."));
// 	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_FORBIDDEN_PATHWAYS,
// 						 NeuralNetworkConstants::TAG_TYPE_MODULE,
// 						 "Contains the ids of all modules to which this module may NOT connect."));

	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Prevents a module to be involved in module replacements (modular crossover)."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Protects the bias against changes during evolution."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Proportion of initial connections for this neuron."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Synapses to this neuron are not allowed."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE, 
						 NeuralNetworkConstants::TAG_TYPE_NEURON, 
						 "Synapses from this neuron are not allowed."));


	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_SUPPORTED_TYPES,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "This list specifies all roles this module can act as."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "This list contains all types this module may be replaced by."));
	ntm->addTag(NeuroTag(NeuralNetworkConstants::TAG_PROTOTYPE_TYPE_LIST,
						 NeuralNetworkConstants::TAG_TYPE_MODULE,
						 "Have no clue what this was :)"));

}



/**
 * Destructor.
 */
StandardTagCollection::~StandardTagCollection() {
}




}



