/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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


#include "MouseMoveLabel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QToolTip>
#include <vector>
#include "Math/Math.h"
#include <cmath>

using namespace std;

namespace nerd {


/**
 * Constructs a new MouseMoveLabel. This class inherits QLabel and overwrites mouseMoveEvent(). This is used to display the coordinates of mouse. Must have mParent window. 
 */
MouseMoveLabel::MouseMoveLabel(int index, QWidget * parent) : QLabel(parent), mMatrix(0)
	{
		mIndex = index;
		setMouseTracking(true);
		if(parent == 0){
			Core::log("MouseMoveLabel: Must have have a parent window!", true);
			return;
		}else{
			mPar = parent;
		}
		mMatrix = new MatrixValue();
	}



/**
 * Destructor.
 */
	MouseMoveLabel::~MouseMoveLabel() {
	}
	
	
/**
	 * Prints coordinates (of cursor respective to label) to cursor position when mouse hovers over the label.
 */
	void MouseMoveLabel::mouseMoveEvent ( QMouseEvent * event ){
		QPoint pos = event->pos(); 
		QPoint absPos = pos + this->pos() + mPar->pos(); //add position of cursor to position of widget to position of mParent window

		if(mMatrix == 0 || mMatrix->getMatrixDepth() <= mIndex) {
			QToolTip::showText(absPos, QString("[" + QString::number(pos.x()) + ", " + QString::number(pos.y()) + "]"), this);
			
		} else {
			// get some values
			int matrixWidth = mMatrix->getMatrixWidth(); //matrix width without axes descriptions
			int matrixHeight = mMatrix->getMatrixHeight();
			int labelWidth = width();
			int labelHeight = height();
			
			if(matrixWidth < 2 || matrixHeight < 2) {
				Core::log("MouseMoveLabel: Division by zero detected and prevented. Thank me later!", true);
				return;
			}
			
			// calculate size of buckets
			double xFactor = static_cast<double>(labelWidth) / (static_cast<double>(matrixWidth - 1));
			double yFactor = static_cast<double>(labelHeight) / (static_cast<double>(matrixHeight - 1));
			
			// DO NOT CHANGE THIS! If you do, plots will be borken (probably)
			// TODO why seem the two basin plots different from each other
			double x = ceil((pos.x() + 1) / xFactor);
			double y = matrixHeight - ceil((pos.y() + 1) / yFactor);
			
			// get values from matrix
			double xValue = mMatrix->get(x, 0, mIndex);
			double yValue = mMatrix->get(0, y, mIndex);
			
			// show ToolTip
			QToolTip::showText(absPos, 
				QString("[" + QString::number(xValue)
							+ " (" + QString::number(x) + ") , "
							+ QString::number(yValue) 
							+ " (" + QString::number(y) + ")"
							+ "] -> "
							+ QString::number(mMatrix->get(x, y, mIndex))),
				this);
		}
	}
	
	MatrixValue* MouseMoveLabel::getMatrix() {
		if(mMatrix == 0){
			Core::log("MouseMoveLabel: No matrix found!", true);
			return 0;
		}
		return mMatrix;
	}
	
	void MouseMoveLabel::setMatrix(MatrixValue* matrix) {
		if(matrix == 0) {
			Core::log("MouseMoveLabel: No Matrix given. Aborting:", true);
			return;
		}
		mMatrix->set(matrix->get());
	}	
	
}



