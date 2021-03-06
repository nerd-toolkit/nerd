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

#ifndef PopulationOverviewWidget_H_
#define PopulationOverviewWidget_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QCheckBox>
#include <QPushButton>
#include "Evolution/Population.h"
#include "Event/EventListener.h"
#include "Value/StringValue.h"
#include "Value/IntValue.h"

namespace nerd {


class OverviewTableItem : public QTableWidgetItem {
	public:
		OverviewTableItem(const QString &text);

		virtual bool operator<(const QTableWidgetItem & other) const;
};


/**
 * PopulationOverviewWidget
**/

class PopulationOverviewWidget : public QWidget, public virtual EventListener {

	Q_OBJECT

	public:
		PopulationOverviewWidget(QWidget *parent = 0);
		PopulationOverviewWidget(Population *population, QWidget *parent = 0);
		void setPopulation(Population *population);
		Population* getPopulation() const;

		void eventOccured(Event *event);
		QString getName() const;

	private:
		void setup();

	private slots:
		void updateTable();
		void updateGenerationData();
		void showPropertySelectionWidget();
		void updateSelection();
		void changeSelection();
		void updateSelectablePropertiesList();

	signals:
		void overviewChanged();
		void selectablePropertiesChanged();

	private:
		Population *mPopulation;
		Event *mUpdateTriggerEvent;
		QStringList mProperties;
		QStringList mPossibleProperties;
		QStringList mPropertiesToVisualize;
		bool mSelectAll;

		QVBoxLayout *mMainLayout;
		QTableWidget *mIndividualPropertiesList;
		QPushButton *mSelectVisiblePropertiesButton;

		QList<QCheckBox*> mCheckBoxes;	
		QList<QWidget*> mListEntries;

		QWidget *mPropertiesSelectionWidget;
		QVBoxLayout *mPropertyListLayout;

		QList<QHash<QString, QString> > mGenerationData;

		bool mTableInitialized;

		StringValue *mCurrentStartScriptValue;
		IntValue *mCurrentGenerationValue;
		bool mTogglePreviews;
};
}
#endif

