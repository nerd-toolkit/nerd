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

#include "ValuePlotter.h"
#include <QHBoxLayout>
#include "Gui/ValuePlotter/PlotterItem.h"
#include "Core/Core.h"
#include "Event/EventManager.h"
#include <iostream>
#include "Value/MultiPartValue.h"
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QStringList>
#include <QTime>
#include <QDate>
#include "NerdConstants.h"
#include "Gui/ValuePlotter/ValuePlotterItem.h"
#include <QApplication>

using namespace std;

namespace nerd {


/**
 * Constructor.
 *
 * @param name the name to identify this ValuePlotter when more than one ValuePlotter
 *        is used.
 */
ValuePlotter::ValuePlotter(const QString &name, PlotterWidget *plotter, QWidget *parent)
	: EventListener(), QWidget(parent),
		mName(name), mPlotterWidget(0), mNextStepEvent(0), mNextTryEvent(0), mNextIndividualEvent(0),
		mNextGenerationEvent(0), mNextNetworkIterationEvent(0), 
		mLeftMouseButtonPressed(false), mHistoryUpdateTriggerEvent(0)
		
{
	moveToThread(QCoreApplication::instance()->thread());

	mStaticScalingVStartSetting = 1.0;
	mStaticScalingHStartSetting = 1.0;

	//mName = QString("Value Plotter (").append(name).append(")");
	mName = name;
	setWindowTitle(mName);

	setFocusPolicy(Qt::StrongFocus);
	
	if(plotter == 0) {
		mPlotterWidget = new PlotterWidget(this);
	}
	else {
		mPlotterWidget = plotter;
		mPlotterWidget->setParent(this);
	}

	//mMaxPlotterHistorySize = 1000;
	//mPlotterWidget->setNumberOfValuesToPlot(mMaxPlotterHistorySize);
	mPlotterWidget->setNumberOfValuesToPlot(1000);


	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	//set up the container with default values.
	mDefaultColors.append(Qt::black);
	mDefaultColors.append(Qt::red);
	mDefaultColors.append(Qt::green);
	mDefaultColors.append(Qt::blue);
	mDefaultColors.append(Qt::cyan);
	mDefaultColors.append(Qt::magenta);
	mDefaultColors.append(Qt::darkRed);
	mDefaultColors.append(Qt::darkGreen);
	mDefaultColors.append(Qt::darkBlue);
	mDefaultColors.append(Qt::darkCyan);
	mDefaultColors.append(Qt::darkMagenta);
	mDefaultColors.append(Qt::yellow);
	mDefaultColors.append(Qt::darkGray);
	mDefaultColors.append(Qt::darkYellow);

	mPaintTimer.moveToThread(QApplication::instance()->thread());
	connect(&mPaintTimer, SIGNAL(timeout()), this, SLOT(repaintPlotter()));

	QHBoxLayout *mainLayout = new QHBoxLayout();
	setLayout(mainLayout);

	mainLayout->addWidget(mPlotterWidget);
	mainLayout->setMargin(1);
	mainLayout->setSpacing(0);

	resize(542, 200);
}


/**
 * Destructor.
 */
ValuePlotter::~ValuePlotter() {
	mPaintTimer.stop();
	invalidateEvents();
}


/**
 * Called when an observed Event occured.
 * ValuePlotter is listening for event NextStep to update data and plotter.
 */
void ValuePlotter::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mNextStepEvent && mHistoryUpdateTriggerEvent == mNextStepEvent) {
		mPlotterWidget->updateHistories();
	}
	else if(event == mNextTryEvent && mHistoryUpdateTriggerEvent == mNextTryEvent) {
		mPlotterWidget->updateHistories();
	}
	else if(event == mNextIndividualEvent && mHistoryUpdateTriggerEvent == mNextIndividualEvent) {
		mPlotterWidget->updateHistories();
	}
	else if(event == mNextGenerationEvent && mHistoryUpdateTriggerEvent == mNextGenerationEvent) {
		mPlotterWidget->updateHistories();
	}
	else if(event == mNextNetworkIterationEvent 
			&& mHistoryUpdateTriggerEvent == mNextNetworkIterationEvent) 
	{
		mPlotterWidget->updateHistories();
	}
}


/**
 * Returns the name of this ValuePlotter.
 *
 * @return the name.
 */
QString ValuePlotter::getName() const {
	return mName;
}

const QMap<Event*, QString>& ValuePlotter::getSupportedUpdateTriggerEvents() const {
	return mTriggerEvents;
}


void ValuePlotter::setUpdateTriggerEvent(Event *event) {
	if(mTriggerEvents.keys().contains(event) || event == 0) {
		mHistoryUpdateTriggerEvent = event;
	}
}

Event* ValuePlotter::getUpdateTriggerEvent() const {
	return mHistoryUpdateTriggerEvent;
}

/**
 * Deregisters from all trigger events.
 */
void ValuePlotter::invalidateEvents() {
	if(mNextNetworkIterationEvent != 0) {
		mNextNetworkIterationEvent->removeEventListener(this);
	}
	if(mNextStepEvent != 0) {
		mNextStepEvent->removeEventListener(this);
	}
	if(mNextTryEvent != 0) {
		mNextTryEvent->removeEventListener(this);
	}
	if(mNextGenerationEvent != 0) {
		mNextGenerationEvent->removeEventListener(this);
	}
	if(mNextIndividualEvent != 0) {
		mNextIndividualEvent->removeEventListener(this);
	}
}

/**
 * Initializes the ValuePlotter. Initialization is required. Otherwise the
 * ValuePlotter can not react on NextStep events to update the plotter and
 * the plotted data.
 *
 * If showPlotter() is called before initialization took place, then this
 * method is called automatically.
 */
void ValuePlotter::init() {
	EventManager *em = Core::getInstance()->getEventManager();

	mNextStepEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_STEP_COMPLETED, this);
	mNextTryEvent = em->registerForEvent(NerdConstants::EVENT_EXECUTION_TRY_COMPLETED, this);
	mNextIndividualEvent = em->registerForEvent(
				NerdConstants::EVENT_EXECUTION_INDIVIDUAL_COMPLETED, this);
	mNextGenerationEvent = em->registerForEvent(
				NerdConstants::EVENT_EXECUTION_GENERATION_COMPLETED, this);
	mNextNetworkIterationEvent = em->registerForEvent(
				NerdConstants::EVENT_NNM_NETWORK_ITERATION_COMPLETED, this);

	if(mNextStepEvent != 0) {
		mHistoryUpdateTriggerEvent = mNextStepEvent;
		mTriggerEvents.insert(mNextStepEvent, "Step Completed");
	}
	if(mNextTryEvent != 0) {
		mTriggerEvents.insert(mNextTryEvent, "Try Completed");
		if(mHistoryUpdateTriggerEvent == 0) {
			mHistoryUpdateTriggerEvent = mNextTryEvent;
		}
	}
	if(mNextIndividualEvent != 0) {
		mTriggerEvents.insert(mNextIndividualEvent, "Individual Completed");
		if(mHistoryUpdateTriggerEvent == 0) {
			mHistoryUpdateTriggerEvent = mNextIndividualEvent;
		}
	}
	if(mNextGenerationEvent != 0) {
		mTriggerEvents.insert(mNextGenerationEvent, "Generation Completed");
		if(mHistoryUpdateTriggerEvent == 0) {
			mHistoryUpdateTriggerEvent = mNextGenerationEvent;
		}
	}
	if(mNextNetworkIterationEvent != 0) {
		mTriggerEvents.insert(mNextNetworkIterationEvent, "Network Iteration");
		if(mHistoryUpdateTriggerEvent == 0) {
			mHistoryUpdateTriggerEvent = mNextNetworkIterationEvent;
		}
	}

	emit triggerEventSupportChanged();
}



/**
 * Adds a Value with the given name to the ValuePlotter.<br><br>
 * A value is only added to the ValuePlotter if it is not already contained there.
 * Furhtermore so called MultiPartValues are treated differently. These values
 * are automatically split into their parts and these parts are then added
 * as independent Value to the ValuePlotter. The name of the parts is assembled
 * out of the valueName of the MultiPartValue, a slash, followed by the part name
 * querried at the MultiPartValue.
 *
 * @param valueName the name to display for the value to add.
 * @param value the value to add.
 * @return true if all values could be added.
 *         False if at least one value (if MultiPartValue) failed (still all others are added)
*/
bool ValuePlotter::addValue(const QString &valueName, Value *value) {
	QList<Value*> addedValues;
	QList<QString> addedValueNames;

	//Determine whether this is a single Value or a MultiPartValue
	if(dynamic_cast<MultiPartValue*>(value) != 0) {

		//Add all parts of the Value as separately to observe Values.
		MultiPartValue *mv = dynamic_cast<MultiPartValue*>(value);
		for(int i = 0; i < mv->getNumberOfValueParts(); ++i) {
			addedValues.append(mv->getValuePart(i));
			addedValueNames.append(QString(valueName).append("/")
						.append(mv->getValuePartName(i)));
		}
	}
	else {
		//just add the single Value
		addedValues.append(value);
		addedValueNames.append(valueName);
	}

	bool ok = true;

	for(int k = 0; k < addedValues.size() && k < addedValueNames.size(); ++k) {
		if(!addValueToPlotter(addedValueNames.at(k), addedValues.at(k))) {
			ok = false;
		}
	}

	return ok;
}


/**
 * Adds a single Value to the ValuePlotter. This method automatically chooses an
 * unused color and calls method addValueToPlotter(QString, Value*, QColor).
 *
 * @param valueName the name to display for the value.
 * @param value the Value to add.
 * @return true if successful, false if failed or already part of the plotter.
 */
bool ValuePlotter::addValueToPlotter(const QString &valueName, Value *value) {

	return addValueToPlotter(valueName, value, getSuitableColor());
}


/**
 * Adds a single Value to the ValuePlotter.
 *
 * @param valueName the name to display for the value.
 * @param value the Value to add.
 * @param color the color in which the value should be plotted.
 * @return true if successful, false if failed or already part of the plotter.
 */
bool ValuePlotter::addValueToPlotter(const QString &valueName, Value *value, const QColor &color) {
	if(value == 0) {
		return false;
	}

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		ValuePlotterItem *item = dynamic_cast<ValuePlotterItem*>(items.at(i));
		if(item != 0 && item->getValue() == value) {
			return false;
		}
	}

	ValuePlotterItem *item = new ValuePlotterItem(valueName, color);
	if(!item->setValue(value)) {
		return false;
	}
	item->setHistoryCapacity(mPlotterWidget->getNumberOfValuesToPlot());

	return mPlotterWidget->addPlotterItem(item);
}


/**
 * Removes a single Value from the plotter.
 *
 * @param value the value to remove.
 * @return true if successful, false if it was not part of the plotter.
 */
bool ValuePlotter::removeValue(Value *value) {
	if(value == 0) {
		return false;
	}

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		ValuePlotterItem *item = dynamic_cast<ValuePlotterItem*>(items.at(i));
		if(item != 0 && item->getValue() == value) {
			bool ok = mPlotterWidget->removePlotterItem(item);
			if(ok) {
				delete item;
			}
			return ok;
		}
	}
	return false;
}


/**
 * Returns a Vector with all currently plotted Value objects.
 *
 * @return all currently plotted Vaue obejcts.
 */
QList<Value*> ValuePlotter::getPlottedValues() const {

	QList<Value*> values;
	if(mPlotterWidget == 0) {
		Core::log("Could not fina a PlotterWidet.", true);
		return values;
	}
	mPlotterWidget->enableLegend(mPlotterWidget->isLegendEnabled());

	QList<PlotterItem*> items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		ValuePlotterItem *item = dynamic_cast<ValuePlotterItem*>(items.at(i));
		if(item != 0) {
			values.append(item->getValue());
		}
	}

	return values;
}


/**
 * Returns a Color that is currently suitable taking into account a high contrast
 * of used colors of all currently used Values.
 *
 * @return a color that can be used for a PlotterItem.
 */
QColor ValuePlotter::getSuitableColor(QList<QColor> *ignoreList) {

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();
	QList<QColor> excludedColors;
	if(ignoreList != 0) {
		excludedColors = *ignoreList;
	}

	for(int j = 0; j < mDefaultColors.size(); ++j) {
		QColor color = mDefaultColors.at(j);
		bool isInUse = false;

		for(int i = 0; i < excludedColors.size(); ++i) {
			if(excludedColors.at(i) == color) {
				isInUse = true;
				break;
			}
		}

		if(isInUse) {
			break;
		}

		for(int i = 0; i < items.size(); ++i) {
			PlotterItem *item = items.at(i);
			if(item->getColor() == color) {
				isInUse = true;
				break;
			}
		}

		if(!isInUse) {
			return color;
		}
	}
	return mDefaultColors.at((items.size() + excludedColors.size())
								% mDefaultColors.size());
}



/**
 * Returns a pointer to the internal PlotterWidget for direct access.
 *
 * @return the internal PlotterWidget;
 */
PlotterWidget* ValuePlotter::getPlotterWidget() const {
	return mPlotterWidget;
}


/**
 * Shows the plotter window or widget.
 * If the plotter was not initialized when this method is called, then the initialization
 * is triggered before the widget is shown.
 */
void ValuePlotter::showPlotter() {
	if(mNextStepEvent == 0) {
		init();
	}
	show();
}


/**
 * Repaints the plotter display if it is visible.
 */
void ValuePlotter::repaintPlotter() {
	if(isVisible()) {
		mPlotterWidget->plot();
	}
}

/**
 * Loads data from a file and adds this data as static PlotterItems.
 *
 * @param fileName the name of the data source file.
 */
void ValuePlotter::loadStaticData(const QString &fileName,
				const QString &commentChar,
				const QString &separator)
{

	//Try to open specified file
	QFile file(fileName);

	if(!file.open(QIODevice::ReadOnly)) {
		file.close();
		return;
	}

	removeAllStaticData();

	QVector<QString> names;
	QVector<PlotterItem*> items;
	QList<QColor> colors;

	QTextStream data(&file);

	int dataLineCounter = 0;

	while(!data.atEnd()) {
		QString line = data.readLine().trimmed();

		if(line == "") {
			continue;
		}
		//collect names (usually the form <commentChar> <name>)
		if(line.indexOf(commentChar) == 0) {
			names.append(QString("[")
				.append(line.mid(commentChar.size()).trimmed())
				.append("]"));
		}
		else {
			//parse the actual data series
			dataLineCounter++;

			QStringList values = line.split(separator);

			//always ensure that the number of values in a line match the number
			//of PlotterItems. If not, then add sufficiently many PlotterItems.
			if(values.size() > items.size()) {
				for(int i = items.size(); i < values.size(); ++i) {
					QString name = names.size() > i ? names.at(i) : "[data]";
					QColor color = getSuitableColor(&colors);
					colors.append(color);
					PlotterItem *item = new PlotterItem(name, color);
					item->setStaticHistory(true);
					items.append(item);
				}
			}

			//Extend the history of all PlotterItems (parsing errors are ignored!)
			for(int i = 0; i < values.size(); ++i) {
				PlotterItem *item = items.at(i);
				item->setHistoryCapacity(dataLineCounter);
				item->addToHistory(values.at(i).toDouble());
			}
		}
	}

	//All new PlotterItems are added to the PlotterWidget.
	for(int i = 0; i < items.size(); ++i) {
		mPlotterWidget->addPlotterItem(items.at(i));
	}

	file.close();

	//Make sure the static data is visible.
	showStaticData(true);
}


/**
 * Removes all PlotterItems with static history.
 */
void ValuePlotter::removeAllStaticData() {
	QList<PlotterItem*> items = mPlotterWidget->getPlotterItems();
	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(item->hasStaticHistory()) {
			mPlotterWidget->removePlotterItem(item);
			delete item;
		}
	}
}


/**
 * Changes the visibility status of all PlotterItems with static history.
 *
 * @param show if true, then all static PlotterItems are shown, otherwise they are hidden.
 */
void ValuePlotter::showStaticData(bool show) {
	mShowStaticData = show;

	mPlotterWidget->enableStaticItems(mShowStaticData);

	emit staticDataVisibilityChanged(mShowStaticData);
}


/**
 * Called when the Widget is hidden. This stops the repaint timer to raise performance.
 */
void ValuePlotter::hideEvent(QHideEvent*) {
	mPaintTimer.stop();
}


/**
 * Called when the Widget is shown. This restarts the repaint timer.
 */
void ValuePlotter::showEvent(QShowEvent*) {
	mPaintTimer.setInterval(150);
	mPaintTimer.start();
}


/**
 * Called when a keys was pressed.
 *
 * Currently keys can be used to change the zoom factor (PageUp/PageDown)
 * and to move the view up and down (ArrowUp/ArrowDown).
 *
 * @param event the QKeyEvent that occured.
 */
void ValuePlotter::keyPressEvent(QKeyEvent *event) {
	if(event == 0) {
		return;
	}
	if(event->key() == Qt::Key_PageUp) {
		//Increases vertical scaling (dynamic values or static values (SHIFT))
		if(event->modifiers() & Qt::ShiftModifier) {
			changeStaticDataScaleV(1);
		}
		else {
			changeUserScaleV(1);
		}
	}
	else if(event->key() == Qt::Key_PageDown) {
		//Decreases vertical scaling (dynamic values or static values (SHIFT))
		if(event->modifiers() & Qt::ShiftModifier) {
			changeStaticDataScaleV(-1);
		}
		else {
			changeUserScaleV(-1);
		}
	}
	else if(event->key() == Qt::Key_Down) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT + CTRL: Decrease vertical scaling.of static values.
			changeStaticDataScaleV(-1);
		}
		else if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: Decrease vertical offset of static values.
			changeStaticDataOffsetV(-1);
		}
		else {
			//Other: Decrease vertical offset of PlotterWidget.
			changeUserOffsetV(mPlotterWidget->getUserOffsetV() -
					((-10.0 / ((double) (mPlotterWidget->height() - 20) / 2.0))
					/ mPlotterWidget->getUserScaleV()));

		}
	}
	else if(event->key() == Qt::Key_Up) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT + CTRL: Increase vertical scaling.of static values.
			changeStaticDataScaleV(1);
		}
		else if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: Increase vertical offset of static values.
			changeStaticDataOffsetV(1);
		}
		else {
			//Other: Increase vertical offset of PlotterWidget.
			changeUserOffsetV(mPlotterWidget->getUserOffsetV() -
				((10.0 / ((double) (mPlotterWidget->height() - 20) / 2.0))
					/ mPlotterWidget->getUserScaleV()));
		}
	}
	else if(event->key() == Qt::Key_Left) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT * CTRL: Decrease horizontal scaling of static data.
			changeStaticDataScaleH(-1);
		}
		else if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: Increase horizontal offset of static values.
			changeStaticDataOffsetH(1);
		}
	}
	else if(event->key() == Qt::Key_Right) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT * CTRL: Increase horizontal scaling of static data.
			changeStaticDataScaleH(1);
		}
		else if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: Decrease horizontal offset of static values.
			changeStaticDataOffsetH(-1);
		}
	}
	else if(event->key() == Qt::Key_F8) {
		//(Re-)load the static data fro a file.
		loadStaticData("logfile.txt", "#", ";");
	}
	else if(event->key() == Qt::Key_H) {
		//toggle visibility of static data.
		showStaticData(!mShowStaticData);
	}
	else if(event->key() == Qt::Key_Home) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT + CTRL: Home the horizontal offset of static values.
			setStaticDataOffsetH(0.0);
		}
		else if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: Home the vertical offset of static valus
			setStaticDataOffsetV(0.0);
		}
		else {
			//Other: Home the vertical offset of the PlotterWidget.
			mPlotterWidget->setUserOffsetV(0.0);
		}
	}
	else if(event->key() == Qt::Key_Insert) {
		if((event->modifiers() & Qt::ShiftModifier)
			&& (event->modifiers() & Qt::ControlModifier))
		{
			//SHIFT + CTRL: Sets the horizontal scale of static values to 1.0
			setStaticDataScaleH(1.0);
		}
		if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT + CTRL: Sets the vertical scale of static values to 1.0
			setStaticDataScaleV(1.0);
		}
		else {
			//Other: Sets the vertical scale of the PlotterWidget to 1.0
			mPlotterWidget->setUserScaleV(1.0);
		}
	}
	else if(event->key() == Qt::Key_S) {
		//Saves the current history to a file.
		mPlotterWidget->saveHistoriesToFile(QString("history_")
					.append(QDate::currentDate().toString("yyMMdd"))
					.append(QTime::currentTime().toString("hhmmss"))
					.append(".log"));
	}
	else {
		event->ignore();
	}
}


/**
 * Called when the mouse was moved.
 * This changes the view position when the left mouse buttons is pressed.
 */
void ValuePlotter::mouseMoveEvent(QMouseEvent *event) {
	
	if(mLeftMouseButtonPressed) {

		double width = (double) (mPlotterWidget->height() - 20);

		if(width == 0.0 || mPlotterWidget->getUserScaleV() == 0.0 
			|| mStaticScalingVStartSetting == 0.0 
			|| mStaticScalingHStartSetting == 0.0
			|| mPlotterWidget->getNumberOfValuesToPlot() == 0)
		{
			Core::log("ValuePlotter Warning: Scaling or number of plotted values out of range!");	
			return;
		}

		if(event->modifiers() & Qt::ShiftModifier) {
			//SHIFT: change horizontal and vertical
			//       offset of all static PlotterItems

			double offsetV = mStaticOffsetVStartSetting
						- ((((mMouseStartPosition.y() - event->y())
							/ (width / 2.0))
						/ mPlotterWidget->getUserScaleV()) / mStaticScalingVStartSetting);

			double offsetH = mStaticOffsetHStartSetting
						- ((((mMouseStartPosition.x() - event->x())
							/ width)
						/ mStaticScalingHStartSetting)
						* ((double) mPlotterWidget->getNumberOfValuesToPlot()));

			cerr << "3: " << offsetV << " " << offsetH << endl;
			setStaticDataOffsetV(offsetV);
			setStaticDataOffsetH(offsetH);
		}
		else {
			//change vertical offset of the PlotterWidget.

			double offsetV = mOffsetVStartSetting
						- (((mMouseStartPosition.y() - event->y())
							/ (width / 2.0))
						/ mPlotterWidget->getUserScaleV());

			changeUserOffsetV(offsetV);
		}
	}
}

/**
 * Called when a mouse button was pressed.
 */
void ValuePlotter::mousePressEvent(QMouseEvent *event) {
	mMouseStartPosition = event->pos();
	mOffsetVStartSetting = mPlotterWidget->getUserOffsetV();

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(item->hasStaticHistory()) {
			mStaticOffsetVStartSetting = item->getOffsetV();
			mStaticOffsetHStartSetting = item->getOffsetH();
			mStaticScalingHStartSetting = item->getScalingH();
			mStaticScalingVStartSetting = item->getScalingV();
			break;
		}
	}

	mLeftMouseButtonPressed = true;
}


/**
 * Called when a mouse button was released.
 */
void ValuePlotter::mouseReleaseEvent(QMouseEvent*) {
	mLeftMouseButtonPressed = false;
}

/**
 * Called when the mouse wheel was rotated.
 * Currently this controls the zoom factor of the plotter display.
 */
void ValuePlotter::wheelEvent(QWheelEvent *event) {
	if(event->modifiers() & Qt::ShiftModifier) {
		if(event->modifiers() & Qt::ControlModifier) {
			//static PlotterItem: scale horizontal
			changeStaticDataScaleH(event->delta() > 0 ? -1 : 1);
		}
		else {
			//static PlotterItems: scale vertical
			changeStaticDataScaleV(event->delta() > 0 ? -1 : 1);
		}
	}
	else {
		//dynamic PlotterItems: scale vertical
		changeUserScaleV(event->delta() > 0 ? -1 : 1);
	}
}


/**
 * Changes the vertical user scale factor. Count specifies the number of scale changes.
 * In each change the old scale factor is multiplied by a fixed factor (currently
 * 1.5) to allow exponential zooming.
 *
 * @param count the number of scale factor ticks to change. If positive the factor is
 *              increased, otherwise it is decreased.
 */
void ValuePlotter::changeUserScaleV(int count) {

	double factor = 1.5;
	double scale = mPlotterWidget->getUserScaleV();

	if(count == 0) {
		return;
	}
	if(count < 0) {
		scale *= (count * factor * -1.0);
	}
	else {
		scale /= (count * factor);
	}
	mPlotterWidget->setUserScaleV(scale);
}


/**
 * Changes the vertical user offset.
 *
 * @param offset the desired offset.
 */
void ValuePlotter::changeUserOffsetV(double offset) {

	mPlotterWidget->setUserOffsetV(offset);
}


/**
 * Changes the vertical scaling of all PlotterItems with static history.
 *
 * @param count the number of scaling changes per PlotterItem.
 */
void ValuePlotter::changeStaticDataScaleV(int count) {

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	double scale = 1.0;

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);

		if(!item->hasStaticHistory()) {
			continue;
		}

		scale = item->getScalingV();

		if(count == 0) {
			return;
		}
		if(count < 0) {
			scale += (count * 0.01);
		}
		else {
			scale += (count * 0.01);
		}
		item->setScalingV(scale);
	}
	emit staticScaleVChanged(scale);
}


/**
 * Changes the vertical offset of all PlotterItems with static history.
 *
 * @param count the number of change ticks per PlotterItem.
 */
void ValuePlotter::changeStaticDataOffsetV(int count) {

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	double change = ((1.0 / ((double) (mPlotterWidget->height() - 20.0) / 2.0))
					/ mPlotterWidget->getUserScaleV());

	double offset = 0.0;

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}

		offset = item->getOffsetV();
		offset = offset - (((double) count) * change);
		item->setOffsetV(offset);
	}
	emit staticOffsetVChanged(offset);
}


void ValuePlotter::changeStaticDataOffsetH(double offset) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	double localOffset = 0.0;

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}

		localOffset = item->getOffsetH();
		localOffset += offset;
		item->setOffsetH(localOffset);
	}
	emit staticOffsetHChanged(localOffset);
}

void ValuePlotter::changeStaticDataOffsetV(double offset) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	double localOffset = 0.0;

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}

		localOffset = item->getOffsetV();
		localOffset += offset;
		item->setOffsetV(localOffset);
	}
	emit staticOffsetVChanged(localOffset);
}


/**
 * Sets the vertical scaling of all PlotterItems with static history directly.
 *
 * @param scale the scale to set for all static PlotterItems.
 */
void ValuePlotter::setStaticDataScaleV(double scale) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}
		item->setScalingV(scale);
	}
	emit staticScaleVChanged(scale);
}


/**
 * Sets the horizontal scaling of all PlotterItems with static history directly.
 *
 * @param scale the scale to set for all static PlotterItems.
 */
void ValuePlotter::setStaticDataScaleH(double scale) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}
		item->setScalingH(scale);
	}
	emit staticScaleHChanged(scale);
}


/**
 * Sets the vertical offset of all PlotterItems with static content directly.
 *
 * @param offset the offset to set for all static PlotterItems.
 */
void ValuePlotter::setStaticDataOffsetV(double offset) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}
		item->setOffsetV(offset);
	}
	emit staticOffsetVChanged(offset);
}


/**
 * Sets the horizontal offset of all PlotterItems with static content directly.
 *
 * @param offset the offset to set for all static PlotterItems.
 */
void ValuePlotter::setStaticDataOffsetH(double offset) {
	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}
		item->setOffsetH(offset);
	}
	emit staticOffsetHChanged(offset);
}



/**
 * Changes the horizontal offset off all static PlotterItems.
 * Count determines the number of changes (currently 5 per count).
 * Count can be negavite to alter the offset to the other direction.
 *
 * @param count the number of offset changes.
 */
void ValuePlotter::changeStaticDataOffsetH(int count) {

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	//the change per count
	double change = 5;

	double offset = 0.0;

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);
		if(!item->hasStaticHistory()) {
			continue;
		}

		offset = item->getOffsetH();
		offset = offset - (((double) count) * change);

		item->setOffsetH(offset);
	}
	emit staticOffsetHChanged(offset);
}


/**
 * Changes the horizontal scaling of all static PlotterItems
 * by a multiple of count (can also be negative to scale down)
 * (currently it is 0.01 * count).
 *
 * @param count the multiplyer for the scale changes.
 */
void ValuePlotter::changeStaticDataScaleH(int count) {

	//the change per count.
	double scaleChange = 0.01;
	double scale = 0.0;

	const QList<PlotterItem*> &items = mPlotterWidget->getPlotterItems();

	for(int i = 0; i < items.size(); ++i) {
		PlotterItem *item = items.at(i);

		if(!item->hasStaticHistory()) {
			continue;
		}

		scale = item->getScalingH();

		if(count == 0) {
			return;
		}
		if(count < 0) {
			scale += (count * scaleChange);
		}
		else {
			scale += (count * scaleChange);
		}
		item->setScalingH(scale);
	}
	emit staticScaleHChanged(scale);
}




}



