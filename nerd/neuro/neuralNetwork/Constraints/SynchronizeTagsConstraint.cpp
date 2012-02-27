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



#include "SynchronizeTagsConstraint.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Util/Util.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"
#include <Value/ULongLongValue.h>


using namespace std;

namespace nerd {


/**
 * Constructs a new SynchronizeTagsConstraint.
 */
SynchronizeTagsConstraint::SynchronizeTagsConstraint()
	: GroupConstraint("SynchronizeTags")
{
	mTagList = new StringValue("");
	mPrototypeId = new ULongLongValue();
	mMode = new StringValue("s");
	mModeParams = new StringValue("");
	mRecursive = new BoolValue(false);
	
	addParameter("TagList", mTagList);
	addParameter("PrototypeId", mPrototypeId);
	addParameter("Mode", mMode);
	addParameter("ModeParams", mModeParams);
	addParameter("Recursive", mRecursive);
}


/**
 * Copy constructor. 
 * 
 * @param other the SynchronizeTagsConstraint object to copy.
 */
SynchronizeTagsConstraint::SynchronizeTagsConstraint(const SynchronizeTagsConstraint &other)
	: Object(), ValueChangedListener(), GroupConstraint(other) 
{
	mTagList = dynamic_cast<StringValue*>(getParameter("TagList"));
	mPrototypeId = dynamic_cast<ULongLongValue*>(getParameter("PrototypeId"));
	mMode = dynamic_cast<StringValue*>(getParameter("Mode"));
	mModeParams = dynamic_cast<StringValue*>(getParameter("ModeParams"));
	mRecursive = dynamic_cast<BoolValue*>(getParameter("Recursive"));
}

/**
 * Destructor.
 */
SynchronizeTagsConstraint::~SynchronizeTagsConstraint() {
}

GroupConstraint* SynchronizeTagsConstraint::createCopy() const {
	return new SynchronizeTagsConstraint(*this);
}

bool SynchronizeTagsConstraint::isValid(NeuronGroup*) {
	return true;
}


bool SynchronizeTagsConstraint::applyConstraint(NeuronGroup *owner, 
									CommandExecutor*, 
									QList<NeuralNetworkElement*> &trashcan)
{
	if(owner == 0 || owner->getOwnerNetwork() == 0) {
		//do not do anything.
		return true;
	}

	ModularNeuralNetwork *net = owner->getOwnerNetwork();

	bool inputMode = mMode->get().toLower().contains("i") ? true : false;
	bool outputMode = mMode->get().toLower().contains("o") ? true : false;
	bool groupMode = mMode->get().toLower().contains("g") ? true : false;
	bool synapseMode = (mMode->get().toLower().contains("s")) ? true : false;
	bool neuronMode = (mMode->get().toLower().contains("n")) ? true : false;
	
	if(!synapseMode && !neuronMode) {
		//use synapse mode as default, if not specified.
		synapseMode = true;
	}
	if(!inputMode && !outputMode && !groupMode) {
		//use group mode as default.
		groupMode = true;
	}
	
	QList<Neuron*> validExternalNeurons;
	QList<NeuronGroup*> validGroups = NeuralNetworkUtil::getGroupsFromIdList(net, mModeParams->get());
	for(QListIterator<NeuronGroup*> i(validGroups); i.hasNext();) {
		Util::addWithoutDuplicates<Neuron>(validExternalNeurons, i.next()->getAllEnclosedNeurons());
	}

	QList<Neuron*> neurons;
	QList<Synapse*> innerSynapses;
	QList<Synapse*> incommingSynapses;
	QList<Synapse*> outgoingSynapses;
	
	if(mRecursive->get()) {
		neurons = owner->getAllEnclosedNeurons();
	}
	else {
		neurons = owner->getNeurons();
	}
	
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();

		QList<Synapse*> inSynapses = neuron->getSynapses();
		for(QListIterator<Synapse*> i(inSynapses); i.hasNext();) {
			Synapse *s = i.next();
			if(neurons.contains(s->getSource())) {
				innerSynapses.append(s);
			}
			else {
				if(validExternalNeurons.empty() || validExternalNeurons.contains(s->getSource())) {
					incommingSynapses.append(s);
				}
			}
		}
		QList<Synapse*> outSynapses = neuron->getOutgoingSynapses();
		for(QListIterator<Synapse*> i(outSynapses); i.hasNext();) {
			Synapse *s = i.next();
			if(!neurons.contains(dynamic_cast<Neuron*>(s->getTarget()))) {
				if(validExternalNeurons.empty() 
						|| validExternalNeurons.contains(dynamic_cast<Neuron*>(s->getTarget()))) 
				{
					outgoingSynapses.append(s);
				}
			}
		}
	}
	
	bool networkChanged = false;
	
	QHash<QString, QString> tagList = getPropertyTags(net);
	
	if(synapseMode) {
		if(groupMode) {
			for(QListIterator<Synapse*> i(innerSynapses); i.hasNext();) {
				if(synchronizeTags(i.next(), tagList)) {
					networkChanged = true;
				}
			}
		}
		if(inputMode) {
			for(QListIterator<Synapse*> i(incommingSynapses); i.hasNext();) {
				if(synchronizeTags(i.next(), tagList)) {
					networkChanged = true;
				}
			}
		}
		if(outputMode) {
			for(QListIterator<Synapse*> i(outgoingSynapses); i.hasNext();) {
				if(synchronizeTags(i.next(), tagList)) {
					networkChanged = true;
				}
			}
		}
	}
	
	if(neuronMode) {
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			if(synchronizeTags(i.next(), tagList)) {
				networkChanged = true;
			}
		}
	}

	return !networkChanged;
}
		
bool SynchronizeTagsConstraint::equals(GroupConstraint *constraint) {
	if(!GroupConstraint::equals(constraint)) {
		return false;
	}
	SynchronizeTagsConstraint *cc = 
					dynamic_cast<SynchronizeTagsConstraint*>(constraint);

	if(cc == 0) {
		return false;
	}
	return true;
}

QHash<QString, QString> SynchronizeTagsConstraint::getPropertyTags(ModularNeuralNetwork *net) {
	QHash<QString, QString> propertyTags;
	
	if(net == 0) {
		return propertyTags;
	}
	
	if(mPrototypeId->get() != 0) {
		QList<NeuralNetworkElement*> elements;
		net->getNetworkElements(elements);
		NeuralNetworkElement *element = NeuralNetwork::selectNetworkElementById(
											mPrototypeId->get(), elements);
		if(element == 0) {
			mErrorMessage = QString("Could not find prototype network element with id [")
								.append(QString::number(mPrototypeId->get()))
								.append("]!");
			return propertyTags;
		}
		else {
			propertyTags = element->getProperties();
		}
	}
	else {
		QString propertyListString = mTagList->get();
		QStringList properties = Util::getItemsFromCurlyBraceSeparatedString(propertyListString);
		
		QString errorMessage;
		
		for(QListIterator<QString> i(properties); i.hasNext();) {
			QString property = i.next();
			QStringList tag = property.split("=");
			if(tag.size() == 2) {
				propertyTags.insert(tag.at(0), tag.at(1));
			}
			else if(tag.size() == 1) {
				propertyTags.insert(tag.at(0), "");
			}
			else {
				errorMessage.append("Could not parse property tag list [")
								.append(property).append("]!\n");
			}
		}
		if(errorMessage.length() != 0) {
			mErrorMessage = errorMessage;
		}
	}
	
	return propertyTags;
}

bool SynchronizeTagsConstraint::synchronizeTags(NeuralNetworkElement *element, 
												QHash<QString, QString> &tags) 
{
	if(element == 0 || tags.empty()) {
		return false;
	}
	bool networkModified = false;
	for(QHashIterator<QString, QString> i(tags); i.hasNext();) {
		i.next();
		if(!element->hasExactProperty(i.key())
				|| element->getExactProperty(i.key()) != i.value())
		{
			element->setProperty(i.key(), i.value());
			networkModified = true;
		}
	}
	
	return networkModified;
}


}



