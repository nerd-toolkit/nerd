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


#ifndef NERDNeuralNetworkConstants_H
#define NERDNeuralNetworkConstants_H

#include <QString>


namespace nerd {

	/**
	 * NeuralNetworkConstants.
	 */
	class NeuralNetworkConstants {
	public:

	//**************************************************************************
	//Global Object Names
	//**************************************************************************

		static const QString OBJECT_NEURAL_NETWORK_MANAGER;
		static const QString OBJECT_CONSTRAINT_MANAGER;
		static const QString OBJECT_NEURO_MODULE_MANAGER;
		static const QString OBJECT_EVOLUTION_MANAGER;
		static const QString OBJECT_NEURO_TAG_MANAGER;

	//**************************************************************************
	//Event Names
	//**************************************************************************

		static const QString EVENT_NNM_CURRENT_NETWORKS_REPLACED;
		static const QString EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED;
		static const QString EVENT_NNM_NETWORK_EXECUTION_STARTED;
		static const QString EVENT_NNM_NETWORK_EXECUTION_COMPLETED;
		static const QString EVENT_NNM_NETWORK_ITERATION_COMPLETED;
		static const QString EVENT_NETWORK_CLEAR_MODIFICATION_STACKS;
		static const QString EVENT_CONSTRAINTS_UPDATED;
		static const QString EVENT_NNM_RESET_NETWORKS;

	//**************************************************************************
	//Property Names
	//**************************************************************************

		static const QString PROP_ELEMENT_MODIFIED;
		static const QString PROP_PREFIX_CONSTRAINT_TEMP;

	//**************************************************************************
	//Value Names
	//**************************************************************************

		static const QString VALUE_EVO_STASIS_MODE;
		static const QString VALUE_NNM_BYPASS_NETWORKS;
		static const QString VALUE_EVO_CURRENT_GENERATION_NUMBER;
		static const QString VALUE_ANALYZER_RUN_COUNTER;

	//**************************************************************************
	//Tag Names
	//**************************************************************************

		static const QString TAG_FLIP_NEURON_ACTIVITY;
		static const QString TAG_INPUT_NEURON;
		static const QString TAG_OUTPUT_NEURON;
		static const QString TAG_TYPE_NEURON;
		static const QString TAG_TYPE_SYNAPSE;
		static const QString TAG_TYPE_MODULE;
		static const QString TAG_TYPE_GROUP;
		static const QString TAG_TYPE_NETWORK_ELEMENT;
		static const QString TAG_TYPE_NETWORK;
		static const QString TAG_ELEMENT_LOCATION;
		static const QString TAG_ELEMENT_PROTECTED;
		static const QString TAG_ELEMENT_PROTECT_EXISTENCE;
		static const QString TAG_MODULE_SIZE;
		static const QString TAG_MODULE_INPUT;
		static const QString TAG_MODULE_OUTPUT;
		static const QString TAG_MODULE_EXTENDED_INTERFACE;
		static const QString TAG_MODULE_TYPE;
		static const QString TAG_MODULE_SUPPORTED_TYPES;
		static const QString TAG_MODULE_COMPATIBLE_TYPES;
		static const QString TAG_MODULE_PERMITTED_PATHWAYS;
		static const QString TAG_MODULE_FORBIDDEN_PATHWAYS;
		static const QString TAG_MODULE_IDENTIFIER;
		static const QString TAG_MODULE_PREVENT_REPLACEMENT;
		static const QString TAG_MODULE_CUSTOM_BACKGROUND_COLOR;
		static const QString TAG_MODULE_PROTECT_NEURONS;
		static const QString TAG_MODULE_MAX_NUMBER_OF_NEURONS;
// 		static const QString TAG_MODULE_MAX_NUMBER_OF_NEURONS_RECURSIVE;
		static const QString TAG_SYNAPSE_PROTECT_STRENGTH;
		static const QString TAG_SYNAPSE_PREVIOUS_STRENGTH;
		static const QString TAG_SYNAPSE_MIN_STRENGTH;
		static const QString TAG_SYNAPSE_MAX_STRENGTH;
		static const QString TAG_SYNAPSE_NO_SYNAPSE_TARGET;
		static const QString TAG_SYNAPSE_NO_SYNAPSE_SOURCE;
		static const QString TAG_SYNAPSE_RANDOM_FLIP;
		static const QString TAG_SYNAPSE_LOCATION_OFFSET;
		static const QString TAG_SYNAPSE_HIDE_WEIGHT;
		static const QString TAG_SYNAPSE_FIXED_POSITION;
		static const QString TAG_ELEMENT_CHANGE_PROBABILITY;
		static const QString TAG_ELEMENT_CHANGE_DEVIATION;
		static const QString TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM;
// 		static const QString TAG_SYNAPSE_REINIT_STRENGTH;
		static const QString TAG_NEURON_PROTECT_BIAS;
		static const QString TAG_NEURON_PROTECT_BIAS_EXISTENCE;
		static const QString TAG_NEURON_PREVIOUS_BIAS;
		static const QString TAG_NEURON_MIN_BIAS;
		static const QString TAG_NEURON_MAX_BIAS;
		static const QString TAG_NEURON_INITIAL_CONNECTION_PROPORTION;
		static const QString TAG_NEURON_REINIT_BIAS;
		static const QString TAG_NEURON_ORDER_DEPENDENT;
		static const QString TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES;
		static const QString TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES;
		static const QString TAG_NEURON_SOURCE_EXCLUDE_LIST;
		static const QString TAG_NEURON_PROTECT_TRANSFER_FUNCTION;
		static const QString TAG_NEURON_PROTECT_ACTIVATION_FUNCTION;
		static const QString TAG_CONSTRAINT_SLAVE;
		static const QString TAG_FAST_ITERATIONS;
		static const QString TAG_MAX_NUMBER_OF_SUBMODULES;
		static const QString TAG_PROTOTYPE_TYPE_LIST;
		static const QString TAG_CREATION_DATE;
		static const QString TAG_ELEMENT_VARIABLE_MASTER;
		static const QString TAG_ELEMENT_VARIABLE_SLAVE; 
		static const QString TAG_HIDE_LAYERS;
		static const QString TAG_LAYER_IDENTIFIER;
		static const QString TAG_NETWORK_BDN_FADE_IN_RATE;
		static const QString TAG_NEURON_BDN_INPUT;
		static const QString TAG_NEURON_BDN_OUTPUT;
		static const QString TAG_NEURON_BDN_BOARD_INTERFACE;
		static const QString TAG_EVOLUTION_NEW_ELEMENT;
		static const QString TAG_EVOLUTION_NEW_BIAS;
		
		
	};

}

#endif


