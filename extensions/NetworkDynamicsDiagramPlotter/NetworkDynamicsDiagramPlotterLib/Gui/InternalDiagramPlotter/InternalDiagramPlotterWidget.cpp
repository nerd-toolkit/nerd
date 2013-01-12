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




#include "InternalDiagramPlotterWidget.h"
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
#include "Math/Math.h"
#include <qvarlengtharray.h>
#include <QSvgGenerator>
#include <QTransform>
#include <QPen>


using namespace std;


namespace nerd {

InternalDiagramPlotterWidget::InternalDiagramPlotterWidget(QWidget *parent)
	: QFrame(parent), mPerformanceMode(0), mMajorTickInterval(0.5), mMinorTickInterval(10),
	  mHorizontalOffset(0.0),  mDiagramMode(false), mShowDiagramLines(true),
	  mLineWidth(0), mLineColor(Qt::black)
{
	moveToThread(QCoreApplication::instance()->thread());

	setFrameStyle(QFrame::Box | QFrame::Plain);
    setLineWidth(2);
	
	mAutoScale = new BoolValue(true);

	mDrawableHeight = 0.0;
	mDrawableWidth = 0.0;
	mUserScale = 1.0;
	mUserOffsetX = 0.0;
	mUserOffsetY = 0.0;

	mPerformanceMode = Core::getInstance()->getValueManager()
				->getBoolValue(NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE);
				
				
	mPointColors.append(QColor("white"));
	mPointColors.append(QColor("black"));
	mPointColors.append(QColor("red"));
	
				
	mMatrix.resize(500, 400, 1);
	for(int i = 0; i < 500; ++i) {
		for(int j = 0; j < 400; ++j) {
			if(i == 0) {
				mMatrix.set(i * 0.01);
			}
			else {
				if(j == 0) {
					mMatrix.set(-2.0 + (j * 0.01), i, j, 0);
				}
				else {
					mMatrix.set((i+j) % 3, i, j, 0);
				}
			}
		}
	}
}


/**
 * Destructor.
 * Calls clear() and hereby removes and destroys all PlotterItems.
 */
InternalDiagramPlotterWidget::~InternalDiagramPlotterWidget() {
	clearMatrix();
}



/**
 * Initiates plotting of the Widget.
 */
void InternalDiagramPlotterWidget::plot() {
	update();
}


/**
 * Updates the History of all PlotterItems.
 */
void InternalDiagramPlotterWidget::updateMatrix(const Matrix &matrix) {

	//Lock the PlotterItem list
	QMutexLocker guard(&mUpdateMutex);

	mMatrix = matrix;
}


void InternalDiagramPlotterWidget::clearMatrix() {
	
	//Lock the PlotterItem list
	QMutexLocker guard(&mUpdateMutex);
	
	mMatrix.clear();
}

void InternalDiagramPlotterWidget::centerDiagram() {
	
}

double InternalDiagramPlotterWidget::getUserScale() const {
	return mUserScale;
}

void InternalDiagramPlotterWidget::setUserScale(double scale) {
	mUserScale = scale;
	emit userScaleChanged(mUserScale);
}

double InternalDiagramPlotterWidget::getUserOffsetY() const {
	return mUserOffsetX;
}

double InternalDiagramPlotterWidget::getUserOffsetX() const {
	return mUserOffsetY;
}

void InternalDiagramPlotterWidget::setUserOffset(double offsetX, double offsetY) {
	mUserOffsetX = offsetX;
	mUserOffsetY = offsetY;
	emit userOffsetChanged(mUserOffsetX, mUserOffsetY);
}


bool InternalDiagramPlotterWidget::loadMatrixFromFile(const QString &fileName) {
	return false;
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
bool InternalDiagramPlotterWidget::saveMatrixToFile(const QString &fileName) {

// 	QFile file(fileName);
// 
// 	if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
// 		qDebug("Could not open file %s to write histories.",
// 				fileName.toStdString().c_str());
// 		file.close();
// 		return false;
// 	}
// 	QTextStream output(&file);
// 	int largestHistorySize = 0;
// 	QVector<PlotterItem*> items;
// 
// 	//Lock the PlotterItem list.
// 	QMutexLocker guard(&mUpdateMutex);
// 
// 	//Add the names of the PlotterItems.
// 	for(int i = 0; i < mPlotterItems.size(); ++i) {
// 		PlotterItem *item = mPlotterItems.at(i);
// 		if((ignoreStaticData && item->hasStaticHistory()) || !item->isVisible()) {
// 			continue;
// 		}
// 		output << "# " << item->getName() << endl;
// 		items.append(item);
// 		if(item->getHistory().size() > largestHistorySize) {
// 			largestHistorySize = item->getHistory().size();
// 		}
// 	}
// 	output << endl << endl;
// 
// 	//Add the histories of all PlotterItems.
// 	for(int j = largestHistorySize; j > 0; --j) {
// 		for(int i = 0; i < items.size(); ++i) {
// 			PlotterItem *item = items.at(i);
// 			output << item->getHistoryValue(item->getHistory().size() - j);
// 			if(i < (items.size() - 1)) {
// 				output << ";";
// 			}
// 		}
// 		output << "\n";
// 	}
// 	file.close();
// 	return true;

	return false;
}

void InternalDiagramPlotterWidget::saveDiagramToSvg(const QString &fileName) {
	
// 	QSvgGenerator generator;
// 	generator.setFileName(fileName);
// 	//generator.setSize(QSize(width() - (2.0 * lineWidth()), height() - (2 * lineWidth())));
// 	generator.setTitle(tr("NERD Plotter Diagram"));
// 	generator.setDescription(tr(""));
// 	
// 	QPainter p;
// 	p.begin(&generator);
// 	
// 	p.fillRect(0, 0, width() + 5, height(), Qt::white);
// 	
// 	if(mDiagramMode) {
// 		mDrawableHeight = ((double) height() - 40);
// 		mDrawableWidth = width() - (lineWidth() * 2) - 30.0 - mDiagramShift;
// 		mHorizontalOffset = 30.0 + mDiagramShift;
// 	}
// 	else {
// 		mDrawableHeight = ((double) height() - 20);
// 		mDrawableWidth = width() - (lineWidth() * 2);
// 		mHorizontalOffset = 0.0;
// 	}
// 
// 	drawCoordinateSystem(p);
// 	//drawHistoryGraphs(p);
// 	drawCoordinateRanges(p);
// 
// 	if(mPlotLegend) {
// 		drawLegend(p);
// 	}
// 	
// 	p.end();
}


void InternalDiagramPlotterWidget::setTickIntervals(double major, int minor) {
	mMajorTickInterval = major;
	mMinorTickInterval = minor;
}


double InternalDiagramPlotterWidget::getMajorTickInterval() const {
	return mMajorTickInterval;
}

int InternalDiagramPlotterWidget::getMinorTickInterval() const {
	return mMinorTickInterval;
}

void InternalDiagramPlotterWidget::enableDiagramMode(bool enable) {
	mDiagramMode = enable;
}


bool InternalDiagramPlotterWidget::isInDiagramMode() const {
	return mDiagramMode;
}

void InternalDiagramPlotterWidget::showDiagramLines(bool show) {
	mShowDiagramLines = show;
}


bool InternalDiagramPlotterWidget::isShowingDiagramLines() const {
	return mShowDiagramLines;
}



void InternalDiagramPlotterWidget::setDiagramLineDashPattern(const QVector<qreal> &dashPattern) {
	mDiagramLineDashPattern = dashPattern;
}


QVector<qreal> InternalDiagramPlotterWidget::getDiagramLineDashPattern() const {
	return mDiagramLineDashPattern;
}


void InternalDiagramPlotterWidget::setLineWidth(double lineWidth) {
	mLineWidth = lineWidth;
}


double InternalDiagramPlotterWidget::getLineWidth() const {
	return mLineWidth;
}

void InternalDiagramPlotterWidget::setLineColor(const QColor &color) {
	mLineColor = color;
}


QColor InternalDiagramPlotterWidget::getLineColor() const {
	return mLineColor;
}
		
void InternalDiagramPlotterWidget::enableAutoScale(bool enable) {
	mAutoScale->set(enable);
}


bool InternalDiagramPlotterWidget::isUsingAutoScale() const{
	return mAutoScale->get();
}


/**
 * Called when the Widget should draw its content.
 */
void InternalDiagramPlotterWidget::paintEvent(QPaintEvent *event) {
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
	
	if(mDiagramMode) {
		mDrawableHeight = ((double) height() - 40);
		mDrawableWidth = width() - (lineWidth() * 2) - 30.0 - mDiagramShift;
		mHorizontalOffset = 30.0 + mDiagramShift;
	}
	else {
		mDrawableHeight = ((double) height() - (lineWidth() * 2));
		mDrawableWidth = width() - (lineWidth() * 2);
		mHorizontalOffset = 0.0;
		mVerticalOffset = lineWidth();
	}

	//TODO: break drawCoordinateSystem down to two separate methods (inner lines, outer frame).
	//Then draw histroy graphs from here again.

	drawCoordinateSystem(p);
	drawMatrix(p);
	//drawHistoryGraphs(p); //Note: this also affects the FirstReturnMap plotter!
// 	drawCoordinateRanges(p);

// 	if(mPlotLegend) {
// 		drawLegend(p);
// 	}

	
}


/**
 * Draw a simple coordinate system with some orientation lines.
 *
 * @param painter the painter to use when drawing.
 */
void InternalDiagramPlotterWidget::drawCoordinateSystem(QPainter &painter) {
	//Draw the raster of the diagram.
	double scale = mDrawableHeight / 2.0;
	int w = (int) mDrawableWidth + mHorizontalOffset + (3 * mLineWidth);
	
	QPen solidLinePen(Qt::black, mLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	QPen dashedPen(mLineColor, mLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	dashedPen.setDashPattern(mDiagramLineDashPattern);
	
	if(mShowDiagramLines) {
		painter.setPen(dashedPen);
		
		painter.drawLine(lineWidth() + mHorizontalOffset, (int) (scale) + 10, 
						w, (int) (1 * scale) + 10);
		
		painter.drawLine(lineWidth() + mHorizontalOffset, (int) (0.5 * scale) + 10, 
						w, (int) (0.5 * scale) + 10);
		painter.drawLine(lineWidth() + mHorizontalOffset, (int) (1.5 * scale) + 10, 
						w, (int) (1.5 * scale) + 10);
		
		painter.setPen(solidLinePen);
	}
	
	painter.setPen(solidLinePen);
	painter.drawLine(lineWidth() + mHorizontalOffset, 9, w, 9);
	painter.drawLine(lineWidth() + mHorizontalOffset, (int) (2 * scale) + 10, 
					 w, (int) (2 * scale) + 10 );
	
	//TODO try to ensure that the lines plot over the graphs (except of the lower line)
	//draw history graphs before frame so that the frame is on top.
// 	drawHistoryGraphs(painter);
/*	
	painter.setPen(solidLinePen);
// 	painter.drawLine(lineWidth() + mHorizontalOffset, 9, w, 9);
// 	painter.drawLine(lineWidth() + mHorizontalOffset, (int) (2 * scale) + 10, 
// 					 w, (int) (2 * scale) + 10 );
	painter.drawLine(lineWidth() + mHorizontalOffset, 9, 
					 lineWidth() + mHorizontalOffset, (int) (2 * scale) + 10);
	if(mDiagramMode) {
		painter.drawLine(w, 9, 
						w, (int) (2 * scale) + 20);
	}
	else {
		painter.drawLine(w, 9, 
						w, (int) (2 * scale) + 10);
	}
	
	
	double tickScaling = ((double) mDrawableWidth) / ((double) mHistorySize);
	
	for(int i = 0; i < mHistorySize; ++i) {
		if((i % mMajorTickInterval) == 0) {
			if(mShowDiagramLines) {
				if(mDiagramMode) {
					painter.setPen(dashedPen);
					painter.drawLine(lineWidth() + (i * tickScaling) + mHorizontalOffset, 9, 
								lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 10);
					
					painter.setPen(solidLinePen);
					painter.drawLine(lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 10, 
								lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 20);
				}
				else {
					painter.setPen(dashedPen);
					painter.drawLine(lineWidth() + (i * tickScaling) + mHorizontalOffset, 9, 
								lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 10);
					painter.setPen(solidLinePen);
				}
			}
			else {
// 				painter.drawLine(lineWidth() + (i * tickScaling) + mHorizontalOffset,  mDrawableHeight + 20, 
// 								lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 10);
			}
		}
		else if((i % (mMajorTickInterval / mMinorTickInterval)) == 0 && mDiagramMode) {
			painter.setPen(solidLinePen);
			painter.drawLine(lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 15, 
							 lineWidth() + (i * tickScaling) + mHorizontalOffset, mDrawableHeight + 10);
		}
	}*/

}


/**
 * Plots the history graphs of all visible PlotterItems.
 *
 * @param painter the painter to use when drawing.
 */
void InternalDiagramPlotterWidget::drawMatrix(QPainter &painter) {

	QTransform oldTransform = painter.transform();
	QPen oldPen = painter.pen();
	
	//autoscale
	if(mAutoScale->get()) {
		double scaleX = ((double) mDrawableWidth)  / ((double) mMatrix.getWidth());
		double scaleY = ((double) mDrawableHeight) / ((double) mMatrix.getHeight());
		
		mUserScale = Math::min(scaleX, scaleY);
	}
	
	
	painter.translate(mHorizontalOffset + mUserOffsetX, mVerticalOffset + mUserOffsetY);
	painter.scale(mUserScale, mUserScale);
	
	
	QMutexLocker guard(&mUpdateMutex);
	
	for(int i = 1; i < mMatrix.getWidth(); ++i) {
		for(int j = 1; j < mMatrix.getHeight(); ++j) {
// 			QPen pen(getColor(mMatrix.get(i, j, 0)));
// 			pen.setWidth(1);
// 			painter.setPen(pen);
			painter.drawPoint(i - 1, j - 1);
			//painter.fillRect(i - 1, j - 1, 1, 1, );
		}
	}
	
	
	painter.setTransform(oldTransform);
	
	//always draw on 1:1 scale (and transform the painter).
	
	
// 	double pointDist = ((double) mDrawableWidth) / ((double) mNumberOfValuesToPlot);
// 
// 	{
// 		//Lock PlotterItem list.
// 		QMutexLocker guard(&mUpdateMutex);
// 
// 		for(int i = 0; i < mPlotterItems.size(); ++i) {
// 			PlotterItem *item = mPlotterItems.at(i);
// 
// 			//Ignore all invisible PlotterItems.
// 			if(item == 0 || !item->isVisible()) {
// 				continue;
// 			}
// 			if(item->hasStaticHistory() && !mPlotStaticItems) {
// 				continue;
// 			}
// 
// 			QVector<double> data = item->getHistory();
// 			
// 			QPen oldPen = painter.pen();
// 			
// 			//QPen pen = item->getColor();
// 			QPen pen(item->getColor(), mLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
// 			pen.setDashPattern(item->getDashPattern());
// 			painter.setPen(pen);
// 
// 			//set PlotterItem color.
// 			//painter.setPen(item->getColor());
// 
// 			double x = 0.0;
// 			double y = 0.0;
// 			int mPointCounter = 0;
// 
// 			//Plot history of current Item.
// 			double localPointDist = item->getScalingH() * pointDist;
// 
// 			double drawStartingPosition = mDrawableWidth + mHorizontalOffset 
// 								+ ((double) lineWidth())
// 								+ (item->getOffsetH() * localPointDist);
// 								
// 			QPainterPath path;
// 
// 			int currentIndex = item->getHistoryStartIndex() - 1;
// 			for(int j = data.size() - 1; j >= 0; --j) {
// 				//TODO check how to optimize: draw only points visible on the plotter.
// 
// 				if(currentIndex < 0) {
// 					currentIndex = data.size() - 1;
// 				}
// 				x = drawStartingPosition - (((double) mPointCounter) * localPointDist);
// 				y = translateValue(data[currentIndex],
// 									item->getOffsetV(),
// 									item->getScalingV());
// 				
// 				if(mPointCounter == 0) {
// 					path = QPainterPath(QPointF(x, y));
// 				}
// 				else {
// 					path.lineTo(x, y);
// 				}
// 
// 				mPointCounter++;
// 
// 				--currentIndex;
// 			}
// 			painter.drawPath(path);
// 			painter.setPen(oldPen);
// 		}
// 	}
}


/**
 * Draws the coordinate range numbers.
 *
 * @param painter the painter to use when drawing.
 */
void InternalDiagramPlotterWidget::drawCoordinateRanges(QPainter &painter) {

// 	//prevent division by zero
// 	if(mUserScaleV == 0.0 || mUserScaleH == 0.0) {
// 		return;
// 	}
// 
// 	double scale = mDrawableHeight / 2.0;
// 
// 	QPen pen(Qt::black, mLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
// 	painter.setPen(pen);
// 
// 	QString upperRange = QString::number((1.0 / mUserScaleV) + mUserOffsetV);
// 	QString midRange = QString::number(mUserOffsetV);
// 	QString lowerRange = QString::number((-1.0 / mUserScaleV) + mUserOffsetV);
// 
// 	painter.drawText(2, 13, upperRange);
// 	painter.drawText(2, 14 + (int) scale, midRange);
// 	painter.drawText(2, 13 + (int) (2 * scale), lowerRange);
// 	
// 	if(mDiagramMode) {
// 		double tickScaling = ((double) mDrawableWidth) / ((double) mHistorySize);
// 		for(int i = 0; i < mHistorySize; ++i) {
// 			if((i % mMajorTickInterval) == 0) {
// 				painter.drawText(lineWidth() + (i * tickScaling) + mHorizontalOffset - 50, 
// 								mDrawableHeight + 22, 100, 35, Qt::AlignHCenter | Qt::AlignTop,
// 								QString::number(i));
// 			}
// 		}
// 		if( mHistorySize % mMajorTickInterval == 0) {
// 			painter.drawText(lineWidth() + mHorizontalOffset + mDrawableWidth - 100, 
// 								mDrawableHeight + 22, 100, 35, Qt::AlignRight | Qt::AlignTop,
// 								QString::number(mHistorySize));
// 		}
// 	}
}




/**
 * Calculates the vertical position of a PlotterItem value, taking into account
 * for the offset and scaling of the InternalDiagramPlotterWidget (UserScaling and UserOffset).
 * and the offset of the PlotterItem itself.
 *
 * @param value the value to translate.
 * @param localOffsetV the local vertical offset of the PlotterItem.
 * @param localOffsetH the local horizontal offset of the PlotterItem.
 * @return the transformed value.
 */
// double InternalDiagramPlotterWidget::translateValue(double value, double localOffsetV,
// 			double localScaleV)
// {
// 	value = ((value - localOffsetV) * localScaleV);
// 
// 	return (((((value - mUserOffsetV) * mUserScaleV) - 1.0) * -1.0)
// 			* (mDrawableHeight / 2.0)) + 10.0;
// }

void InternalDiagramPlotterWidget::mouseDoubleClickEvent(QMouseEvent *event) {
// 	if(width() == 0) {
// 		return;
// 	}
// 	double xPos = (((double) width()) - ((double) event->x())) * (((double) mHistorySize) / ((double) width()));
// 	cerr << "X-Pos: " << xPos << endl;
}

const QColor& InternalDiagramPlotterWidget::getColor(int index) const {
	return mPointColors.at(index % mPointColors.size());
}

void InternalDiagramPlotterWidget::hideEvent(QHideEvent *event) {
	
}


void InternalDiagramPlotterWidget::showEvent(QShowEvent *event) {
		
}
void InternalDiagramPlotterWidget::keyPressEvent(QKeyEvent *event) {
		
}


void InternalDiagramPlotterWidget::mouseMoveEvent(QMouseEvent *event) {
		
}

void InternalDiagramPlotterWidget::mousePressEvent(QMouseEvent *event) {
		
}

void InternalDiagramPlotterWidget::mouseReleaseEvent(QMouseEvent *event) {
		
}

void InternalDiagramPlotterWidget::wheelEvent(QWheelEvent *event) {
	double factor = 1.1;
	double scale = mUserScale;
	
	mAutoScale->set(false);
	
	int count = event->delta() > 0 ? -1 : 1;

	if(count == 0) {
		return;
	}
	if(count < 0) {
		scale *= (count * factor * -1.0);
	}
	else {
		scale /= (count * factor);
	}
	mUserScale = scale;
	repaint();
}

}


