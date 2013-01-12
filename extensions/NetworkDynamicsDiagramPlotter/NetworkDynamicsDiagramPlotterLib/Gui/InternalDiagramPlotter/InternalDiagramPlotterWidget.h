

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



#ifndef NERDInternalDiagramPlotterWidget_H
#define NERDInternalDiagramPlotterWidget_H


#include "Math/Matrix.h"
#include <QTimer>
#include <QFrame>
#include <QPaintEvent>
#include <QList>
#include <QMutex>
#include "Value/BoolValue.h"
#include "Value/ValueChangedListener.h"

namespace nerd {

	/**
	 * InternalDiagramPlotterWidget
	 */
	class InternalDiagramPlotterWidget : public QFrame {
	Q_OBJECT

	public:
		InternalDiagramPlotterWidget(QWidget *parent = 0);
		virtual ~InternalDiagramPlotterWidget();

		bool loadMatrixFromFile(const QString &fileName);
		bool saveMatrixToFile(const QString &fileName);
		void saveDiagramToSvg(const QString &fileName);
		
		double getUserScale() const;
		void setUserScale(double scale);
		double getUserOffsetX() const;
		double getUserOffsetY() const;
		void setUserOffset(double offsetX, double offsetY);
		void setTickIntervals(double major, int minor);
		double getMajorTickInterval() const;
		int getMinorTickInterval() const;
		void enableDiagramMode(bool enable);
		bool isInDiagramMode() const;
		void showDiagramLines(bool show);
		bool isShowingDiagramLines() const;
		void setDiagramLineDashPattern(const QVector<qreal> &dashPattern);
		QVector<qreal> getDiagramLineDashPattern() const;
		void setLineWidth(double lineWidth);
		double getLineWidth() const;
		void setLineColor(const QColor &color);
		QColor getLineColor() const;
		void enableAutoScale(bool enable);
		bool isUsingAutoScale() const;

	public slots:
		void updateMatrix(const Matrix &matrix);
		void plot();
		void clearMatrix();
		void centerDiagram();

	signals:
		void userScaleChanged(double currentScale);
		void userOffsetChanged(double currentOffsetX, double currentOffsetY);


	protected:
		virtual void paintEvent(QPaintEvent *event);

		virtual void drawCoordinateSystem(QPainter &painter);
		virtual void drawMatrix(QPainter &painter);
		virtual void drawCoordinateRanges(QPainter &painter);
		//virtual double translateValue(double value, double offsetX, offsetY, double scale);
		virtual void mouseDoubleClickEvent(QMouseEvent *event);
		
		const QColor& getColor(int index) const;
		
	protected:
		virtual void hideEvent(QHideEvent *event);
		virtual void showEvent(QShowEvent *event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void wheelEvent(QWheelEvent *event);

	protected:
		Matrix mMatrix;
		double mDrawableHeight;
		double mDrawableWidth;
		double mUserScale;
		double mUserOffsetX;
		double mUserOffsetY;
		QMutex mUpdateMutex;
		BoolValue *mPerformanceMode;
		double mMajorTickInterval;
		int mMinorTickInterval;
		double mHorizontalOffset;
		double mVerticalOffset;
		bool mDiagramMode;
		bool mShowDiagramLines;
		double mDiagramShift;
		QVector<qreal> mDiagramLineDashPattern;
		double mLineWidth;
		QColor mLineColor;
		QVector<QColor> mPointColors;
		BoolValue *mAutoScale;
	};

}

#endif


