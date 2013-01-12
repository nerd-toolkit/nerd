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


#include "Matrix3x3.h"
#include <math.h>

namespace nerd{


Matrix3x3::Matrix3x3() {
	mMatrix[0][0] = 0.0;
	mMatrix[0][1] = 0.0;
	mMatrix[0][2] = 0.0;
	mMatrix[1][0] = 0.0;
	mMatrix[1][1] = 0.0;
	mMatrix[1][2] = 0.0;
	mMatrix[2][0] = 0.0;
	mMatrix[2][1] = 0.0;
	mMatrix[2][2] = 0.0;
}


Matrix3x3::Matrix3x3(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33) {
	mMatrix[0][0] = m11;
	mMatrix[0][1] = m12;
	mMatrix[0][2] = m13;
	mMatrix[1][0] = m21;
	mMatrix[1][1] = m22;
	mMatrix[1][2] = m23;
	mMatrix[2][0] = m31;
	mMatrix[2][1] = m32;
	mMatrix[2][2] = m33;
}

Matrix3x3::Matrix3x3(const Matrix3x3 &matrix) {
	mMatrix[0][0] = matrix.mMatrix[0][0];
	mMatrix[0][1] = matrix.mMatrix[0][1];
	mMatrix[0][2] = matrix.mMatrix[0][2];
	mMatrix[1][0] = matrix.mMatrix[1][0];
	mMatrix[1][1] = matrix.mMatrix[1][1];
	mMatrix[1][2] = matrix.mMatrix[1][2];
	mMatrix[2][0] = matrix.mMatrix[2][0];
	mMatrix[2][1] = matrix.mMatrix[2][1];
	mMatrix[2][2] = matrix.mMatrix[2][2];
}

void Matrix3x3::set(double m11, double m12, double m13, double m21, double m22, double m23, double m31, double m32, double m33) {
	mMatrix[0][0] = m11;
	mMatrix[0][1] = m12;
	mMatrix[0][2] = m13;
	mMatrix[1][0] = m21;
	mMatrix[1][1] = m22;
	mMatrix[1][2] = m23;
	mMatrix[2][0] = m31;
	mMatrix[2][1] = m32;
	mMatrix[2][2] = m33;
}


void Matrix3x3::set(int row, int col, double m) {
	if((row <= 3) && (row >= 1) && (col <= 3) && (col >= 1)) {
		mMatrix[row-1][col-1] = m;
	}
}

double Matrix3x3::get(int row, int col) const {
	if((row <= 3) && (row >= 1) && (col <= 3) && (col >= 1)) {
		return mMatrix[row-1][col-1];
	}
	return 0.0;
}


}
