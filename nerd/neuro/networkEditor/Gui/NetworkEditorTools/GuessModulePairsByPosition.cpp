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



#include "GuessModulePairsByPosition.h"
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

using namespace std;

namespace nerd {


/**
 * Constructs a new GuessModulePairsByPosition.
 */
GuessModulePairsByPosition::GuessModulePairsByPosition(NeuralNetworkEditor *editor, NeuralNetworkToolbox *owner)
	: NetworkManipulationTool(owner), mEditor(editor), mGroup1(0), mGroup2(0), mCurrentState(0),
	  mReferenceNeuron1(0), mReferenceNeuron2(0), mPositionTransformation(0)
{
	mPositionTransformation = new StringValue("");

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/Tools/GuessModulePairsByPosition/LayoutTransformation", mPositionTransformation);

	connect(this, SIGNAL(showElementPairs()),
			owner, SLOT(useVisualizeElementPairTool()));
}



/**
 * Destructor.
 */
GuessModulePairsByPosition::~GuessModulePairsByPosition() {
}


void GuessModulePairsByPosition::clear() {
	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}
	mCurrentState = 0;
	mGroup1 = 0;
	mGroup2 = 0;
	mReferenceNeuron1 = 0;
	mReferenceNeuron2 = 0;
	NetworkManipulationTool::clear();
}


void GuessModulePairsByPosition::activate(NetworkVisualization *visu) {
	NetworkManipulationTool::activate(visu);

	mCurrentState = 0;
	mGroup1 = 0;
	mGroup2 = 0;
	mReferenceNeuron1 = 0;
	mReferenceNeuron2 = 0;
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
	
}

QString GuessModulePairsByPosition::getStatusMessage() {
	return QString("Guess Group Pairs By Position: Select frist group");
}

void GuessModulePairsByPosition::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition)
{
	if(mVisuContext == 0 || mVisuContext != source || !(event->buttons() & Qt::RightButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHandler();
	if(handler == 0) {
		return;
	}

	if(mGroup1 == 0 || mGroup2 == 0) {

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
		if(selectedGroup == 0) {
			emit done();
			return;
		}
		if(mGroup1 == 0) {
			mGroup1 = selectedGroup;
			updateStatusMessage("Guess Group Pairs by Position: Select second group");
			return;
		}
		else if(mGroup1 != selectedGroup) {
			mGroup2 = selectedGroup;
			updateStatusMessage("Guess Group Pairs by Position: Select first reference neuron");
			return;
		}
	}
	else if(mReferenceNeuron1 == 0 || mReferenceNeuron2 == 0) {
		
		Neuron *selectedNeuron = 0;
		QList<PaintItem*> items = mVisuContext->getPaintItems();
		for(int i = items.size() -1; i >= 0; --i) {
			PaintItem *item = items.at(i);
			NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
			if(neuron != 0 && neuron->isHit(globalPosition, event->buttons(), mVisuContext->getScaling())) {
				selectedNeuron = neuron->getNeuron();
				break;
			}
		}
		if(selectedNeuron == 0) {
			emit done();
			return;
		}
		if(mReferenceNeuron1 == 0) {
			mReferenceNeuron1 = selectedNeuron;
			updateStatusMessage("Guess Group Pairs by Position: Select second reference neuron");
			return;
		}
		else if(mReferenceNeuron2 == 0 && mReferenceNeuron1 != selectedNeuron) {
			mReferenceNeuron2 = selectedNeuron;

			createPairString();
			emit done();
			emit showElementPairs();
			return;
		}
	}
	emit done();

}


void GuessModulePairsByPosition::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
}

void GuessModulePairsByPosition::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}

void GuessModulePairsByPosition::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
}

void GuessModulePairsByPosition::createPairString() {
	
	if(mGroup1 == 0 || mGroup2 == 0 || mReferenceNeuron1 == 0 || mReferenceNeuron2 == 0) {
		return;
	}

	QString mIdString;

	bool mirrorHorizontally = false;
	bool mirrorVertically = false;

	if(mPositionTransformation->get().toLower().contains("h")) {
		mirrorHorizontally = true;
	}
	if(mPositionTransformation->get().toLower().contains("v")) {
		mirrorVertically = true;
	}

	Vector3D referencePoint1 = mReferenceNeuron1->getPosition();
	Vector3D referencePoint2 = mReferenceNeuron2->getPosition();
	

	QList<NeuroModule*> firstGroupModules = mGroup1->getAllEnclosedModules();
	QList<NeuroModule*> secondGroupModules = mGroup2->getAllEnclosedModules();

	QList<Neuron*> firstGroupNeurons = mGroup1->getAllEnclosedNeurons();
	QList<Neuron*> secondGroupNeurons = mGroup2->getAllEnclosedNeurons();

	int countMismatchingElements = 0;
	QString missingElementNames;

	for(QListIterator<NeuroModule*> i(firstGroupModules); i.hasNext();) {
		NeuroModule *m1 = i.next();

		Vector3D posDifference = (m1->getPosition() - referencePoint1);

		if(mirrorHorizontally) {
			posDifference.setX(-1 * posDifference.getX());
		}
		if(mirrorVertically) {
			posDifference.setY(-1 * posDifference.getY());
		}
		Vector3D requiredPosition = referencePoint2 + posDifference;

		NeuroModule *bestPartner = 0;
		double lowestDifference;

		for(QListIterator<NeuroModule*> j(secondGroupModules); j.hasNext();) {
			NeuroModule *m2 = j.next();

			if(bestPartner == 0) {
				bestPartner = m2;
				lowestDifference = (m2->getPosition() - requiredPosition).length();
				continue;
			}

			Vector3D difference = m2->getPosition() - requiredPosition;
			if(difference.length() < lowestDifference) {
				lowestDifference = difference.length();
				bestPartner = m2;
			}
			
		}
		if(bestPartner != 0) {
			secondGroupModules.removeAll(bestPartner);
			if(mIdString != "") {
				mIdString.append("|");
			}
			mIdString.append(QString::number(m1->getId())).append(",")
						.append(QString::number(bestPartner->getId()));
		}
		else {
			++countMismatchingElements;
			missingElementNames.append(m1->getName()).append(" (")
					.append(QString::number(m1->getId())).append(")\n");
		}
	}

	for(int i = 0; i < secondGroupModules.size(); ++i) {
		NeuroModule *elem = secondGroupModules.at(i);
		++countMismatchingElements;
		missingElementNames.append(elem->getName()).append(" (")
				.append(QString::number(elem->getId())).append(")\n");
	}

	QList<Neuron*> sortedNeurons;
	for(QListIterator<Neuron*> i(firstGroupNeurons); i.hasNext();) {
		Neuron *neuron = i.next();
		double distance = (neuron->getPosition() - referencePoint1).length();

		bool added = false;
		for(int j = 0; j < sortedNeurons.size(); ++j) {
			Neuron *neuron2 = sortedNeurons.at(j);
			if(distance < (neuron2->getPosition() - referencePoint1).length()) {
				sortedNeurons.insert(j, neuron);
				added = true;
				break;
			}
		}
		if(!added) {
			sortedNeurons.append(neuron);
		}
	}

	firstGroupNeurons = sortedNeurons;

	for(QListIterator<Neuron*> i(firstGroupNeurons); i.hasNext();) {
		Neuron *n1 = i.next();

		Vector3D posDifference = (n1->getPosition() - referencePoint1);

		if(mirrorHorizontally) {
			posDifference.setX(-1 * posDifference.getX());
		}
		if(mirrorVertically) {
			posDifference.setY(-1 * posDifference.getY());
		}
		Vector3D requiredPosition = referencePoint2 + posDifference;

		Neuron *bestPartner = 0;
		double lowestDifference;

		for(QListIterator<Neuron*> j(secondGroupNeurons); j.hasNext();) {
			Neuron *n2 = j.next();

			if(bestPartner == 0) {
				bestPartner = n2;
				lowestDifference = (n2->getPosition() - requiredPosition).length();
				continue;
			}

			Vector3D difference = n2->getPosition() - requiredPosition;
			if(difference.length() < lowestDifference) {
				lowestDifference = difference.length();
				bestPartner = n2;
			}
			
		}
		if(bestPartner != 0) {
			secondGroupNeurons.removeAll(bestPartner);
			if(mIdString != "") {
				mIdString.append("|");
			}
			mIdString.append(QString::number(n1->getId())).append(",")
						.append(QString::number(bestPartner->getId()));
		}
		else {
			++countMismatchingElements;
			missingElementNames.append(n1->getNameValue().get()).append(" (")
					.append(QString::number(n1->getId())).append(")\n");
		}
	}

	for(int i = 0; i < secondGroupNeurons.size(); ++i) {
		Neuron *elem = secondGroupNeurons.at(i);
		++countMismatchingElements;
		missingElementNames.append(elem->getNameValue().get()).append(" (")
				.append(QString::number(elem->getId())).append(")\n");
	}

	EditorMessageWidget *messageBoard = 0;
	if(mEditor != 0) {
		messageBoard = mEditor->getMessageWidget();
		messageBoard->setMessage("Guessing Group Pairs By Position:");
	}
	
	if(countMismatchingElements > 0 && messageBoard != 0) {
		messageBoard->addMessage(QString("GuessModulePairsByPosition: Could not find pairs for all "
					"neurons in group1 [")
					.append(QString::number(countMismatchingElements)).append( " missing elements]\n")
					.append(missingElementNames));
	}
	else if(messageBoard != 0) {
		messageBoard->addMessage("Success");
	}

	QApplication::clipboard()->setText(mIdString);
}

}



