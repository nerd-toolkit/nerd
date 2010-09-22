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

#include "PopulationOverviewWidget.h"
#include "Core/Core.h"
#include "EvolutionConstants.h"
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>

namespace nerd {

PopulationOverviewWidget::PopulationOverviewWidget(QWidget *parent) : QWidget(parent), 
		mPopulation(0), mUpdateTriggerEvent(0), mMainLayout(0), 
		mIndividualPropertiesList(0), mSelectVisiblePropertiesButton(0), 
		mPropertiesSelectionWidget(0), mPropertyListLayout(0), mTableInitialized(false)
{
	setup();
	emit overviewChanged();
}


PopulationOverviewWidget::PopulationOverviewWidget(Population *population, QWidget *parent) 
		: QWidget(parent), mUpdateTriggerEvent(0), mMainLayout(0), 
		mIndividualPropertiesList(0), mSelectVisiblePropertiesButton(0), 
		mPropertiesSelectionWidget(0), mPropertyListLayout(0), mTableInitialized(false)
{
	setup();
	setPopulation(population);
	emit overviewChanged();
}

void PopulationOverviewWidget::setup() {
	mSelectAll = false;
	mMainLayout = new QVBoxLayout();
	setLayout(mMainLayout);
	
	mIndividualPropertiesList = new QTableWidget(0,0);
	// The next two lines will remove the row-numbers on the left side of the table.
// 	mIndividualPropertiesList->verticalHeader()->setEnabled(true);
// 	mIndividualPropertiesList->verticalHeader()->hide();
	mIndividualPropertiesList->setSortingEnabled(true);

	mMainLayout->addWidget(mIndividualPropertiesList);

	mSelectVisiblePropertiesButton = new QPushButton("Select Properties");
	mMainLayout->addWidget(mSelectVisiblePropertiesButton);

	mUpdateTriggerEvent = Core::getInstance()->getEventManager()->
		registerForEvent(EvolutionConstants::EVENT_EVO_SELECTION_COMPLETED, this);
	if(mUpdateTriggerEvent == 0) {
		Core::log("PopulationOverviewWidget: Could not find required Event.");
		return;
	}

	connect(this, SIGNAL(overviewChanged()), this, SLOT(updateTable()));
	connect(mSelectVisiblePropertiesButton, SIGNAL(clicked()), this, 
		SLOT(showPropertySelectionWidget()));

	connect(this, SIGNAL(selectablePropertiesChanged()), this, 
		SLOT(updateSelectablePropertiesList()));

	mPropertiesSelectionWidget = new QWidget();
	mPropertyListLayout = new QVBoxLayout();
	QVBoxLayout *propertySelectionWidgetLayout = new QVBoxLayout(mPropertiesSelectionWidget);
	QPushButton *applyPropertySelection = new QPushButton("Apply Selection", 
		mPropertiesSelectionWidget);
	QPushButton *changeAllSelections = new QPushButton("(De)select All", 
		mPropertiesSelectionWidget);
	QHBoxLayout *buttonLayout = new QHBoxLayout();

	mPropertiesSelectionWidget->setWindowTitle("Property Selection");
	mPropertiesSelectionWidget->setAttribute(Qt::WA_QuitOnClose, false);
	mPropertiesSelectionWidget->setAttribute(Qt::WA_DeleteOnClose, false);
	mPropertiesSelectionWidget->setLayout(propertySelectionWidgetLayout);
	propertySelectionWidgetLayout->addItem(mPropertyListLayout);
	propertySelectionWidgetLayout->addItem(buttonLayout);

	buttonLayout->addWidget(changeAllSelections);
	buttonLayout->addWidget(applyPropertySelection);
	connect(applyPropertySelection, SIGNAL(clicked()),this, SLOT(updateSelection()));
	connect(changeAllSelections, SIGNAL(clicked()),this, SLOT(changeSelection()));
}

void PopulationOverviewWidget::setPopulation(Population *population) {
	mPopulation = population;
	if(population == 0) {
		return;
	}
	updateGenerationData();
// 	QList<Individual*> individuals = mPopulation->getIndividuals();
// 	for(int i = 0; i < individuals.size(); i++) {
// 		QList<QString> properties = individuals.at(i)->getPropertyNames();	
// 		for(int j = 0; j < properties.size(); j++) {	
// 			if(!mPossibleProperties.contains(properties.at(j))) {
// 				mPossibleProperties.push_back(properties.at(j));
// 			}
// 		}
// 	}
// 	mPropertiesToVisualize = mPossibleProperties;
// 
// 	mPropertiesToVisualize.move(mPropertiesToVisualize.indexOf("ID"), 0);
// 	emit selectablePropertiesChanged();
	emit overviewChanged();
}

Population* PopulationOverviewWidget::getPopulation() const {
	return mPopulation;
}

void PopulationOverviewWidget::eventOccured(Event *event) {
	if(event == mUpdateTriggerEvent && event != 0) {
		updateGenerationData();
		emit overviewChanged();
	}
}

QString PopulationOverviewWidget::getName() const {
	QString widgetName = "PopulationOverview/" + mPopulation->getName();
	return widgetName;
}

/**
 * Creates a new list of properties that exist for the current generation.
 */
void PopulationOverviewWidget::updateGenerationData() {
	if(mPopulation == 0) {
		return;
	}
	QStringList tablePropertyNames = mPropertiesToVisualize;
	QStringList oldPossibleProperties = mPossibleProperties;
	QStringList individualProperties; 
	QList<Individual*> individuals = mPopulation->getIndividuals();

	mGenerationData.clear();
	mPossibleProperties.clear();
	for(int i = 0; i < individuals.size(); i++) {
		QList<QString> properties = individuals.at(i)->getPropertyNames();	
		individualProperties += properties;
		QHash <QString, QString> individualProperties;
		for(int j = 0; j < properties.size(); j++) {	
			individualProperties[properties.at(j)] = individuals.at(i)->
				getProperty(properties.at(j));
			if(!tablePropertyNames.contains(properties.at(j)) 
				&& !oldPossibleProperties.contains(properties.at(j))) 
			{
				tablePropertyNames.push_back(properties.at(j));
			}
			if(!mPossibleProperties.contains(properties.at(j))) {
				mPossibleProperties.push_back(properties.at(j));
			}
		}
		mGenerationData.push_back(individualProperties);
	}
	// check, whether there are properties from the old table, that do no longer exist in any individual.
	for(int i = 0; i < mPropertiesToVisualize.size(); i++) {
		if(!individualProperties.contains(mPropertiesToVisualize.at(i))) {
			tablePropertyNames.removeAt(tablePropertyNames.indexOf(mPropertiesToVisualize.at(i)));
		}
	}
	mPropertiesToVisualize = tablePropertyNames;

	bool update = false;
	if(oldPossibleProperties.size() == mPossibleProperties.size()) {
		for(int i = 0; i < mPossibleProperties.size(); i++) {
			if(!oldPossibleProperties.contains(mPossibleProperties.at(i))) {
				update = true;
				continue;
			}
		}
	}
	else {
		update = true;
	}
	if(update) {
		emit selectablePropertiesChanged();
	}
}

/**
 * Updates the table content. Removes the property-columns that are no longer needed and adds new columns for properties which were not visualized before. Removes and updates the contents of the table-cells.
 */
void PopulationOverviewWidget::updateTable() {
	if(mIndividualPropertiesList == 0 || mPopulation == 0) {
		return;
	}
	mIndividualPropertiesList->setSortingEnabled(false);
	mIndividualPropertiesList->setRowCount(0);

	mIndividualPropertiesList->setColumnCount(mPropertiesToVisualize.size());
	
	mIndividualPropertiesList->setRowCount(mGenerationData.size());
	mIndividualPropertiesList->setHorizontalHeaderLabels(mPropertiesToVisualize);
	for(int i = 0; i < mGenerationData.size(); i++) {
		QHash<QString, QString> properties = mGenerationData.at(i);
		QHash<QString, QString>::iterator iterate;
 		for(iterate = properties.begin(); iterate != properties.end(); ++iterate) {
			QString name = iterate.key();
			if(mPropertiesToVisualize.contains(name)) {
				int index = mPropertiesToVisualize.indexOf(name);
				QTableWidgetItem *item = new QTableWidgetItem(iterate.value());
				item->setData(Qt::DisplayRole, QVariant(iterate.value()));
		
				mIndividualPropertiesList->setItem(i, index, item);
			}
		}
	}
	mIndividualPropertiesList->setSortingEnabled(true);

}


/**
 * Opens a window, where all available properties are shown. 
 * Here the user can make a selection of the properties to be shown in the table.
 */
void PopulationOverviewWidget::showPropertySelectionWidget() {
	if(!mPropertiesSelectionWidget->isVisible()) {
		emit selectablePropertiesChanged();
		mPropertiesSelectionWidget->show();
	}
	else {
		mPropertiesSelectionWidget->hide();
	}
}


void PopulationOverviewWidget::updateSelection() {
	QStringList properties;
	for(int i = 0; i < mCheckBoxes.size(); i++) {
		if(mCheckBoxes.at(i)->checkState() == Qt::Checked) {
			properties.push_back(mPossibleProperties.at(i));	
		}
	}
	mPropertiesToVisualize = properties;
	if(mPropertiesToVisualize.contains("ID")) {
		mPropertiesToVisualize.move(mPropertiesToVisualize.indexOf("ID"), 0);
	}
	emit overviewChanged();
}


/**
 * Slot-method, that reacts on pushing the select/deselect all button in the property selection window (this method simply switches between check all and uncheck all).
 */
void PopulationOverviewWidget::changeSelection() {
	
	for(int i = 0; i < mCheckBoxes.size(); i++) {
		if(mSelectAll) {
			mCheckBoxes.at(i)->setCheckState(Qt::Checked);
		}
		else {
			mCheckBoxes.at(i)->setCheckState(Qt::Unchecked);
		}
	}
	mSelectAll = !mSelectAll;

}

/**
 * Updates the list of selectable properties in the select properties view and creates new checkboxes and labels if necessary.
 */
void PopulationOverviewWidget::updateSelectablePropertiesList() {
	mCheckBoxes.clear();
	while(mListEntries.size() > 0) {
		QWidget *tmp = mListEntries.at(0);
		mPropertyListLayout->removeWidget(tmp);
		mListEntries.pop_front();
		delete tmp;
	}

	for(int i = 0; i < mPossibleProperties.size(); i++) {
			QWidget *entry = new QWidget(mPropertiesSelectionWidget);
			mListEntries.push_back(entry);
			QHBoxLayout *entryLayout = new QHBoxLayout();
			entry->setLayout(entryLayout);
			mPropertyListLayout->addWidget(entry);
			
			QLabel *name = new QLabel(mPossibleProperties.at(i), mPropertiesSelectionWidget);
			entryLayout->addWidget(name);
			entryLayout->setSpacing(1);
			QCheckBox *selectForVisualization = new QCheckBox(mPropertiesSelectionWidget);
			bool isVisible = mPropertiesToVisualize.contains(mPossibleProperties.at(i));
			if(isVisible) {
				selectForVisualization->setCheckState(Qt::Checked);
			}
			else {
				selectForVisualization->setCheckState(Qt::Unchecked);
			}
			entryLayout->addWidget(selectForVisualization);
			mCheckBoxes.push_back(selectForVisualization);
			entryLayout->setAlignment(selectForVisualization, Qt::AlignRight);
			entryLayout->setAlignment(name, Qt::AlignLeft);
			
		}
	if(mPropertiesSelectionWidget->isVisible()) {
		mPropertiesSelectionWidget->hide();
		showPropertySelectionWidget();
	}
}

}

