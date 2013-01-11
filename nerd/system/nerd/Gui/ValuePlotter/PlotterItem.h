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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
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
 *   clearly by citing the nerd homepage and the nerd overview paper.      *
 ***************************************************************************/



#ifndef NERDPlotterItem_H
#define NERDPlotterItem_H

#include "Value/Value.h"
#include <QVector>
#include <QList>
#include <QColor>
#include <QString>

namespace nerd {

	/**
	 * PlotterItem.
	 *
	 * Represents a Value object to be plotted in a PlotterPanel.
	 * A PlotterItem contains all relevant information about how to plot the
	 * values of the associated Value. After each simulation step, the PlotterPanel
	 * calls update() at each PlotterItem, which gives a PlotterItem the chance to
	 * update the internal history of values.
	 *
	 * The history of a PlotterItem can also be set to static, which means that the
	 * history is not automatically extended when update() is called.
	 */
	class PlotterItem {
	public:
		PlotterItem(const QString &name, const QColor &color = Qt::black);
		virtual ~PlotterItem();

		virtual void update();

		void setHistoryCapacity(int capacity);
		int getHistoryCapacity() const;
		QVector<double> getHistory() const;
		void setHistory(QVector<double> history);
		int getHistoryStartIndex() const;
		void addToHistory(double value);
		double getHistoryValue(int index);

		void setName(const QString &name);
		const QString& getName() const;

		void setColor(const QColor &color);
		const QColor& getColor() const;
		
		void setDashPattern(const QVector<qreal> &pattern);
		QVector<qreal> getDashPattern() const;

		void setStaticHistory(bool staticHistory);
		bool hasStaticHistory() const;

		void setVisible(bool visible);
		bool isVisible() const;

		void setScalingV(double scaling);
		void setScalingH(double scaling);
		void setOffsetV(double offset);
		void setOffsetH(double offset);
		double getScalingV() const;
		double getScalingH() const;
		double getOffsetV() const;
		double getOffsetH() const;

	protected:
		QVector<double> mHistory;
		int mHistoryStartPosition;
		int mHistoryMaxCapacity;
		double mScalingV;
		double mScalingH;
		double mOffsetV;
		double mOffsetH;
		QString mName;
		QColor mColor;
		bool mHasStaticHistory;
		bool mVisible;
		QVector<qreal> mDashPattern;
	};

}

#endif



