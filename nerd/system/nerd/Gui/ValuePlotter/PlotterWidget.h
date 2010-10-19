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



#ifndef NERDPlotterWidget_H
#define NERDPlotterWidget_H


#include "Gui/ValuePlotter/PlotterItem.h"
#include <QTimer>
#include <QFrame>
#include <QPaintEvent>
#include <QList>
#include <QMutex>
#include "Value/BoolValue.h"
#include "Value/ValueChangedListener.h"

namespace nerd {

	/**
	 * PlotterWidget
	 */
	class PlotterWidget : public QFrame {
	Q_OBJECT

	public:
		PlotterWidget(QWidget *parent = 0);
		virtual ~PlotterWidget();

		bool addPlotterItem(PlotterItem *item);
		bool removePlotterItem(PlotterItem *item);
		const QList<PlotterItem*>& getPlotterItems() const;

		void setNumberOfValuesToPlot(int numberOfValues);
		int getNumberOfValuesToPlot() const;
		void setHistoryCapacity(int size);

		double getUserScaleV() const;
		void setUserScaleV(double scale);
		double getUserOffsetV() const;
		void setUserOffsetV(double offset);

		bool saveHistoriesToFile(const QString &fileName);

		void enableStaticItems(bool enable);
		bool areStaticItemsEnabled() const;

		void enableLegend(bool enable);
		bool isLegendEnabled() const;

	public slots:
		void updateHistories();
		void plot();
		void clear();

	signals:
		void userScaleVChanged(double currentScale);
		void userOffsetVChanged(double currentOffsetV);
		void plotterItemCollectionChanged();

	protected:
		virtual void paintEvent(QPaintEvent *event);

		virtual void drawCoordinateSystem(QPainter &painter);
		virtual void drawHistoryGraphs(QPainter &painter);
		virtual void drawCoordinateRanges(QPainter &painter);
		virtual void drawLegend(QPainter &painter);
		virtual double translateValue(double value, double localOffsetV, double localScaleV);
		virtual void mouseDoubleClickEvent(QMouseEvent *event);

	protected:
		QList<PlotterItem*> mPlotterItems;
		double mDrawableHeight;
		double mDrawableWidth;
		double mUserScaleH;
		double mUserScaleV;
		double mUserOffsetH;
		double mUserOffsetV;
		int mMaxValueNameSize;
		int mNumberOfValuesToPlot;
		int mHistorySize;
		QMutex mUpdateMutex;
		bool mPlotStaticItems;
		bool mPlotLegend;
		BoolValue *mPerformanceMode;
	};

}

#endif


