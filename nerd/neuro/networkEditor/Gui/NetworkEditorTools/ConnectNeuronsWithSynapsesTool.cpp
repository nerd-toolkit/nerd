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



#include "ConnectNeuronsWithSynapsesTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Network/NeuralNetworkElement.h"
#include <QClipboard>
#include <QApplication>
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "Value/ValueManager.h"
#include "Gui/NetworkEditorTools/NeuralNetworkToolbox.h"
#include "Util/NetworkConnectivityUtil.h"
#include "Gui/NetworkEditorCommands/InsertSynapseCommand.h"

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructs a new ConnectNeuronsWithSynapsesTool.
	 */
	ConnectNeuronsWithSynapsesTool::ConnectNeuronsWithSynapsesTool(NeuralNetworkEditor *editor, NeuralNetworkToolbox *owner, int modus)
		: NetworkManipulationTool(owner), mModus(modus), mEditor(editor), mGroup1(0), mGroup2(0), mCurrentState(0),
		mTargetElement(0)
	{	
	}
	 
	
	
	/**
	 * Destructor.
	 */
	ConnectNeuronsWithSynapsesTool::~ConnectNeuronsWithSynapsesTool() {
	}
	
	
	void ConnectNeuronsWithSynapsesTool::clear() {
		if(mVisuContext != 0) {
			mVisuContext->removeMouseListener(this);
		}
		mCurrentState = 0;
		mGroup1 = 0;
		mGroup2 = 0;
		mTargetElement = 0;
		NetworkManipulationTool::clear();
	}
	
	
	void ConnectNeuronsWithSynapsesTool::activate(NetworkVisualization *visu) {
		NetworkManipulationTool::activate(visu);
		
		mCurrentState = 0;
		mGroup1 = 0;
		mGroup2 = 0;
		mTargetElement = 0;
		if(mVisuContext != 0) {
			mVisuContext->addMouseListener(this);
		}

		if((mModus & MODUS_SELECTED_ELEMENTS) != 0) {
			
			//do it immediately
			QList<Neuron*> neurons;
			
			QList<PaintItem*> items = mVisuContext->getSelectedItems();
			for(QListIterator<PaintItem*> i(items); i.hasNext();) {
				PaintItem *item = i.next();
				NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
				if(neuron != 0) {
					neurons.append(neuron->getNeuron());
				}
			}
			
			if(neurons.size() > 0) {
				interconnectSingleGroup(neurons);
			}
			emit done();
		}
		
	}
	
	QString ConnectNeuronsWithSynapsesTool::getStatusMessage() {
		return mBaseName + " Select source(s)";
	}
	
	void ConnectNeuronsWithSynapsesTool::mouseButtonPressed(NetworkVisualization *source, 
														QMouseEvent *event, const QPointF &globalPosition)
	{
		if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::RightButton)) {
			return;
		}
		
		QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());
		
		NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
		if(handler == 0) {
			return;
		}
		
		if(mGroup1 == 0 || mGroup2 == 0) {
			
			QPointF pos = globalPosition;
			NeuronGroup *selectedGroup = 0;
			
			QList<PaintItem*> items = mVisuContext->getPaintItems();
			for(int i = items.size() -1; i >= 0; --i) {
				PaintItem *item = items.at(i);
				ModuleItem *module = dynamic_cast<ModuleItem*>(item);
				if(module != 0 && module->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					selectedGroup = module->getNeuroModule();
					break;
				}
				GroupItem *group = dynamic_cast<GroupItem*>(item);
				if(group != 0 && group->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					selectedGroup = group->getNeuronGroup();
					break;
				}
			}
			
			if(mGroup1 != 0 && selectedGroup != 0) {
				//we have two modules... connect

				mGroup2 = selectedGroup;
				
				QList<Neuron*> g1Neurons = mGroup1->getNeurons();
				QList<Neuron*> sources;
				for(QListIterator<Neuron*> i(g1Neurons); i.hasNext();) {
					sources.append(i.next());
				}
				
				QList<Neuron*> g2Neurons = mGroup2->getNeurons();
				QList<SynapseTarget*> targets;
				for(QListIterator<Neuron*> i(g2Neurons); i.hasNext();) {
					targets.append(i.next());
				}
				
				connectElements(sources, targets);
				emit done();
				return;
			}
			
			if(mGroup1 == 0) {
				mGroup1 = selectedGroup;
			}

			if((mModus & MODUS_SINGLE_GROUP) != 0) {
				//we have only one group, but the modus is single-group... connect
				
				if(mGroup1 != 0) {

					interconnectSingleGroup(mGroup1->getNeurons());
					emit done();
					return;
				}
			}
			else {
				if(mGroup1 != 0 && mTargetElement != 0) {
					//we already have a single element and now the second module... connect

					QList<Neuron*> sources;
					sources.append(dynamic_cast<Neuron*>(mTargetElement));
					
					QList<Neuron*> neurons = mGroup1->getNeurons();
					QList<SynapseTarget*> targets;
					for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
						targets.append(i.next());
					}
					
					connectElements(sources, targets);
					emit done();
					return;
				}
			}
			//if a group was found (and we do not have a partner yet), then wait for the second object.
			if(selectedGroup != 0 && mGroup1 != 0 && mGroup2 == 0 && mTargetElement == 0) {
				updateStatusMessage(mBaseName + " Select target(s)");
				return;
			}
		}
		if(mTargetElement == 0) {
			Neuron *selectedNeuron = 0;
			Synapse *selectedSynapse = 0;
			QList<PaintItem*> items = mVisuContext->getPaintItems();
			for(int i = items.size() -1; i >= 0; --i) {
				PaintItem *item = items.at(i);
				NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
				SynapseItem *synapse = dynamic_cast<SynapseItem*>(item);
				if(neuron != 0 && neuron->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					selectedNeuron = neuron->getNeuron();
					break;
				}
				else if(synapse != 0 && synapse->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
					selectedSynapse = synapse->getSynapse();
					break;
				}
			}
			if(selectedNeuron == 0 && selectedSynapse == 0) {
				emit done();
				return;
			}

			mTargetElement = selectedNeuron;
			if(mTargetElement == 0) {
				mTargetElement = selectedSynapse;
			}

			if(mTargetElement != 0 && mGroup1 != 0) {
				//we have a module and a single object!

				QList<Neuron*> sources = mGroup1->getNeurons();
				QList<SynapseTarget*> targets;
				targets.append(mTargetElement);
				
				connectElements(sources, targets);
				
				emit done();
				return;
			}
			else if(mTargetElement != 0 && mGroup1 == 0) {

				updateStatusMessage(mBaseName + " Select target(s)");
				return;
			}
			
		}
		emit done();
		
	}
	
	
	void ConnectNeuronsWithSynapsesTool::mouseButtonReleased(NetworkVisualization*, 
														 QMouseEvent*, const QPointF&) 
	{
	}
	
	void ConnectNeuronsWithSynapsesTool::mouseDoubleClicked(NetworkVisualization*,
														QMouseEvent*, const QPointF&)
	{
		
	}
	
	void ConnectNeuronsWithSynapsesTool::mouseDragged(NetworkVisualization*, 
												  QMouseEvent*, const QPointF&)
	{
	}
	
	void ConnectNeuronsWithSynapsesTool::setModus(int modus) {
		mModus = modus;
	}
	
	void ConnectNeuronsWithSynapsesTool::setBasicName(const QString &name) {
		mBaseName = name;
	}
	
	void ConnectNeuronsWithSynapsesTool::interconnectSingleGroup(const QList<Neuron*> neurons) {
		
		int modi = 0;
		if((mModus & MODUS_IGNORE_INTERFACES) != 0) {
			modi = (modi | NetworkConnectivityUtil::MODUS_IGNORE_INTERFACES);
		}
		
		QList<SynapseTarget*> targets;
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			targets.append(i.next());
		}
		
		SynapseSet set = NetworkConnectivityUtil::fullyConnectElements(mVisuContext->getNeuralNetwork(), 
																	   neurons, targets, modi);
	
		if(set.mSynapses.size() > 0) {
			createInsertSynapseCommand(set);
		}
	}
	
	
	void ConnectNeuronsWithSynapsesTool::connectElements(const QList<Neuron*> &source, 
														 const QList<SynapseTarget*> &target) 
	{
		int modi = 0;
		if((mModus & MODUS_IGNORE_INTERFACES) != 0) {
			modi = (modi | NetworkConnectivityUtil::MODUS_IGNORE_INTERFACES);
		}
		if((mModus & MODUS_BIDIRECTIONAL) != 0) {
			modi = (modi | NetworkConnectivityUtil::MODUS_BIDIRECTIONAL);
		}
		
		SynapseSet set = NetworkConnectivityUtil::fullyConnectElements(mVisuContext->getNeuralNetwork(), 
																	   source, target, modi);
		
		if(set.mSynapses.size() > 0) {
			createInsertSynapseCommand(set);
		}
	}
	
	
	void ConnectNeuronsWithSynapsesTool::createInsertSynapseCommand(SynapseSet set) {
		
		InsertSynapseCommand *isc = new InsertSynapseCommand(mVisuContext,
									set.mSynapses, set.mSources, set.mTargets, set.mPositions);
		
		mVisuContext->getCommandExecutor()->executeCommand(isc);
	}
	
}



