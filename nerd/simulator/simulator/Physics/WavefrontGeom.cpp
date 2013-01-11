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


#include <QFile>
#include <QDataStream>

#include "WavefrontGeom.h"
#include <iostream>
#include <QStringList>

using namespace std;

namespace nerd {


	WavefrontGeom::WavefrontGeom(SimObject *sim, const QString& filename, double scale) :
		TriangleGeom(sim), mScale(scale), mFileName("")
	{
		mSimObject = sim;
		setFilename(filename);
	}


	WavefrontGeom::WavefrontGeom(const WavefrontGeom &geom) 
		: TriangleGeom(geom), mScale(geom.mScale), mFileName(geom.mFileName)
	{
		QVector<Vector3D> points;
		for(int i = 0; i < geom.mPoints.size(); ++i) {
			points.append(geom.mPoints.at(i));
		}
		mTriangles.clear();
		for(int i = 0; i < geom.mTriangles.size(); ++i) {
			mTriangles.append(geom.mTriangles.at(i));
		}
		setPoints(points);
	}



	WavefrontGeom::~WavefrontGeom() {
	}



	SimGeom* WavefrontGeom::createCopy() const {
		return new WavefrontGeom(*this);
	}



	void WavefrontGeom::setScale(double scale) {
		QVector<Vector3D> points;

		for(int i = 0; i < mPoints.size(); ++i)
			points.append(scale/mScale * mPoints[i]);

		setPoints(points);

		mScale = scale;
	}



	void WavefrontGeom::setFilename(const QString& filename) {

		if(filename == mFileName) {
			return;
		}

		mFileName = filename;

		if(filename == "") {
			mPoints.clear();
			mTriangles.clear();
			return;
		}

		QFile file(filename);

		if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			file.close();
			return;
		}

		QVector<Vector3D> points;
		QVector<Triangle> triangles;

		// parse file
		while(!file.atEnd()) {
			QString line = file.readLine();

			QStringList entries = line.split(" ");
			if(entries.size() < 4) {
				continue;
			}

			if(entries.at(0) == "v") {
				// load vertex
				double x = entries.at(1).toDouble() * mScale;
				double y = entries.at(2).toDouble() * mScale; 
				double z = entries.at(3).toDouble() * mScale;
				points.append(Vector3D(x, y, z));
			} else if(entries.at(0) == "f") {
				// load face
				int a = entries.at(1).toInt();
				int b = entries.at(2).toInt();
				int c = entries.at(3).toInt();
				triangles.append(Triangle(a - 1, b - 1, c - 1));
			}
		}
		file.close();

		mTriangles.clear();
		setPoints(points);
		mTriangles = triangles;
	}
}
