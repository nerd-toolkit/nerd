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

#include "ValuePlotterItem.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"

namespace nerd {

ValuePlotterItem::ValuePlotterItem(const QString &name, const QColor &color)
	: PlotterItem(name, color), mValue(0)
{
}

ValuePlotterItem::~ValuePlotterItem() {
}

/**
 * Updates the history with the current value of the observed Value object.
 * Does nothing, if the history is set to static or if no Value object is set.
 */
void ValuePlotterItem::update() {
	if(mValue == 0 || mHasStaticHistory) {
		return;
	}
	double currentValue = 0.0;
	{
		//if DoubleValue
		DoubleValue *dv = dynamic_cast<DoubleValue*>(mValue);
		if(dv != 0) {
			currentValue = dv->get();
		}
		else {
			IntValue *iv = dynamic_cast<IntValue*>(mValue);
			if(iv != 0) {
				currentValue = (double) iv->get();
			}
			else {
				qDebug("PlotterItem: Value was not DoubleValue or IntValue!");
			}
		}
	}
	addToHistory(currentValue);
	return;
}

/**
 * Set the Value that is observed and plotted by this PlotterItem.
 * Currently only DoubleValue and IntValue objects are accepted.
 * Other Values are ignored and false is returned.
 *
 * @param valueToPlot the value to represent in a plotter.,
 * @return true if successful (DoubleValue or IntValue), otherwise false.
 */
bool ValuePlotterItem::setValue(Value *valueToPlot) {
	if(dynamic_cast<DoubleValue*>(valueToPlot) == 0
		&& dynamic_cast<IntValue*>(valueToPlot) == 0)
	{
		return false;
	}
	if(valueToPlot != mValue) {
		mValue = valueToPlot;
		mHistory.clear();
		mHistoryStartPosition = 0;
	}
	return true;
}


/**
 * Returns the Value object that is currently represented by this PlotterItem.
 *
 * @return the Value represented by this item.
 */
Value* ValuePlotterItem::getValue() const {
	return mValue;
}

}


