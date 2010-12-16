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



#include "ValuePlotterWidget.h" 
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

using namespace std;

namespace nerd {

//TODO initialize all members.
ValuePlotterWidget::ValuePlotterWidget(const QString &name, int activeTab, QWidget *parent)
	: QWidget(parent), mStaticDataLoader(0), mControlArea(0)
{
	moveToThread(QCoreApplication::instance()->thread());

	QString windowName = QString("Value Plotter (").append(name).append(")");
	setWindowTitle(windowName);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	setLayout(mainLayout);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);

	mSplitter = new QSplitter(this);
	mainLayout->addWidget(mSplitter);

	mValuePlotter = new ValuePlotter(windowName);
	mValuePlotter->show();
	mSplitter->addWidget(mValuePlotter);

	mControlArea = new QTabWidget(mSplitter);
	mSplitter->addWidget(mControlArea);
	mControlArea->setMinimumSize(0, 0);
	mControlArea->resize(50, 100);

	mSplitter->setStretchFactor(0, 100);
	mSplitter->setStretchFactor(1, 1);

	//create general tab
	QWidget *generalTab = new QWidget(mControlArea);
	QVBoxLayout *generalLayout = new QVBoxLayout(generalTab);
	generalLayout->setMargin(2);
	generalTab->setLayout(generalLayout);

	QGroupBox *historyBox = new QGroupBox("Plotter Range", generalTab);
	QVBoxLayout *historySizeLayout = new QVBoxLayout(historyBox);
	historySizeLayout->setMargin(2);
	historyBox->setLayout(historySizeLayout);

	mHistorySizeField = new QLineEdit(QString::number(mValuePlotter->getPlotterWidget()
						->getNumberOfValuesToPlot()), historyBox);
	historySizeLayout->addWidget(mHistorySizeField);
	generalLayout->addWidget(historyBox);

	QGroupBox *offsetBox = new QGroupBox("Vertical Offset", generalTab);
	QVBoxLayout *offsetLayout = new QVBoxLayout(offsetBox);
	offsetLayout->setMargin(2);
	offsetBox->setLayout(offsetLayout);
	mVerticalOffset = new QLineEdit("0", offsetBox);
	offsetLayout->addWidget(mVerticalOffset);
	generalLayout->addWidget(offsetBox);

	QGroupBox *scaleBox = new QGroupBox("Vertical Scaling", generalTab);
	QVBoxLayout *scaleLayout = new QVBoxLayout(scaleBox);
	scaleLayout->setMargin(2);
	scaleBox->setLayout(scaleLayout);
	mVerticalScale = new QLineEdit("1.0", scaleBox);
	scaleLayout->addWidget(mVerticalScale);
	generalLayout->addWidget(scaleBox);

	mControlArea->addTab(generalTab, "General");

	//create value tab
	QWidget *valueTab = new QWidget(mControlArea);
	QVBoxLayout *valueLayout = new QVBoxLayout(valueTab);
	valueLayout->setMargin(2);
	valueTab->setLayout(valueLayout);

	mPlotterItemsScroller = new QScrollArea(valueTab);
	mPlotterItemsScroller->setWidgetResizable(true);
	valueLayout->addWidget(mPlotterItemsScroller);
	QWidget *container = new QWidget();
	mPlotterItemsScroller->setWidget(container);
	mPlotterItemScrollerLayout = new QVBoxLayout(container);
	container->setLayout(mPlotterItemScrollerLayout);
	mPlotterItemScrollerLayout->setMargin(2);
	mPlotterItemScrollerLayout->setSpacing(2);
	mPlotterItemScrollerLayout->addStretch();
	mSelectAllCheckBox = new QCheckBox("Select all", valueTab);
	mSelectAllCheckBox->setChecked(true);
	valueLayout->addWidget(mSelectAllCheckBox);
	

	mControlArea->addTab(valueTab, "Values");


	//create the offline tab
	QWidget *offlineTab = new QWidget(mControlArea);
	QVBoxLayout *offlineLayout = new QVBoxLayout(offlineTab);
	offlineLayout->setMargin(2);
	offlineTab->setLayout(offlineLayout);

	mLoadStaticValuesButton = new QPushButton("Load Offline Data", offlineTab);
	offlineLayout->addWidget(mLoadStaticValuesButton);

	QGroupBox *staticOffsetBox = new QGroupBox("Vert. / Hori. Offset", offlineTab);
	mStaticOffsetV = new QLineEdit("0", staticOffsetBox);
	mStaticOffsetH = new QLineEdit("0", staticOffsetBox);
	QHBoxLayout *staticOffsetLayout = new QHBoxLayout(staticOffsetBox);
	staticOffsetLayout->setMargin(2);
	staticOffsetBox->setLayout(staticOffsetLayout);
	staticOffsetLayout->addWidget(mStaticOffsetV);
	staticOffsetLayout->addWidget(mStaticOffsetH);
	offlineLayout->addWidget(staticOffsetBox);

	QGroupBox *staticScaleBox = new QGroupBox("Vert. / Hori. Scale", offlineTab);
	mStaticScaleV = new QLineEdit("1.0", staticScaleBox);
	mStaticScaleH = new QLineEdit("1.0", staticScaleBox);
	QHBoxLayout *staticScaleLayout = new QHBoxLayout(staticScaleBox);
	staticScaleLayout->setMargin(2);
	staticScaleBox->setLayout(staticScaleLayout);
	staticScaleLayout->addWidget(mStaticScaleV);
	staticScaleLayout->addWidget(mStaticScaleH);
	offlineLayout->addWidget(staticScaleBox);

	offlineLayout->addStretch(100);

	mShowStaticValues = new QCheckBox("Show Offline Data", offlineTab);
	mShowStaticValues->setChecked(true);
	offlineLayout->addWidget(mShowStaticValues);


	mControlArea->addTab(offlineTab, "Offline");

	//Create special tab
	QWidget *specialTab = new QWidget(mControlArea);
	QVBoxLayout *specialLayout = new QVBoxLayout(specialTab);
	specialLayout->setMargin(2);
	specialTab->setLayout(specialLayout);

	mShowLegendCheckBox = new QCheckBox("Show Legend", specialTab);
	mShowLegendCheckBox->setChecked(true);
	specialLayout->addWidget(mShowLegendCheckBox);

	QGroupBox *updateEventSelectorBox = new QGroupBox("Plotter Update Event", specialTab);
	mUpdateEventSelector = new QComboBox(updateEventSelectorBox);
	mUpdateEventSelector->setEditable(false);
	QHBoxLayout *updateEventSelectorLayout = new QHBoxLayout();
	updateEventSelectorLayout->setMargin(2);
	updateEventSelectorBox->setLayout(updateEventSelectorLayout);
	updateEventSelectorLayout->addWidget(mUpdateEventSelector);
	specialLayout->addWidget(updateEventSelectorBox);

	mClearHistoryButton = new QPushButton("Clear History");
	specialLayout->addWidget(mClearHistoryButton);
	

	specialLayout->addStretch();

	mControlArea->addTab(specialTab, "Special");


	mControlArea->setCurrentIndex(activeTab);

	connect(mHistorySizeField, SIGNAL(returnPressed()),
			this, SLOT(plotterRangeChanged()));
	connect(this, SIGNAL(setPlotterRange(const QString&)),
			mHistorySizeField, SLOT(setText(const QString&)));
	connect(mValuePlotter->getPlotterWidget(), SIGNAL(userScaleVChanged(double)),
			this, SLOT(plotterScaleVChanged(double)));
	connect(mValuePlotter->getPlotterWidget(), SIGNAL(userOffsetVChanged(double)),
			this, SLOT(plotterOffsetVChanged(double)));
	connect(mVerticalOffset, SIGNAL(returnPressed()),
			this, SLOT(changeOffsetV()));
	connect(mVerticalScale, SIGNAL(returnPressed()),
			this, SLOT(changeScaleV()));

	connect(mStaticScaleV, SIGNAL(returnPressed()),
			this, SLOT(changeStaticScaleV()));
	connect(mStaticScaleH, SIGNAL(returnPressed()),
			this, SLOT(changeStaticScaleH()));
	connect(mStaticOffsetV, SIGNAL(returnPressed()),
			this, SLOT(changeStaticOffsetV()));
	connect(mStaticOffsetH, SIGNAL(returnPressed()),
			this, SLOT(changeStaticOffsetH()));

	connect(mSelectAllCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(selectAllItems(int)));

	connect(mValuePlotter, SIGNAL(staticOffsetHChanged(double)),
			this, SLOT(staticOffsetHChanged(double)));
	connect(mValuePlotter, SIGNAL(staticOffsetVChanged(double)),
			this, SLOT(staticOffsetVChanged(double)));
	connect(mValuePlotter, SIGNAL(staticScaleHChanged(double)),
			this, SLOT(staticScaleHChanged(double)));
	connect(mValuePlotter, SIGNAL(staticScaleVChanged(double)),
			this, SLOT(staticScaleVChanged(double)));
	connect(mValuePlotter, SIGNAL(triggerEventSupportChanged()),
			this, SLOT(supportedTriggerEventsChanged()));

	connect(mShowStaticValues, SIGNAL(toggled(bool)),
			mValuePlotter, SLOT(showStaticData(bool)));
	connect(mValuePlotter, SIGNAL(staticDataVisibilityChanged(bool)),
			mShowStaticValues, SLOT(setChecked(bool)));

	connect(mLoadStaticValuesButton, SIGNAL(pressed()),
			this, SLOT(loadStaticDataButtonPressed()));
	connect(mValuePlotter->getPlotterWidget(), SIGNAL(plotterItemCollectionChanged()),
			this, SLOT(plotterItemCollectionChanged()));

	connect(mShowLegendCheckBox, SIGNAL(toggled(bool)),
			this, SLOT(showLegendCheckBoxChanged(bool)));

	connect(mUpdateEventSelector, SIGNAL(activated(int)), 
			this, SLOT(triggerEventSelected(int)));

	connect(this, SIGNAL(updateSupportedTriggerEvents()),
			this, SLOT(supportedTriggerEventsChanged()), Qt::QueuedConnection);
	connect(mClearHistoryButton, SIGNAL(pressed()),
			this, SLOT(clearHistory()));

	generalLayout->addStretch(100);

	resize(542, 200);

}

ValuePlotterWidget::~ValuePlotterWidget() {
	if(mStaticDataLoader != 0) {
		mStaticDataLoader->stopLoader();
		mStaticDataLoader->wait();
		delete mStaticDataLoader;
	}
	delete mValuePlotter;
}


void ValuePlotterWidget::showWidget() {
	mValuePlotter->showPlotter();
	show();
}

ValuePlotter* ValuePlotterWidget::getValuePlotter() const {
	return mValuePlotter;
}


void ValuePlotterWidget::setPlotterRange(int range) {
	mValuePlotter->getPlotterWidget()->setNumberOfValuesToPlot(range);
	mValuePlotter->getPlotterWidget()->setHistoryCapacity(range);

	emit setPlotterRange(QString::number(
				mValuePlotter->getPlotterWidget()->getNumberOfValuesToPlot()));
}


void ValuePlotterWidget::setTriggerEventName(const QString &eventName) {

	if(eventName == "None") {
		mValuePlotter->setUpdateTriggerEvent(0);
		emit updateSupportedTriggerEvents();
		return;
	}

	QMap<Event*, QString> triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();
	int index = 1; 
	for(QMapIterator<Event*, QString> i(triggerEvents); i.hasNext();) {
		i.next();
		if(i.value() == eventName) {
			mValuePlotter->setUpdateTriggerEvent(i.key());
			break;
		}
		index++;
	}

	emit updateSupportedTriggerEvents();
}

void ValuePlotterWidget::hideHandle(bool hide) {
	QList<int> sizes;
	if(hide) {
		sizes.append(width() + 10);
		sizes.append(0);
	}
	else {
		sizes.append(width() - 20);
		sizes.append(20);
	}
	mSplitter->setSizes(sizes);
}

void ValuePlotterWidget::plotterRangeChanged() {
	bool ok = true;
	int size = Math::max(2, mHistorySizeField->text().toInt(&ok));

	if(ok && size > 1) {
		mValuePlotter->getPlotterWidget()->setNumberOfValuesToPlot(size);
		mValuePlotter->getPlotterWidget()->setHistoryCapacity(size);
	}
	else {
		size = mValuePlotter->getPlotterWidget()->getNumberOfValuesToPlot();
	}
	if(QString::number(size) != mHistorySizeField->text()) {
		mHistorySizeField->setText(QString::number(size));
	}
}

void ValuePlotterWidget::plotterScaleVChanged(double scale) {
	mVerticalScale->setText(QString::number(scale));
}


void ValuePlotterWidget::plotterOffsetVChanged(double offset) {
	mVerticalOffset->setText(QString::number(offset));
}


void ValuePlotterWidget::changeScaleV() {
	bool ok = true;
	double scale = mVerticalScale->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->getPlotterWidget()->setUserScaleV(scale);
	}
	else {
		mVerticalScale->setText(QString::number(
				mValuePlotter->getPlotterWidget()->getUserScaleV()));
	}
}


void ValuePlotterWidget::changeOffsetV() {
	bool ok = true;
	double offset = mVerticalOffset->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->getPlotterWidget()->setUserOffsetV(offset);
	}
	else {
		mVerticalOffset->setText(QString::number(
				mValuePlotter->getPlotterWidget()->getUserOffsetV()));
	}
}


void ValuePlotterWidget::staticScaleVChanged(double scale) {
	mStaticScaleV->setText(QString::number(scale));
}


void ValuePlotterWidget::staticScaleHChanged(double scale) {
	mStaticScaleH->setText(QString::number(scale));
}


void ValuePlotterWidget::staticOffsetVChanged(double offset) {
	mStaticOffsetV->setText(QString::number(offset));
}


void ValuePlotterWidget::staticOffsetHChanged(double offset) {
	mStaticOffsetH->setText(QString::number(offset));
}


void ValuePlotterWidget::changeStaticScaleV() {
	bool ok = true;
	double scale = mStaticScaleV->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->setStaticDataScaleV(scale);
	}
}


void ValuePlotterWidget::changeStaticScaleH() {
	bool ok = true;
	double scale = mStaticScaleH->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->setStaticDataScaleH(scale);
	}
}


void ValuePlotterWidget::changeStaticOffsetV() {
	bool ok = true;
	double offset = mStaticOffsetV->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->setStaticDataOffsetV(offset);
	}
}


void ValuePlotterWidget::changeStaticOffsetH() {
	bool ok = true;
	double offset = mStaticOffsetH->text().toDouble(&ok);
	if(ok) {
		mValuePlotter->setStaticDataOffsetH(offset);
	}
}

void ValuePlotterWidget::loadStaticDataButtonPressed() {

	if(mStaticDataLoader != 0 && mStaticDataLoader->isRunning()) {
		return;
	}

	mLoadStaticValuesButton->setEnabled(false);

	Properties &props = Core::getInstance()->getProperties();

	QString fileChooserDirectory = ".";
	if(props.hasProperty("CurrentFileChooserPath")) {
		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
	}

	QString fileName = QFileDialog::getOpenFileName(this, tr("Load Settings"),
							fileChooserDirectory);

	if(fileName == "") {
		mLoadStaticValuesButton->setEnabled(true);
		return;
	}

	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);

	if(mStaticDataLoader != 0) {
		mStaticDataLoader->stopLoader();
		mStaticDataLoader->wait();
		delete mStaticDataLoader;
	}
	mStaticDataLoader = new StaticDataLoader(this, fileName, "#", ";");
	mStaticDataLoader->start();

	mLoadStaticValuesButton->setText("Loading Data...");

	//mValuePlotter->loadStaticData(fileName, "#", ";");
}

void ValuePlotterWidget::plotterItemCollectionChanged() {

	QList<PlotterItem*> items = mValuePlotter->getPlotterWidget()
			->getPlotterItems();

	QVector<PlotterItemControlPanel*> currentPanels = mPlotterItemPanels;
	mPlotterItemPanels.clear();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		bool found = false;
		for(int j = 0; j < currentPanels.size(); ++j) {
			PlotterItemControlPanel *panel = currentPanels.at(j);
			if(panel != 0 && panel->getPlotterItem() == item) {
				mPlotterItemPanels.append(panel);
				currentPanels.remove(j);
				found = true;
				break;
			}
		}
		if(!found) {
			PlotterItemControlPanel *panel = new PlotterItemControlPanel(item);
			mPlotterItemPanels.append(panel);
		}
	}

	//remove items from widget
 	while(mPlotterItemScrollerLayout->takeAt(0) != 0) {
	}

	//destroy all superflous panels
	for(int i = 0; i < currentPanels.size(); ++i) {
		delete currentPanels.at(i);
	}

	//update the widget
	for(int i = 0; i < mPlotterItemPanels.size(); ++i) {
		mPlotterItemScrollerLayout->addWidget(mPlotterItemPanels.at(i));
	}
	mPlotterItemScrollerLayout->addStretch();
}


void ValuePlotterWidget::showLegendCheckBoxChanged(bool) {
	mValuePlotter->getPlotterWidget()->enableLegend(mShowLegendCheckBox->isChecked());
}

void ValuePlotterWidget::enableLoadStaticDataButton() {
	mLoadStaticValuesButton->setText("Load Offline Data");
	mLoadStaticValuesButton->setEnabled(true);
}

void ValuePlotterWidget::supportedTriggerEventsChanged() {
	mUpdateEventSelector->clear();
	QMap<Event*, QString> triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();

	mUpdateEventSelector->addItem("None");
	mUpdateEventSelector->setCurrentIndex(0);

	int index = 1;
	for(QMapIterator<Event*, QString> i(triggerEvents); i.hasNext();) {
		i.next();
		mUpdateEventSelector->addItem(i.value());
		if(mValuePlotter->getUpdateTriggerEvent() == i.key()) {
			mUpdateEventSelector->setCurrentIndex(index);
		}
		++index;
	}
}


void ValuePlotterWidget::triggerEventSelected(int index) {
	QString eventName = mUpdateEventSelector->itemText(index);

	if(eventName == "None") {
		mValuePlotter->setUpdateTriggerEvent(0);
		return;
	}

	QMap<Event*, QString> triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();

	for(QMapIterator<Event*, QString> i(triggerEvents); i.hasNext();) {
		i.next();
		if(i.value() == eventName) {
			mValuePlotter->setUpdateTriggerEvent(i.key());
		}
	}
}

void ValuePlotterWidget::selectAllItems(int) {

	for(int i = 0; i < mPlotterItemPanels.size(); ++i) {
		mPlotterItemPanels.at(i)->visibilityCheckBoxChanged((int) mSelectAllCheckBox->isChecked());
	}
}


void ValuePlotterWidget::clearHistory() {
	if(mValuePlotter == 0 || mValuePlotter->getPlotterWidget() == 0) {
		return;
	}
	mValuePlotter->getPlotterWidget()->clearHistory();
}


}



