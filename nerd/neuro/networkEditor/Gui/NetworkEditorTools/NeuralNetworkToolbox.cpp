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
	mConnectNeuronsWithSynapsesTool = new ConnectNeuronsWithSynapsesTool(owner, this, 0);

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

void NeuralNetworkToolbox::alignNetworkElementsCenterHoriztonally() {
	alignNetworkElements(ALIGN_TYPE_HORIZONTAL, ALIGN_MODE_CENTER);
}

 
void NeuralNetworkToolbox::alignNetworkElementsCenterVertically() {
	alignNetworkElements(ALIGN_TYPE_VERTICAL, ALIGN_MODE_CENTER);
}

void NeuralNetworkToolbox::alignNetworkElementsDistanceHorizontally() {
	alignNetworkElements(ALIGN_TYPE_HORIZONTAL, ALIGN_MODE_DISTANCE);
}


void NeuralNetworkToolbox::alignNetworkElementsDistanceVertically() {
	alignNetworkElements(ALIGN_TYPE_VERTICAL, ALIGN_MODE_DISTANCE);
}

void NeuralNetworkToolbox::alignNetworkElementsSpacingHorizontally() {
	alignNetworkElements(ALIGN_TYPE_HORIZONTAL, ALIGN_MODE_SPACING);
}


void NeuralNetworkToolbox::alignNetworkElementsSpacingVertically() {
	alignNetworkElements(ALIGN_TYPE_VERTICAL, ALIGN_MODE_SPACING);
}


void NeuralNetworkToolbox::alignModuleSizeHorizontally() {
	alignModuleSize(ALIGN_TYPE_HORIZONTAL);
}

void NeuralNetworkToolbox::alignModuleSizeVertically() {
	alignModuleSize(ALIGN_TYPE_VERTICAL);
}

void NeuralNetworkToolbox::alignNetworkElementsLeft() {
	alignNetworkElements(ALIGN_TYPE_VERTICAL, ALIGN_MODE_LEFT);
}


void NeuralNetworkToolbox::alignNetworkElementsRight() {
	alignNetworkElements(ALIGN_TYPE_VERTICAL, ALIGN_MODE_RIGHT);
}


void NeuralNetworkToolbox::alignNetworkElementsTop() {
	alignNetworkElements(ALIGN_TYPE_HORIZONTAL, ALIGN_MODE_TOP);
}


void NeuralNetworkToolbox::alignNetworkElementsBottom() {
	alignNetworkElements(ALIGN_TYPE_HORIZONTAL, ALIGN_MODE_BOTTOM);
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

void NeuralNetworkToolbox::fullyConnectGroupsBidirectional() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_BIDIRECTIONAL);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Groups Bidirectionally. ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectGroupsUniDirectional() {
	mConnectNeuronsWithSynapsesTool->setModus(0);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Groups Unidirectionally. ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectGroupsBidirectionalIgnoreInterfaces() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_BIDIRECTIONAL 
						| ConnectNeuronsWithSynapsesTool::MODUS_IGNORE_INTERFACES);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Groups Bidirectionally (Ignore Interfaces). ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectGroupsUniDirectionalIgnoreInterfaces() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_IGNORE_INTERFACES);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Groups Unidirectionally (Ignore Interfaces). ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectSelectedNeurons() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_SELECTED_ELEMENTS);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Selected Neurons. ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectSelectedNeuronsIgnoreInterfaces() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_SELECTED_ELEMENTS 
						| ConnectNeuronsWithSynapsesTool::MODUS_IGNORE_INTERFACES);
	mConnectNeuronsWithSynapsesTool->setBasicName("Connect Selected Neurons (Ignore Interfaces). ");
	setTool(mConnectNeuronsWithSynapsesTool);
}

void NeuralNetworkToolbox::fullyConnectSingleGroup() {
	mConnectNeuronsWithSynapsesTool->setModus(ConnectNeuronsWithSynapsesTool::MODUS_SINGLE_GROUP);
	mConnectNeuronsWithSynapsesTool->setBasicName("Interconnect Single Group. ");
	setTool(mConnectNeuronsWithSynapsesTool);
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
	
	//add connect/create synapse operators
	
	QMenu *connectivityMenu = mNetworkMenu->addMenu("Connectivity");
	
	QAction *connectSelectedNeurons = connectivityMenu->addAction("All Selected");
	connectSelectedNeurons->setToolTip("Fully connects all selected elements.");
	connect(connectSelectedNeurons, SIGNAL(triggered()),
			this, SLOT(fullyConnectSelectedNeurons()));
	
	QAction *connectSingleGroup = connectivityMenu->addAction("Single Group");
	connectSingleGroup->setToolTip("Fully interconnects a single group.");
	connect(connectSingleGroup, SIGNAL(triggered()),
			this, SLOT(fullyConnectSingleGroup()));
	
// 	QAction *connectAllWithinGroup = connectivityMenu->addAction("All in Group");
// 	connectAllWithinGroup->setToolTip("Fully connects all neurons in a group.");
// 	connect(connectAllWithinGroup, SIGNAL(triggered()),
// 			this, SLOT(fullyConnect()));
	
	QAction *connectGroupsUnidirectional = connectivityMenu->addAction("Groups Unidir.");
	connectGroupsUnidirectional->setToolTip("Connects two groups unidirectionally");
	connect(connectGroupsUnidirectional, SIGNAL(triggered()),
			this, SLOT(fullyConnectGroupsUniDirectional()));
	
	QAction *connectGroupsBidirectional = connectivityMenu->addAction("Groups Bidir.");
	connectGroupsBidirectional->setToolTip("Connects two groups bidirectionally");
	connect(connectGroupsBidirectional, SIGNAL(triggered()),
			this, SLOT(fullyConnectGroupsBidirectional()));
	
	
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
	
	QMenu *alignmentMenu = mNetworkMenu->addMenu("Alignment");
	
	QAction *alignAllSynapses = alignmentMenu->addAction("&Align Synapses");
	alignAllSynapses->setShortcut(tr("ctrl+a"));
	connect(alignAllSynapses, SIGNAL(triggered()),
			this, SLOT(alignAllSynapses()));
	
	QAction *alignSelectedSynapses = alignmentMenu->addAction("&Align Selected Synapses");
	alignSelectedSynapses->setShortcut(tr("ctrl+shift+a"));
	connect(alignSelectedSynapses, SIGNAL(triggered()),
			this, SLOT(alignSelectedSynapses()));
	
	alignmentMenu->addSeparator();

	QAction *alignSelectedTop = alignmentMenu->addAction("Align Top");
	//alignSelectedTop->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedTop, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsTop()));
	
	QAction *alignSelectedNeuronsHorizontally = alignmentMenu->addAction("Align Hori. Center");
	alignSelectedNeuronsHorizontally->setShortcut(tr("ctrl+shift+h"));
	connect(alignSelectedNeuronsHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsCenterHoriztonally()));
	
	QAction *alignSelectedBottom = alignmentMenu->addAction("Align Bottom");
	//alignSelectedBottom->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedBottom, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsBottom()));
	
	QAction *alignSelectedNeuronDistancesHorizontally = 
	alignmentMenu->addAction("Align Hori. Distance");
	alignSelectedNeuronDistancesHorizontally->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedNeuronDistancesHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsDistanceHorizontally()));
	
	QAction *alignSelectedNeuronSpacingHorizontally = 
	alignmentMenu->addAction("Align Hori. Spacing");
	//alignSelectedNeuronSpacingHorizontally->setShortcut(tr("ctrl+shift+alt+h"));
	connect(alignSelectedNeuronSpacingHorizontally, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsSpacingHorizontally()));

	
	alignmentMenu->addSeparator();
	
	QAction *alignSelectedLeft = alignmentMenu->addAction("Align Left");
	//alignSelectedLeft->setShortcut(tr("ctrl+shift+v"));
	connect(alignSelectedLeft, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsLeft()));
	
	QAction *alignSelectedNeuronsVertically = alignmentMenu->addAction("Align Vert. Center");
	alignSelectedNeuronsVertically->setShortcut(tr("ctrl+shift+v"));
	connect(alignSelectedNeuronsVertically, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsCenterVertically()));
	
	QAction *alignSelectedRight = alignmentMenu->addAction("Align Right");
	//alignSelectedRight->setShortcut(tr("ctrl+shift+v"));
	connect(alignSelectedRight, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsRight()));


	QAction *alignSelectedNeuronDistancesVertically = 
			alignmentMenu->addAction("Align Vert. Distance");
	alignSelectedNeuronDistancesVertically->setShortcut(tr("ctrl+shift+alt+v"));
	connect(alignSelectedNeuronDistancesVertically, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsDistanceVertically()));
	
	QAction *alignSelectedNeuronSpacingVertically = 
	alignmentMenu->addAction("Align Vert. Spacing");
	//alignSelectedNeuronSpacingVertically->setShortcut(tr("ctrl+shift+alt+v"));
	connect(alignSelectedNeuronSpacingVertically, SIGNAL(triggered()),
			this, SLOT(alignNetworkElementsSpacingVertically()));
	
	
	alignmentMenu->addSeparator();
	
	QAction *alignSelectedModuleSizesHorizontally = 
				alignmentMenu->addAction("Align Module Widths");
	connect(alignSelectedModuleSizesHorizontally, SIGNAL(triggered()),
			this, SLOT(alignModuleSizeHorizontally()));

	QAction *alignSelectedModuleSizesVertically = 
				alignmentMenu->addAction("Align Module Heights");
	connect(alignSelectedModuleSizesVertically, SIGNAL(triggered()),
			this, SLOT(alignModuleSizeVertically()));

// 	QAction *alignAccordingToLocations = mNetworkMenu->addAction("Align to Location Properties");
// 	alignAccordingToLocations->setShortcut(tr("ctrl+shift+alt+a"));
// 	connect(alignAccordingToLocations, SIGNAL(triggered()),
// 			this, SLOT(alignNetworkElementsAccordingToLocationProperties()));
// 
	QAction *setAllLocationProperties = alignmentMenu->addAction("Sync Location Properties");
	connect(setAllLocationProperties, SIGNAL(triggered()),
			this, SLOT(setAllLocationProperties()));

	mNetworkMenu->addSeparator();

	QAction *replaceModuleAction = mNetworkMenu->addAction("Replace Modules");
	replaceModuleAction->setShortcut(tr("x"));
	connect(replaceModuleAction, SIGNAL(triggered()),
			this, SLOT(useReplaceModuleTool()));

	mNetworkMenu->addSeparator();
	
	QAction *resetNetworkAction = mNetworkMenu->addAction("&Reset Network");
	resetNetworkAction->setShortcut(tr("ctrl+alt+r"));
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


void NeuralNetworkToolbox::alignNetworkElements(int type, int mode) {
	if(mOwner == 0) {
		return;
	}
	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	QList<NeuralNetworkElement*> selectedNeurons;
	QList<QRectF> boundingBoxes;

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			if(neuron != 0) {
				selectedNeurons.append(neuron);
				boundingBoxes.append(item->getBoundingBox());
			}
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			if(module != 0) {
				selectedNeurons.append(module);
				boundingBoxes.append(item->getBoundingBox());
			}
			continue;
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			NeuronGroup *group = groupItem->getNeuronGroup();
			if(group != 0) {
				selectedNeurons.append(group);
				boundingBoxes.append(item->getBoundingBox());
			}
			continue;
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			Synapse *synapse = synapseItem->getSynapse();
			if(synapse != 0) {
				selectedNeurons.append(synapse);
				boundingBoxes.append(item->getBoundingBox());
			}
			continue;
		}
	}

	if(!selectedNeurons.empty()) {
		int c_type = (type == ALIGN_TYPE_HORIZONTAL) ? AlignNeuronsCommand::TYPE_HORIZONTAL : AlignNeuronsCommand::TYPE_VERTICAL;
		int c_mode = 0;
		switch(mode) {
			case ALIGN_MODE_TOP:
				c_mode = AlignNeuronsCommand::MODE_TOP;
				break;
			case ALIGN_MODE_BOTTOM:
				c_mode = AlignNeuronsCommand::MODE_BOTTOM;
				break;
			case ALIGN_MODE_LEFT:
				c_mode = AlignNeuronsCommand::MODE_LEFT;
				break;
			case ALIGN_MODE_RIGHT:
				c_mode = AlignNeuronsCommand::MODE_RIGHT;
				break;
			case ALIGN_MODE_CENTER:
				c_mode = AlignNeuronsCommand::MODE_CENTER;
				break;
			case ALIGN_MODE_DISTANCE:
				c_mode = AlignNeuronsCommand::MODE_DISTANCE;
				break;
			case ALIGN_MODE_SPACING:
				c_mode = AlignNeuronsCommand::MODE_SPACING;
				break;
		};
		Command *command = new AlignNeuronsCommand(c_type, c_mode, visu, selectedNeurons, boundingBoxes);
		visu->getCommandExecutor()->executeCommand(command);
	}
}

void NeuralNetworkToolbox::alignModuleSize(int type) {
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
		int mode = (type == ALIGN_TYPE_HORIZONTAL) ? AlignModuleSizeCommand::HORIZONTAL : AlignModuleSizeCommand::VERTICAL;
		Command *command = new AlignModuleSizeCommand(mode, visu, selectedModules);
		visu->getCommandExecutor()->executeCommand(command);
	}
}


}




