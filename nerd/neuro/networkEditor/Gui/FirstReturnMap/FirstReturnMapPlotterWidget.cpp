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



#include "FirstReturnMapPlotterWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QCoreApplication>
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new FirstReturnMapPlotterWidget.
 */
FirstReturnMapPlotterWidget::FirstReturnMapPlotterWidget()
	: mPenWidth(2.0), mConnectPoints(false)
{
	moveToThread(QCoreApplication::instance()->thread());
}


/**
 * Destructor.
 */
FirstReturnMapPlotterWidget::~FirstReturnMapPlotterWidget() {
}


void FirstReturnMapPlotterWidget::setPenWidth(double width) {
	mPenWidth = Math::max(0.0, width);
}


void FirstReturnMapPlotterWidget::enableConnectedPoints(bool enable) {
	mConnectPoints = enable;
}


void FirstReturnMapPlotterWidget::drawCoordinateSystem(QPainter &painter) {
	//Draw the raster of the diagram.
// 	double scale = mDrawableHeight / 2.0;
// 	int w = (int) mDrawableWidth;
// 
// 	painter.setPen(Qt::lightGray);
// 	painter.drawLine(lineWidth(), 9, w, 9);
// 	painter.drawLine(lineWidth(), (int) (scale) + 10, w, (int) (1 * scale) + 10);
// 	painter.drawLine(lineWidth(), (int) (2 * scale) + 10, w, (int) (2 * scale) + 10);
// 	painter.drawLine(lineWidth(), (int) (0.5 * scale) + 10, w, (int) (0.5 * scale) + 10);
// 	painter.drawLine(lineWidth(), (int) (1.5 * scale) + 10, w, (int) (1.5 * scale) + 10);

	drawHistoryGraphs(painter);
}

/**
 * Plots the history graphs of all visible PlotterItems.
 *
 * @param painter the painter to use when drawing.
 */
void FirstReturnMapPlotterWidget::drawHistoryGraphs(QPainter &painter) {

	if(mNumberOfValuesToPlot == 0) {
		return;
	}

	{
		//Lock PlotterItem list.
		QMutexLocker guard(&mUpdateMutex);

// 		if(mPlotterItems.size() == 1) {
		for(int i = 0; i < mPlotterItems.size(); ++i) {

			PlotterItem *item = mPlotterItems.at(i);

			if(!item->isVisible()) {
				continue;
			}

			const QVector<double> &data = item->getHistory();

			//set PlotterItem color.
			QColor color = item->getColor();
			QPen pen = QPen(color);
			pen.setWidthF(mPenWidth);

			int prevX = 0;
			int prevY = 0;

			int counter = 0;
			int currentIndex = item->getHistoryStartIndex() - 1;
			for(int j = 1; j < data.size(); ++j) {
				double x = 0.0;
				double y = 0.0;
				if(currentIndex < 0) {
					currentIndex = data.size() - 1;
				}
				if(currentIndex == 0) {
					x = (data[data.size() - 1] + 1.0) / 2.0 * width();
					y = (data[0] + 1.0) / 2.0 * ((double) height());
				}
				else {
					x = (data[currentIndex-1] + 1.0) / 2.0 * width();
					y = (data[currentIndex] + 1.0) / 2.0 * ((double) height());
				}

				color.setAlphaF(1.0 - ((1.0 / (data.size()) * counter)));
				pen.setColor(color);
				painter.setPen(pen);

				painter.drawPoint((int) x, (int) y);
				if(mConnectPoints && counter > 0) {
					painter.drawLine((int) x, (int) y, prevX, prevY);
				}
				prevX = (int) x;
				prevY = (int) y;

				++counter;
				--currentIndex;
			}
		}
// 		else if(mPlotterItems.size() == 2) {
// 			
// 		}
	}
}


/**
 * Draws the coordinate range numbers.
 *
 * @param painter the painter to use when drawing.
 */
void FirstReturnMapPlotterWidget::drawCoordinateRanges(QPainter&) {

// 	//prevent division by zero
// 	if(mUserScaleV == 0.0 || mUserScaleH == 0.0) {
// 		return;
// 	}
// 
// 	double scale = mDrawableHeight / 2.0;
// 
// 	painter.setPen(Qt::black);
// 
// 	QString upperRange = QString::number((1.0 / mUserScaleV) + mUserOffsetV);
// 	QString midRange = QString::number(mUserOffsetV);
// 	QString lowerRange = QString::number((-1.0 / mUserScaleV) + mUserOffsetV);
// 
// 	painter.drawText(9, 13, upperRange);
// 	painter.drawText(9, 14 + (int) scale, midRange);
// 	painter.drawText(9, 13 + (int) (2 * scale), lowerRange);

}


}



