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

#include "MultiplePopulationOverviewWindowWidget.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/Evolution.h"
#include "Core/Core.h"
#include "EvolutionConstants.h"


namespace nerd {

MultiplePopulationOverviewWindowWidget::MultiplePopulationOverviewWindowWidget(
									const QString &worldName,QWidget *parent) 
	: QWidget(parent), mInitialized(false), mCurrentGenerationValue(0), mWorldName(worldName)
{
	setWindowTitle("Population Overview - Gen 0");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	mTriggerAction = new QAction("Population Overview", 0);
	mTabWidget = new QTabWidget();

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);

	mainLayout->addWidget(mTabWidget);

	connect(mTriggerAction, SIGNAL(triggered()), this, SLOT(showOverview()));
	connect(this, SIGNAL(changeTitle(QString)), this, SLOT(setWindowTitle(QString)));

	mCurrentGenerationValue = Evolution::getEvolutionManager()->getCurrentGenerationValue();
	mCurrentGenerationValue->addValueChangedListener(this);

}

MultiplePopulationOverviewWindowWidget::~MultiplePopulationOverviewWindowWidget() {
	while(mPopulationOverviews.size() > 0) {
		PopulationOverviewWidget *tmp = mPopulationOverviews.at(0);
		mPopulationOverviews.pop_front();
		delete tmp;
	}
}



void MultiplePopulationOverviewWindowWidget::addOverview(PopulationOverviewWidget *overview) {
	QString name = "";
	if(overview == 0) {
		return;
	}
	if(overview->getPopulation() != 0) {
		name = overview->getPopulation()->getName();
	}
	mTabWidget->addTab(overview, name);
	mPopulationOverviews.push_back(overview);
}

QAction* MultiplePopulationOverviewWindowWidget::getAction() const {
	return mTriggerAction;
}

void MultiplePopulationOverviewWindowWidget::showOverview() {
	if(!mInitialized) {
		EvolutionManager *eManager = Evolution::getEvolutionManager();
		QList<World*> worlds = eManager->getEvolutionWorlds();
		for(int i = 0; i < worlds.size(); i++) {
			World *world = worlds.at(i);
			if(mWorldName == "" || mWorldName == world->getName()) {
				QList<Population*> populations = world->getPopulations();
				for(int j = 0; j < populations.size(); j++) {
					PopulationOverviewWidget *newOverview = 
						new PopulationOverviewWidget(populations.at(j));
					addOverview(newOverview);
				}
			}
		}
	
		mCurrentGenerationValue = Core::getInstance()->getValueManager()->getIntValue(
			EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);
		if(mCurrentGenerationValue == 0) {
			Core::log("MultiplePopulationOverviewWindowWidget: Could not find required value.");
			return;
		}
		mCurrentGenerationValue->addValueChangedListener(this);
		setWindowTitle("Population Overview - Gen " 
			+ QString::number(mCurrentGenerationValue->get() - 1));
	}
	mInitialized = true;
	show();
}

void MultiplePopulationOverviewWindowWidget::valueChanged(Value *value) {
	if(value == mCurrentGenerationValue && value != 0) {
		emit changeTitle("Population Overview - Gen " + QString::number(
			mCurrentGenerationValue->get() - 1));
	}
}

QString MultiplePopulationOverviewWindowWidget::getName() const {
	return "MultiplePopulationOverviewWindowWidget";
}

}
