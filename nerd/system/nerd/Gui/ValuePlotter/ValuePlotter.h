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



#ifndef NERDValuePlotter_H
#define NERDValuePlotter_H

#include <QTimer>
#include "Gui/ValuePlotter/PlotterWidget.h"
#include "Gui/ValuePlotter/PlotterItem.h"
#include <QList>
#include <QPushButton>
#include "Event/EventListener.h"
#include <QString>
#include <QColor>
#include <QPoint>
#include <QDockWidget>

namespace nerd {

	/**
	 * ValuePlotter.
	 *
	 * This plotter can be used to plot time series of Values. Therefore
	 * Values can be added encapsulated in so called PlotterItems. These
	 * PlotterItems describe the value and especially its value history.
	 * This value history is updated automatically after each simulation step.
	 *
	 * It is possible to make a PlotterItem static, which means that the
	 * history is not changed during simuation. Instead these PlotterItems
	 * remain static and can be used to compare a time series with the
	 * dynamic (live plotted) values. This is especially useful with the
	 * ability to load time series from a file and store them in static
	 * PlotterItems.
	 *
	 * Static and dynamic PlotterItems can be scaled and moved independently
	 * of each other. A user therefore can load a time series and move and
	 * scale it to fit for instance a trigger signal in the simulator, which
	 * simplifies the comparison of values of different variable ranges
	 * (e.g. sensory data recorded from a real robot).
	 *
	 * PlotterItems can be hidden if they should be temporarily not be drawn.
	 * While hidden the history of such PlotterItems is still updated
	 * after each simulation step.
	 */
	class ValuePlotter : public QWidget, public virtual EventListener {
	Q_OBJECT

	public:
		ValuePlotter(const QString &name, PlotterWidget *plotter = 0, QWidget *parent = 0);
		virtual ~ValuePlotter();

		virtual void eventOccured(Event *event);
		virtual QString getName() const;

		void init();

		bool addValue(const QString &valueName, Value *value);
		bool removeValue(Value *value);
		QList<Value*> getPlottedValues() const;

		QColor getSuitableColor(QList<QColor> *ignoreList = 0);

		PlotterWidget* getPlotterWidget() const;

		QMap<Event*, QString> getSupportedUpdateTriggerEvents() const;
		void setUpdateTriggerEvent(Event *event);
		Event* getUpdateTriggerEvent() const;

		void invalidateEvents();

	public slots:
		void showPlotter();
		void repaintPlotter();
		void loadStaticData(const QString &fileName,
							const QString &commentChar,
							const QString &separator);
		void removeAllStaticData();
		void showStaticData(bool show);

		

	signals:
		void staticScaleVChanged(double scale);
		void staticScaleHChanged(double scale);
		void staticOffsetVChanged(double offset);
		void staticOffsetHChanged(double offset);
		void staticDataVisibilityChanged(bool visible);
		void triggerEventSupportChanged();

	protected:
		virtual void hideEvent(QHideEvent *event);
		virtual void showEvent(QShowEvent *event);
		virtual void keyPressEvent(QKeyEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void wheelEvent(QWheelEvent *event);

	public:
		//TODO maybe move these methods to PlotterWidget.
		void changeUserScaleV(int count);
		void changeUserOffsetV(double offset);
		void changeStaticDataScaleV(int count);
		void changeStaticDataOffsetV(int count);
		void setStaticDataScaleV(double scale);
		void setStaticDataScaleH(double scale);
		void setStaticDataOffsetV(double offset);
		void setStaticDataOffsetH(double offset);
		void changeStaticDataOffsetH(int count);
		void changeStaticDataScaleH(int count);
		void changeStaticDataOffsetV(double offset);
		void changeStaticDataOffsetH(double offset);

		bool addValueToPlotter(const QString &valueName, Value *value);
		bool addValueToPlotter(const QString &valueName, Value *value,
							const QColor &color);

		

	private:
		QString mName;
		QTimer mPaintTimer;
		PlotterWidget *mPlotterWidget;
		Event *mNextStepEvent;
		Event *mNextTryEvent;
		Event *mNextIndividualEvent;
		Event *mNextGenerationEvent;
		Event *mNextNetworkIterationEvent;
// 		int mMaxPlotterHistorySize;
		QList<QColor> mDefaultColors;
		bool mShowStaticData;

		//mouse handling helpers
		QPoint mMouseStartPosition;
		double mOffsetVStartSetting;
		double mStaticOffsetVStartSetting;
		double mStaticOffsetHStartSetting;
		double mStaticScalingHStartSetting;
		double mStaticScalingVStartSetting;
		bool mLeftMouseButtonPressed;

		QMap<Event*, QString> mTriggerEvents;

		Event *mHistoryUpdateTriggerEvent;

	};

}

#endif


