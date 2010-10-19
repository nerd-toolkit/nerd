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




#include "PlotterWidget.h"
#include <QPainter>
#include <QMutexLocker>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QThread>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "NerdConstants.h"
#include <QCoreApplication>


using namespace std;

namespace nerd {

PlotterWidget::PlotterWidget(QWidget *parent)
	: QFrame(parent), mHistorySize(0), mPlotStaticItems(true), mPlotLegend(true), 
	  mPerformanceMode(0)
{
	moveToThread(QCoreApplication::instance()->thread());

	setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(2);

	mDrawableHeight = 0.0;
	mDrawableWidth = 0.0;
	mUserScaleH = 1.0;
	mUserScaleV = 1.0;
	mUserOffsetH = 0.0;
	mUserOffsetV = 0.0;
	mMaxValueNameSize = 35;
	mNumberOfValuesToPlot = 1000;

	mPerformanceMode = Core::getInstance()->getValueManager()
				->getBoolValue(NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE);
}


/**
 * Destructor.
 * Calls clear() and hereby removes and destroys all PlotterItems.
 */
PlotterWidget::~PlotterWidget() {
	clear();
}



/**
 * Initiates plotting of the Widget.
 */
void PlotterWidget::plot() {
	update();
}


/**
 * Updates the History of all PlotterItems.
 */
void PlotterWidget::updateHistories() {

	//Lock the PlotterItem list
	QMutexLocker guard(&mUpdateMutex);

	for(int i = 0; i < mPlotterItems.size(); ++i) {
		mPlotterItems.at(i)->update();
	}
}

/**
 * Adds a PlotterItem to the PlotterWidget.
 *
 * @param item the item to add.
 * @return true if successful, false if already contained.
 */
bool PlotterWidget::addPlotterItem(PlotterItem *item) {

	//Lock the PlotterItem list
	QMutexLocker guard(&mUpdateMutex);

	if(item == 0 || mPlotterItems.contains(item)) {
		return false;
	}
	mPlotterItems.append(item);

	if(item->getHistoryCapacity() > mHistorySize) {
		mHistorySize = item->getHistoryCapacity(); //TODO rescale all other capacities?
	}

	//TODO update.
	emit plotterItemCollectionChanged();
	return true;
}


/**
 * Removes a single PlotterItem from the PlotterWidget.
 *
 * @param item the PlotterItem to remove.
 * @return true if successful, false if not contained.
 */
bool PlotterWidget::removePlotterItem(PlotterItem *item) {

	//Lock the PlotterItem list
	QMutexLocker guard(&mUpdateMutex);

	if(item == 0 || !mPlotterItems.contains(item)) {
		return false;
	}
	mPlotterItems.removeAll(item);
	//TODO update.
	emit plotterItemCollectionChanged();
	return true;
}


const QList<PlotterItem*>& PlotterWidget::getPlotterItems() const {
	return mPlotterItems;
}


void PlotterWidget::setNumberOfValuesToPlot(int numberOfValues) {
	mNumberOfValuesToPlot = numberOfValues;
}

int PlotterWidget::getNumberOfValuesToPlot() const {
	return mNumberOfValuesToPlot;
}

void PlotterWidget::setHistoryCapacity(int size) {
	//Lock the PlotterItem list.
	QMutexLocker guard(&mUpdateMutex);

	mHistorySize = size;

	//Add the names of the PlotterItems.
	for(int i = 0; i < mPlotterItems.size(); ++i) {
		PlotterItem *item = mPlotterItems.at(i);
		if(item->hasStaticHistory()) {
			continue;
		}
		item->setHistoryCapacity(size);
	}
}

double PlotterWidget::getUserScaleV() const {
	return mUserScaleV;
}

void PlotterWidget::setUserScaleV(double scale) {
	mUserScaleV = scale;
	emit userScaleVChanged(mUserScaleV);
}

double PlotterWidget::getUserOffsetV() const {
	return mUserOffsetV;
}

void PlotterWidget::setUserOffsetV(double offset) {
	mUserOffsetV = offset;
	emit userOffsetVChanged(mUserOffsetV);
}


/**
 * Creates a file named fileName and fills it with the history data of
 * all currently available and visible dynamic PlotterItems.
 *
 * PlotterItems with static content are ignored.
 *
 * @param fileName the name of the file to write the histories to.
 * @return true if successful, otherwise false.
 */
bool PlotterWidget::saveHistoriesToFile(const QString &fileName) {

	QFile file(fileName);

	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		qDebug("Could not open file %s to write histories.",
				fileName.toStdString().c_str());
		file.close();
		return false;
	}
	QTextStream output(&file);
	int largestHistorySize = 0;
	QVector<PlotterItem*> items;

	//Lock the PlotterItem list.
	QMutexLocker guard(&mUpdateMutex);

	//Add the names of the PlotterItems.
	for(int i = 0; i < mPlotterItems.size(); ++i) {
		PlotterItem *item = mPlotterItems.at(i);
		if(item->hasStaticHistory() || !item->isVisible()) {
			continue;
		}
		output << "# " << item->getName() << endl;
		items.append(item);
		if(item->getHistory().size() > largestHistorySize) {
			largestHistorySize = item->getHistory().size();
		}
	}
	output << endl << endl;

	//Add the histories of all PlotterItems.
	for(int j = largestHistorySize; j > 0; --j) {
		for(int i = 0; i < items.size(); ++i) {
			PlotterItem *item = items.at(i);
			output << item->getHistoryValue(item->getHistory().size() - j);
			if(i < (items.size() - 1)) {
				output << ";";
			}
		}
		output << "\n";
	}
	file.close();
	return true;
}

void PlotterWidget::enableStaticItems(bool enable) {
	mPlotStaticItems = enable;
}


bool PlotterWidget::areStaticItemsEnabled() const {
	return mPlotStaticItems;
}


void PlotterWidget::enableLegend(bool enable) {
	mPlotLegend = enable;
}


bool PlotterWidget::isLegendEnabled() const {
	return mPlotLegend;
}


/**
 * Removes and destroys all currently available PlotterItems.
 */
void PlotterWidget::clear() {
	while(!mPlotterItems.isEmpty()) {
		PlotterItem *item = mPlotterItems.at(0);
		mPlotterItems.removeAll(item);
		delete item;
	}
}


/**
 * Called when the Widget should draw its content.
 */
void PlotterWidget::paintEvent(QPaintEvent *event) {
	QFrame::paintEvent(event);

	if(!isVisible()) {
		return;
	}

	if(mPerformanceMode != 0 && mPerformanceMode->get()) {
		QPainter p(this);
		p.fillRect(0, 0, width(), height(), QColor(0, 255, 255));
		return;
	}

	QPainter p(this);
	p.fillRect(0, 0, width(), height(), Qt::white);

	mDrawableHeight = ((double) height() - 20);
	mDrawableWidth = width() - (lineWidth() * 2);

	drawCoordinateSystem(p);
	drawHistoryGraphs(p);
	drawCoordinateRanges(p);

	if(mPlotLegend) {
		drawLegend(p);
	}

	
}


/**
 * Draw a simple coordinate system with some orientation lines.
 *
 * @param painter the painter to use when drawing.
 */
void PlotterWidget::drawCoordinateSystem(QPainter &painter) {
	//Draw the raster of the diagram.
	double scale = mDrawableHeight / 2.0;
	int w = (int) mDrawableWidth;

	painter.setPen(Qt::lightGray);
	painter.drawLine(lineWidth(), 9, w, 9);
	painter.drawLine(lineWidth(), (int) (scale) + 10, w, (int) (1 * scale) + 10);
	painter.drawLine(lineWidth(), (int) (2 * scale) + 10, w, (int) (2 * scale) + 10);
	painter.drawLine(lineWidth(), (int) (0.5 * scale) + 10, w, (int) (0.5 * scale) + 10);
	painter.drawLine(lineWidth(), (int) (1.5 * scale) + 10, w, (int) (1.5 * scale) + 10);

}


/**
 * Plots the history graphs of all visible PlotterItems.
 *
 * @param painter the painter to use when drawing.
 */
void PlotterWidget::drawHistoryGraphs(QPainter &painter) {

	if(mNumberOfValuesToPlot == 0) {
		return;
	}

	double pointDist = ((double) mDrawableWidth) / ((double) mNumberOfValuesToPlot);

	{
		//Lock PlotterItem list.
		QMutexLocker guard(&mUpdateMutex);

		for(int i = 0; i < mPlotterItems.size(); ++i) {
			PlotterItem *item = mPlotterItems.at(i);

			//Ignore all invisible PlotterItems.
			if(item == 0 || !item->isVisible()) {
				continue;
			}
			if(item->hasStaticHistory() && !mPlotStaticItems) {
				continue;
			}

			const QVector<double> &data = item->getHistory();

			//set PlotterItem color.
			painter.setPen(item->getColor());

			double x = 0.0;
			double y = 0.0;
			double old_x = 0.0;
			double old_y = 0.0;
			int mPointCounter = 0;

			//Plot history of current Item.
			double localPointDist = item->getScalingH() * pointDist;

			double drawStartingPosition = mDrawableWidth + ((double) lineWidth())
								+ (item->getOffsetH() * localPointDist);

			int currentIndex = item->getHistoryStartIndex() - 1;
			for(int j = data.size() - 1; j >= 0; --j) {
				//TODO check how to optimize: draw only points visible on the plotter.

				if(currentIndex < 0) {
					currentIndex = data.size() - 1;
				}
				x = drawStartingPosition - (((double) mPointCounter) * localPointDist);
				y = translateValue(data[currentIndex],
									item->getOffsetV(),
									item->getScalingV());

				if(mPointCounter != 0) {
					painter.drawLine((int) x, (int) y, (int) old_x, (int) old_y);
				}

				mPointCounter++;
				old_x = x;
				old_y = y;

				--currentIndex;
			}
		}
	}
}


/**
 * Draws the coordinate range numbers.
 *
 * @param painter the painter to use when drawing.
 */
void PlotterWidget::drawCoordinateRanges(QPainter &painter) {

	//prevent division by zero
	if(mUserScaleV == 0.0 || mUserScaleH == 0.0) {
		return;
	}

	double scale = mDrawableHeight / 2.0;

	painter.setPen(Qt::black);

	QString upperRange = QString::number((1.0 / mUserScaleV) + mUserOffsetV);
	QString midRange = QString::number(mUserOffsetV);
	QString lowerRange = QString::number((-1.0 / mUserScaleV) + mUserOffsetV);

	painter.drawText(9, 13, upperRange);
	painter.drawText(9, 14 + (int) scale, midRange);
	painter.drawText(9, 13 + (int) (2 * scale), lowerRange);
}


/**
 * Draws the names of all visible PlooterItems in their color as legend.
 *
 * @param painter the painter to use when drawing.
 */
void PlotterWidget::drawLegend(QPainter &painter) {

	int row = 0;
	for(int i = 0; i < mPlotterItems.size(); ++i) {
		PlotterItem *item = mPlotterItems.at(i);

		if(item == 0 || !item->isVisible()) {
			continue;
		}

		painter.setPen(item->getColor());
		QString name = item->getName();
		int maxSize = name.count();
		if(maxSize > mMaxValueNameSize) {
			maxSize = mMaxValueNameSize;
		}
		painter.drawText(10, 26 + (15 * row), name.mid(name.size() - maxSize));
		row++;
	}
}


/**
 * Calculates the vertical position of a PlotterItem value, taking into account
 * for the offset and scaling of the PlotterWidget (UserScaling and UserOffset).
 * and the offset of the PlotterItem itself.
 *
 * @param value the value to translate.
 * @param localOffsetV the local vertical offset of the PlotterItem.
 * @param localOffsetH the local horizontal offset of the PlotterItem.
 * @return the transformed value.
 */
double PlotterWidget::translateValue(double value, double localOffsetV,
			double localScaleV)
{
	value = ((value - localOffsetV) * localScaleV);

	return (((((value - mUserOffsetV) * mUserScaleV) - 1.0) * -1.0)
			* (mDrawableHeight / 2.0)) + 10.0;
}

void PlotterWidget::mouseDoubleClickEvent(QMouseEvent *event) {
	if(width() == 0) {
		return;
	}
	double xPos = (((double) width()) - ((double) event->x())) * (((double) mHistorySize) / ((double) width()));
	cerr << "X-Pos: " << xPos << endl;
}


}


