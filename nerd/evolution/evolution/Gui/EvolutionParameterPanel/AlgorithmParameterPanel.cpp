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



#include "AlgorithmParameterPanel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include "Gui/EvolutionParameterPanel/EvolutionOperatorPanel.h"
#include <QLabel> 
#include "Value/IntValue.h"
#include "NerdConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new AlgorithmParameterPanel.
 */
AlgorithmParameterPanel::AlgorithmParameterPanel(const QString &algorithmPrefix)
	: QWidget(), mAlgorithmPrefix(algorithmPrefix)
{
	setWindowTitle("Evolution Algorithm Panel");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	EventManager *em = Core::getInstance()->getEventManager();
	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this, false);


	ValueManager *vm = Core::getInstance()->getValueManager();
	QList<QString> parameterNames = vm->getValueNamesMatchingPattern(algorithmPrefix + ".*");

	QList<QString> operatorNames;

	for(QListIterator<QString> i(parameterNames); i.hasNext();) {
		QString name = i.next();
		
		bool alreadyDetected = false;
		for(QListIterator<QString> j(operatorNames); j.hasNext();) {
			if(name.startsWith(algorithmPrefix + j.next())) {
				alreadyDetected = true;
				break;
			}
		}		
		if(alreadyDetected) {
			continue;
		}

		QString operatorNameRest = name.mid(algorithmPrefix.size());

		int indexOfNextSeparator = operatorNameRest.indexOf("/");
		if(indexOfNextSeparator < 0) {
			continue;
		}

		QString operatorName = operatorNameRest.mid(0, indexOfNextSeparator);

		operatorNames.append(operatorName);
	}

	QVBoxLayout *mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	mOperatorArea = new QWidget();

	mOperatorLayout = new QVBoxLayout();
	mOperatorArea->setLayout(mOperatorLayout);
	mOperatorLayout->setContentsMargins(1, 0, 1, 0);
	
	for(QListIterator<QString> i(operatorNames); i.hasNext();) {
		QString name = i.next();

		mOperatorPanels.append(new EvolutionOperatorPanel(algorithmPrefix, name));
	}

	sortOperatorPanel();

	QScrollArea *scrollArea = new QScrollArea();
	scrollArea->setWidget(mOperatorArea);
	scrollArea->setWidgetResizable(true);
	scrollArea->setContentsMargins(1, 0, 1, 0);
	mainLayout->addWidget(scrollArea);
	scrollArea->show();
	mOperatorArea->show();
}



/**
 * Destructor.
 */
AlgorithmParameterPanel::~AlgorithmParameterPanel() {
	
}

QString AlgorithmParameterPanel::getName() const {
	return "AlgorithmParameterPanel";
}

void AlgorithmParameterPanel::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		for(QListIterator<EvolutionOperatorPanel*> i(mOperatorPanels); i.hasNext();) {
			i.next()->cleanUp();
		}
		mShutDownEvent->removeEventListener(this);
	}
}


void AlgorithmParameterPanel::sortOperatorPanel() {

	//TODO check why it is not possible to remove the widgets and then to add them in a different order.

// 	//Delete previous operator panels
// 	for(int i = mOperatorLayout->count(); i > 0; --i) {
// 		cerr << "check: " << i << mOperatorLayout->count() << endl;
// 		QLayoutItem *item = mOperatorLayout->itemAt(i - 1);
// 		EvolutionOperatorPanel *panel = dynamic_cast<EvolutionOperatorPanel*>(item->widget());
// 		if(panel != 0) {
// 			mOperatorLayout->removeWidget(panel);
// 		}
// 	}

	sortPanels();

	//add operator panels
	for(QListIterator<EvolutionOperatorPanel*> i(mOperatorPanels); i.hasNext();) {
		EvolutionOperatorPanel *panel = i.next();
		mOperatorLayout->addWidget(panel);
	}
	mOperatorLayout->addStretch(5);

	mOperatorArea->show();
}

void AlgorithmParameterPanel::sortPanels() {
	//sort panels

	ValueManager *vm = Core::getInstance()->getValueManager();

	QHash<EvolutionOperatorPanel*, int> mIndices;
	QList<EvolutionOperatorPanel*> operatorPanels = mOperatorPanels;
	mOperatorPanels.clear();
	for(QListIterator<EvolutionOperatorPanel*> i(operatorPanels); i.hasNext();) {
		EvolutionOperatorPanel *panel = i.next();
		IntValue *indexValue = vm->getIntValue(panel->getPrefix() + panel->getOperatorName() 
									+ "/Config/OperatorIndex");
		if(indexValue != 0) {
			int index = indexValue->get();
			mIndices.insert(panel, index);

			bool inserted = false;
			for(int j = 0; j < mOperatorPanels.size(); ++j) {
				EvolutionOperatorPanel *p = mOperatorPanels.at(j);
				if(mIndices.value(p) > index) {
					mOperatorPanels.insert(j, panel);
					inserted = true;
					break;
				}
			}
			if(!inserted) {
				mOperatorPanels.append(panel);
			}
		}
		else {
			mIndices.insert(panel, -999999);
			mOperatorPanels.append(panel);
		}
	}
}


}



