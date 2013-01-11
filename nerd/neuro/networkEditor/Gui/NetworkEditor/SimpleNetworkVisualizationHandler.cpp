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



#include "SimpleNetworkVisualizationHandler.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleSynapseItem.h"
#include "Gui/NetworkEditor/SimpleNeuronItem.h"
#include "Network/Synapse.h"
#include "Network/Neuron.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Gui/NetworkEditor/SimpleModuleItem.h"
#include "Gui/NetworkEditor/SimpleGroupItem.h"
#include "Math/Math.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include <QPointF>
#include <QRectF>
#include "Util/Tracer.h"
#include "Util/NetworkEditorUtil.h"
#include "NeuralNetworkConstants.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

#define GRID_X_INCREMENT 150.0
#define GRID_Y_NEWLINE_INCREMENT -50.0
#define GRID_NEWLINE_POSITION 1200.0

using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleNetworkVisualizationHandler.
 */
SimpleNetworkVisualizationHandler::SimpleNetworkVisualizationHandler(const QString &name, NetworkVisualization *owner)
	: NetworkVisualizationHandler(name, owner), mNetwork(0)
{
	mMaxSizeOfNeuronPool = new IntValue(150);
	mMaxSizeOfSynapsePool = new IntValue(200);
	mMaxSizeOfModulePool = new IntValue(30);
	mMaxSizeOfGroupPool = new IntValue(30);

	addParameter("MaxNeuronPoolSize", mMaxSizeOfNeuronPool);
	addParameter("MaxSynapsePoolSize", mMaxSizeOfSynapsePool);
	addParameter("MaxModulePoolSize", mMaxSizeOfModulePool);
	addParameter("MaxGroupPoolSize", mMaxSizeOfGroupPool);

	mNeuronItemPrototype = new SimpleNeuronItem(owner);
	mSynapseItemPrototype = new SimpleSynapseItem(owner);
	mModuleItemPrototype = new SimpleModuleItem(owner);
	mGroupItemPrototype = new SimpleGroupItem(owner);

	mGridPosition = QPointF(20.0, 20.0);

}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleNetworkVisualizationHandler object to copy.
 */
SimpleNetworkVisualizationHandler::SimpleNetworkVisualizationHandler(const SimpleNetworkVisualizationHandler &other) 
	: NetworkVisualizationHandler(other), mNetwork(0)
{
	mNeurons.clear();
	mSynapses.clear();

	mMaxSizeOfNeuronPool = dynamic_cast<IntValue*>(getParameter("MaxNeuronPoolSize"));
	mMaxSizeOfSynapsePool = dynamic_cast<IntValue*>(getParameter("MaxSynapsePoolSize"));
	mMaxSizeOfModulePool = dynamic_cast<IntValue*>(getParameter("MaxModulePoolSize"));
	mMaxSizeOfGroupPool = dynamic_cast<IntValue*>(getParameter("MaxGroupPoolSize"));

	mNeuronItemPrototype = other.mNeuronItemPrototype->createCopy();
	mSynapseItemPrototype = other.mSynapseItemPrototype->createCopy();
	mModuleItemPrototype = other.mModuleItemPrototype->createCopy();
	mGroupItemPrototype = other.mGroupItemPrototype->createCopy();

	mGridPosition = other.mGridPosition;
}

/**
 * Destructor.
 */
SimpleNetworkVisualizationHandler::~SimpleNetworkVisualizationHandler() {
	TRACE("SimpleNetworkVisualizationHandler::~SimpleNetworkVisualizationHandler");

	delete mNeuronItemPrototype;
	delete mSynapseItemPrototype;
	delete mModuleItemPrototype;
	delete mGroupItemPrototype;
}



bool SimpleNetworkVisualizationHandler::setNeuralNetwork(ModularNeuralNetwork *network) {
	TRACE("SimpleNetworkVisualizationHandler::setNeuralNetwork");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
	QMutex *selectionMutex = 0;
	if(mOwner != 0) {
		selectionMutex = mOwner->getSelectionMutex();
	}
	if(selectionMutex != 0) {
		selectionMutex->lock();
	}

	conserveSelection();

	NetworkVisualizationHandler::setNeuralNetwork(network);
	clearVisualization();
	mNetwork = network;
	bool ok = updateNetworkView();
	restoreSelection();

	if(selectionMutex != 0) {
		selectionMutex->unlock();
	}

	return ok;
}




bool SimpleNetworkVisualizationHandler::updateNetworkView() {
	TRACE("SimpleNetworkVisualizationHandler::updateNetworkView");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
	
	QList<NeuronItem*> currentNeuronItems = mNeurons;
	QList<SynapseItem*> currentSynapseItems = mSynapses;
	QList<ModuleItem*> currentModuleItems = mModules;
	QList<GroupItem*> currentGroupItems = mGroups;

	for(QListIterator<ModuleItem*> i(currentModuleItems); i.hasNext();) {
		ModuleItem *moduleItem = i.next();
		QList<PaintItem*> members = moduleItem->getMemberItems();
		for(QListIterator<PaintItem*> j(members); j.hasNext();) {
			moduleItem->removeMemberItem(j.next());
		}
	}


	mNeurons.clear();
	mSynapses.clear();
	mModules.clear();
	mGroups.clear();

	if(mNetwork != 0) {

		//setup neurons
		QList<Neuron*> neurons = mNetwork->getNeurons();
		
		for(int i = 0; i < neurons.size(); ++i) {

			Neuron *neuron = neurons.at(i);

			NeuronItem *item = getNeuronItem(neuron);
			if(item == 0) {
				if(!mNeuronItemPool.empty()) {
					item = mNeuronItemPool.takeFirst();
				}
				if(item == 0) {
					item = mNeuronItemPrototype->createCopy();
				}
			}

			if(neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				NetworkEditorUtil::setPaintItemLocation(item, 
						neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
			else {
				setLocationByGrid(item);
			}

			item->setNeuron(neuron);
			item->setActive(true);
			item->setHidden(false);

			if(!mNeurons.contains(item)) {
				mNeuronLookup.insert(neuron, item);
				mNeurons.append(item);
			}

			currentNeuronItems.removeAll(item);
		}

		//remove obsolete neuron items
		for(QListIterator<NeuronItem*> i(currentNeuronItems); i.hasNext();) {
			NeuronItem *item = i.next();
			item->setHidden(true);
			item->setActive(false);
			item->setToHiddenLayer(false);
			mNeuronLookup.remove(item->getNeuron());
			item->setNeuron(0);
			item->setParent(0);
			mNeuronItemPool.append(item);
		}

		//setup synapses
		QList<Synapse*> synapses = mNetwork->getSynapses();

		for(int i = 0; i < synapses.size(); ++i) {

			Synapse *synapse = synapses.at(i);

			SynapseItem *item = getSynapseItem(synapse);

			if(item == 0) {
				if(!mSynapseItemPool.empty()) {
					item = mSynapseItemPool.takeFirst();
				}
				if(item == 0) {
					item = mSynapseItemPrototype->createCopy();
				}
			}
			if(synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				NetworkEditorUtil::setPaintItemLocation(item, 
						synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
			else {
				setLocationByGrid(item);
			}

			item->setSynapse(synapse);
			item->setActive(true);
			item->setHidden(false);

			if(!mSynapses.contains(item)) {
				mSynapseLookup.insert(synapse, item);
				mSynapses.append(item);
			}

			currentSynapseItems.removeAll(item);
		}

		//remove obsolete synapse items
		for(QListIterator<SynapseItem*> i(currentSynapseItems); i.hasNext();) {
			SynapseItem *item = i.next();
			item->setHidden(true);
			item->setActive(false);
			item->setToHiddenLayer(false);
			mSynapseLookup.remove(item->getSynapse());
			item->setSynapse(0);
			item->setParent(0);
			mSynapseItemPool.append(item);
		}

		//setup modules
		QList<NeuroModule*> modules = mNetwork->getNeuroModules();

		for(int i = 0; i < modules.size(); ++i) {

			NeuroModule *module = modules.at(i);

			ModuleItem *item = getModuleItem(module);

			if(item == 0) {
				if(!mModuleItemPool.empty()) {
					item = mModuleItemPool.takeFirst();
				}
				if(item == 0) {
					item = mModuleItemPrototype->createCopy();
				}
			}
			if(module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				NetworkEditorUtil::setPaintItemLocation(item, 
						module->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
			else {
				setLocationByGrid(item);
			}
			if(module->hasProperty(NeuralNetworkConstants::TAG_MODULE_SIZE)) {
				NetworkEditorUtil::setModuleItemSize(item, 
						module->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE));
			}

			item->setModule(module);
			item->setActive(true);
			item->setHidden(false);

			QList<PaintItem*> memberItems = item->getMemberItems();

			for(QListIterator<PaintItem*> k(memberItems); k.hasNext();) {
				PaintItem *it = k.next();
				QPointF pos = it->getGlobalPosition();
				item->removeMemberItem(it);
				it->setLocalPosition(pos);
			}

			if(!mModules.contains(item)) {
				mModuleLookup.insert(module, item);
				mModules.append(item);
			}
			currentModuleItems.removeAll(item);

			//add member neurons to module
			QRectF moduleBounds = item->getBoundingBox();
			QList<Neuron*> members = module->getNeurons();

			for(QListIterator<Neuron*> j(members); j.hasNext();) {
				Neuron *n = j.next();

				if(n == 0) {
					continue;
				}

				NeuronItem *neuronItem = getNeuronItem(n);
				if(neuronItem == 0) {
					continue;
				}
				QRectF neuronBounds = neuronItem->getBoundingBox();

				QPointF pos = neuronItem->getLocalPosition() - item->getGlobalPosition();
				pos.setX(Math::min(Math::max(pos.x(), (neuronBounds.width() / 2.0)),
						 moduleBounds.width() - (neuronBounds.width() / 2.0)));
				pos.setY(Math::min(Math::max(pos.y(), (neuronBounds.height() / 2.0)),
						 moduleBounds.height() - (neuronBounds.height() / 2.0)));
				

				neuronItem->setLocalPosition(pos);
				item->addMemberItem(neuronItem);
			}

			//add member synapses to module
			for(QListIterator<Synapse*> j(synapses); j.hasNext();) {		
				Synapse *s = j.next();
				if(s == 0) {
					continue;
				}
				Neuron *n = s->getSource();
				if(n == 0) {
					continue;
				}

				if(!members.contains(n)) {
					continue;
				}

				SynapseItem *synapseItem = getSynapseItem(s);
				if(synapseItem == 0) {
					continue;
				}
				synapseItem->setLocalPosition(
								synapseItem->getLocalPosition() - item->getGlobalPosition());

				item->addMemberItem(synapseItem);
			}	
		}

		//remove obsolete module items
		for(QListIterator<ModuleItem*> i(currentModuleItems); i.hasNext();) {
			ModuleItem *item = i.next();
			item->setHidden(true);
			item->setActive(false);
			item->setToHiddenLayer(false);
			mModuleLookup.remove(item->getNeuroModule());
			item->setModule(0);
			item->setParent(0);
			mModuleItemPool.append(item);
		}

		//check if submodules have to be added.
		for(int i = 0; i < modules.size(); ++i) {
			NeuroModule *module = modules.at(i);

			if(module == 0) {
				continue;
			}

			ModuleItem *moduleItem = getModuleItem(module);

			if(moduleItem == 0) {
				continue;
			}

			QList<NeuroModule*> subModules = module->getSubModules();

			for(QListIterator<NeuroModule*> j(subModules); j.hasNext();) {
				NeuroModule *subModule = j.next();

				if(subModule == 0) {
					continue;
				}

				ModuleItem *subModuleItem = getModuleItem(subModule);

				if(subModuleItem == 0) {
					continue;
				}

				subModuleItem->setLocalPosition(
						subModuleItem->getLocalPosition() - moduleItem->getGlobalPosition());
				moduleItem->addMemberItem(subModuleItem);
			}
		}

		//setup groups
		QList<NeuronGroup*> groups = mNetwork->getNeuronGroups();

		for(int i = 0; i < groups.size(); ++i) {

			NeuronGroup *group = groups.at(i);
			if(dynamic_cast<NeuroModule*>(group) != 0) {
				//ignore neuro modules
				continue;
			}

			GroupItem *item = getGroupItem(group);

			if(item == 0) {
				if(!mGroupItemPool.empty()) {
					item = mGroupItemPool.takeFirst();
				}
				if(item == 0) {
					item = mGroupItemPrototype->createCopy();
				}
			}
			if(group->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				NetworkEditorUtil::setPaintItemLocation(item, 
						group->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
			else {
				setLocationByGrid(item);
			}

			item->setNeuronGroup(group);
			item->setActive(true);
			item->setHidden(false);

			if(!mGroups.contains(item)) {
				mGroupLookup.insert(group, item);
				mGroups.append(item);
			}

			currentGroupItems.removeAll(item);
		}

		//remove obsolete group items
		for(QListIterator<GroupItem*> i(currentGroupItems); i.hasNext();) {
			GroupItem *item = i.next();
			item->setHidden(true);
			item->setActive(false);
			item->setToHiddenLayer(false);
			mGroupLookup.remove(item->getNeuronGroup());
			item->setNeuronGroup(0);
			item->setParent(0);
			mGroupItemPool.append(item);
		}

		//update location property
		for(QListIterator<NeuronItem*> i(mNeurons); i.hasNext();) {
			NeuronItem *item = i.next();
			if(item == 0) {
				continue;
			}
			Neuron *neuron = item->getNeuron();
			if(neuron == 0 || neuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				continue;
			}
			neuron->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
											QString::number(item->getGlobalPosition().x()) + "," 
											+ QString::number(item->getGlobalPosition().y()) + ",0");
		}
		for(QListIterator<SynapseItem*> i(mSynapses); i.hasNext();) {
			SynapseItem *item = i.next();
			if(item == 0) {
				continue;
			}
			Synapse *synapse = item->getSynapse();
			if(synapse == 0 || synapse->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				continue;
			}
			synapse->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION,
 											QString::number(item->getGlobalPosition().x()) + "," 
											+ QString::number(item->getGlobalPosition().y()) + ",0");
		}
		for(QListIterator<ModuleItem*> i(mModules); i.hasNext();) {
			ModuleItem *item = i.next();
			if(item == 0) {
				continue;
			}
			NeuroModule *module = item->getNeuroModule();
			if(module == 0 || module->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				continue;
			}
			module->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION,
											QString::number(item->getGlobalPosition().x()) + "," 
											+ QString::number(item->getGlobalPosition().y()) + ",0");
		}
		for(QListIterator<GroupItem*> i(mGroups); i.hasNext();) {
			GroupItem *item = i.next();
			if(item == 0) {
				continue;
			}
			NeuronGroup *group = item->getNeuronGroup();
			if(group == 0 || group->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION)) {
				continue;
			}
			group->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION,
											QString::number(item->getGlobalPosition().x()) + "," 
											+ QString::number(item->getGlobalPosition().y()) + ",0");
		}
	}
	else {
		clearVisualization();
	}
	updateVisualizationSet(); 
	
	if(mOwner != 0) {
		mOwner->updateHiddenLayerMode();
	}

	return true;
}


void SimpleNetworkVisualizationHandler::updateVisualizationSet() {
	TRACE("SimpleNetworkVisualizationHandler::updateVisualizationSet");

	for(QListIterator<NeuronItem*> i(mNeurons); i.hasNext();) {
		NeuronItem *item = i.next();
		if(item->isActive()) {
			mOwner->addPaintItem(item);
		}
		else {
			mOwner->removePaintItem(item);
		}
	}
	for(QListIterator<NeuronItem*> i(mNeuronItemPool); i.hasNext();) {
		mOwner->removePaintItem(i.next());
	}
	for(QListIterator<SynapseItem*> i(mSynapses); i.hasNext();) {
		SynapseItem *item = i.next();
		if(item->isActive()) {
			mOwner->addPaintItem(item);
		}
		else {
			mOwner->removePaintItem(item);
		}
	}	
	for(QListIterator<SynapseItem*> i(mSynapseItemPool); i.hasNext();) {
		mOwner->removePaintItem(i.next());
	}
	for(QListIterator<ModuleItem*> i(mModules); i.hasNext();) {
		ModuleItem *item = i.next();
		if(item->isActive()) {
			mOwner->addPaintItem(item);
		}
		else {
			mOwner->removePaintItem(item);
		}
	}
	for(QListIterator<ModuleItem*> i(mModuleItemPool); i.hasNext();) {
		mOwner->removePaintItem(i.next());
	}
	for(QListIterator<GroupItem*> i(mGroups); i.hasNext();) {
		GroupItem *item = i.next();
		if(item->isActive()) {
			mOwner->addPaintItem(item);
		}
		else {
			mOwner->removePaintItem(item);
		}
	}
	for(QListIterator<GroupItem*> i(mGroupItemPool); i.hasNext();) {
		mOwner->removePaintItem(i.next());
	}
}


void SimpleNetworkVisualizationHandler::clearVisualization() {
	TRACE("SimpleNetworkVisualizationHandler::clearVisualization");

	QList<NeuronItem*> neurons = mNeurons;
	neurons << mNeuronItemPool;
	mNeurons.clear();
	mNeuronItemPool = neurons;
	for(QListIterator<NeuronItem*> i(mNeuronItemPool); i.hasNext();) {
		NeuronItem *item = i.next();
		QPointF pos = item->getGlobalPosition();
		item->setParent(0);
		item->setLocalPosition(pos);
		item->setNeuron(0);
		item->setActive(false);
		item->setHidden(true);
		item->setSelected(false);
	}
	QList<SynapseItem*> synapses = mSynapses;
	synapses << mSynapseItemPool;
	mSynapses.clear();
	mSynapseItemPool = synapses;
	for(QListIterator<SynapseItem*> i(mSynapseItemPool); i.hasNext();) {
		SynapseItem *item = i.next();
		QPointF pos = item->getGlobalPosition();
		item->setParent(0);
		item->setLocalPosition(pos);
		item->setSynapse(0);
		item->setActive(false);
		item->setHidden(true);
		item->setSelected(false);
	}
	QList<ModuleItem*> modules = mModules;
	modules << mModuleItemPool;
	mModules.clear();
	mModuleItemPool = modules;
	for(QListIterator<ModuleItem*> i(mModuleItemPool); i.hasNext();) {
		ModuleItem *item = i.next();
		QPointF pos = item->getGlobalPosition();
		item->setParent(0);
		item->setLocalPosition(pos);
		item->setModule(0);
		item->setActive(false);
		item->setHidden(true);
		item->setSelected(false);
		QList<PaintItem*> members = item->getMemberItems();
		for(QListIterator<PaintItem*> j(members); j.hasNext();) {
			PaintItem *it = j.next();
			item->removeMemberItem(it);
			it->setParent(0);
		}
	}
	QList<GroupItem*> groups = mGroups;
	groups << mGroupItemPool;
	mGroups.clear();
	mGroupItemPool = groups;
	for(QListIterator<GroupItem*> i(mGroupItemPool); i.hasNext();) {
		GroupItem *item = i.next();
		QPointF pos = item->getGlobalPosition();
		item->setParent(0);
		item->setLocalPosition(pos);
		item->setNeuronGroup(0);
		item->setActive(false);
		item->setHidden(true);
		item->setSelected(false);
	}

	mNeuronLookup.clear();
	mSynapseLookup.clear();
	mModuleLookup.clear();
	mGroupLookup.clear();

	updateVisualizationSet();


	while(mNeuronItemPool.size() > Math::max(0, mMaxSizeOfNeuronPool->get())) {
		NeuronItem *item = mNeuronItemPool.last();
		mNeuronItemPool.removeAll(item);
		delete item;
	}
	while(mSynapseItemPool.size() > Math::max(0, mMaxSizeOfSynapsePool->get())) {
		SynapseItem *item = mSynapseItemPool.last();
		mSynapseItemPool.removeAll(item);
		delete item;
	}
	while(mModuleItemPool.size() > Math::max(0, mMaxSizeOfModulePool->get())) {
		ModuleItem *item = mModuleItemPool.last();
		mModuleItemPool.removeAll(item);
		delete item;
	}
	while(mGroupItemPool.size() > Math::max(0, mMaxSizeOfGroupPool->get())) {
		GroupItem *item = mGroupItemPool.last();
		mGroupItemPool.removeAll(item);
		delete item;
	}
	
	if(mGridPosition.y() > 500) {
		mGridPosition = QPointF(20.0, 250.0);
	}

}


void SimpleNetworkVisualizationHandler::conserveSelection() {

	if(mOwner == 0 || !mSelectedElements.empty()) {
		return;
	}
	mSelectedElements.clear();

	QList<PaintItem*> selectedElements = mOwner->getSelectedItems();
	for(QListIterator<PaintItem*> i(selectedElements); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			mSelectedElements.append(neuronItem->getNeuron());
			continue;
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			mSelectedElements.append(synapseItem->getSynapse());
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			mSelectedElements.append(moduleItem->getNeuroModule());
			continue;
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			mSelectedElements.append(groupItem->getNeuronGroup());
			continue;
		}
	}
}


void SimpleNetworkVisualizationHandler::restoreSelection() {

	QList<PaintItem*> selectedItems;

	for(QListIterator<NeuralNetworkElement*> i(mSelectedElements); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		Neuron *neuron = dynamic_cast<Neuron*>(elem);
		if(neuron != 0) {
			NeuronItem *item = getNeuronItem(neuron);
			if(item != 0) {
				selectedItems.append(item);
			}
		}
		Synapse *synapse = dynamic_cast<Synapse*>(elem);
		if(synapse != 0) {
			SynapseItem *item = getSynapseItem(synapse);
			if(item != 0) {
				selectedItems.append(item);
			}
		}
		NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
		if(module != 0) {
			ModuleItem *item = getModuleItem(module);
			if(item != 0) {
				selectedItems.append(item);
			}
		}
		NeuronGroup *group = dynamic_cast<NeuronGroup*>(elem);
		if(group != 0) {
			GroupItem *item = getGroupItem(group);
			if(item != 0) {
				selectedItems.append(item);
			}
		}
	}
	if(mOwner != 0) {
		mOwner->setSelectedItems(selectedItems);
	}
	mSelectedElements.clear();
}

void SimpleNetworkVisualizationHandler::setSelectionHint(QList<NeuralNetworkElement*> elems) {
	mSelectedElements = elems;
}


bool SimpleNetworkVisualizationHandler::addNeuronItem(NeuronItem *neuronItem) {
	TRACE("SimpleNetworkVisualizationHandler::addNeuronItem");

	if(neuronItem == 0 || neuronItem->getNeuron() == 0 
		|| mNeuronLookup.keys().contains(neuronItem->getNeuron()))
	{
		return false;
	}

	mNeuronLookup.insert(neuronItem->getNeuron(), neuronItem);
	mNeurons.append(neuronItem);

	updateVisualizationSet();

	return true;
}


bool SimpleNetworkVisualizationHandler::addSynapseItem(SynapseItem *synapseItem) {
	TRACE("SimpleNetworkVisualizationHandler::addSynapseItem");

	if(synapseItem == 0 || synapseItem->getSynapse() == 0 
		|| mSynapseLookup.keys().contains(synapseItem->getSynapse()))
	{
		return false;
	}

	mSynapseLookup.insert(synapseItem->getSynapse(), synapseItem);
	mSynapses.append(synapseItem);

	updateVisualizationSet();

	return true;
}


bool SimpleNetworkVisualizationHandler::addModuleItem(ModuleItem *moduleItem) {
	TRACE("SimpleNetworkVisualizationHandler::addModuleItem");

	if(moduleItem == 0 || moduleItem->getNeuroModule() == 0 
		|| mModuleLookup.keys().contains(moduleItem->getNeuroModule()))
	{
		return false;
	}

	mModuleLookup.insert(moduleItem->getNeuroModule(), moduleItem);
	mModules.append(moduleItem);

	updateVisualizationSet();

	return true;
}

bool SimpleNetworkVisualizationHandler::addGroupItem(GroupItem *groupItem) {
	TRACE("SimpleNetworkVisualizationHandler::addGroupItem");

	if(groupItem == 0 || groupItem->getNeuronGroup() == 0 
		|| mGroupLookup.keys().contains(groupItem->getNeuronGroup()))
	{
		return false;
	}

	mGroupLookup.insert(groupItem->getNeuronGroup(), groupItem);
	mGroups.append(groupItem);

	updateVisualizationSet();

	return true;
}


bool SimpleNetworkVisualizationHandler::removeNeuronItem(NeuronItem *neuronItem) {
	TRACE("SimpleNetworkVisualizationHandler::removeNeuronItem");

	if(neuronItem == 0 || !mNeurons.contains(neuronItem))
	{
		return false;
	}

	if(neuronItem->getNeuron() != 0) {
		Neuron *neuron = mNeuronLookup.key(neuronItem);
		if(neuron != 0) {
			QHash<Neuron*, NeuronItem*>::iterator i = mNeuronLookup.find(neuron);
			if(i != mNeuronLookup.end()) {
				mNeuronLookup.erase(i);
			}
		}
	}
	mOwner->removePaintItem(neuronItem);
	mNeurons.removeAll(neuronItem);
	
	updateVisualizationSet();

	return true;
}



bool SimpleNetworkVisualizationHandler::removeSynapseItem(SynapseItem *synapseItem) {
	TRACE("SimpleNetworkVisualizationHandler::removeSynapseItem");

	if(synapseItem == 0 || !mSynapses.contains(synapseItem))
	{
		return false;
	}

	if(synapseItem->getSynapse() != 0) {
		Synapse *synapse = mSynapseLookup.key(synapseItem);
		if(synapse != 0) {
			QHash<Synapse*, SynapseItem*>::iterator i = mSynapseLookup.find(synapse);
			if(i != mSynapseLookup.end()) {
				mSynapseLookup.erase(i);
			}
		}
	}
	mOwner->removePaintItem(synapseItem);
	mSynapses.removeAll(synapseItem);

	synapseItem->setHidden(true);
	synapseItem->setActive(false);
	
	updateVisualizationSet();

	return true;
}


bool SimpleNetworkVisualizationHandler::removeModuleItem(ModuleItem *moduleItem) {
	TRACE("SimpleNetworkVisualizationHandler::removeModuleItem");

	if(moduleItem == 0 || !mModules.contains(moduleItem))
	{
		return false;
	}

	if(moduleItem->getNeuroModule() != 0) {	
		NeuroModule *module = mModuleLookup.key(moduleItem);
		if(module != 0) {
			QHash<NeuroModule*, ModuleItem*>::iterator i = mModuleLookup.find(module);
			if(i != mModuleLookup.end()) {
				mModuleLookup.erase(i);
			}
		}
	}
	mOwner->removePaintItem(moduleItem);
	mModules.removeAll(moduleItem);
	
	updateVisualizationSet();

	return true;
}


bool SimpleNetworkVisualizationHandler::removeGroupItem(GroupItem *groupItem) {
	TRACE("SimpleNetworkVisualizationHandler::removeGroupItem");

	if(groupItem == 0 || !mGroups.contains(groupItem))
	{
		return false;
	}

	if(groupItem->getNeuronGroup() != 0) {
		NeuronGroup *group = mGroupLookup.key(groupItem);
		if(group != 0) {
			QHash<NeuronGroup*, GroupItem*>::iterator i = mGroupLookup.find(group);
			if(i != mGroupLookup.end()) {
				mGroupLookup.erase(i);
			}
		}
	}
	mOwner->removePaintItem(groupItem);
	mGroups.removeAll(groupItem);
	
	updateVisualizationSet();

	return true;
}


NeuronItem* SimpleNetworkVisualizationHandler::getNeuronItem(Neuron *neuron) {
	if(neuron == 0) {
		return 0;
	}
	return mNeuronLookup.value(neuron);
}


QList<NeuronItem*> SimpleNetworkVisualizationHandler::getNeuronItems() const {
	return mNeurons;
}


SynapseItem* SimpleNetworkVisualizationHandler::getSynapseItem(Synapse *synapse) {
	if(synapse == 0) {
		return 0;
	}
	return mSynapseLookup.value(synapse);
}


QList<SynapseItem*> SimpleNetworkVisualizationHandler::getSynapseItems() const {
	return mSynapses;
}

ModuleItem* SimpleNetworkVisualizationHandler::getModuleItem(NeuroModule *module) {
	if(module == 0) {
		return 0;
	}
	return mModuleLookup.value(module);
}


QList<ModuleItem*> SimpleNetworkVisualizationHandler::getModuleItems() const {
	return mModules;
}


GroupItem* SimpleNetworkVisualizationHandler::getGroupItem(NeuronGroup *group) {
	if(group == 0) {
		return 0;
	}
	return mGroupLookup.value(group);
}


QList<GroupItem*> SimpleNetworkVisualizationHandler::getGroupItems() const {
	return mGroups;
}

QList<PaintItem*> SimpleNetworkVisualizationHandler::getPaintItems() const {
	QList<PaintItem*> items;
	for(QListIterator<NeuronItem*> i(mNeurons); i.hasNext();) {
		items.append(i.next());
	}
	for(QListIterator<SynapseItem*> i(mSynapses); i.hasNext();) {
		items.append(i.next());
	}
	for(QListIterator<ModuleItem*> i(mModules); i.hasNext();) {
		items.append(i.next());
	}
	for(QListIterator<GroupItem*> i(mGroups); i.hasNext();) {
		items.append(i.next());
	}
	return items;
}

NeuronItem* SimpleNetworkVisualizationHandler::getNeuronItemPrototype() const {
	return mNeuronItemPrototype;
}


SynapseItem* SimpleNetworkVisualizationHandler::getSynapseItemPrototype() const {
	return mSynapseItemPrototype;
}


ModuleItem* SimpleNetworkVisualizationHandler::getModuleItemPrototype() const {
	return mModuleItemPrototype;
}

GroupItem* SimpleNetworkVisualizationHandler::getGroupItemPrototype() const {
	return mGroupItemPrototype;
}


void SimpleNetworkVisualizationHandler::setLocationByGrid(PaintItem *item) {
	item->setLocalPosition(mGridPosition);
	
	mGridPosition += QPointF(GRID_X_INCREMENT, 15.0);
	if(mGridPosition.x() > GRID_NEWLINE_POSITION) {
		mGridPosition = QPointF(20.0, mGridPosition.y() + GRID_Y_NEWLINE_INCREMENT);
	}
}


void SimpleNetworkVisualizationHandler::markElementsAsSelected(
						QList<NeuralNetworkElement*> elements) 
{
	//get selected items
	QList<PaintItem*> selectedItems;

	for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();

		Neuron *neuron = dynamic_cast<Neuron*>(elem);
		if(neuron != 0) {
			PaintItem *item = getNeuronItem(neuron);
			if(item != 0) {
				selectedItems.append(item);
			}
			continue;
		}

		Synapse *synapse = dynamic_cast<Synapse*>(elem);
		if(synapse != 0) {
			PaintItem *item = getSynapseItem(synapse);
			if(item != 0) {
				selectedItems.append(item);
			}
			continue;
		}

		NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
		if(module != 0) {
			PaintItem *item = getModuleItem(module);
			if(item != 0) {
				selectedItems.append(item);
			}
			continue;
		}

		NeuronGroup * group = dynamic_cast<NeuronGroup*>(elem);	
		if(group != 0) {
			PaintItem *item = getGroupItem(group);
			if(item != 0) {
				selectedItems.append(item);
			}
			continue;
		}
	}

	mOwner->setSelectedItems(selectedItems);
}

}




