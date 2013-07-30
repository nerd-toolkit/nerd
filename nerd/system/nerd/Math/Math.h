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


#ifndef NERDMath_H
#define NERDMath_H

#include <QVector>
#include <QPoint>
#include <QPointF>
#include "Math/Vector3D.h"

namespace nerd {

	/**
	 * Math.
	 *
	 * Provides some mathematical methods for convenience.
	 */
	class Math {
	public:
		Math();
		virtual ~Math();

		static double round(double value, int numberOfDigits = 5);
		static bool compareDoubles(double value1, double value2, 
						int precision = 5, bool verbose = false);
		static bool compareDoubles(double value1, double value2, 
						double maxError);
		static int forceToRange(int value, int min, int max);
		static double forceToRangeDouble(double value, double min, double max);
		static double forceToDegreeRange(double value, double shift = 0);
		static Vector3D forceToDegreeRange(Vector3D vector, double shift = 0);
		static double forceToRadRange(double value);

		static int abs(int value);
		static double abs(double value);
		
		static double sign(double value, bool zeroIsThirdSign = false);

		static double min(double value1, double value2);
		static int min(int value1, int value2);
		static qulonglong min(qulonglong value1, qulonglong value2);
		static double max(double value1, double value2);
		static int max(int value1, int value2);
		static qulonglong max(qulonglong value1, qulonglong value2);

		static double calculateGaussian(double mean, double deviation);
		static double calculateUniformNoise(double mean, double range);
		static double getNextUniformlyDistributedValue(double range);
		static double getNextGaussianDistributedValue(double deviation);

		static double distance(const QPoint &p1, const QPoint &p2);
		static double distance(const QPointF &p1, const QPointF &p2);
		static double distance(const Vector3D &p1, const Vector3D &p2);

		static double sin(double value);
		static double cos(double value);

		static QPointF centerOfLine(const QPointF &p1, const QPointF &p2, double offset = 0.0);
		static Vector3D centerOfLine(const Vector3D &p1, const Vector3D &p2, double offset = 0.0);
		
		static long long factorial(int x);

	public:
		static const double PI;
		static QVector<long long> mFactorials;

	};

}

#endif


