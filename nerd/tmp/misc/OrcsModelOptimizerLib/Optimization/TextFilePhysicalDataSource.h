/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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

#ifndef NERD_TEXTFILEPHYSICALDATASOURCE_H
#define NERD_TEXTFILEPHYSICALDATASOURCE_H


#include "PhysicalDataSource.h"
#include <QMap>
#include <QVector>


class QString;
class QFile;
class QTextStream;

namespace nerd {

typedef QMap<int, double> PositionDoubleValueMap;

class TextFilePhysicalDataSource : public PhysicalDataSource {
	public:
		
		TextFilePhysicalDataSource(Trigger *trigger,
									Normalization *triggerNormalization,
									QVector<StringValue*> trainDataValueNames,
									QVector<Normalization*> trainDataValueNormalizations,
									const QString &configDocumentPath, 
									const QString &dataFilePath);
		
		~TextFilePhysicalDataSource();

		virtual bool init();
		virtual QString getName() const;
		virtual int readData();
		virtual bool readData(int motionLengthInSteps);
		virtual double getData(int channel, int simulationStep);

	protected:
		enum error {NOERROR, FRAMEINCOMPLETE, READERROR};

		virtual int readDataFrames(int motionLengthInSteps);
		virtual error readNextFrame(PositionDoubleValueMap *posValMap);
		virtual int getValuePosition(const QString &valueName);

	protected:

		QFile *mDataFile;
		QTextStream *mTextStream;
		uint mFrameTimeLength;
		QVector< QVector<double>* > mData;
};

} // namespace nerd
#endif
