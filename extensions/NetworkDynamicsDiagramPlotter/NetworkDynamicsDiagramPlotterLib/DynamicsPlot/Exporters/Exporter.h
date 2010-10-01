/***************************************************************************
 *   NERD - Open Robot Control and Simulation Library                      *
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
 *   The NERD library is part of the EU project ALEAR                      *
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



#ifndef NERDExporter_H
#define NERDExporter_H

#include "Value/MatrixValue.h"
#include "Value/StringValue.h"
#include "Core/SystemObject.h"
#include <QVector>
#include <QString>

namespace nerd {

	/**
	 * Exporter for external plotting programs, such as Matlab. 
	 */
	class Exporter : public virtual SystemObject, public EventListener{
		public:
			Exporter();
			virtual ~Exporter();
			virtual QString getName() const;

			virtual bool init();
			virtual bool bind();
			virtual bool cleanUp();
			virtual void eventOccured(Event *event);

		protected:
			ValueManager *vM;

		private:
			void writeToFile(QVector<QVector<QString> > outV, QString fileName);
			void prepareExport(QString plotters);
						
			Event *mStartEvent;
			Event *mFinishEvent;
			
			Value *mPlotterProgramValue;
			Value *mActiveCalculatorValue;
			
			QString mRunningCalculator;
			QString mPlotters;

	};
	



}

#endif



