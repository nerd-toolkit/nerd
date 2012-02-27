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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#ifndef NERDSignal_H
#define NERDSignal_H

#include "Event/Event.h"
#include "Event/EventListener.h"
#include "Core/ParameterizedObject.h"
#include "Core/SystemObject.h"
#include "Value/BoolValue.h"

namespace nerd {

	/**
	 * Signal.
	 * Simple Base Class for signals. Provides two pure virtual functions which have to be implemented by the derived classes. resetSignal() is called after the ResetSimulation-event and updateSignal() is called after the NextStep-event. Both function are called if the Enabled parameter is set to true.
	 * The derived classes have to explicitly call the constructor. 
	 * The class also provides the TimeStepSize and CurrentStep value which will be needed by most signals.
	 * Signal is a SystemObject, so don't delete it manually!
	 */
	class Signal : public virtual EventListener, public virtual ParameterizedObject, public virtual SystemObject {
	public:
		Signal(QString name, QString prefix);
		virtual ~Signal();

		virtual void updateSignal() = 0;
		virtual void resetSignal() = 0;

		virtual QString getName() const;
		virtual void eventOccured(Event *event);

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

	protected:
		BoolValue *mEnabledValue;
		Event *mNextStepEvent;
		Event *mResetSimulationEvent;
		DoubleValue *mTimeStepSizeValue;
		IntValue *mCurrentStepValue;
		QString mName;
	};

}

#endif
