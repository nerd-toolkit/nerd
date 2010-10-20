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
#include "Gui/NetworkEditorCommands/SetLocationPropertiesCommand.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Util/NetworkEditorUtil.h"
#include "NeuralNetworkConstants.h"
#include "Gui/NetworkEditorCommands/AlignNeuronsCommand.h"

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
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	ModularNeuralNetwork *network = visu->getNeuralNetwork();
	if(network != 0) {
		network->reset();
	}
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

	QList<Synapse*> synapses = net->getSynapses();
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

	QAction *grabIdAction = mNetworkMenu->addAction("&Grab ID");
	grabIdAction->setShortcut(tr("ctrl+g"));
	connect(grabIdAction, SIGNAL(triggered()),
			this, SLOT(useGrabIdTool()));

	QAction *showElementPairsAction = mNetworkMenu->addAction("&Show Element Pairs");
	showElementPairsAction->setShortcut(tr("ctrl+shift+p"));
	connect(showElementPairsAction, SIGNAL(triggered()),
			this, SLOT(useVisualizeElementPairTool()));

	mNetworkMenu->addSeparator();
	
	QAction *resetNetworkAction = mNetworkMenu->addAction("&Reset Network");
	resetNetworkAction->setShortcut(tr("ctrl+r"));
	connect(resetNetworkAction, SIGNAL(triggered()),
			this, SLOT(resetCurrentNetwork()));

	mNetworkMenu->addSeparator();

	//add constraint resolvers

	QAction *runAllConstraintsAction = new RunConstraintsAction("&Resolve All Constraints",
							true, 25, mOwner);
	runAllConstraintsAction->setShortcut(tr("ctrl+shift+r"));	
	mNetworkMenu->addAction(runAllConstraintsAction);

	QAction *runConstraintsStepAction = new RunConstraintsAction("&Resolve All Constraints (Step)",
							true, 1, mOwner);
	runConstraintsStepAction->setShortcut(tr("ctrl+shift+alt+r"));	
	mNetworkMenu->addAction(runConstraintsStepAction);

	QAction *runSelectedConstraintsStepAction = new RunConstraintsAction(
										"&Resolve Selected Constraints (Step)",
							false, 1, mOwner);
	runSelectedConstraintsStepAction->setShortcut(tr("ctrl+shift+alt+s"));	
	mNetworkMenu->addAction(runSelectedConstraintsStepAction);

	mNetworkMenu->addSeparator();

	//add align synapse operators
	
	QAction *alignAllSynapses = mNetworkMenu->addAction("&Align Synapses");
	alignAllSynapses->setShortcut(tr("ctrl+a"));
	connect(alignAllSynapses, SIGNAL(triggered()),
			this, SLOT(alignAllSynapses()));

	QAction *alignSelectedNeuronsHorizontally = mNetworkMenu->addAction("Align Neurons Horizontally");
	alignSelectedNeuronsHorizontally->setShortcut(tr("ctrl+shift+h"));
	connect(alignSelectedNeuronsHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNeuronsHoriztonally()));

	QAction *alignSelectedNeuronsVertically = mNetworkMenu->addAction("Align Neurons Vertically");
	alignSelectedNeuronsVertically->setShortcut(tr("ctrl+shift+v"));
	connect(alignSelectedNeuronsVertically, SIGNAL(triggered()),
			this, SLOT(alignNeuronsVertically()));

	QAction *alignSelectedNeuronDistancesHorizontally = 
					mNetworkMenu->addAction("Align Neuron Dist Horizontally");
	alignSelectedNeuronDistancesHorizontally->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedNeuronDistancesHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNeuronDistanceHorizontally()));

	QAction *alignSelectedNeuronDistancesVertically = 
					mNetworkMenu->addAction("Align Neuron Dist Vertically");
	alignSelectedNeuronDistancesVertically->setShortcut(tr("ctrl+shift+alt+v"));
	connect(alignSelectedNeuronDistancesVertically, SIGNAL(triggered()),
			this, SLOT(alignNeuronDistanceVertically()));

	QAction *alignSelectedSynapses = mNetworkMenu->addAction("&Align Selected Synapses");
	alignSelectedSynapses->setShortcut(tr("ctrl+shift+a"));
	connect(alignSelectedSynapses, SIGNAL(triggered()),
			this, SLOT(alignSelectedSynapses()));

	QAction *alignAccordingToLocations = mNetworkMenu->addAction("Align to Location Properties");
	alignAccordingToLocations->setShortcut(tr("ctrl+shift+alt+a"));
	connect(alignAccordingToLocations, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsAccordingToLocationProperties()));

	QAction *setAllLocationProperties = mNetworkMenu->addAction("Sync Location Properties");
	setAllLocationProperties->setShortcut(tr("ctrl+shift+alt+s"));
	connect(setAllLocationProperties, SIGNAL(triggered()),
			this, SLOT(setAllLocationProperties()));

	QAction *replaceModuleAction = mNetworkMenu->addAction("Replace Modules");
	replaceModuleAction->setShortcut(tr("x"));
	connect(replaceModuleAction, SIGNAL(triggered()),
			this, SLOT(useReplaceModuleTool()));

	mNetworkMenu->addSeparator();
	
	QAction *guessGroupIdsAction = mNetworkMenu->addAction("Guess Group Ids");
	connect(guessGroupIdsAction, SIGNAL(triggered()),
			this, SLOT(useGuessGroupIdsTool()));

	QAction *guessGroupIdsByPositionAction = mNetworkMenu->addAction("Guess Group Ids By Location");
	connect(guessGroupIdsByPositionAction, SIGNAL(triggered()),
			this, SLOT(useGuessGroupIdsByPositionTool()));

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
	QList<Neuron*> selectedNeurons;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(i.next());
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			if(neuron != 0) {
				selectedNeurons.append(neuron);
			}
		}
	}

	if(!selectedNeurons.empty()) {
		int mode = horizontally ? AlignNeuronsCommand::HORIZONTAL : AlignNeuronsCommand::VERTICAL;
		Command *command = new AlignNeuronsCommand(mode, adjustDistance, visu, selectedNeurons);
		visu->getCommandExecutor()->executeCommand(command);
	}
}


}




