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



#include "PlotterItem.h"

#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

namespace nerd {

PlotterItem::PlotterItem(const QString &name, const QColor &color)
	: mHistoryStartPosition(0),
		mHistoryMaxCapacity(200), mScalingV(1.0),
		mScalingH(1.0), mOffsetV(0.0), mOffsetH(0.0),
		mName(name), mColor(color),
		mHasStaticHistory(false), mVisible(true)
{
}

PlotterItem::~PlotterItem() {
}


/**
 * Does nothing by default. Can be used to update the plotter item's history.
 */
void PlotterItem::update() {
}


/**
 * Adds a double to the history.
 *
 * @param value the double to add.
 */
void PlotterItem::addToHistory(double value) {

	if(mHistory.size() < mHistoryMaxCapacity) {
		mHistoryStartPosition = 0;
		mHistory.append(value);
	}
	else {
		mHistory[mHistoryStartPosition] = value;
		mHistoryStartPosition++;
		if(mHistoryStartPosition >= mHistoryMaxCapacity) {
			mHistoryStartPosition = 0;
		}
	}
}



/**
 * Returns the history value with a given index.
 *
 * @param index the index of the requested value.
 * @return the history value at the index..
 */
double PlotterItem::getHistoryValue(int index) {
	if((index < 0) || (index >= mHistory.size())) {
		return 0.0;
	}
	return mHistory[(index + mHistoryStartPosition) % mHistory.size()];
}


/**
 * Sets the maximal history size.
 * If the current history contains more data than allowed by the
 * new maximal size, then the history is truncated to fit the new
 * size constraints.
 *
 * @param capacity the maximal history capacity.
 */
void PlotterItem::setHistoryCapacity(int capacity) {

	if(mHistoryMaxCapacity == capacity) {
		return;
	}

	QVector<double> copy(mHistory);
	mHistory.clear();

	int index = mHistoryStartPosition;
	for(int i = 0; i < capacity && i < copy.size(); ++i) {
		index--;
		if(index < 0) {
			index = copy.size() - 1;
		}
		mHistory.prepend(copy[index]);
	}

	mHistoryStartPosition = 0;
	mHistoryMaxCapacity = capacity;
}


/**
 * Returns the maximal history capacity. The history nevertheless
 * may be smaller than this.
 *
 * @return the maximal history size.
 */
int PlotterItem::getHistoryCapacity() const {
	return mHistoryMaxCapacity;
}


/**
 * Returns a reference to the current value history.
 * @return the history.
 */
QVector<double> PlotterItem::getHistory() const {
	return mHistory;
}

/**
 * Sets the history from a vector, starting with index 0.
 * If history is larger than allowed by mHistoryMaxCapacity,
 * then the oldest elements in the history are not added to the
 * value history.
 *
 * @param history the new history.
 */
void PlotterItem::setHistory(QVector<double> history) {
	int startIndex = 0;
	if(history.size() > mHistoryMaxCapacity) {
		startIndex = history.size() - mHistoryMaxCapacity;
	}
	//TODO test if startindex is correct.
	mHistory.clear();
	for(int i = startIndex; i < history.size(); ++i) {
		mHistory.append(history.at(i));
	}
}

/**
 * Returns the index within the history where the values
 * start. This start position may be shifted around in the
 * history for performance reasons. (Ring Buffer)
 *
 * It can be assumed that the entire mHistoryMaxCapacity values
 * represent valid data when this starting index is not 0.
 *
 * @return the start index of the history values.
 */
int PlotterItem::getHistoryStartIndex() const {
	return mHistoryStartPosition;
}





/**
 * Sets the name of the PlotterItem.
 * @param name the new name.
 */
void PlotterItem::setName(const QString &name) {
	mName = name;
}


/**
 * Returns the name of the PlotterItem.
 * @return the name of the PloterItem.
 */
const QString& PlotterItem::getName() const {
	return mName;
}


/**
 * Sets the color to be used while plotting the history.
 * @param color the new color.
 */
void PlotterItem::setColor(const QColor &color) {
	mColor = color;
}


/**
 * Returns the color used to plot the represented value history.
 * @return the plot color.
 */
const QColor& PlotterItem::getColor() const {
	return mColor;
}

/**
 *  Sets the dash pattern to be used to draw this graph.
 */
void PlotterItem::setDashPattern(const QVector<qreal> &pattern) {
	mDashPattern = pattern;
}


/**
 * Returns the dash pattern used to draw this graph.
 */
QVector<qreal> PlotterItem::getDashPattern() const {
	return mDashPattern;
}

/**
 * Sets the history to static or dynamic. A static history is NOT extended
 * when upate() is called, while a dynamic history is extended by a new
 * value whenever update() in this situation.
 *
 * @param staticHistory if true the history is static, otherwise it is dynamic.
 */
void PlotterItem::setStaticHistory(bool staticHistory) {
	mHasStaticHistory = staticHistory;
}


/**
 * Returns true if the history is static.
 * @return true if the history is static, otherwise false.
 */
bool PlotterItem::hasStaticHistory() const {
	return mHasStaticHistory;
}


/**
 * Sets the visibility of the PlotterItem. If visible is false, then the plotter
 * item will still be udpated, but it is not drawn on the plotter.
 *
 * @param visible to enable drawing set to true, otherwise to false.
 */
void PlotterItem::setVisible(bool visible) {
	mVisible = visible;
}


/**
 * Returns true if the PlotterItem currently is visible, thus whether it is
 * drawn in the plotter.
 *
 * @return true if the PlotterItem is visible, otherwise false.
 */
bool PlotterItem::isVisible() const {
	return mVisible;
}


void PlotterItem::setScalingV(double scaling) {
	mScalingV = scaling;
}


void PlotterItem::setScalingH(double scaling) {
	mScalingH = scaling;
}


void PlotterItem::setOffsetV(double offset) {
	mOffsetV = offset;
}


void PlotterItem::setOffsetH(double offset) {
	mOffsetH = offset;
}


double PlotterItem::getScalingV() const {
	return mScalingV;
}


double PlotterItem::getScalingH() const {
	return mScalingH;
}


double PlotterItem::getOffsetV() const {
	return mOffsetV;
}


double PlotterItem::getOffsetH() const {
	return mOffsetH;
}




}




