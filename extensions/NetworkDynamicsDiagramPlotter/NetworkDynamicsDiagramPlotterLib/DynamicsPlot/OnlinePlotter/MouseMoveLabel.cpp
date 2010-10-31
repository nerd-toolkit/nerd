/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
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



#include "MouseMoveLabel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QToolTip>
#include <vector>

using namespace std;

namespace nerd {


/**
 * Constructs a new MouseMoveLabel. This class inherits QLabel and overwrites mouseMoveEvent(). This is used to display the coordinates of mouse. Must have mParent window. 
 */
	MouseMoveLabel::MouseMoveLabel(QWidget *parent): QLabel(parent)
	{
		setMouseTracking(true);
		if(parent == 0){
			Core::log("MouseMoveLabel: Must have have a parent window!", true);
			return;
		}else{
			mPar = parent;
		}
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

		if(mMatrix == 0){
			QToolTip::showText(absPos, QString("[" + QString::number(pos.x()) + ", " + QString::number(pos.y()) + "]"), this);
		}else{
			int matrixWidth = mMatrix->getMatrixWidth() - 1; //matrix width without axes descriptions
			int matrixHeight = mMatrix->getMatrixHeight() - 1;
			int labelWidth = width();
			int labelHeight = height();
			double xFactor = static_cast<double>(labelWidth) / static_cast<double>(matrixWidth);
			double yFactor = static_cast<double>(labelHeight) / static_cast<double>(matrixHeight);
			double xValue = mMatrix->get(static_cast<int>((pos.x() + 1) / xFactor + 0.5), 0, 0);
			double yValue = mMatrix->get(0, matrixHeight - static_cast<int>((pos.y() + 1) / yFactor + 0.5), 0);
			QToolTip::showText(absPos, QString("(" + QString::number(xValue) + ", " + QString::number(yValue) + ") -> " + QString::number(mMatrix->get(static_cast<int>((pos.x() + 1) / xFactor + 0.5), mMatrix->getMatrixHeight() - static_cast<int>((pos.y() + 1)/yFactor + 0.5) + 1, 0))), this);
		}
	}
	
	void MouseMoveLabel::setMatrix(MatrixValue *matrix){
		if(matrix == 0){
			Core::log("MouseMoveLabel: No matrix found! Using coordinates instead.", true);
			return;
		}
		mMatrix = matrix;	
	}
	
	
	
}



