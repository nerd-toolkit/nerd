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



#include "NeatEvolutionAlgorithm.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Neat/NeatGenome.h"
#include "Evolution/World.h"
#include "Math/Random.h"
#include "Evolution/Evolution.h"

using namespace std;

namespace nerd {



NeatNodeInnovation::NeatNodeInnovation(NeatNodeGene *node, NeatConnectionGene *splittedConnection, 
								NeatConnectionGene *link1, NeatConnectionGene *link2)
	: mNode(node), mSplittedConnection(splittedConnection), mLink1(link1), mLink2(link2)
{}

NeatLinkInnovation::NeatLinkInnovation(NeatConnectionGene *connection)
	: mConnection(connection) {}

/**
 * Constructs a new NeatEvolutionAlgorithm.
 */
NeatEvolutionAlgorithm::NeatEvolutionAlgorithm()
	: EvolutionAlgorithm("NEAT"), mGenomeTemplate(0)
{
	mLinkWeightMutationPower = new DoubleValue(1.0);
	mLinkWeightMutationProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateOnlyProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateLinkWeightsProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateToggleEnableProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateReenableProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateAddNodeProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateAddLinkProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMutateAddRecurrentLinkProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMateMultiPointProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMateMultiPointAvgProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMateSinglePointProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mMateOnlyProb = new NormalizedDoubleValue(0.1, 0.0, 1.0);
	mNumberOfInputNeurons = new IntValue(37);
	mNumberOfOutputNeurons = new IntValue(42);
	mStartFullyConnected = new BoolValue(true);

	addParameter("Mutation/LinkWeightMutationPower", mLinkWeightMutationPower);
	addParameter("Mutation/LinkWeightMutationProb", mLinkWeightMutationProb);
	addParameter("Mutation/MutateOnlyProb", mMutateOnlyProb);
	addParameter("Mutation/LinkWeightsProb", mMutateLinkWeightsProb);
	addParameter("Mutation/ToggleEnableProb", mMutateToggleEnableProb);
	addParameter("Mutation/ReenableProb", mMutateReenableProb);
	addParameter("Mutation/AddNodeProb", mMutateAddNodeProb);
	addParameter("Mutation/AddLinkProb", mMutateAddLinkProb);
	addParameter("Mutation/AddLoopProb", mMutateAddRecurrentLinkProb);
	addParameter("Mating/MultiPointProb", mMateMultiPointProb);
	addParameter("Mating/MultiPointAvgProb", mMateMultiPointAvgProb);
	addParameter("Mating/SinglePointProb", mMateSinglePointProb);
	addParameter("Mating/MateOnlyProb", mMateOnlyProb);
	addParameter("Control/NumberOfInputNeurons", mNumberOfInputNeurons);
	addParameter("Control/NumberOfOutputNeurons", mNumberOfOutputNeurons);
	addParameter("Control/StartFullyConnected", mStartFullyConnected);
}


/**
 * Copy constructor. 
 * 
 * @param other the NeatEvolutionAlgorithm object to copy.
 */
NeatEvolutionAlgorithm::NeatEvolutionAlgorithm(const NeatEvolutionAlgorithm &other) 
	: EvolutionAlgorithm(other), mGenomeTemplate(0)
{
	mLinkWeightMutationPower = dynamic_cast<DoubleValue*>(
								getParameter("Mutation/LinkWeightMutationPower"));
	mLinkWeightMutationProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/LinkWeightMutationProb"));
	mMutateOnlyProb  = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/MutateOnlyProb"));
	mMutateLinkWeightsProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/LinkWeightsProb"));
	mMutateToggleEnableProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/ToggleEnableProb"));
	mMutateReenableProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/ReenableProb"));
	mMutateAddNodeProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/AddNodeProb"));
	mMutateAddLinkProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/AddLinkProb"));
	mMutateAddRecurrentLinkProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mutation/AddLoopProb"));
	mMateMultiPointProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mating/MultiPointProb"));
	mMateMultiPointAvgProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mating/MultiPointAvgProb"));
	mMateSinglePointProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mating/SinglePointProb"));
	mMateOnlyProb = dynamic_cast<NormalizedDoubleValue*>(
								getParameter("Mating/MateOnlyProb"));
	mNumberOfInputNeurons = dynamic_cast<IntValue*>(
								getParameter("Control/NumberOfInputNeurons"));
	mNumberOfOutputNeurons = dynamic_cast<IntValue*>(
								getParameter("Control/NumberOfOutputNeurons"));
	mStartFullyConnected = dynamic_cast<BoolValue*>(
								getParameter("Control/StartFullyConnected"));
}

/**
 * Destructor.
 */
NeatEvolutionAlgorithm::~NeatEvolutionAlgorithm() {
}


int NeatEvolutionAlgorithm::getRequiredNumberOfParentsPerIndividual() const {
	return 2;
}

EvolutionAlgorithm* NeatEvolutionAlgorithm::createCopy() {
	return new NeatEvolutionAlgorithm(*this);
}

bool NeatEvolutionAlgorithm::createNextGeneration(QList<Individual*> &trashcan) {
	bool verbose = false;

	//make sure that the number of interface neurons remains stable during algorithm
	int numberOfInputNeurons = mNumberOfInputNeurons->get();
	int numberOfOutputNeurons = mNumberOfOutputNeurons->get();

	mNodeInnovations.clear();
	mLinkInnovations.clear();

	if(mGenomeTemplate == 0) {
		//create genome template
		mGenomeTemplate = new NeatGenome();
		mGenomeTemplate->getNodeGenes().append(
				new NeatNodeGene(NeatGenome::generateUniqueId(), NeatNodeGene::BIAS));

		QList<NeatNodeGene*> inputNodes;
		QList<NeatNodeGene*> outputNodes;

		for(int i = 0; i < numberOfInputNeurons; ++i) {
			NeatNodeGene *node = new NeatNodeGene(NeatGenome::generateUniqueId(), NeatNodeGene::SENSOR);
			mGenomeTemplate->getNodeGenes().append(node);
			inputNodes.append(node);
		}
		for(int i = 0; i < numberOfOutputNeurons; ++i) {
			NeatNodeGene *node = new NeatNodeGene(NeatGenome::generateUniqueId(), NeatNodeGene::OUTPUT);
			mGenomeTemplate->getNodeGenes().append(node);
			outputNodes.append(node);
		}
		if(mStartFullyConnected->get()) {
			//create synapses from all inputs to all outputs
			for(QListIterator<NeatNodeGene*> i(inputNodes); i.hasNext();) {
				NeatNodeGene *out = i.next();
				for(QListIterator<NeatNodeGene*> j(outputNodes); j.hasNext();) {
					NeatNodeGene *in = j.next();
					mGenomeTemplate->getConnectionGenes().append(
						new NeatConnectionGene(NeatGenome::generateUniqueId(), in->mId,
									out->mId, Random::nextSign() * Random::nextDouble()));
				}
			}
		}
	}

	if(mOwnerWorld == 0) {
		return false;
	}

	if(verbose) {
		Core::log("NeatEvolutionAlgorithm: createNextGeneration()");
	}

	QList<Population*> populations = mOwnerWorld->getPopulations();
	for(QListIterator<Population*> i(populations); i.hasNext();) {
		Population *pop = i.next();
		QList<Individual*> individuals = pop->getIndividuals();

		for(QListIterator<Individual*> j(individuals); j.hasNext();) {

			Core::getInstance()->executePendingTasks();
			
			Individual *ind = j.next();

			if(verbose) {
				Core::log(QString("NeatEvolutionAlgorithm: Manipulating neuron with id ")
							.append(QString::number(ind->getId())));
			}

			//ignore protected individuals
			if(ind->isGenomeProtected()) {
				cerr << "Ind " << ind->getId() << " is protected" << endl;
				continue;
			}

			if(ind->getGenome() != 0 && dynamic_cast<NeatGenome*>(ind->getGenome()) == 0) {
				Core::log("NeatEvolutionAlgorithm: Could not handle genome.");
				//delete individual from the population, because the genome is not a NeuralNetwork
				pop->getIndividuals().removeAll(ind);

				trashcan.append(ind); //move to thrash
				continue;
			}

			NeatGenome *newGenome = 0;

			//Neat mutation and mating
			bool mutateOnly = Random::nextDouble() < mMutateOnlyProb->get() ? true : false;
			bool mateOnly = Random::nextDouble() < mMateOnlyProb->get() ? true : false;


			if(ind->getParents().empty()) {
				//create new genome.
				newGenome = new NeatGenome(*mGenomeTemplate);
				mateOnly = false;
				cerr << "Ind " << ind->getId() << ": create new Genome." << endl;
			}
			else if(ind->getParents().size() == 2 && !mutateOnly) {
				//do mating
				NeatGenome *parent1 = dynamic_cast<NeatGenome*>(ind->getParents().at(0)->getGenome());
				NeatGenome *parent2 = dynamic_cast<NeatGenome*>(ind->getParents().at(1)->getGenome());

				//check if the smaller of the parents should be preferred during mating.
				//this happens when both parents have the same fitness.
				bool preferSmallerGenome = false;
				if(ind->hasProperty("ParentsEqual")) {
					ind->removeProperty("ParentsEqual");
					preferSmallerGenome = true;
				}

				if(parent1 == 0 || parent2 == 0) {
					Core::log("NeatEvolutionAlgorihtm: Parents did not have a NeatGenome!");
					trashcan.append(ind);
					pop->getIndividuals().removeAll(ind);
					continue;
				}
				newGenome = mate(parent1, parent2, preferSmallerGenome);
				cerr << "Ind " << ind->getId() << ": mating." << endl;
			}
			else if(ind->getParents().size() > 0) {
				NeatGenome *parent1 = dynamic_cast<NeatGenome*>(ind->getParents().at(0)->getGenome());

				if(parent1 == 0) {
					Core::log("NeatEvolutionAlgorihtm: Parents did not have a NeatGenome!");
					trashcan.append(ind);
					pop->getIndividuals().removeAll(ind);
					continue;
				}

				newGenome = new NeatGenome(*parent1);
				mateOnly = false;
				cerr << "Ind " << ind->getId() << ": single parent (or two)." << endl;
			}

			if(!mateOnly) {
				mutate(newGenome);
			}

			ind->setGenome(newGenome);
			addGenomeProperty(newGenome, ind);
		}
	}

	//store genomes to file.
	saveGenerationToFile();

	if(verbose) {
		Core::log("NeatEvolutionAlgorithm: Done with execution.");
	}

	return true;
}


bool NeatEvolutionAlgorithm::reset() {
	return true;
}

NeatGenome* NeatEvolutionAlgorithm::mate(NeatGenome *parent1, NeatGenome *parent2, 
										bool preferSmallerGenome) 
{
	if(parent1 == 0 || parent2 == 0) {
		return 0;
	}
	if(Random::nextDouble() < mMateMultiPointProb->get()) {
		return mateMultiPoint(parent1, parent2, preferSmallerGenome, false);
	}
	else if(Random::nextDouble() < mMateMultiPointAvgProb->get()) {
		return mateMultiPoint(parent1, parent2, preferSmallerGenome, true);
	}
	
	return new NeatGenome(*parent1);
}


void NeatEvolutionAlgorithm::mutate(NeatGenome *genome) {
	if(genome == 0) {
		return;
	}
	if(Random::nextDouble() < mMutateAddNodeProb->get()) {
// 		cerr << "Mutate add Node" << endl;
		mutateAddNode(genome);
	}
	else if(Random::nextDouble() < mMutateAddLinkProb->get()) {
// 		cerr << "Mutate add link" << endl;
		mutateAddLink(genome);
	}
	else {
// 		cerr << "Mutate Other" << endl;
		if(Random::nextDouble() < mMutateLinkWeightsProb->get()) {
			mutateLinkWeights(genome);
		}
		if(Random::nextDouble() < mMutateToggleEnableProb->get()) {
			mutateToggleEnable(genome);
		}
		if(Random::nextDouble() < mMutateReenableProb->get()) {
			mutateReenable(genome);
		}
	}
}



NeatGenome* NeatEvolutionAlgorithm::mateMultiPoint(NeatGenome *parent1, NeatGenome *parent2, 
								bool preferSmallerGenome, bool average) 
{
	if(parent1 == 0 || parent2 == 0) {
		return 0;
	}
	if(preferSmallerGenome) {
		if(parent1->getConnectionGenes().size() > parent2->getConnectionGenes().size()) {
			//flip parents (better is parent1)
			NeatGenome *tmp = parent1;
			parent1 = parent2;
			parent2 = tmp;
		}
	}

	NeatGenome *genome = new NeatGenome();

	//make sure that all input, output and bias neurons are present in the new genome.
	for(QListIterator<NeatNodeGene*> i(parent1->getNodeGenes()); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node->mType != NeatNodeGene::HIDDEN) {
			genome->addNodeGene(new NeatNodeGene(*node));
		}
	}

	QList<NeatConnectionGene*> &genes1 = parent1->getConnectionGenes();
	QList<NeatConnectionGene*> &genes2 = parent2->getConnectionGenes();

	QList<NeatConnectionGene*>::iterator g1 = genes1.begin();
	QList<NeatConnectionGene*>::iterator g2 = genes2.begin();

	while(g1 != genes1.end() || g2 != genes2.end()) {
		NeatConnectionGene candidate(0, 0, 0, 0.0);

		if(g1 == genes1.end()) {
			//stop adding 
			break;
		}
		else if(g2 == genes2.end()) {
			candidate = *(*g1);
			++g1;
		}
		else {
			long id1 = (*g1)->mId;
			long id2 = (*g2)->mId;

			if(id1 == id2) {
				if(Random::nextDouble() < 0.5) {
					candidate = *(*g1);
				}
				else {
					candidate = *(*g2);
				}
				if((*g1)->mEnabled == false && (*g2)->mEnabled == false) {
					if(Random::nextDouble() < 0.75) {
						candidate.mEnabled  = false;
					}
					else {
						candidate.mEnabled = true;
					}
				}
				if(average) {
					//use the average weight of both parents
					candidate.mWeight = ((*g1)->mWeight + (*g2)->mWeight) / 2.0;
				}
				++g1;
				++g2;
			}
			else if(id1 < id2) {
				candidate = *(*g1);
				++g1;
			}
			else if(id1 > id2) {
				++g2;
				continue;
			}

			//check if candidate represents an already available connection
			bool skipCandidate = false;
			for(QListIterator<NeatConnectionGene*> i(genome->getConnectionGenes()); i.hasNext();) {
				NeatConnectionGene *link = i.next();
				if((link->mInputNode == candidate.mInputNode 
							&& link->mOutputNode == candidate.mOutputNode)
					|| (link->mInputNode == link->mOutputNode 
							&& link->mInputNode == candidate.mOutputNode 
							&& link->mOutputNode == candidate.mInputNode)) 
				{
					skipCandidate = true;
					break;
				}
			}
			if(skipCandidate) {
				continue;
			}
		}

		//add gene
		NeatConnectionGene *newLink = new NeatConnectionGene(candidate);
		genome->addConnectionGene(newLink);

		//check if nodes have to be added.
		NeatNodeGene *inputNode = getNodeGene(newLink->mInputNode, genome);
		if(inputNode == 0) {
			genome->addNodeGene(new NeatNodeGene(newLink->mInputNode, NeatNodeGene::HIDDEN));
		}
		NeatNodeGene *outputNode = getNodeGene(newLink->mOutputNode, genome);
		if(outputNode == 0) {
			genome->addNodeGene(new NeatNodeGene(newLink->mOutputNode, NeatNodeGene::HIDDEN));
		}
	}

	return genome;
}


void NeatEvolutionAlgorithm::mutateAddNode(NeatGenome *genome) {
	if(genome == 0) {
		return;
	}

	QList<NeatConnectionGene*> enabledConnections;
	QList<NeatConnectionGene*> availableConnections = genome->getConnectionGenes();

	QList<NeatNodeGene*> availableNodes = genome->getNodeGenes();
	int biasNodeId = -1;
	for(QListIterator<NeatNodeGene*> i(availableNodes); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node->mType == NeatNodeGene::BIAS) {
			biasNodeId = node->mId;
			break;
		}
	}

	for(QListIterator<NeatConnectionGene*> i(availableConnections); i.hasNext();) {
		NeatConnectionGene *link = i.next();
		if(link->mEnabled && link->mOutputNode != biasNodeId) {
			enabledConnections.append(link);
		}
	}
	
	if(enabledConnections.empty()) {
		//no link to split.
		return;
	}

	NeatConnectionGene *mutatedLink = 0;
	//for small genomes (< 15) bias the node selection towards old links
	if(availableConnections.size() < 15) {
		for(QListIterator<NeatConnectionGene*> i(enabledConnections); i.hasNext();) {
			NeatConnectionGene *link = i.next();
			if(Random::nextDouble() < 0.3) {
				mutatedLink = link;
				break;
			}
		}
	}
	if(mutatedLink == 0) {
		//choose at random
		mutatedLink = enabledConnections.at(Random::nextInt(enabledConnections.size()));
	}

	if(mutatedLink == 0) {
		return;
	}

	//check if this mutation was already done this generation
	for(QListIterator<NeatNodeInnovation> i(mNodeInnovations); i.hasNext();) {
		NeatNodeInnovation innovation = i.next();
		if(innovation.mNode != 0 && innovation.mSplittedConnection != 0
			&& innovation.mLink1 != 0 && innovation.mLink2 != 0) 
		{
			if(innovation.mSplittedConnection->mId == mutatedLink->mId) {
				//copy existing innovation
				NeatNodeGene *newNode = new NeatNodeGene(*innovation.mNode);
				NeatConnectionGene *newLink1 = new NeatConnectionGene(*innovation.mLink1);
				NeatConnectionGene *newLink2 = new NeatConnectionGene(*innovation.mLink2);

				genome->getNodeGenes().append(newNode);
				genome->addConnectionGene(newLink1);
				genome->addConnectionGene(newLink2);

				mutatedLink->mEnabled = false;
				return;
			}
		}
	}

	//this is a new innovation
	NeatNodeGene *newNode = new NeatNodeGene(NeatGenome::generateUniqueId(), NeatNodeGene::HIDDEN);
	NeatConnectionGene *newLink1 = new NeatConnectionGene(NeatGenome::generateUniqueId(), 
											newNode->mId, mutatedLink->mOutputNode, 1.0);
	NeatConnectionGene *newLink2 = new NeatConnectionGene(NeatGenome::generateUniqueId(), 
											mutatedLink->mInputNode, newNode->mId, mutatedLink->mWeight);
	genome->getNodeGenes().append(newNode);
	genome->addConnectionGene(newLink1);
	genome->addConnectionGene(newLink2);

	mutatedLink->mEnabled = false;

	//memorize this innovation
	mNodeInnovations.append(NeatNodeInnovation(newNode, mutatedLink, newLink1, newLink2));
	
}

void NeatEvolutionAlgorithm::mutateAddLink(NeatGenome *genome) {
	if(genome == 0) {
		return;
	}
	QList<NeatConnectionGene*> &links = genome->getConnectionGenes();
	QList<NeatNodeGene*> inputNodes = genome->getNodeGenes();
	QList<NeatNodeGene*> outputNodes = genome->getNodeGenes();
	QList<NeatNodeGene*> inputOutputNodes = genome->getNodeGenes();

	//remove all neurons that can not be target of a synapse.
	for(QListIterator<NeatNodeGene*> i(genome->getNodeGenes()); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node != 0 
			&& (node->mType == NeatNodeGene::BIAS || node->mType == NeatNodeGene::SENSOR)) 
		{
			inputNodes.removeAll(node);
			inputOutputNodes.removeAll(node);
		}
	}

	int inputId = 0;
	int outputId = 0;

	if(Random::nextDouble() < mMutateAddRecurrentLinkProb->get()) {

		//remove all neurons that can not be target of a loop
		for(QListIterator<NeatConnectionGene*> i(links); i.hasNext();) {
			NeatConnectionGene *link = i.next();
			NeatNodeGene *input = getNodeGene(link->mInputNode, genome);
			NeatNodeGene *output = getNodeGene(link->mOutputNode, genome);
	
			if(input == 0 || output == 0) {
				Core::log("NeatEvolutionAlgorithm: Input or Output id was invalid.");
				return;
			}
			if(input == output) {
				inputOutputNodes.removeAll(input);
			}
		}
		if(!inputOutputNodes.empty()) {
			NeatNodeGene *node = inputOutputNodes.at(Random::nextInt(inputOutputNodes.size()));
			inputId = node->mId;
			outputId = node->mId;
		}
	}
	else if(!inputNodes.empty() && !outputNodes.empty()) {
		NeatNodeGene *input = inputNodes.at(Random::nextInt(inputNodes.size()));

		QList<NeatNodeGene*> outputs = outputNodes;

		//remove all neurons that can not be a source for the chosen input.
		for(QListIterator<NeatConnectionGene*> i(links); i.hasNext();) {
			NeatConnectionGene *link = i.next();
			NeatNodeGene *out = getNodeGene(link->mOutputNode, genome);
	
			if(out == 0) {
				Core::log("NeatEvolutionAlgorithm: Input or Output id was invalid.");
				return;
			}
			if(input->mId == link->mInputNode) {
				outputs.removeAll(out);
			}
		}

		if(!outputs.empty()) {
			inputId = input->mId;
			outputId = outputs.at(Random::nextInt(outputs.size()))->mId;
		}
	}
	else {
		//can not add a link.
		return;
	}

	//check if there is already an innovation with this link
	NeatConnectionGene *availableInnovation = 0;
	for(QListIterator<NeatLinkInnovation> i(mLinkInnovations); i.hasNext();) {
		NeatConnectionGene *link = i.next().mConnection;
		if(link != 0 && link->mInputNode == inputId && link->mOutputNode == outputId) {
			availableInnovation = link;
			break;
		}
	}
	if(availableInnovation != 0) {
		NeatConnectionGene *newGene = new NeatConnectionGene(*availableInnovation);
		genome->addConnectionGene(newGene);
	}
	else {
		double weight = Random::nextSign() * Random::nextDouble() * 10.0; //TODO param
		NeatConnectionGene *newGene = new NeatConnectionGene(NeatGenome::generateUniqueId(), 
											inputId, outputId, weight);
		links.append(newGene);
		mLinkInnovations.append(NeatLinkInnovation(newGene));
	}
}

void NeatEvolutionAlgorithm::mutateLinkWeights(NeatGenome *genome) {
	QList<NeatConnectionGene*> &links = genome->getConnectionGenes();	

	double geneTotal = links.size();
	double endPart = geneTotal * 0.8;
	double num = 0.0;
	double rate = 1.0;
	double power = mLinkWeightMutationPower->get();
	double powerMod = 1.0;

	bool severe = Random::nextDouble() < 0.5 ? true : false;
	double gaussPoint = 0.0;
	double coldGaussPoint = 0.0;
	

	for(QListIterator<NeatConnectionGene*> i(links); i.hasNext();) {
		NeatConnectionGene *link = i.next();

		if(severe) {
			gaussPoint = 0.3;
			coldGaussPoint = 0.1;
		}
		else if((geneTotal >= 10.0) && (num > endPart)) {
			gaussPoint = 0.5;
			coldGaussPoint = 0.3;
		}
		else {
			//Half the time don't do any cold mutations
			if(Random::nextDouble() > 0.5) {
				gaussPoint = 1.0 - rate;
				coldGaussPoint = 1.0 - rate - 0.1;
			}
			else {
				gaussPoint = 1.0 - rate;
				coldGaussPoint = 1.0 - rate;
			}
		}

		double randnum = Random::nextSign() * Random::nextDouble() * power * powerMod;
		double randchoice = Random::nextDouble();
		if(randchoice > gaussPoint) {
			link->mWeight += randnum;
		}
		else if(randchoice > coldGaussPoint) {
			link->mWeight = randnum;
		}

		num += 1.0;
	}
}

void NeatEvolutionAlgorithm::mutateToggleEnable(NeatGenome *genome) {

	if(genome == 0 || genome->getConnectionGenes().empty()) {
		return;
	}
	NeatConnectionGene *gene = genome->getConnectionGenes().at(Random::nextInt(
								genome->getConnectionGenes().size()));

	if(gene->mEnabled == true) {
		//make sure that the target neuron is linked by another link to avoid isolation of network parts.
		bool found = false;
		for(QListIterator<NeatConnectionGene*> i(genome->getConnectionGenes()); i.hasNext();) {
			NeatConnectionGene *g = i.next();
			if(g->mId != gene->mId && g->mInputNode == gene->mInputNode && g->mEnabled) {
				found = true;
			}
		}
		if(!found) {
			return;
		}
	}
	gene->mEnabled = !gene->mEnabled;
}

void NeatEvolutionAlgorithm::mutateReenable(NeatGenome *genome) {
	QList<NeatConnectionGene*> links = genome->getConnectionGenes();
	QList<NeatConnectionGene*> disabledLinks;

	for(QListIterator<NeatConnectionGene*> i(links); i.hasNext();) {
		NeatConnectionGene *link = i.next();
		if(link->mEnabled == false) {
			disabledLinks.append(link);
		}
	}
	if(!disabledLinks.empty()) {
		NeatConnectionGene *link = disabledLinks.at(Random::nextInt(disabledLinks.size()));
		if(link != 0) {
			link->mEnabled = true;
		}
	}
}


NeatNodeGene* NeatEvolutionAlgorithm::getNodeGene(int id, NeatGenome *genome) {
	for(QListIterator<NeatNodeGene*> i(genome->getNodeGenes()); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node->mId == id) {
			return node;
		}
	}
	return 0;
}

void NeatEvolutionAlgorithm::addGenomeProperty(NeatGenome *genome, Individual *individual) {
	if(genome == 0 || individual == 0) {
		return;
	}
	QString genomeText;
	for(int i = 0; i < genome->getNodeGenes().size(); ++i) {
		if(i != 0) {
			genomeText.append(",");
		}
		NeatNodeGene *node = genome->getNodeGenes().at(i);
		genomeText.append(QString::number(node->mId));
		if(node->mType == NeatNodeGene::SENSOR) {
			genomeText.append("i");
		}
		else if(node->mType == NeatNodeGene::OUTPUT) {
			genomeText.append("o");
		}
	}
	genomeText.append("|");
	for(int i = 0; i < genome->getConnectionGenes().size(); ++i) {
		if(i != 0) {
			genomeText.append(",");
		}
		NeatConnectionGene *link = genome->getConnectionGenes().at(i);

		QString enabledMarker = link->mEnabled ? "[" : "{";
		QString enabledMarkerEnd = link->mEnabled ? "]" : "}";
		genomeText = genomeText + QString::number(link->mId) 
					+ enabledMarker + QString::number(link->mOutputNode) 
					+ "," + QString::number(link->mInputNode) + enabledMarkerEnd
					+ QString::number(link->mWeight, 'f', 2);
	}
	individual->setProperty("NeatGenome", genomeText);
}


void NeatEvolutionAlgorithm::saveGenerationToFile() {
	EvolutionManager *em = Evolution::getEvolutionManager();
	QString dir = em->getEvolutionWorkingDirectory();
	dir = dir + "/neat";
	Core::getInstance()->enforceDirectoryPath(dir);
	int genId = em->getCurrentGenerationValue()->get();

	for(QListIterator<World*> i(em->getEvolutionWorlds()); i.hasNext();) {
		World *world = i.next();
		for(QListIterator<Population*> j(world->getPopulations()); j.hasNext();) {
			Population *pop = j.next();
			QString fileName = dir + "/w_" + world->getName() + "_p_" 
					+ pop->getName() + "_gen" + QString::number(genId) + ".neat";
			QFile file(fileName);
			if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
				Core::log("NeatEvolutionAlgorithm: Could not save generation to file " + fileName);
				file.close();
				continue;
			}
			QTextStream output(&file);
			
			for(QListIterator<Individual*> k(pop->getIndividuals()); k.hasNext();) {
				Individual *ind = k.next();
				if(ind == 0) {
					continue;
				}
				NeatGenome *genome = dynamic_cast<NeatGenome*>(ind->getGenome());
				if(genome == 0) {
					continue;
				}
				output << genome->getGenomeAsString() << endl;
			}

			file.close();
		}
	}
}

/**
 * Adds a NeatConnectionGene at the correct location within the gene to ensure
 * an increasing id over the entire gene list.
 */
// void NeatEvolutionAlgorithm::addConnectionGene(NeatGenome *genome, NeatConnectionGene *link) {
// 	if(genome == 0 || link == 0) {
// 		return;
// 	}
// 
// 	QList<NeatConnectionGene*> &links = genome->getConnectionGenes();
// 	QList<NeatConnectionGene*>::iterator i = links.begin();
// 	for(; i != links.end(); ++i) {
// 		if((*i)->mId > link->mId) {
// 			links.insert(i, link);
// 			break;
// 		}
// 	}
// 	if(i == links.end()) {
// 		links.append(link);
// 	}
// }
// 
// void NeatEvolutionAlgorithm::addNodeGene(NeatGenome *genome, NeatNodeGene *node) {
// 	if(genome == 0 || node == 0) {
// 		return;
// 	}
// 
// 	QList<NeatNodeGene*> &nodes = genome->getNodeGenes();
// 	QList<NeatNodeGene*>::iterator i = nodes.begin();
// 	for(; i != nodes.end(); ++i) {
// 		if((*i)->mId > node->mId) {
// 			nodes.insert(i, node);
// 			break;
// 		}
// 	}
// 	if(i == nodes.end()) {
// 		nodes.append(node);
// 	}
// }

}



