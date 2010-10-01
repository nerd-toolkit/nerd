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



#ifndef NERDPlotterExecutionLoop_H
#define NERDPlotterExecutionLoop_H

#include <QString>
#include <QHash>
#include "Value/ValueChangedListener.h"
#include <QThread>
#include "Core/SystemObject.h"
#include "Event/EventListener.h"
#include "Value/BoolValue.h"

namespace nerd {

	/**
	 * PlotterExecutionLoop.
	 *
	 */
	class PlotterExecutionLoop  : public QThread, public virtual ValueChangedListener,
								  public virtual SystemObject, public virtual EventListener	
	{
	public:
		PlotterExecutionLoop();
		virtual ~PlotterExecutionLoop();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		virtual QString getName() const;

	protected:
		virtual void run();
		void performWait();

	signals:
		void quitMainApplication();

	private:
		bool mDoShutDown;
		Event *mShutDownEvent;
		IntValue *mDelayValue;
		BoolValue *mExecutePlotters;
	};

}

#endif




