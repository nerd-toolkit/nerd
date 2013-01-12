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



#include "NeatGenome.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Value/ValueManager.h"
#include "Neat/NeatConstants.h"
#include <QTextStream>
#include <QStringList>

using namespace std;

namespace nerd {


NeatNodeGene::NeatNodeGene(long id, int type) 
	: mId(id), mType(type)
{}

NeatNodeGene::NeatNodeGene(const NeatNodeGene &other)
	: mId(other.mId), mType(other.mType)
{}


NeatConnectionGene::NeatConnectionGene(long id, long inNode, long outNode, double weight)
	: mId(id), mInputNode(inNode), mOutputNode(outNode), mWeight(weight), mEnabled(true)
{}

NeatConnectionGene::NeatConnectionGene(const NeatConnectionGene &other)
	: mId(other.mId), mInputNode(other.mInputNode), mOutputNode(other.mOutputNode),
	  mWeight(other.mWeight), mEnabled(other.mEnabled)
{}



long NeatGenome::mIdCounter = 0;

long NeatGenome::generateUniqueId() {
	return ++mIdCounter;
}


/**
 * Constructs a new NeatGenome.
 */
NeatGenome::NeatGenome()
	: mDisjointFactor(0), mExcessFactor(0), mMutationDifferenceFactor(0)
{
	mId = NeatGenome::generateUniqueId();

	ValueManager *vm = Core::getInstance()->getValueManager();
	mDisjointFactor = vm->getDoubleValue(NeatConstants::VALUE_GENOME_DISJOINT_FACTOR);
	mExcessFactor = vm->getDoubleValue(NeatConstants::VALUE_GENOME_EXCESS_FACTOR);
	mMutationDifferenceFactor = vm->getDoubleValue(
							NeatConstants::VALUE_GENOME_MUTATION_DIFFERENCE_FACTOR);

	if(mDisjointFactor == 0) {
		mDisjointFactor = new DoubleValue(0.5);
		vm->addValue(NeatConstants::VALUE_GENOME_DISJOINT_FACTOR, mDisjointFactor);
	}
	if(mExcessFactor == 0) {
		mExcessFactor = new DoubleValue(0.5);
		vm->addValue(NeatConstants::VALUE_GENOME_EXCESS_FACTOR, mExcessFactor);
	}
	if(mMutationDifferenceFactor == 0) {
		mMutationDifferenceFactor = new DoubleValue(0.5);
		vm->addValue(NeatConstants::VALUE_GENOME_MUTATION_DIFFERENCE_FACTOR, mMutationDifferenceFactor);
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the NeatGenome object to copy.
 */
NeatGenome::NeatGenome(const NeatGenome &other) {
	mId = other.mId;
	for(QListIterator<NeatNodeGene*> i(other.mNodes); i.hasNext();) {
		mNodes.append(new NeatNodeGene(*(i.next())));
	}
	for(QListIterator<NeatConnectionGene*> i(other.mConnectionGenes); i.hasNext();) {
		mConnectionGenes.append(new NeatConnectionGene(*(i.next())));
	}

	mDisjointFactor = other.mDisjointFactor;
	mExcessFactor = other.mExcessFactor;
	mMutationDifferenceFactor = other.mMutationDifferenceFactor;
}

/**
 * Destructor.
 */
NeatGenome::~NeatGenome() {
	while(!mNodes.empty()) {
		NeatNodeGene *node = mNodes.at(0);
		mNodes.removeAll(node);
		delete node;
	}
	while(!mConnectionGenes.empty()) {
		NeatConnectionGene *connection = mConnectionGenes.at(0);
		mConnectionGenes.removeAll(connection);
		delete connection;
	}
}



QString NeatGenome::getName() const {
	return QString("NeatGenome[").append(QString::number(mId)).append("]");
}

void NeatGenome::setId(long id) {
	mId = id;
}


long NeatGenome::getId() const {
	return mId;
}



QList<NeatNodeGene*>& NeatGenome::getNodeGenes() {
	return mNodes;
}


QList<NeatConnectionGene*>& NeatGenome::getConnectionGenes() {
	return mConnectionGenes;
}



QString NeatGenome::getGenomeAsString() const {
	QString genomeString;
	QTextStream genome(&genomeString);
	bool first = true;
	for(QListIterator<NeatNodeGene*> i(mNodes); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(first) {
			first = false;
		}
		else {
			genome << "|";
		}
		genome << node->mId << "," << node->mType;
	}
	genome << "#";
	first = true;
	for(QListIterator<NeatConnectionGene*> i(mConnectionGenes); i.hasNext();) {
		NeatConnectionGene *link = i.next();
		if(first) {
			first = false;
		}
		else {
			genome << "|";
		}
		genome << link->mId << "," << link->mOutputNode << "," << link->mInputNode << "," 
			   << link->mWeight << "," << (link->mEnabled ? "1" : "0");
	}	

	return genomeString;
}

bool NeatGenome::setFromString(const QString &genomeString) {
	QStringList genomeParts = genomeString.split("#");
	if(genomeParts.size() != 2) {
		return false;
	}
	QStringList nodeList = genomeParts.at(0).split("|");
	QStringList linkList = genomeParts.at(1).split("|");

	//destroy previous elements
	while(!mNodes.empty()) {
		NeatNodeGene *node = mNodes.at(0);
		mNodes.removeAll(node);
		delete node;
	}
	while(!mConnectionGenes.empty()) {
		NeatConnectionGene *link = mConnectionGenes.at(0);
		mConnectionGenes.removeAll(link);
		delete link;
	}

	//create new nodes
	for(QListIterator<QString> i(nodeList); i.hasNext();) {
		QString nodeString = i.next();
		QStringList nodeElements = nodeString.split(",");
		if(nodeElements.size() != 2) {
			Core::log("NeatGenome: Problem parsing node [" + nodeString + "]!");
			return false;
		}
		bool ok = true;
		long id = nodeElements.at(0).toLong(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing node [" + nodeString + "]!");
			return false;
		}
		int type = nodeElements.at(1).toInt(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing node [" + nodeString + "]!");
			return false;
		}
		NeatNodeGene *node = new NeatNodeGene(id, type);
		if(!addNodeGene(node)) {
			Core::log("NeatGenome: Could not add node [" + nodeString + "]!");
			return false;
		}
	}

	//create new links
	for(QListIterator<QString> i(linkList); i.hasNext();) {
		QString linkString = i.next();
		QStringList linkElements = linkString.split(",");
		if(linkElements.size() != 5) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		bool ok = true;
		long id = linkElements.at(0).toLong(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		long outputNode = linkElements.at(1).toLong(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		long inputNode = linkElements.at(2).toLong(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		double weight = linkElements.at(3).toDouble(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		int enabled = linkElements.at(4).toInt(&ok);
		if(!ok) {
			Core::log("NeatGenome: Problem parsing link [" + linkString + "]!");
			return false;
		}
		NeatConnectionGene *link = new NeatConnectionGene(id, inputNode, outputNode, weight);
		link->mEnabled = (enabled == 0) ? false : true;

		if(!addConnectionGene(link)) {
			Core::log("NeatGenome: Could not add link [" + linkString + "]!");
			return false;
		}
	}

	return true;
	
}

bool NeatGenome::isCompatible(NeatSpeciesOrganism *organism, double compatibilityThreshold) {
	NeatGenome *genome = dynamic_cast<NeatGenome*>(organism);
	if(genome == 0) {
		return false;
	}

	int excessCounter = 0;
	int matchCounter = 0;
	int mutationDifference = 0;
	int disjointCounter = 0;

	QList<NeatConnectionGene*> &otherGenes = genome->getConnectionGenes();

	QList<NeatConnectionGene*>::iterator p1 = mConnectionGenes.begin();
	QList<NeatConnectionGene*>::iterator p2 = otherGenes.begin();

	while(!(p1 == mConnectionGenes.end() && p2 == otherGenes.end())) {
		if(p1 == mConnectionGenes.end()) {
			++excessCounter;
			++p2;
		}
		else if(p2 == otherGenes.end()) {
			++excessCounter;
			++p1;
		}
		else {
			long innov1 = (*p1)->mId;
			long innov2 = (*p2)->mId;

			if(innov1 == innov2) {
				++matchCounter;
				mutationDifference += Math::abs((*p1)->mWeight - (*p2)->mWeight);
				++p1;
				++p2;
			}
			else if(innov1 < innov2) {
				++p1;
				++disjointCounter;
			}
			else if(innov1 > innov2) {
				++p2;
				++disjointCounter;
			}
		}
	}

	return ((mDisjointFactor->get() * ((double) disjointCounter))
				+ (mExcessFactor->get() * ((double) excessCounter))
				+ (mMutationDifferenceFactor->get() * ((double) mutationDifference))) 
			<= compatibilityThreshold;

}


/**
 * Adds a NeatConnectionGene at the correct location within the gene to ensure
 * an increasing id over the entire gene list.
 */
bool NeatGenome::addConnectionGene(NeatConnectionGene *link) {
	if(link == 0) {
		return false;
	}
	bool foundInput = false;
	bool foundOutput = false;
	for(QListIterator<NeatNodeGene*> i(mNodes); i.hasNext();) {
		NeatNodeGene *node = i.next();
		if(node->mId == link->mInputNode) {
			foundInput = true;
		}
		if(node->mId == link->mOutputNode) {
			foundOutput = true;
		}
		if(foundInput && foundOutput) {
			break;
		}
	}
	if(!foundInput || !foundOutput) {
		Core::log("NeatGenome: Could not add link because of missing input / output nodes.");
		return false;
	}

	QList<NeatConnectionGene*> &links = mConnectionGenes;
	QList<NeatConnectionGene*>::iterator i = links.begin();
	for(; i != links.end(); ++i) {
		if((*i)->mId == link->mId) {
			//do not allow two links with the same id
			return false;
		}
		if((*i)->mId > link->mId) {
			links.insert(i, link);
			break;
		}
	}
	if(i == links.end()) {
		links.append(link);
	}
	return true;
}

bool NeatGenome::addNodeGene(NeatNodeGene *node) {
	if(node == 0) {
		return false;
	}

	QList<NeatNodeGene*> &nodes = mNodes;
	QList<NeatNodeGene*>::iterator i = nodes.begin();
	for(; i != nodes.end(); ++i) {	
		//Do not allow two nodes with the same id
		if((*i)->mId == node->mId) {
			return false;
		}
		if((*i)->mId > node->mId) {
			nodes.insert(i, node);
			break;
		}
	}
	if(i == nodes.end()) {
		nodes.append(node);
	}
	return true;
}


}



