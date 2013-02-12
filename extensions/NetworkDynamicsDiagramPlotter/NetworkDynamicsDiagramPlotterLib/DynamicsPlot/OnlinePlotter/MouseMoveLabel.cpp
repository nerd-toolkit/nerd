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
		QPoint curPos = event->pos(); 
		QPoint gloPos = event->globalPos();

		if(mMatrix == 0 || mMatrix->getMatrixDepth() <= mIndex) {
			QToolTip::showText(gloPos,
					QString("[" + QString::number(curPos.x())
								+ ", "
								+ QString::number(curPos.y())
								+ "]"), this);
			
		} else {
			int matrixHeight = mMatrix->getMatrixHeight();
			
			double x = curPos.x() + 1; // Position in current Widget plus on for matrix
			double y = matrixHeight - curPos.y() - 1; // same reversed
			
			// show tool tip
			QToolTip::showText(gloPos, 
				QString("(" + QString::number(mMatrix->get(x, 0, mIndex))
							+ ", "
							+ QString::number(mMatrix->get(0, y, mIndex))
							+ "): "
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
		mMatrix = (MatrixValue*)matrix->createCopy();
	}

	void MouseMoveLabel::setMatrixValue(double v, int w, int h, int d) {
		if(mMatrix == 0) {
			Core::log("MouseMoveLabel: Matrix is NULL object", true);
			return;
		}
		mMatrix->set(v, w, h, d);
	}	
	
}
