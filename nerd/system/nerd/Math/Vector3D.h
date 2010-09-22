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

#ifndef Vector3D_H_
#define Vector3D_H_


namespace nerd{

/**
 * Math-Class for a 3D-Vector. This class provides several methods and operators to work with 3D-vectors (normalization, skalar-multiplication, cross-product...). 
 */
class Vector3D {

	public:
		Vector3D();
		Vector3D(double x, double y, double z);
		Vector3D(const Vector3D &vector);

		void setX(double x);
		void setY(double y);
		void setZ(double z);
		void set(double x, double y, double z);
		void set(const Vector3D &other);

		double getX() const;
		double getY() const;
		double getZ() const;
		const double* get() const;

		void normalize();
		double length();

		bool equals(const Vector3D &vector, int precision = 2) const;

		static Vector3D crossProduct(const Vector3D &vector1, const Vector3D &vector2);

		friend bool operator==(const Vector3D &lhs, const Vector3D &rhs);
		friend bool operator!=(const Vector3D &lhs, const Vector3D &rhs);
		friend double operator*(const Vector3D& lhs, const Vector3D& rhs);
		friend Vector3D operator*(const double lhs, const Vector3D& rhs);
		friend Vector3D operator*(const Vector3D& lhs, const double rhs);
		friend Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs);
		friend Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs);
		friend Vector3D operator-(const Vector3D& rhs);


	private:
		double mVector[3];

};
}
#endif
