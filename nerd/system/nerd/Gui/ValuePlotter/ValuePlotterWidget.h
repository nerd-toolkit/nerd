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



#ifndef NERDValuePlotterWidget_H
#define NERDValuePlotterWidget_H

#include <QWidget>
#include "Gui/ValuePlotter/ValuePlotter.h"
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QCheckBox>
#include "Gui/ValuePlotter/PlotterItemControlPanel.h"
#include <QVector>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSplitter>

namespace nerd {

	class StaticDataLoader;

	/**
	 * ValuePlotterWidget
	 */
	class ValuePlotterWidget : public QWidget {
	Q_OBJECT
	public:
		ValuePlotterWidget(const QString &name, int activeTab = 0, QWidget *parent = 0);
		virtual ~ValuePlotterWidget();

		ValuePlotter* getValuePlotter() const;

		void setPlotterRange(int range);
		void setTriggerEventName(const QString &eventName);
		void hideHandle(bool hide);

	signals:
		void updateSupportedTriggerEvents();
		void setPlotterRange(const QString&);

	public slots:
		void showWidget();
		void plotterRangeChanged();
		void plotterScaleVChanged(double scale);
		void plotterOffsetVChanged(double offset);
		void changeScaleV();
		void changeOffsetV();
		void staticScaleVChanged(double scale);
		void staticScaleHChanged(double scale);
		void staticOffsetVChanged(double offset);
		void staticOffsetHChanged(double offset);
		void changeStaticScaleV();
		void changeStaticScaleH();
		void changeStaticOffsetV();
		void changeStaticOffsetH();
		void loadStaticDataButtonPressed();
		void plotterItemCollectionChanged();
		void showLegendCheckBoxChanged(bool checked);
		void enableLoadStaticDataButton();
		void supportedTriggerEventsChanged();
		void triggerEventSelected(int index);
		void selectAllItems(int state);
		void clearHistory();

	protected:
		ValuePlotter *mValuePlotter;
		QPushButton *mLoadStaticValuesButton;
		QPushButton *mShowStaticValuesButton;
		QLineEdit *mHistorySizeField;
		QLineEdit *mVerticalOffset;
		QLineEdit *mVerticalScale;
		QLineEdit *mStaticOffsetV;
		QLineEdit *mStaticOffsetH;
		QLineEdit *mStaticScaleV;
		QLineEdit *mStaticScaleH;
		QCheckBox *mShowStaticValues;
		QScrollArea *mPlotterItemsScroller;
		QVBoxLayout *mPlotterItemScrollerLayout;
		QVector<PlotterItemControlPanel*> mPlotterItemPanels;
		QCheckBox *mShowLegendCheckBox;
		StaticDataLoader *mStaticDataLoader;
		QComboBox *mUpdateEventSelector;
		QTabWidget *mControlArea;
		QCheckBox *mSelectAllCheckBox;
		QSplitter *mSplitter;
		QPushButton *mClearHistoryButton;
	};

}

#endif


