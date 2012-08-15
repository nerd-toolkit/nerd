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


#include "NeuralNetworkToolbox.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditorTools/InsertNeuronNetworkTool.h"
#include "Gui/NetworkEditorTools/RunConstraintsAction.h"
#include "Gui/NetworkEditorCommands/AlignSynapseCommand.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Network/Synapse.h"
#include <Network/Neuro.h>
#include "Gui/NetworkEditorCommands/SetLocationPropertiesCommand.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Util/NetworkEditorUtil.h"
#include "Util/Util.h"
#include <Event/TriggerEventTask.h>
#include "NeuralNetworkConstants.h"
#include "Gui/NetworkEditorCommands/AlignNeuronsCommand.h"
#include "Gui/NetworkEditorCommands/AlignModuleSizeCommand.h"
#include <qvarlengtharray.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkToolbox.
 */
NeuralNetworkToolbox::NeuralNetworkToolbox(NeuralNetworkEditor *owner)
	: QObject(owner), mOwner(owner), mCurrentTool(0), mModificationsEnabled(0)
{

	if(mOwner != 0) {
		connect(mOwner, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(editorTabChanged(int)));
		connect(mOwner, SIGNAL(enableNetworkModifications(bool)),
				this, SLOT(enableModifications(bool)));
	}

	//create tools
	mInsertNeuronTool = new InsertNeuronNetworkTool(this);
	mInsertModuleTool = new InsertModuleNetworkTool(this);
	mInsertSynapseTool = new InsertSynapseNetworkTool(this);
	mRemoveObjectsTool = new RemoveObjectsNetworkTool(this);
	mRemoveSelectedObjectsTool = new RemoveSelectedObjectsNetworkTool(this);
	mGrabIdTool = new GrabNetworkElementIdTool(this);
	mShowElementPairTool = new ShowElementPairTool(this);
	mCopyPasteTool = new CopyPasteNetworkTool(this);
	mReconnectSynapseTool = new ReconnectSynapseNetworkTool(this);
	mReplaceModuleTool = new ReplaceModuleTool(this);
	mGuessGroupPairsTool = new GuessModulePairs(owner, this);
	mGuessGroupPairsByPositionTool = new GuessModulePairsByPosition(owner, this);

	addNetworkMenu();
}


/**
 * Destructor.
 */
NeuralNetworkToolbox::~NeuralNetworkToolbox() {
}


void NeuralNetworkToolbox::editorTabChanged(int) {
	setTool(0);
}

void NeuralNetworkToolbox::toolIsDone() {
	setTool(0);
}

void NeuralNetworkToolbox::useInsertNeuronTool() {
	setTool(mInsertNeuronTool);
}

void NeuralNetworkToolbox::useInsertModuleTool() {
	setTool(mInsertModuleTool);
}

void NeuralNetworkToolbox::useInsertSynapseTool() {
	setTool(mInsertSynapseTool);
}

void NeuralNetworkToolbox::useRemoveObjectsTool() {
	setTool(mRemoveObjectsTool);
}

void NeuralNetworkToolbox::useRemoveSelectedObjectsTool() {
	setTool(mRemoveSelectedObjectsTool);
}

void NeuralNetworkToolbox::useGrabIdTool() {
	setTool(mGrabIdTool);
}


void NeuralNetworkToolbox::useCopyPasteTool() {
	setTool(mCopyPasteTool);
}

void NeuralNetworkToolbox::resetCurrentNetwork() {
	if(mOwner == 0) {
		return;
	}
	//trigger a network reset in the main execution thread!
	TriggerEventTask::scheduleEvent(Neuro::getNeuralNetworkManager()->getResetNetworksEvent());
}

void NeuralNetworkToolbox::enableModifications(bool enable) {
	if(!enable) {
		setTool(0);
	}
	mModificationsEnabled = enable;
	mNetworkMenu->setEnabled(enable);
}


void NeuralNetworkToolbox::alignAllSynapses() {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	ModularNeuralNetwork *net = visu->getNeuralNetwork();
	if(net == 0) {
		return;
	}

	QList<Synapse*> allSynapses = net->getSynapses();
	QList<Synapse*> synapses;
	for(QListIterator<Synapse*> i(allSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		if(!synapse->hasProperty(NeuralNetworkConstants::TAG_SYNAPSE_FIXED_POSITION)) {
			synapses.append(synapse);
		}
	}
	if(!synapses.empty()) {
		Command *command = new AlignSynapseCommand(visu, synapses);
		visu->getCommandExecutor()->executeCommand(command);
	}
}


void NeuralNetworkToolbox::alignSelectedSynapses() {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Synapse*> selectedSynapses;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(i.next());
		if(synapseItem != 0) {
			Synapse *synapse = synapseItem->getSynapse();
			if(synapse != 0) {
				selectedSynapses.append(synapse);
			}
		}
	}

	if(!selectedSynapses.empty()) {
		Command *command = new AlignSynapseCommand(visu, selectedSynapses);
		visu->getCommandExecutor()->executeCommand(command);
	}
}

void NeuralNetworkToolbox::selectSynapsesOfMarkedNeurons() {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0 && visu->getVisualizationHander() != 0) {
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<Synapse*> selectedSynapses;
	
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(i.next());
		if(neuronItem != 0 && neuronItem->getNeuron() != 0) {
			Util::addWithoutDuplicates(selectedSynapses, neuronItem->getNeuron()->getSynapses());
			Util::addWithoutDuplicates(selectedSynapses, neuronItem->getNeuron()->getOutgoingSynapses());
		}
	}
	QList<PaintItem*> newSelectedSynapseItems;
	for(QListIterator<Synapse*> i(selectedSynapses); i.hasNext();) {
		Synapse *synapse = i.next();
		PaintItem *synapseItem = handler->getSynapseItem(synapse);
		if(synapseItem != 0) {
			newSelectedSynapseItems.append(synapseItem);
		}
	}
	visu->setSelectedItems(newSelectedSynapseItems);
}

void NeuralNetworkToolbox::setAllLocationProperties() {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	QList<PaintItem*> allItems = visu->getPaintItems();

	QList<NeuralNetworkElement*> elements;
	for(QListIterator<PaintItem*> i(allItems); i.hasNext();) {
		PaintItem *item = i.next();
		if(dynamic_cast<NeuronItem*>(item) != 0) {
			Neuron *neuron = dynamic_cast<NeuronItem*>(item)->getNeuron();
			if(neuron != 0) {
				elements.append(neuron);
			}
		}
		else if(dynamic_cast<SynapseItem*>(item) != 0) {
			Synapse *synapse = dynamic_cast<SynapseItem*>(item)->getSynapse();
			if(synapse != 0) {
				elements.append(synapse);
			}
		}
		else if(dynamic_cast<ModuleItem*>(item) != 0) {
			NeuroModule *module = dynamic_cast<ModuleItem*>(item)->getNeuroModule();
			if(module != 0) {
				elements.append(module);
			}
		}
		else if(dynamic_cast<GroupItem*>(item) != 0) {
			NeuronGroup *group = dynamic_cast<GroupItem*>(item)->getNeuronGroup();
			if(group != 0) {
				elements.append(group);
			}
		}
	}
	
	if(!allItems.empty()) {
		Command *command = new SetLocationPropertiesCommand(visu, elements);
		visu->getCommandExecutor()->executeCommand(command);
	}
}


void NeuralNetworkToolbox::alignNetworkElementsAccordingToLocationProperties() {

	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	visu->sortItems();
	QList<PaintItem*> allItems = visu->getPaintItems();

	for(QListIterator<PaintItem*> i(allItems); i.hasNext();) {
		PaintItem *item = i.next();
		if(dynamic_cast<NeuronItem*>(item) != 0) {
			Neuron *neuron = dynamic_cast<NeuronItem*>(item)->getNeuron();
			if(neuron != 0) {
				NetworkEditorUtil::setPaintItemLocation(item, 
							neuron->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
		}
		else if(dynamic_cast<SynapseItem*>(item) != 0) {
			Synapse *synapse = dynamic_cast<SynapseItem*>(item)->getSynapse();
			if(synapse != 0) {
				NetworkEditorUtil::setPaintItemLocation(item, 
							synapse->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
		}
		else if(dynamic_cast<ModuleItem*>(item) != 0) {
			ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
			NeuroModule *module = moduleItem->getNeuroModule();
			if(module != 0) {
				NetworkEditorUtil::setPaintItemLocation(item, 
							module->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
				NetworkEditorUtil::setModuleItemSize(moduleItem, 
							module->getProperty(NeuralNetworkConstants::TAG_MODULE_SIZE));
			}
		}
		else if(dynamic_cast<GroupItem*>(item) != 0) {
			NeuronGroup *group = dynamic_cast<GroupItem*>(item)->getNeuronGroup();
			if(group != 0) {
				NetworkEditorUtil::setPaintItemLocation(item, 
							group->getProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION));
			}
		}
	}

}

void NeuralNetworkToolbox::alignNeuronsHoriztonally() {
	alignNeurons(true, false);
}

 
void NeuralNetworkToolbox::alignNeuronsVertically() {
	alignNeurons(false, false);
}

void NeuralNetworkToolbox::alignNeuronDistanceHorizontally() {
	alignNeurons(true, true);
}


void NeuralNetworkToolbox::alignNeuronDistanceVertically() {
	alignNeurons(false, true);
}

void NeuralNetworkToolbox::alignModuleSizeHorizontally() {
	alignModuleSize(true);
}

void NeuralNetworkToolbox::alignModuleSizeVertically() {
	alignModuleSize(false);
}


void NeuralNetworkToolbox::useVisualizeElementPairTool() {
	setTool(mShowElementPairTool);
}

void NeuralNetworkToolbox::useReconnectSynapseTool() {
	setTool(mReconnectSynapseTool);
}

void NeuralNetworkToolbox::useReplaceModuleTool() {
	setTool(mReplaceModuleTool);
}

void NeuralNetworkToolbox::useGuessGroupIdsTool() {
	setTool(mGuessGroupPairsTool);
}

void NeuralNetworkToolbox::useGuessGroupIdsByPositionTool() {
	setTool(mGuessGroupPairsByPositionTool);
}

void NeuralNetworkToolbox::updateViewMode() {

}


void NeuralNetworkToolbox::addNetworkMenu() {
	if(mOwner == 0) {
		return;
	}

	QMenuBar *menuBar = mOwner->menuBar();
	
	mNetworkMenu = menuBar->addMenu("Neural Network");

	QAction *insertNeuronAction = mNetworkMenu->addAction("Insert &Neuron");
	insertNeuronAction->setShortcut(tr("n"));
	connect(insertNeuronAction, SIGNAL(triggered()),
			this, SLOT(useInsertNeuronTool()));

	QAction *insertModuleAction = mNetworkMenu->addAction("Insert &Module");
	insertModuleAction->setShortcut(tr("m"));
	connect(insertModuleAction, SIGNAL(triggered()),
			this, SLOT(useInsertModuleTool()));

	QAction *insertSynapseAction = mNetworkMenu->addAction("Insert &Synapse");
	insertSynapseAction->setShortcut(tr("s"));
	connect(insertSynapseAction, SIGNAL(triggered()),
			this, SLOT(useInsertSynapseTool()));

	QAction *reconnectSynapseAction = mNetworkMenu->addAction("&Reconnect Synapse");
	reconnectSynapseAction->setShortcut(tr("r"));
	connect(reconnectSynapseAction, SIGNAL(triggered()),
			this, SLOT(useReconnectSynapseTool()));


	QAction *cloneObjectsAction = mNetworkMenu->addAction("&Clone Objects");
	cloneObjectsAction->setShortcut(tr("ctrl+c"));
	connect(cloneObjectsAction, SIGNAL(triggered()),
			this, SLOT(useCopyPasteTool()));

	mNetworkMenu->addSeparator();

// 	QAction *removeObjectsAction = mNetworkMenu->addAction("&Delete Object");
// 	removeObjectsAction->setShortcut(tr("d"));
// 	connect(removeObjectsAction, SIGNAL(triggered()),
// 			this, SLOT(useRemoveObjectsTool()));

	QAction *removeSelectedObjectsAction = mNetworkMenu->addAction("&Delete Selected Objects");
	removeSelectedObjectsAction->setShortcut(Qt::Key_Delete);
	connect(removeSelectedObjectsAction, SIGNAL(triggered()),
			this, SLOT(useRemoveSelectedObjectsTool()));


	mNetworkMenu->addSeparator();
	
	QAction *selectSynapsesOfMarkedNeurons = mNetworkMenu->addAction("Select Synapses of Selected Neurons");
	selectSynapsesOfMarkedNeurons->setShortcut(tr("ctrl+s"));
	connect(selectSynapsesOfMarkedNeurons, SIGNAL(triggered(bool)),
			this, SLOT(selectSynapsesOfMarkedNeurons()));
	
	mNetworkMenu->addSeparator();

	//add align synapse operators
	
	QAction *alignAllSynapses = mNetworkMenu->addAction("&Align Synapses");
	alignAllSynapses->setShortcut(tr("ctrl+a"));
	connect(alignAllSynapses, SIGNAL(triggered()),
			this, SLOT(alignAllSynapses()));
	
	QAction *alignSelectedSynapses = mNetworkMenu->addAction("&Align Selected Synapses");
	alignSelectedSynapses->setShortcut(tr("ctrl+shift+a"));
	connect(alignSelectedSynapses, SIGNAL(triggered()),
			this, SLOT(alignSelectedSynapses()));

	QAction *alignSelectedNeuronsHorizontally = mNetworkMenu->addAction("Align Elements Horizontally");
	alignSelectedNeuronsHorizontally->setShortcut(tr("ctrl+shift+h"));
	connect(alignSelectedNeuronsHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNeuronsHoriztonally()));

	QAction *alignSelectedNeuronsVertically = mNetworkMenu->addAction("Align Elements Vertically");
	alignSelectedNeuronsVertically->setShortcut(tr("ctrl+shift+v"));
	connect(alignSelectedNeuronsVertically, SIGNAL(triggered()),
			this, SLOT(alignNeuronsVertically()));

	QAction *alignSelectedNeuronDistancesHorizontally = 
					mNetworkMenu->addAction("Align Element Dist Horizontally");
	alignSelectedNeuronDistancesHorizontally->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedNeuronDistancesHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNeuronDistanceHorizontally()));

	QAction *alignSelectedNeuronDistancesVertically = 
					mNetworkMenu->addAction("Align Element Dist Vertically");
	alignSelectedNeuronDistancesVertically->setShortcut(tr("ctrl+shift+alt+v"));
	connect(alignSelectedNeuronDistancesVertically, SIGNAL(triggered()),
			this, SLOT(alignNeuronDistanceVertically()));
	
	QAction *alignSelectedModuleSizesHorizontally = 
					mNetworkMenu->addAction("Align Module Widths");
	connect(alignSelectedModuleSizesHorizontally, SIGNAL(triggered()),
			this, SLOT(alignModuleSizeHorizontally()));

	QAction *alignSelectedModuleSizesVertically = 
					mNetworkMenu->addAction("Align Module Heights");
	connect(alignSelectedModuleSizesVertically, SIGNAL(triggered()),
			this, SLOT(alignModuleSizeVertically()));

// 	QAction *alignAccordingToLocations = mNetworkMenu->addAction("Align to Location Properties");
// 	alignAccordingToLocations->setShortcut(tr("ctrl+shift+alt+a"));
// 	connect(alignAccordingToLocations, SIGNAL(triggered()),
// 			this, SLOT(alignNetworkElementsAccordingToLocationProperties()));
// 
	QAction *setAllLocationProperties = mNetworkMenu->addAction("Sync Location Properties");
	connect(setAllLocationProperties, SIGNAL(triggered()),
			this, SLOT(setAllLocationProperties()));

	mNetworkMenu->addSeparator();

	QAction *replaceModuleAction = mNetworkMenu->addAction("Replace Modules");
	replaceModuleAction->setShortcut(tr("x"));
	connect(replaceModuleAction, SIGNAL(triggered()),
			this, SLOT(useReplaceModuleTool()));

	mNetworkMenu->addSeparator();
	
	QAction *resetNetworkAction = mNetworkMenu->addAction("&Reset Network");
	resetNetworkAction->setShortcut(tr("ctrl+r"));
	connect(resetNetworkAction, SIGNAL(triggered()),
			this, SLOT(resetCurrentNetwork()));


	mToolsMenu = menuBar->addMenu("Tools");

	//add constraint resolvers

	QAction *runAllConstraintsAction = new RunConstraintsAction("&Resolve All Constraints",
							true, 25, mOwner);
	runAllConstraintsAction->setShortcut(tr("ctrl+shift+r"));	
	mToolsMenu->addAction(runAllConstraintsAction);

	QAction *runConstraintsStepAction = new RunConstraintsAction("&Resolve All Constraints (Step)",
							true, 1, mOwner);
	runConstraintsStepAction->setShortcut(tr("ctrl+shift+alt+r"));	
	mToolsMenu->addAction(runConstraintsStepAction);

	QAction *runSelectedConstraintsStepAction = new RunConstraintsAction(
										"&Resolve Selected Constraints (Step)",
							false, 1, mOwner);
	runSelectedConstraintsStepAction->setShortcut(tr("ctrl+shift+alt+s"));	
	mToolsMenu->addAction(runSelectedConstraintsStepAction);

	mToolsMenu->addSeparator();
	
	QAction *guessGroupIdsAction = mToolsMenu->addAction("Guess Group Ids");
	connect(guessGroupIdsAction, SIGNAL(triggered()),
			this, SLOT(useGuessGroupIdsTool()));

	QAction *guessGroupIdsByPositionAction = mToolsMenu->addAction("Guess Group Ids By Location");
	connect(guessGroupIdsByPositionAction, SIGNAL(triggered()),
			this, SLOT(useGuessGroupIdsByPositionTool()));

	mToolsMenu->addSeparator();

	QAction *grabIdAction = mToolsMenu->addAction("&Grab ID");
	grabIdAction->setShortcut(tr("ctrl+g"));
	connect(grabIdAction, SIGNAL(triggered()),
			this, SLOT(useGrabIdTool()));

	QAction *showElementPairsAction = mToolsMenu->addAction("&Show Element Pairs");
	showElementPairsAction->setShortcut(tr("ctrl+shift+p"));
	connect(showElementPairsAction, SIGNAL(triggered()),
			this, SLOT(useVisualizeElementPairTool()));


	//mViewMenu = menuBar->addMenu("View");


}

void NeuralNetworkToolbox::setTool(NetworkManipulationTool *currentTool) {
	if(mOwner == 0 || !mModificationsEnabled) {
		return;
	}
	if(mCurrentTool != 0) {
		mCurrentTool->clear();
	}
	mCurrentTool = currentTool;

	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		mCurrentTool = 0;
		return;
	}

	if(mCurrentTool != 0) {
		mCurrentTool->activate(visu);
	}

}


void NeuralNetworkToolbox::alignNeurons(bool horizontally, bool adjustDistance) {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<NeuralNetworkElement*> selectedNeurons;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			if(neuron != 0) {
				selectedNeurons.append(neuron);
			}
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			if(module != 0) {
				selectedNeurons.append(module);
			}
			continue;
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			NeuronGroup *group = groupItem->getNeuronGroup();
			if(group != 0) {
				selectedNeurons.append(group);
			}
			continue;
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			Synapse *synapse = synapseItem->getSynapse();
			if(synapse != 0) {
				selectedNeurons.append(synapse);
			}
			continue;
		}
	}

	if(!selectedNeurons.empty()) {
		int mode = horizontally ? AlignNeuronsCommand::HORIZONTAL : AlignNeuronsCommand::VERTICAL;
		Command *command = new AlignNeuronsCommand(mode, adjustDistance, visu, selectedNeurons);
		visu->getCommandExecutor()->executeCommand(command);
	}
}

void NeuralNetworkToolbox::alignModuleSize(bool horizontally) {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<NeuroModule*> selectedModules;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *item = i.next();
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			if(module != 0) {
				selectedModules.append(module);
			}
			continue;
		}
	}

	if(!selectedModules.empty()) {
		int mode = horizontally ? AlignModuleSizeCommand::HORIZONTAL : AlignModuleSizeCommand::VERTICAL;
		Command *command = new AlignModuleSizeCommand(mode, visu, selectedModules);
		visu->getCommandExecutor()->executeCommand(command);
	}
}


}




