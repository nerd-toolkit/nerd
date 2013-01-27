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

#include "NeuralNetworkConstants.h"

using namespace nerd;

//**************************************************************************
//Global Object Names
//**************************************************************************

const QString NeuralNetworkConstants::OBJECT_NEURAL_NETWORK_MANAGER
		= "NeuralNetworkManager";

const QString NeuralNetworkConstants::OBJECT_CONSTRAINT_MANAGER
		= "NetworkConstraintManager";

const QString NeuralNetworkConstants::OBJECT_NEURO_MODULE_MANAGER
		= "NeuroModuleManager";

const QString NeuralNetworkConstants::OBJECT_EVOLUTION_MANAGER
		= "EvolutionManager";

const QString NeuralNetworkConstants::OBJECT_NEURO_TAG_MANAGER
		= "NeuroTagManager";
		
const QString NeuralNetworkConstants::OBJECT_NEURO_MODULATOR_MANAGER
		= "NeuroModulatorManager";

//**************************************************************************
//Event Names
//**************************************************************************

const QString NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED 
		= "/NeuralNetwork/CurrentNetworksReplaced";

const QString NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED
		= "/NeuralNetwork/NetworkStructuresChanged";
		
const QString NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_PARAMETERS_CHANGED
		= "/NeuralNetwork/NetworkParametersChanged";

const QString NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_STARTED
		= "/NeuralNetwork/ExecutionStarted";

const QString NeuralNetworkConstants::EVENT_NNM_NETWORK_EXECUTION_COMPLETED
		= "/NeuralNetwork/ExecutionCompleted";

const QString NeuralNetworkConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED
		= "/NeuralNetwork/NetworkIterationCompleted";

const QString NeuralNetworkConstants::EVENT_NETWORK_CLEAR_MODIFICATION_STACKS
		= "/NeuralNetwork/ClearModificationStacks";
		
const QString NeuralNetworkConstants::EVENT_CONSTRAINTS_UPDATED
		= "/NeuralNetwork/ConstraintsUpdated";
		
const QString NeuralNetworkConstants::EVENT_NNM_RESET_NETWORKS
		= "/NeuralNetwork/ResetNetworks";

//**************************************************************************
//Property Names
//**************************************************************************


const QString NeuralNetworkConstants::PROP_ELEMENT_MODIFIED
		= "Modified";
		
const QString NeuralNetworkConstants::PROP_PREFIX_CONSTRAINT_TEMP
		= "_##_";

//**************************************************************************
//Value Names
//**************************************************************************

const QString NeuralNetworkConstants::VALUE_EVO_STASIS_MODE
		= "/Evolution/StasisMode";

const QString NeuralNetworkConstants::VALUE_NNM_BYPASS_NETWORKS
		= "/NeuralNetwork/BypassNetworks";
		
		const QString NeuralNetworkConstants::VALUE_NNM_NUMBER_OF_ITERATIONS_PER_STEP
		= "/NeuralNetwork/UpdatesPerStep";

const QString NeuralNetworkConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER
		= "/Evolution/CurrentGeneration";
		
//used to enable one-shot constraints in the networks analyzer extension
const QString NeuralNetworkConstants::VALUE_ANALYZER_RUN_COUNTER
		= "/Plotter/AnalyzerRunCounter";
		
const QString NeuralNetworkConstants::VALUE_ENABLE_NEURO_MODULATORS
		= "/NeuralNetwork/EnableNeuroModulators";
		
const QString NeuralNetworkConstants::VALUE_DISABLE_NEURAL_PLASTICITY
		= "/NeuralNetwork/DisableNeuralPlasticity";
		
const QString NeuralNetworkConstants::VALUE_DISABLE_NETWORK_UPDATE
		= "/NeuralNetwork/DisableNetworkUpdate";

//**************************************************************************
//Tag Names
//**************************************************************************

const QString NeuralNetworkConstants::TAG_FLIP_NEURON_ACTIVITY
		= "FLIP_ACTIVITY";

const QString NeuralNetworkConstants::TAG_INPUT_NEURON 
		= "TYPE_INPUT";

const QString NeuralNetworkConstants::TAG_OUTPUT_NEURON
		= "TYPE_OUTPUT";

const QString NeuralNetworkConstants::TAG_TYPE_NEURON
		= "N";

const QString NeuralNetworkConstants::TAG_TYPE_SYNAPSE
		= "S";

const QString NeuralNetworkConstants::TAG_TYPE_MODULE
		= "M";

const QString NeuralNetworkConstants::TAG_TYPE_GROUP
		= "G";

const QString NeuralNetworkConstants::TAG_TYPE_NETWORK_ELEMENT
		= "E";
		
const QString NeuralNetworkConstants::TAG_TYPE_NETWORK
		= "NN";

const QString NeuralNetworkConstants::TAG_ELEMENT_PROTECTED
		= "Protected";

const QString NeuralNetworkConstants::TAG_ELEMENT_PROTECT_EXISTENCE
		= "ProtectExistence";
		
const QString NeuralNetworkConstants::TAG_ELEMENT_DRAW_AS_DISABLED
		= "DrawAsDisabled";

const QString NeuralNetworkConstants::TAG_ELEMENT_LOCATION
		= "_LOCATION";

const QString NeuralNetworkConstants::TAG_MODULE_SIZE
		= "_MODULE_SIZE";

const QString NeuralNetworkConstants::TAG_MODULE_INPUT
		= "MODULE_INPUT";

const QString NeuralNetworkConstants::TAG_MODULE_OUTPUT
		= "MODULE_OUTPUT";

const QString NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE
		= "MODULE_EXTENDED_INTERFACE";

const QString NeuralNetworkConstants::TAG_MODULE_TYPE
		= "MODULE_TYPE";

const QString NeuralNetworkConstants::TAG_MODULE_SUPPORTED_TYPES
		= "MODULE_SUPPORTED_TYPES";

const QString NeuralNetworkConstants::TAG_MODULE_COMPATIBLE_TYPES
		= "MODULE_COMPATIBLE_TYPES";

const QString NeuralNetworkConstants::TAG_MODULE_PERMITTED_PATHWAYS
		= "MODULE_PERMITTED_PATHWAYS";

const QString NeuralNetworkConstants::TAG_MODULE_FORBIDDEN_PATHWAYS
		= "MODULE_FORBIDDEN_PATHWAYS";

const QString NeuralNetworkConstants::TAG_MODULE_IDENTIFIER
		= "_MODULE_IDENTIFIER";

const QString NeuralNetworkConstants::TAG_MODULE_PREVENT_REPLACEMENT
		= "NO_MODULE_REPLACEMENT";
		
const QString NeuralNetworkConstants::TAG_MODULE_CUSTOM_BACKGROUND_COLOR
		= "BackgroundColor";
		
const QString NeuralNetworkConstants::TAG_MODULE_PROTECT_NEURONS
		= "ProtectNeurons";
		
const QString NeuralNetworkConstants::TAG_MODULE_MAX_NUMBER_OF_NEURONS
		= "MaxNumberOfNeurons";
		
// const QString NeuralNetworkConstants::TAG_MODULE_MAX_NUMBER_OF_NEURONS_RECURSIVE
// 		= "MaxNumberOfEnclosedNeurons";

const QString NeuralNetworkConstants::TAG_SYNAPSE_PROTECT_STRENGTH
		= "ProtectStrength";

const QString NeuralNetworkConstants::TAG_SYNAPSE_PREVIOUS_STRENGTH
		= "_SYNAPSE_PREVIOUS_STRENGTH";

const QString NeuralNetworkConstants::TAG_SYNAPSE_MIN_STRENGTH
		= "MinStrength";

const QString NeuralNetworkConstants::TAG_SYNAPSE_MAX_STRENGTH
		= "MaxStrength";

const QString NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_TARGET
		= "NoSynapseTarget";

const QString NeuralNetworkConstants::TAG_SYNAPSE_NO_SYNAPSE_SOURCE
		= "NoSynapseSource";

const QString NeuralNetworkConstants::TAG_SYNAPSE_RANDOM_FLIP
		= "_RandomFlip";
		
const QString NeuralNetworkConstants::TAG_SYNAPSE_LOCATION_OFFSET
		= "_LOC_OFFSET";
		
const QString NeuralNetworkConstants::TAG_SYNAPSE_HIDE_WEIGHT
		= "HideWeight";
		
const QString NeuralNetworkConstants::TAG_SYNAPSE_FIXED_POSITION
		= "FixedSynapsePosition";

const QString NeuralNetworkConstants::TAG_ELEMENT_CHANGE_PROBABILITY
		= "ChangeProbability";

const QString NeuralNetworkConstants::TAG_ELEMENT_CHANGE_DEVIATION
		= "ChangeDeviation";
		
const QString NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM
		= "_RDOF";

// const QString NeuralNetworkConstants::TAG_SYNAPSE_REINIT_STRENGTH 
// 		= "_ReInitWeight";

const QString NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS
		= "ProtectBias";

const QString NeuralNetworkConstants::TAG_NEURON_PROTECT_BIAS_EXISTENCE
		= "ProtectBiasExistence";

const QString NeuralNetworkConstants::TAG_NEURON_PREVIOUS_BIAS
		= "_NEURON_PREVIOUS_BIAS";

const QString NeuralNetworkConstants::TAG_NEURON_MIN_BIAS
		= "MinBias";

const QString NeuralNetworkConstants::TAG_NEURON_MAX_BIAS
		= "MaxBias";

const QString NeuralNetworkConstants::TAG_NEURON_INITIAL_CONNECTION_PROPORTION
		= "InitConnectionProportion";

const QString NeuralNetworkConstants::TAG_NEURON_REINIT_BIAS 
		= "_ReInitBias";
		
const QString NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT
		= "ODN";
		
const QString NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_IN_SYNAPSES
		= "MaxNumberOfInSynapses";
		
const QString NeuralNetworkConstants::TAG_NEURON_MAX_NUMBER_OF_OUT_SYNAPSES
		= "MaxNumberOfOutSynapses";
		
const QString NeuralNetworkConstants::TAG_NEURON_SOURCE_EXCLUDE_LIST
		= "NeuronSourceExcludeList";
		
const QString NeuralNetworkConstants::TAG_NEURON_PROTECT_TRANSFER_FUNCTION
		= "ProtectTF";
		
const QString NeuralNetworkConstants::TAG_NEURON_PROTECT_ACTIVATION_FUNCTION
		= "ProtectAF";

const QString NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE
		= "_ConstraintSlave";

const QString NeuralNetworkConstants::TAG_FAST_ITERATIONS
		= "FAST";

const QString NeuralNetworkConstants::TAG_MAX_NUMBER_OF_SUBMODULES
		= "MaxNumberOfSubModules";

const QString NeuralNetworkConstants::TAG_PROTOTYPE_TYPE_LIST
		= "TYPE_LIST";

const QString NeuralNetworkConstants::TAG_CREATION_DATE 
		= "_CreationDate";

const QString NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_MASTER
		= "EQ_VAR";

const QString NeuralNetworkConstants::TAG_ELEMENT_VARIABLE_SLAVE
		= "EQ";

const QString NeuralNetworkConstants::TAG_HIDE_LAYERS 
		= "HideLayers";

const QString NeuralNetworkConstants::TAG_LAYER_IDENTIFIER 
		= "Layer";

const QString NeuralNetworkConstants::TAG_NETWORK_BDN_FADE_IN_RATE
		= "FadeInRate";
		
const QString NeuralNetworkConstants::TAG_NEURON_BDN_INPUT 
		= "BDN_In";
		
const QString NeuralNetworkConstants::TAG_NEURON_BDN_OUTPUT
		= "BDN_Out";
		
const QString NeuralNetworkConstants::TAG_NEURON_BDN_BOARD_INTERFACE
		= "BDN_BOARD_INTERFACE";
		
const QString NeuralNetworkConstants::TAG_EVOLUTION_NEW_ELEMENT
		= "__NEW__";
		
const QString NeuralNetworkConstants::TAG_EVOLUTION_NEW_BIAS
		= "__NEW_BIAS__";
		
const QString NeuralNetworkConstants::TAG_DISABLE_NEURO_MODULATORS
		= "DisableNeuroModulators";
		
		