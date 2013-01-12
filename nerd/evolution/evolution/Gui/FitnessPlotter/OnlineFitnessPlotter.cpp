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

#include "OnlineFitnessPlotter.h"
#include "Value/ValueManager.h"
#include "Core/Core.h"
#include <QVector>
#include <QWidget>
#include <QCoreApplication>

namespace nerd {

OnlineFitnessPlotter::OnlineFitnessPlotter(bool autoCleanup)
	: mFitnessPlotter(0), mAutoCleanup(autoCleanup)
{
	mFitnessPlotter = new ValuePlotterWidget("Fitness Plotter", 1, 0);
	mTriggerAction = new QAction("Fitness Plotter", 0);

	mFitnessPlotter->moveToThread(QCoreApplication::instance()->thread());
	mTriggerAction->moveToThread(QCoreApplication::instance()->thread());

	connect(mTriggerAction, SIGNAL(triggered()),
			mFitnessPlotter, SLOT(showWidget()));

	connect(mTriggerAction, SIGNAL(triggered()),
			this, SLOT(notifyActionTrigger()));

	Core::getInstance()->addSystemObject(this);
}

OnlineFitnessPlotter::~OnlineFitnessPlotter() {
	if(!mAutoCleanup) {
		//delete mFitnessPlotter;
	}
}


QString OnlineFitnessPlotter::getName() const {
	return "OnlineFitnessPlotter";
}



bool OnlineFitnessPlotter::init() {
	bool ok = true;
	return ok;
}


bool OnlineFitnessPlotter::bind() {
	bool ok = true;

	ValueManager *vm = Core::getInstance()->getValueManager();

	QList<QString> fitnessValueNames = 
			vm->getValueNamesMatchingPattern("/Statistics/Fitness/.*", true);

	for(QListIterator<QString> i(fitnessValueNames); i.hasNext();) {
		QString name = i.next();
		QString shortName = name.mid(name.lastIndexOf("/") + 1);
		QString fitnessName = name.mid(20, name.size() - 21 - shortName.size());
		
		Value *value = vm->getValue(name);
		mFitnessPlotter->getValuePlotter()->addValue(
				shortName.append(" [").append(fitnessName).append("]"), value);
	}

	//make the ValuePlotter to plot only a sequence of maximal 100 doubles
	mFitnessPlotter->setPlotterRange(100);

	mFitnessPlotter->getValuePlotter()->init();
	mFitnessPlotter->setTriggerEventName("Generation Completed");

	return ok;
}


bool OnlineFitnessPlotter::cleanUp() {
	ValuePlotter *plotter = mFitnessPlotter->getValuePlotter();

	QList<Value*> values = plotter->getPlottedValues();
	for(QListIterator<Value*> i(values); i.hasNext();) {
		plotter->removeValue(i.next());
	}
	if(!mAutoCleanup) {
		Core::getInstance()->removeSystemObject(this);
	}
	return true;
}



void OnlineFitnessPlotter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
}


/**
 * Returns the QAction that initializes and shows the ValuePlotter with the 
 * FitnessFunction statistics. 
 * Such an action can be added safely to any QMenu as it is not destroyed in the
 * constructor of the OnlineFitnessPlotter. Therefore it should be deleted by
 * a QMenu or manually by a user to prevent memory leaks.
 *
 * @return the QAction to show the ValuePlotter.
 */
QAction* OnlineFitnessPlotter::getAction() const {
	return mTriggerAction;
}

ValuePlotterWidget* OnlineFitnessPlotter::getPlotter() const {
	return mFitnessPlotter;
}


void OnlineFitnessPlotter::notifyActionTrigger() {
	mFitnessPlotter->setTriggerEventName("Generation Completed");
}

}



