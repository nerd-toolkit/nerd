/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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



#ifndef NERDOnlinePlotter_H
#define NERDOnlinePlotter_H

#include <QString>
#include <QWidget>
#include "Value/ValueManager.h"
#include "Value/Value.h"
#include "Value/MatrixValue.h"
#include "Value/ValueChangedListener.h"
#include "Core/SystemObject.h"
#include "Event/EventListener.h"
#include "Value/StringValue.h"

namespace nerd {

	/**
	 * OnlinePlotter.
	 *
	 */
	class OnlinePlotter : public QWidget, public virtual SystemObject, public virtual EventListener  {//unused:, public virtual ValueChangedListener
		Q_OBJECT
		public:
			OnlinePlotter();
			virtual ~OnlinePlotter();
			virtual QString getName() const;
			virtual bool init();
			virtual bool bind();
			virtual bool cleanUp();
			virtual void eventOccured(Event *event);

		signals:
			void dataPrepared(QString name, MatrixValue *dataMatrix);
			
		protected:
			ValueManager *mVM;

		private:
			Value *mPlotterProgramValue;
			Value *mActiveCalculatorValue;
			QString mRunningCalculator;
			
			Event *mStartEvent;
			Event *mFinishEvent;

			void prepareData(QString calculator);
	};

}

#endif




