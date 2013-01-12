

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



#include "InternalDiagramPlotterMainWidget.h" 
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QSplitter>
#include <QGroupBox>
#include "Core/Core.h"
#include <QFileDialog>
#include <iostream>
#include "Gui/ValuePlotter/StaticDataLoader.h"
#include <QMapIterator>
#include <QCoreApplication>
#include "Math/Math.h"
#include <QLabel>
#include "Value/ColorValue.h"

using namespace std;

namespace nerd {

//TODO initialize all members.
InternalDiagramPlotterMainWidget::InternalDiagramPlotterMainWidget(const QString &name, int activeTab, QWidget *parent)
	: QWidget(parent)
{
	moveToThread(QCoreApplication::instance()->thread());

	QString windowName = QString(name);
	setWindowTitle(windowName);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	setLayout(mainLayout);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);

	mSplitter = new QSplitter(this);
	mainLayout->addWidget(mSplitter);

	mDiagramPlotter = new InternalDiagramPlotterWidget();
	mDiagramPlotter->show();
	mSplitter->addWidget(mDiagramPlotter);

	
	mControlArea = new QTabWidget(mSplitter);
	mControlArea->setMinimumSize(0, 0);
	mControlArea->resize(50, 100);
	
	
// 	QScrollArea *mainScrollArea = new QScrollArea(mSplitter);
// 	mainScrollArea->setWidgetResizable(true);
// 	mainScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// 	mainScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
// 	mainScrollArea->setMinimumWidth(200);
// 	mainScrollArea->setWidget(mControlArea);
// 	mSplitter->addWidget(mainScrollArea);
// 	
// 	
// 	//mSplitter->addWidget(mControlArea);
// 	
// 
// 	mSplitter->setStretchFactor(0, 100);
// 	mSplitter->setStretchFactor(1, 1);
// 
// 	//create general tab
// 	QWidget *generalTab = new QWidget(mControlArea);
// 	QVBoxLayout *generalLayout = new QVBoxLayout();
// 	generalLayout->setMargin(2);
// 	generalLayout->setSpacing(1);
// 	generalTab->setLayout(generalLayout);
// 
// 	QHBoxLayout *historyRangeLayout = new QHBoxLayout();
// 	historyRangeLayout->setMargin(2);
// 	QLabel *rangeLabel = new QLabel("Range", generalTab);
// 	rangeLabel->setMinimumWidth(65);
// 	historyRangeLayout->addWidget(rangeLabel);
// 
// 	mHistorySizeField = new QLineEdit(QString::number(mValuePlotter->getPlotterWidget()
// 						->getNumberOfValuesToPlot()), generalTab);
// 	historyRangeLayout->addWidget(mHistorySizeField);
// 	generalLayout->addLayout(historyRangeLayout);
// 
// 	QHBoxLayout *historyOffsetLayout = new QHBoxLayout();
// 	historyOffsetLayout->setMargin(2);
// 	QLabel *offsetLabel = new QLabel("Y Offset", generalTab);
// 	offsetLabel->setMinimumWidth(65);
// 	mVerticalOffset = new QLineEdit("0", generalTab);
// 	historyOffsetLayout->addWidget(offsetLabel);
// 	historyOffsetLayout->addWidget(mVerticalOffset);
// 	generalLayout->addLayout(historyOffsetLayout);
// 	
// 	QHBoxLayout *historyScaleLayout = new QHBoxLayout();
// 	historyScaleLayout->setMargin(2);
// 	QLabel *scaleLabel = new QLabel("Y Scaling", generalTab);
// 	scaleLabel->setMinimumWidth(65);
// 	mVerticalScale = new QLineEdit("1.0", generalTab);
// 	historyScaleLayout->addWidget(scaleLabel);
// 	historyScaleLayout->addWidget(mVerticalScale);
// 	generalLayout->addLayout(historyScaleLayout);
// 
// // 	QGroupBox *scaleBox = new QGroupBox("Vertical Scaling", generalTab);
// // 	QVBoxLayout *scaleLayout = new QVBoxLayout(scaleBox);
// // 	scaleLayout->setMargin(2);
// // 	scaleBox->setLayout(scaleLayout);
// // 	mVerticalScale = new QLineEdit("1.0", scaleBox);
// // 	scaleLayout->addWidget(mVerticalScale);
// // 	generalLayout->addWidget(scaleBox);
// 	
// 	mSaveGraphButton = new QPushButton("Save Graph");
// 	generalLayout->addWidget(mSaveGraphButton);
// 	
// 	generalLayout->addStretch(100);
// 
// 	mControlArea->addTab(generalTab, "General");
// 
// 	//create value tab
// 	QWidget *valueTab = new QWidget(mControlArea);
// 	QVBoxLayout *valueLayout = new QVBoxLayout(valueTab);
// 	valueLayout->setMargin(2);
// 	valueTab->setLayout(valueLayout);
// 
// 	mPlotterItemsScroller = new QScrollArea(valueTab);
// 	mPlotterItemsScroller->setWidgetResizable(true);
// 	valueLayout->addWidget(mPlotterItemsScroller);
// 	QWidget *container = new QWidget();
// 	mPlotterItemsScroller->setWidget(container);
// 	mPlotterItemScrollerLayout = new QVBoxLayout(container);
// 	container->setLayout(mPlotterItemScrollerLayout);
// 	mPlotterItemScrollerLayout->setMargin(2);
// 	mPlotterItemScrollerLayout->setSpacing(2);
// 	mPlotterItemScrollerLayout->addStretch();
// 	mSelectAllCheckBox = new QCheckBox("Select all", valueTab);
// 	mSelectAllCheckBox->setChecked(true);
// 	valueLayout->addWidget(mSelectAllCheckBox);
// 	
// 
// 	mControlArea->addTab(valueTab, "Values");
// 
// 
// 	//create the offline tab
// 	QWidget *offlineTab = new QWidget(mControlArea);
// 	QVBoxLayout *offlineLayout = new QVBoxLayout(offlineTab);
// 	offlineLayout->setMargin(2);
// 	offlineTab->setLayout(offlineLayout);
// 
// 	mLoadStaticValuesButton = new QPushButton("Load Offline Data", offlineTab);
// 	offlineLayout->addWidget(mLoadStaticValuesButton);
// 
// 	QHBoxLayout *staticOffsetLayout = new QHBoxLayout();
// 	staticOffsetLayout->setMargin(2);
// 	QLabel *staticOffsetLabel = new QLabel("X/Y Offset", offlineTab);
// 	staticOffsetLabel->setMinimumWidth(75);
// 	mStaticOffsetV = new QLineEdit("0", offlineTab);
// 	mStaticOffsetH = new QLineEdit("0", offlineTab);
// 	staticOffsetLayout->addWidget(staticOffsetLabel);
// 	staticOffsetLayout->addWidget(mStaticOffsetV);
// 	staticOffsetLayout->addWidget(mStaticOffsetH);
// 	offlineLayout->addLayout(staticOffsetLayout);
// 
// 	QHBoxLayout *staticScaleLayout = new QHBoxLayout();
// 	staticScaleLayout->setMargin(2);
// 	QLabel *staticScaleLabel = new QLabel("X/Y Scale", offlineTab);
// 	staticScaleLabel->setMinimumWidth(75);
// 	mStaticScaleV = new QLineEdit("1.0", offlineTab);
// 	mStaticScaleH = new QLineEdit("1.0", offlineTab);
// 	staticScaleLayout->addWidget(staticScaleLabel);
// 	staticScaleLayout->addWidget(mStaticScaleV);
// 	staticScaleLayout->addWidget(mStaticScaleH);
// 	offlineLayout->addLayout(staticScaleLayout);
// 
// 	offlineLayout->addStretch(100);
// 
// 	mShowStaticValues = new QCheckBox("Show Offline Data", offlineTab);
// 	mShowStaticValues->setChecked(true);
// 	offlineLayout->addWidget(mShowStaticValues);
// 
// 
// 	mControlArea->addTab(offlineTab, "Offline");
// 	
// 	//Create Diagram tab
// 	QWidget *diagramTab = new QWidget(mControlArea);
// 	QVBoxLayout *diagramLayout = new QVBoxLayout(diagramTab);
// 	diagramLayout->setMargin(2);
// 	diagramTab->setLayout(diagramLayout);
// 	
// 	QHBoxLayout *legendLayout = new QHBoxLayout();
// 	mShowLegendCheckBox = new QCheckBox("Legend", diagramTab);
// 	mShowLegendCheckBox->setChecked(true);
// 	legendLayout->addWidget(mShowLegendCheckBox);
// 	
// 	mUseOpaqueLegendCheckBox = new QCheckBox("Opaque", diagramTab);
// 	mUseOpaqueLegendCheckBox->setChecked(mValuePlotter->getPlotterWidget()->isSolidLegend());
// 	legendLayout->addWidget(mUseOpaqueLegendCheckBox);
// 	diagramLayout->addLayout(legendLayout);
// 	
// 	QHBoxLayout *diagramConfigLayout = new QHBoxLayout();
// 	mDiagramModeCheckBox = new QCheckBox("Diagram", diagramTab);
// 	mDiagramModeCheckBox->setChecked(mValuePlotter->getPlotterWidget()->isInDiagramMode());
// 	diagramConfigLayout->addWidget(mDiagramModeCheckBox);
// 	
// 	mPlotValuesInLegendCheckBox = new QCheckBox("Values", diagramTab);
// 	mPlotValuesInLegendCheckBox->setChecked(mValuePlotter->getPlotterWidget()->isShowingValuesInLegend());
// 	diagramConfigLayout->addWidget(mPlotValuesInLegendCheckBox);
// 	
// 	diagramLayout->addLayout(diagramConfigLayout);
// 	
// 	
// 	QLabel *dashPatternLabel = new QLabel("Line");
// 	mDiagramLineDashPattern = new QLineEdit("1,5", diagramTab);
// 	mDiagramLineDashPattern->setToolTip("The dash pattern used for the interval lines in the diagram.");
// 	mDiagramLineDashPattern->setFixedWidth(40);
// 	mDiagramLineWidthEdit = new QLineEdit("0", diagramTab);
// 	mDiagramLineWidthEdit->setToolTip("The line width. A 0 means cosmetic (always 1).");
// 	mDiagramLineWidthEdit->setFixedWidth(25);
// 	mDiagramLineColorEdit = new QLineEdit("black", diagramTab);
// 	mDiagramLineColorEdit->setToolTip("The color of the diagram lines.");
// 	QHBoxLayout *dashPatternLayout = new QHBoxLayout();
// 	dashPatternLayout->addWidget(dashPatternLabel);
// 	dashPatternLayout->addWidget(mDiagramLineWidthEdit);
// 	dashPatternLayout->addWidget(mDiagramLineDashPattern);
// 	dashPatternLayout->addWidget(mDiagramLineColorEdit);
// 	diagramLayout->addLayout(dashPatternLayout);
// 	
// // 	mShowDiagramLinesCheckBox = new QCheckBox("Show Diagram Lines", diagramTab);
// // 	mShowDiagramLinesCheckBox->setChecked(mValuePlotter->getPlotterWidget()->isShowingDiagramLines());
// // 	diagramLayout->addWidget(mShowDiagramLinesCheckBox);
// 	
// // 	QLabel *tickLabel = new QLabel("Ticks");
// 	mDiagramMajorTickIntervalEdit = new QLineEdit("");
// 	mDiagramMajorTickIntervalEdit->setText(QString::number(
// 							mValuePlotter->getPlotterWidget()->getMajorTickInterval()));
// 	mDiagramMajorTickIntervalEdit->setToolTip("Interval of large vertical lines (major ticks).");
// 	mDiagramMinorTickIntervalEdit = new QLineEdit("");
// 	mDiagramMinorTickIntervalEdit->setText(QString::number(
// 							mValuePlotter->getPlotterWidget()->getMinorTickInterval()));
// 	mDiagramMinorTickIntervalEdit->setToolTip("Number of minor ticks between two major ticks.");
// 	
// 	
// 	
// 	
// // 	QLabel *legendOverrideLabel = new QLabel("LB:");
// 	mLegendBoxOverRideEdit = new QLineEdit("");
// 	mLegendBoxOverRideEdit->setToolTip("Overrides the default size and position of the legend box.\n"
// 										"Format: shiftX,shiftY or shiftX,shiftY,width,height");
// 	
// // 	QLabel *diagramShiftLabel = new QLabel("Shift.");
// 	mDiagramShiftEdit = new QLineEdit("0");
// 	mDiagramShiftEdit->setToolTip("Shift entire diagram to match the front header.");
// 	
// 	QHBoxLayout *tickIntervalLayout = new QHBoxLayout();
// // 	tickIntervalLayout->addWidget(tickLabel);
// 	tickIntervalLayout->addWidget(mDiagramMajorTickIntervalEdit);
// 	tickIntervalLayout->addWidget(mDiagramMinorTickIntervalEdit);
// 	
// // 	tickIntervalLayout->addWidget(legendOverrideLabel);
// 	tickIntervalLayout->addWidget(mLegendBoxOverRideEdit);
// // 	tickIntervalLayout->addWidget(diagramShiftLabel);
// 	tickIntervalLayout->addWidget(mDiagramShiftEdit);
// 	diagramLayout->addLayout(tickIntervalLayout);
// 	
// // 	QHBoxLayout *legendOffsetLayout = new QHBoxLayout();
// // 	legendOffsetLayout->addWidget(legendOverrideLabel);
// // 	legendOffsetLayout->addWidget(mLegendBoxOverRideEdit);
// // 	legendOffsetLayout->addWidget(diagramShiftLabel);
// // 	legendOffsetLayout->addWidget(mDiagramShiftEdit);
// 	
// // 	diagramLayout->addLayout(legendOffsetLayout);
// 
// 	diagramLayout->addStretch();
// 
// 	mControlArea->addTab(diagramTab, "Diagram");
// 	
// 	
// 	//Create special tab
// 	QWidget *specialTab = new QWidget(mControlArea);
// 	QVBoxLayout *specialLayout = new QVBoxLayout(specialTab);
// 	specialLayout->setMargin(2);
// 	specialTab->setLayout(specialLayout);
// 
// 	QGroupBox *updateEventSelectorBox = new QGroupBox("Plotter Update Event", specialTab);
// 	mUpdateEventSelector = new QComboBox(updateEventSelectorBox);
// 	mUpdateEventSelector->setEditable(false);
// 	QHBoxLayout *updateEventSelectorLayout = new QHBoxLayout();
// 	updateEventSelectorLayout->setMargin(2);
// 	updateEventSelectorBox->setLayout(updateEventSelectorLayout);
// 	updateEventSelectorLayout->addWidget(mUpdateEventSelector);
// 	specialLayout->addWidget(updateEventSelectorBox);
// 
// 	mClearHistoryButton = new QPushButton("Clear History");
// 	specialLayout->addWidget(mClearHistoryButton);
// 	
// 	QLabel *dialogSizeLabel = new QLabel("Set Dialog Size");
// 	mDialogSizeEdit = new QLineEdit("");
// 	mDialogSizeEdit->setToolTip("Change the size of the plotter to width,height.");
// 	
// 	QHBoxLayout *dialogSizeLayout = new QHBoxLayout();
// 	dialogSizeLayout->addWidget(dialogSizeLabel);
// 	dialogSizeLayout->addWidget(mDialogSizeEdit);
// 	
// 	specialLayout->addLayout(dialogSizeLayout);
// 
// 	specialLayout->addStretch();
// 
// 	mControlArea->addTab(specialTab, "Special");
// 
// 
// 	mControlArea->setCurrentIndex(activeTab);
// 
// 	connect(mHistorySizeField, SIGNAL(returnPressed()),
// 			this, SLOT(plotterRangeChanged()));
// 	connect(this, SIGNAL(setPlotterRange(const QString&)),
// 			mHistorySizeField, SLOT(setText(const QString&)));
// 	connect(mValuePlotter->getPlotterWidget(), SIGNAL(userScaleVChanged(double)),
// 			this, SLOT(plotterScaleVChanged(double)));
// 	connect(mValuePlotter->getPlotterWidget(), SIGNAL(userOffsetVChanged(double)),
// 			this, SLOT(plotterOffsetVChanged(double)));
// 	connect(mVerticalOffset, SIGNAL(returnPressed()),
// 			this, SLOT(changeOffsetV()));
// 	connect(mVerticalScale, SIGNAL(returnPressed()),
// 			this, SLOT(changeScaleV()));
// 	connect(mSaveGraphButton, SIGNAL(pressed()),
// 			this,SLOT(saveDiagramToSvg()));
// 
// 	connect(mStaticScaleV, SIGNAL(returnPressed()),
// 			this, SLOT(changeStaticScaleV()));
// 	connect(mStaticScaleH, SIGNAL(returnPressed()),
// 			this, SLOT(changeStaticScaleH()));
// 	connect(mStaticOffsetV, SIGNAL(returnPressed()),
// 			this, SLOT(changeStaticOffsetV()));
// 	connect(mStaticOffsetH, SIGNAL(returnPressed()),
// 			this, SLOT(changeStaticOffsetH()));
// 
// 	connect(mSelectAllCheckBox, SIGNAL(stateChanged(int)),
// 			this, SLOT(selectAllItems(int)));
// 
// 	connect(mValuePlotter, SIGNAL(staticOffsetHChanged(double)),
// 			this, SLOT(staticOffsetHChanged(double)));
// 	connect(mValuePlotter, SIGNAL(staticOffsetVChanged(double)),
// 			this, SLOT(staticOffsetVChanged(double)));
// 	connect(mValuePlotter, SIGNAL(staticScaleHChanged(double)),
// 			this, SLOT(staticScaleHChanged(double)));
// 	connect(mValuePlotter, SIGNAL(staticScaleVChanged(double)),
// 			this, SLOT(staticScaleVChanged(double)));
// 	connect(mValuePlotter, SIGNAL(triggerEventSupportChanged()),
// 			this, SLOT(supportedTriggerEventsChanged()));
// 
// 	connect(mShowStaticValues, SIGNAL(toggled(bool)),
// 			mValuePlotter, SLOT(showStaticData(bool)));
// 	connect(mValuePlotter, SIGNAL(staticDataVisibilityChanged(bool)),
// 			mShowStaticValues, SLOT(setChecked(bool)));
// 
// 	connect(mLoadStaticValuesButton, SIGNAL(pressed()),
// 			this, SLOT(loadStaticDataButtonPressed()));
// 	connect(mValuePlotter->getPlotterWidget(), SIGNAL(plotterItemCollectionChanged()),
// 			this, SLOT(plotterItemCollectionChanged()));
// 
// 	connect(mShowLegendCheckBox, SIGNAL(toggled(bool)),
// 			this, SLOT(showLegendCheckBoxChanged(bool)));
// 	connect(mUseOpaqueLegendCheckBox, SIGNAL(toggled(bool)),
// 			this, SLOT(useOpaqueLegendCheckBoxChanged(bool)));
// 	connect(mDiagramModeCheckBox, SIGNAL(toggled(bool)),
// 			this, SLOT(diagramModeCheckBoxChanged(bool)));
// 	connect(mPlotValuesInLegendCheckBox, SIGNAL(toggled(bool)),
// 			this, SLOT(plotValuesInLegendCheckBoxChanged(bool)));
// 	connect(mLegendBoxOverRideEdit, SIGNAL(returnPressed()),
// 			this, SLOT(legendOffsetEditChanged()));
// 	connect(mDiagramMajorTickIntervalEdit, SIGNAL(returnPressed()),
// 			this, SLOT(tickIntervalEditChanged()));
// 	connect(mDiagramMinorTickIntervalEdit, SIGNAL(returnPressed()),
// 			this, SLOT(tickIntervalEditChanged()));
// // 	connect(mShowDiagramLinesCheckBox, SIGNAL(toggled(bool)),
// // 			this, SLOT(showDiagramLinesCheckBoxChanged(bool)));
// 	connect(mDiagramLineDashPattern, SIGNAL(returnPressed()),
// 			this, SLOT(diagramLineDashPatternEditChanged()));
// 	connect(mDiagramLineWidthEdit, SIGNAL(returnPressed()),
// 			this, SLOT(diagramLineWidthEditChanged()));
// 	connect(mDiagramShiftEdit, SIGNAL(returnPressed()),
// 			this, SLOT(diagramShiftEditChanged()));
// 	connect(mDiagramLineColorEdit, SIGNAL(returnPressed()),
// 			this, SLOT(diagramLineColorEditChanged()));
// 
// 	connect(mUpdateEventSelector, SIGNAL(activated(int)), 
// 			this, SLOT(triggerEventSelected(int)));
// 
// 	connect(this, SIGNAL(updateSupportedTriggerEvents()),
// 			this, SLOT(supportedTriggerEventsChanged()), Qt::QueuedConnection);
// 	connect(mClearHistoryButton, SIGNAL(pressed()),
// 			this, SLOT(clearHistory()));
// 	connect(mDialogSizeEdit,SIGNAL(returnPressed()),
// 			this, SLOT(dialogSizeChanged()));
// 
// 	generalLayout->addStretch(100);
// 	
// 	//trigger an update of the dash pattern and line width.
// 	diagramLineDashPatternEditChanged();
// 	diagramLineWidthEditChanged();
// 	diagramLineColorEditChanged();

	resize(542, 200);

}

InternalDiagramPlotterMainWidget::~InternalDiagramPlotterMainWidget() {
	delete mDiagramPlotter;
}


void InternalDiagramPlotterMainWidget::showWidget() {
// 	mDiagramPlotter->showPlotter();
	show();
}

InternalDiagramPlotterWidget* InternalDiagramPlotterMainWidget::getDiagramPlotter() const {
	return mDiagramPlotter;
}




}



