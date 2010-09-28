
/***************************************************************************
 *   ORCS - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Ferry Bachmann                *
 *   christian.rempis@uni-osnabrueck.de +                                  *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The ORCS library is part of the EU project ALEAR                      *
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
 ***************************************************************************/



#ifndef ORCSMatrix_H
#define ORCSMatrix_H

#include <QString>
#include <QHash>
#include <QVector>

namespace orcs {

	/**
	 * Matrix.
	 * 
	 * This matrix is currently just a 3 dimensional container, not a mathematical matrix
	 * with mathematical methods. For mathematical containers, use Vector3D or Matrix3x3.
	 *
	 */
	class Matrix {
	public:
		Matrix(int width = 1, int height = 1, int depth = 1);
		Matrix(const Matrix &other);
		virtual ~Matrix();

		Matrix& operator=(const Matrix &other);

		void setWidth(int width);
		void setHeight(int height);
		void setDepth(int depth);
		void resize(int width, int height, int depth);
		void clear();
		void fill(double value);

		int getWidth() const;
		int getHeight() const;
		int getDepth() const;

		void set(double value, int w = 0, int h = 0, int d = 0);
		double get(int w, int h, int d);

		//void clear(double clearValue);
		const QVector<QVector<QVector<double> > >& getData() const;

	private:
		QVector<QVector<QVector<double> > > mMatrix;
		int mWidth;
		int mHeight;
		int mDepth;

	};

}

#endif



