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



#include "GuessModulePairs.h"
#include <iostream>
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
 * Constructs a new GuessModulePairs.
 */
GuessModulePairs::GuessModulePairs(NeuralNetworkEditor *editor, NeuralNetworkToolbox *owner)
	: NetworkManipulationTool(owner), mEditor(editor), mGroup1(0), mGroup2(0), mCurrentState(0)
{
	ValueManager *vm = Core::getInstance()->getValueManager();

	mFirstPhrase = new StringValue("Left");
	mSecondPhrase = new StringValue("Right");
	mBlackList = new StringValue("Module,Hidden");
	mForceReplacements = new BoolValue(false);

	vm->addValue("/Tools/GuessModulePairs/FirstPhrase", mFirstPhrase);
	vm->addValue("/Tools/GuessModulePairs/SecondPhrase", mSecondPhrase);
	vm->addValue("/Tools/GuessModulePairs/IgnoredNames", mBlackList);
	vm->addValue("/Tools/GuessModulePairs/ForceReplacements", mForceReplacements);

	connect(this, SIGNAL(showElementPairs()),
			owner, SLOT(useVisualizeElementPairTool()));
}


/**
 * Destructor.
 */
GuessModulePairs::~GuessModulePairs() {
}

void GuessModulePairs::clear() {
	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
	}
	mCurrentState = 0;
	mGroup1 = 0;
	mGroup2 = 0;
	NetworkManipulationTool::clear();
}


void GuessModulePairs::activate(NetworkVisualization *visu) {
	NetworkManipulationTool::activate(visu);

	mCurrentState = 0;
	mGroup1 = 0;
	mGroup2 = 0;
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
	}
	
}

QString GuessModulePairs::getStatusMessage() {
	return QString("Guess Group Pairs: Select frist group");
}

void GuessModulePairs::mouseButtonPressed(NetworkVisualization *source, 
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
			updateStatusMessage("Guess Group Pairs: Select second group");
			return;
		}
		else if(mGroup1 != selectedGroup) {
			mGroup2 = selectedGroup;

			createPairString();
			emit done();
			emit showElementPairs();
			return;
		}
	}
	emit done();

}


void GuessModulePairs::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
}

void GuessModulePairs::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}

void GuessModulePairs::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
}

void GuessModulePairs::createPairString() {
	QString mIdString;
	

	QStringList firstPhrases = mFirstPhrase->get().split(",");
	QStringList secondPhrases = mSecondPhrase->get().split(",");
	QStringList ignoredNames = mBlackList->get().split(",");

	QList<NeuroModule*> firstGroupModules;
	QList<NeuroModule*> secondGroupModules;

	firstGroupModules = mGroup1->getAllEnclosedModules();
	secondGroupModules = mGroup2->getAllEnclosedModules();

	QList<Neuron*> firstGroupNeurons = mGroup1->getAllEnclosedNeurons();
	QList<Neuron*> secondGroupNeurons = mGroup2->getAllEnclosedNeurons();

	int countMismatchingElements = 0;
	QString missingElementNames;

	for(QListIterator<NeuroModule*> i(firstGroupModules); i.hasNext();) {
		NeuroModule *m1 = i.next();
		
		if(ignoredNames.contains(m1->getName().trimmed())) {
			continue;
		}

		bool foundPartner = false;

		for(QListIterator<NeuroModule*> j(secondGroupModules); j.hasNext();) {
			NeuroModule *m2 = j.next();
			
			if(ignoredNames.contains(m2->getName().trimmed())) {
				continue;
			}
			
			for(int k = 0; k < firstPhrases.size() && k < secondPhrases.size(); ++k) {
				QString name = m1->getName();
				
				name = name.replace(firstPhrases.at(k).trimmed(), secondPhrases.at(k).trimmed()).trimmed();
				if(mForceReplacements->get() && (name == m1->getName().trimmed())) {
					continue;
				}
				if(name == m2->getName()) {
					if(mIdString != "") {
						mIdString.append("|");
					}
					mIdString.append(QString::number(m1->getId())).append(",")
							 .append(QString::number(m2->getId()));
					foundPartner = true;
					
					break;
				}
			}
		}
		if(!foundPartner) {
			++countMismatchingElements;
			missingElementNames.append(m1->getName()).append("\n");
		}
	}

	for(QListIterator<Neuron*> i(firstGroupNeurons); i.hasNext();) {
		Neuron *n1 = i.next();

		bool foundPartner = false;

		for(QListIterator<Neuron*> j(secondGroupNeurons); j.hasNext();) {
			Neuron *n2 = j.next();
			for(int k = 0; k < firstPhrases.size() && k < secondPhrases.size(); ++k) {
				QString name = n1->getNameValue().get();
				name = name.replace(firstPhrases.at(k).trimmed(), secondPhrases.at(k).trimmed()).trimmed();
				
				if(mForceReplacements->get() && (name == n1->getNameValue().get().trimmed())) {
					continue;
				}
				
				if(name == n2->getNameValue().get()) {
					if(mIdString != "") {
						mIdString.append("|");
					}
					mIdString.append(QString::number(n1->getId())).append(",")
							 .append(QString::number(n2->getId()));
					foundPartner = true;
					break;
				}
			}
		}
		if(!foundPartner) {
			++countMismatchingElements;
			missingElementNames.append(n1->getNameValue().get()).append("\n");
		}
	}

	EditorMessageWidget *messageBoard = 0;
	if(mEditor != 0) {
		messageBoard = mEditor->getMessageWidget();
		messageBoard->setMessage("Guessing Group Pairs:");
	}
	
	if(countMismatchingElements > 0 && messageBoard != 0) {
		messageBoard->addMessage(QString("GuessModulePairs: Could not find pairs for all "
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



