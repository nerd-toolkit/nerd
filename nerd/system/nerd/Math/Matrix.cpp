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



#include "Matrix.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new Matrix.
 */
Matrix::Matrix(int width, int height, int depth)
	: mWidth(width), mHeight(height), mDepth(depth)
{
	resize(mWidth, mHeight, mDepth);
}


/**
 * Copy constructor. 
 * 
 * @param other the Matrix object to copy.
 */
Matrix::Matrix(const Matrix &other) 
	: mMatrix(other.mMatrix), mWidth(other.mWidth), mHeight(other.mHeight), mDepth(other.mDepth)
{
}

/**
 * Destructor.
 */
Matrix::~Matrix() {
}

Matrix& Matrix::operator=(const Matrix &other) {
	mWidth = other.mWidth;
	mHeight = other.mHeight;
	mDepth = other.mDepth;
	mMatrix = other.mMatrix;
	return *this;
}

void Matrix::setWidth(int width) {
	mWidth = Math::max(0, width);
	resize(mWidth, mHeight, mDepth);
}

void Matrix::setHeight(int height) {
	mHeight = Math::max(0, height);
	resize(mWidth, mHeight, mDepth);
}

void Matrix::setDepth(int depth) {
	mDepth = Math::max(0, depth);
	resize(mWidth, mHeight, mDepth);
}

void Matrix::resize(int width, int height, int depth) {
	width = Math::max(0, width);
	height = Math::max(0, height);
	depth = Math::max(0, depth);
	mMatrix.resize(depth); 
	for(int i = 0; i < mMatrix.size(); ++i) {
		QVector<QVector<double> > &v1 = mMatrix[i];
		v1.resize(height);
		for(int j = 0; j < v1.size(); ++j) {
			QVector<double> &v2 = v1[j];
			v2.resize(width);
		}
	}
	mWidth = width;
	mHeight = height;
	mDepth = depth;
}

void Matrix::clear() {
	mMatrix.clear();
	mWidth = 0;
	mHeight = 0;
	mDepth = 0;
}

void Matrix::fill(double value) {
	for(int i = 0; i < mMatrix.size(); ++i) {
		QVector<QVector<double> > &v1 = mMatrix[i];
		for(int j = 0; j < v1.size(); ++j) {
			QVector<double> &v2 = v1[j];
			for(int k = 0; k < v2.size(); ++k) {
				v2[k] = value;
			}
		}
	}
}

int Matrix::getWidth() const {
	return mWidth;
}

int Matrix::getHeight() const {
	return mHeight;
}

int Matrix::getDepth() const {
	return mDepth;
}

void Matrix::set(double value, int w, int h, int d) {
	if(w >= 0 && w < mWidth && h >= 0 && h < mHeight && d >= 0 && d < mDepth) {
		mMatrix[d][h][w] = value;
	}
}

double Matrix::get(int w, int h, int d) {
	if(w >= 0 && w < mWidth && h >= 0 && h < mHeight && d >= 0 && d < mDepth) {
		return mMatrix[d][h][w];
	}
	return 0.0;
}

//void Matrix::clear(double clearValue) {
//	for(int i = 0; i < mMatrix.size(); ++i) {
//		QVector<QVector<double> > &v1 = mMatrix[i];
//		for(int j = 0; j < v1.size(); ++j) {
//			QVector<double> &v2 = v1[j];
//			for(int k = 0; k < v2.size(); ++k) {
//				v2[k] = clearValue;
//			}
//		}
//	}
//}

QVector<QVector<QVector<double> > > Matrix::getData() const {
	return mMatrix;
}


}



