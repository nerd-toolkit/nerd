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



#include "FirstReturnMapPlotter.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QCoreApplication>
#include "Gui/FirstReturnMap/FirstReturnMapPlotter.h"
#include "Gui/FirstReturnMap/FirstReturnMapPlotterWidget.h"
#include <QGroupBox>
#include "NeuralNetworkConstants.h"
#include "NerdConstants.h"
#include "Network/Neuro.h"
#include "Event/EventManager.h"
#include "NeuralNetworkConstants.h"
#include "Network/NeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Util/Tracer.h"
#include "NerdConstants.h"
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QCheckBox>
#include "Gui/ValuePlotter/PlotterItemControlPanel.h"
#include <QVector>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QComboBox>
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new FirstReturnMapPlotter.
 */
FirstReturnMapPlotter::FirstReturnMapPlotter(const QString &name, int plotMode, 
		NeuralNetworkEditor *editor, int activeTab, QWidget *parent)
	: QWidget(parent), mEditor(editor), mPlotMode(plotMode),
	  mShutDownEvent(0), mNetworksReplacedEvent(0), mNetworksModifiedEvent(0)
{
	moveToThread(QCoreApplication::instance()->thread());

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentTabChanged(int)));
	}

	EventManager *em = Core::getInstance()->getEventManager();
	
	//register for network events
	mNetworksReplacedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORKS_REPLACED, this);
	mNetworksModifiedEvent = em->registerForEvent(
				NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);
	mShutDownEvent = em->registerForEvent(
				NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this);


	currentTabChanged(0);

	QString windowName = QString("First Return Map (").append(name).append(")");
	setWindowTitle(windowName);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);

	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	setLayout(mainLayout);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(1);

	mSplitter = new QSplitter(this);
	mainLayout->addWidget(mSplitter);

	mValuePlotter = new ValuePlotter(windowName, new FirstReturnMapPlotterWidget());
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

	QGroupBox *historyBox = new QGroupBox("History Size", generalTab);
	QVBoxLayout *historySizeLayout = new QVBoxLayout(historyBox);
	historySizeLayout->setMargin(2);
	historyBox->setLayout(historySizeLayout);
	
	mValuePlotter->getPlotterWidget()->setHistoryCapacity(10);
	mValuePlotter->getPlotterWidget()->setNumberOfValuesToPlot(10);
	mHistorySizeField = new QLineEdit(QString::number(mValuePlotter->getPlotterWidget()
						->getNumberOfValuesToPlot()), historyBox);
	historySizeLayout->addWidget(mHistorySizeField);
	generalLayout->addWidget(historyBox);

	mAddSelectedButton = new QPushButton("Add Selected", generalTab);
	generalLayout->addWidget(mAddSelectedButton);

	mRemoveNeuronsButton = new QPushButton("Remove All", generalTab);
	generalLayout->addWidget(mRemoveNeuronsButton);

	mClearHistoryButton = new QPushButton("Clear Hisory", generalTab);
	generalLayout->addWidget(mClearHistoryButton);

	generalLayout->addStretch();

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

	mLineWidthField = new QLineEdit("2.0");
	specialLayout->addWidget(mLineWidthField);

	mConnectLinesCheckBox = new QCheckBox("Connect Points");
	mConnectLinesCheckBox->setCheckable(true);
	mConnectLinesCheckBox->setChecked(false);
	specialLayout->addWidget(mConnectLinesCheckBox);
	

	specialLayout->addStretch();

	mControlArea->addTab(specialTab, "Special");


	mControlArea->setCurrentIndex(activeTab);

	connect(mAddSelectedButton, SIGNAL(pressed()),
			this, SLOT(addSelectedNeuronsButtonPressed()));
	connect(mRemoveNeuronsButton, SIGNAL(pressed()),
			this, SLOT(removeNeuronsButtonPressed()));
	connect(mClearHistoryButton, SIGNAL(pressed()),
			this, SLOT(clearHistory()));
	connect(mHistorySizeField, SIGNAL(returnPressed()),
			this, SLOT(plotterRangeChanged()));
	connect(this, SIGNAL(setPlotterRange(const QString&)),
			mHistorySizeField, SLOT(setText(const QString&)));

	connect(mSelectAllCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(selectAllItems(int)));

	connect(mValuePlotter->getPlotterWidget(), SIGNAL(plotterItemCollectionChanged()),
			this, SLOT(plotterItemCollectionChanged()));

	connect(mShowLegendCheckBox, SIGNAL(toggled(bool)),
			this, SLOT(showLegendCheckBoxChanged(bool)));

	connect(mUpdateEventSelector, SIGNAL(activated(int)), 
			this, SLOT(triggerEventSelected(int)));

	connect(this, SIGNAL(updateSupportedTriggerEvents()),
			this, SLOT(supportedTriggerEventsChanged()), Qt::QueuedConnection);

	connect(mLineWidthField, SIGNAL(returnPressed()),
			this, SLOT(changeLineWidth()));
	connect(mConnectLinesCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(enablePointConnections()));

	generalLayout->addStretch(100);

	resize(400, 200);
}


/**
 * Destructor.
 */
FirstReturnMapPlotter::~FirstReturnMapPlotter() {
	QMutexLocker guard(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mValuePlotter != 0) {
		mValuePlotter->invalidateEvents();
	}

	removeAllNeurons();

	if(mEditor != 0) {
		disconnect(mEditor, SIGNAL(tabSelectionChanged(int)),
				   this, SLOT(currentTabChanged(int)));

		QList<NetworkVisualization*> visus = mEditor->getNetworkVisualizations();
		for(QListIterator<NetworkVisualization*> i(visus); i.hasNext();) {
			NetworkVisualization *visu = i.next();
			
			disconnect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
					   this, SLOT(neuralNetworkChanged(ModularNeuralNetwork*)));
			disconnect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
					   this, SLOT(neuralNetworkModified(ModularNeuralNetwork*)));
		}
	}

	if(mNetworksModifiedEvent != 0) {
		mNetworksModifiedEvent->removeEventListener(this);
	}
	if(mNetworksReplacedEvent != 0) {
		mNetworksReplacedEvent->removeEventListener(this);
	}	
	if(mShutDownEvent != 0) {
		mShutDownEvent->removeEventListener(this);
	}
	delete mValuePlotter;
}

QString FirstReturnMapPlotter::getName() const {
	return "FirstReturnMapPlotter";
}


void FirstReturnMapPlotter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mNetworksReplacedEvent) {
		removeAllNeurons();
	}
	else if(event == mNetworksModifiedEvent) {
		neuralNetworkModified(0);
	}
	else if(event == mShutDownEvent) {
		removeAllNeurons();
	}
}


void FirstReturnMapPlotter::addPlottedNeuron(Neuron *neuron) {

	if(mEditor == 0 || neuron == 0 || mNetworksReplacedEvent == 0 
		|| mNetworksModifiedEvent == 0 || mShutDownEvent == 0) 
	{
		return;
	}

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		removeAllNeurons();
		return;
	}

	NeuralNetwork *net = visu->getNeuralNetwork();
	if(net == 0) {
		removeAllNeurons();
		return;
	}

// 	//Allow only at max 2 values to be plotted here.
// 	if(getValuePlotter()->getPlottedValues().size() >= 2) {
// 		return;
// 	}

	DoubleValue *valueToPlot = 0;
	if(mPlotMode == FirstReturnMapPlotter::PLOT_NEURON_ACTIVATION) {
		valueToPlot = &neuron->getActivationValue();
	}
	else {
		valueToPlot = &neuron->getOutputActivationValue();
	}

	if(!net->getNeurons().contains(neuron)) {
		return;
	}

	if(!mPlottedNeurons.contains(neuron)) {
		getValuePlotter()->addValue(neuron->getNameValue().get(), valueToPlot);
		mPlottedNeurons.append(neuron);
	}
}

ValuePlotter* FirstReturnMapPlotter::getValuePlotter() const {
	return mValuePlotter;
}

void FirstReturnMapPlotter::setPlotterRange(int range) {
	mValuePlotter->getPlotterWidget()->setNumberOfValuesToPlot(range);
	mValuePlotter->getPlotterWidget()->setHistoryCapacity(range);

	emit setPlotterRange(QString::number(
				mValuePlotter->getPlotterWidget()->getNumberOfValuesToPlot()));
}

void FirstReturnMapPlotter::setTriggerEventName(const QString &eventName) {
	if(eventName == "None") {
		mValuePlotter->setUpdateTriggerEvent(0);
		emit updateSupportedTriggerEvents();
		return;
	}

	const QMap<Event*, QString> &triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();
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

void FirstReturnMapPlotter::hideHandle(bool hide) {
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


void FirstReturnMapPlotter::showWidget() {
	mValuePlotter->showPlotter();
	show();
}

void FirstReturnMapPlotter::plotterRangeChanged() {
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


void FirstReturnMapPlotter::plotterItemCollectionChanged() {
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

void FirstReturnMapPlotter::showLegendCheckBoxChanged(bool) {
	mValuePlotter->getPlotterWidget()->enableLegend(mShowLegendCheckBox->isChecked());
}

void FirstReturnMapPlotter::supportedTriggerEventsChanged() {
	mUpdateEventSelector->clear();
	const QMap<Event*, QString> &triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();

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

void FirstReturnMapPlotter::triggerEventSelected(int index) {
	QString eventName = mUpdateEventSelector->itemText(index);

	if(eventName == "None") {
		mValuePlotter->setUpdateTriggerEvent(0);
		return;
	}

	const QMap<Event*, QString> &triggerEvents = mValuePlotter->getSupportedUpdateTriggerEvents();

	for(QMapIterator<Event*, QString> i(triggerEvents); i.hasNext();) {
		i.next();
		if(i.value() == eventName) {
			mValuePlotter->setUpdateTriggerEvent(i.key());
		}
	}
}

void FirstReturnMapPlotter::selectAllItems(int) {
	for(int i = 0; i < mPlotterItemPanels.size(); ++i) {
		mPlotterItemPanels.at(i)->visibilityCheckBoxChanged((int) mSelectAllCheckBox->isChecked());
	}
}

void FirstReturnMapPlotter::currentTabChanged(int) {
	removeAllNeurons();
	
	if(mEditor == 0) {
		return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(neuralNetworkChanged(ModularNeuralNetwork*)));
		connect(visu, SIGNAL(neuralNetworkModified(ModularNeuralNetwork*)),
				this, SLOT(neuralNetworkModified(ModularNeuralNetwork*)));
	}
}

void FirstReturnMapPlotter::neuralNetworkModified(ModularNeuralNetwork*) {
	//check if all neurons are still valid
	if(mEditor == 0) {
		removeAllNeurons();
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		removeAllNeurons();
		return;
	}

	NeuralNetwork *net = visu->getNeuralNetwork();
	if(net == 0) {
		removeAllNeurons();
		return;
	}	

	QList<Neuron*> neurons = mPlottedNeurons;
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		if(!net->getNeurons().contains(neuron)) {
			getValuePlotter()->removeValue(&neuron->getActivationValue());
			getValuePlotter()->removeValue(&neuron->getOutputActivationValue());
			mPlottedNeurons.removeAll(neuron);
		}
	}
}

void FirstReturnMapPlotter::neuralNetworkChanged(ModularNeuralNetwork*) {
	removeAllNeurons();
}

void FirstReturnMapPlotter::addSelectedNeuronsButtonPressed() {
	if(mEditor == 0) {
		return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	
	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		
		NeuronItem *item = dynamic_cast<NeuronItem*>(i.next());
	
			if(item != 0 && !mPlottedNeurons.contains(item->getNeuron())) {
				Neuron *neuron = item->getNeuron();
				addPlottedNeuron(neuron);
// 				mPlottedNeurons.append(neuron);
// 				if(mPlotMode == FirstReturnMapPlotter::PLOT_NEURON_ACTIVATION) {
// 					getValuePlotter()->addValue(neuron->getNameValue().get(), 
// 												&neuron->getActivationValue());
// 				}
// 				else {
// 					getValuePlotter()->addValue(neuron->getNameValue().get(),
// 												&neuron->getOutputActivationValue());
// 				}
			}
	}
}


void FirstReturnMapPlotter::removeNeuronsButtonPressed() {
	removeAllNeurons();
}


void FirstReturnMapPlotter::changeLineWidth() {
	FirstReturnMapPlotterWidget *plotter = 
			dynamic_cast<FirstReturnMapPlotterWidget*>(mValuePlotter->getPlotterWidget());
	if(plotter != 0) {
		plotter->setPenWidth(mLineWidthField->text().toDouble());
	}
}


void FirstReturnMapPlotter::enablePointConnections() {
	FirstReturnMapPlotterWidget *plotter = 
			dynamic_cast<FirstReturnMapPlotterWidget*>(mValuePlotter->getPlotterWidget());
	if(plotter != 0) {
		plotter->enableConnectedPoints(mConnectLinesCheckBox->isChecked());
	}
}

void FirstReturnMapPlotter::clearHistory() {
	FirstReturnMapPlotterWidget *plotter = 
			dynamic_cast<FirstReturnMapPlotterWidget*>(mValuePlotter->getPlotterWidget());
	if(plotter != 0) {
		plotter->clearHistory();
	}
}


void FirstReturnMapPlotter::removeAllNeurons() {
	if(mPlottedNeurons.empty()) {
		return;
	}
	QList<Value*> values = getValuePlotter()->getPlottedValues();
	for(QListIterator<Value*> i(values); i.hasNext();) {
		getValuePlotter()->removeValue(i.next());
	}
	mPlottedNeurons.clear();
}

}



