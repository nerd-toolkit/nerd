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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#ifndef NERDInternalDiagramPlotterMainWidget_H
#define NERDInternalDiagramPlotterMainWidget_H

#include <QWidget>
#include "Gui/InternalDiagramPlotter/InternalDiagramPlotterWidget.h"
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
	 * InternalDiagramPlotterMainWidget
	 */
	class InternalDiagramPlotterMainWidget : public QWidget {
	Q_OBJECT
	public:
		InternalDiagramPlotterMainWidget(const QString &name, int activeTab = 0, QWidget *parent = 0);
		virtual ~InternalDiagramPlotterMainWidget();

		InternalDiagramPlotterWidget* getDiagramPlotter() const;

		void setTriggerEventName(const QString &eventName);

// 	signals:

	public slots:
		void showWidget();
// 		void plotterRangeChanged();
// 		void plotterScaleVChanged(double scale);
// 		void plotterOffsetVChanged(double offset);
// 		void changeScaleV();
// 		void changeOffsetV();
// 		void staticScaleVChanged(double scale);
// 		void staticScaleHChanged(double scale);
// 		void staticOffsetVChanged(double offset);
// 		void staticOffsetHChanged(double offset);
// 		void changeStaticScaleV();
// 		void changeStaticScaleH();
// 		void changeStaticOffsetV();
// 		void changeStaticOffsetH();
// 		void loadStaticDataButtonPressed();
// 		void plotterItemCollectionChanged();
// 		void showLegendCheckBoxChanged(bool checked);
// 		void useOpaqueLegendCheckBoxChanged(bool checked);
// 		void diagramModeCheckBoxChanged(bool checked);
// 		void plotValuesInLegendCheckBoxChanged(bool checked);
// 		//void showDiagramLinesCheckBoxChanged(bool checked);
// 		void diagramLineDashPatternEditChanged();
// 		void diagramLineWidthEditChanged();
// 		void diagramLineColorEditChanged();
// 		void saveDiagramToSvg();
// 		void legendOffsetEditChanged();
// 		void tickIntervalEditChanged();
// 		void diagramShiftEditChanged();
// 		void enableLoadStaticDataButton();
// 		void supportedTriggerEventsChanged();
// 		void triggerEventSelected(int index);
// 		void selectAllItems(int state);
// 		void clearHistory();
// 		void dialogSizeChanged();

	protected:
		InternalDiagramPlotterWidget *mDiagramPlotter;
		QSplitter *mSplitter;
		QTabWidget *mControlArea;
// 		QPushButton *mLoadStaticValuesButton;
// 		QPushButton *mShowStaticValuesButton;
// 		QPushButton *mSaveGraphButton;
// 		QLineEdit *mHistorySizeField;
// 		QLineEdit *mVerticalOffset;
// 		QLineEdit *mVerticalScale;
// 		QLineEdit *mStaticOffsetV;
// 		QLineEdit *mStaticOffsetH;
// 		QLineEdit *mStaticScaleV;
// 		QLineEdit *mStaticScaleH;
// 		QCheckBox *mShowStaticValues;
// 		QScrollArea *mPlotterItemsScroller;
// 		QVBoxLayout *mPlotterItemScrollerLayout;
// 		QVector<PlotterItemControlPanel*> mPlotterItemPanels;
// 		QCheckBox *mShowLegendCheckBox;
// 		QCheckBox *mUseOpaqueLegendCheckBox;
// 		QCheckBox *mDiagramModeCheckBox;
// 		QCheckBox *mPlotValuesInLegendCheckBox;
// 		//QCheckBox *mShowDiagramLinesCheckBox;
// 		QLineEdit *mDiagramLineDashPattern;
// 		QLineEdit *mDiagramLineWidthEdit;
// 		QLineEdit *mDiagramLineColorEdit;
// 		QLineEdit *mLegendBoxOverRideEdit;
// 		QLineEdit *mDiagramMajorTickIntervalEdit;
// 		QLineEdit *mDiagramMinorTickIntervalEdit;
// 		QLineEdit *mDiagramShiftEdit;
// 		StaticDataLoader *mStaticDataLoader;
// 		QComboBox *mUpdateEventSelector;
// 		QCheckBox *mSelectAllCheckBox;
// 		QPushButton *mClearHistoryButton;
// 		QLineEdit *mDialogSizeEdit;
	};

}

#endif


