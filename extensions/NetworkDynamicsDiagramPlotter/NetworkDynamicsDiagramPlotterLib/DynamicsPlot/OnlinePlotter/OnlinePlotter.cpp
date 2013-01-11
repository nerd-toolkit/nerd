/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#include "OnlinePlotter.h"
#include <iostream>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/MatrixValue.h"
#include "Value/BoolValue.h"
#include "Event/EventManager.h"
#include "DynamicsPlotConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new OnlinePlotter.
 */
	OnlinePlotter::OnlinePlotter()
	{
		Core::getInstance()->addSystemObject(this);
	}


/**
 * Destructor.
 */
	OnlinePlotter::~OnlinePlotter() {
		
	}

	bool OnlinePlotter::init() {
		bool ok = true;

		return ok;
	}

	bool OnlinePlotter::bind() {
		bool ok = true;

		EventManager *em = Core::getInstance()->getEventManager();
		 
		mStartEvent = em->registerForEvent(DynamicsPlotConstants::EVENT_CALCULATION_STARTED, this);
		mFinishEvent = em->registerForEvent(DynamicsPlotConstants::EVENT_CALCULATION_COMPLETED, this);
		
		mVM = Core::getInstance()->getValueManager();
		mActiveCalculatorValue = mVM->getStringValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER);
		mPlotterProgramValue = mVM->getStringValue(DynamicsPlotConstants::VALUE_PLOTTER_OUTPUT_FORMAT);		
		mPlotterOnlineValue = mVM->getBoolValue("/DynamicsPlotters/InbuiltPlotterOnline");

		if(mStartEvent == 0 
			|| mFinishEvent == 0 
			|| mActiveCalculatorValue == 0
			|| mPlotterProgramValue == 0 
			|| mPlotterOnlineValue == 0)
		{
			Core::log("OnlinePlotter: Could not find all required value objects!", true);
			ok = false;
		}
		
		return ok;
	}

	bool OnlinePlotter::cleanUp() {
		bool ok = true;

		return ok;
	}
	
	QString OnlinePlotter::getName() const {
		return "OnlinePlotter";
	}
	
	void OnlinePlotter::eventOccured(Event *event) {
		if(event == 0) {
			return;
		}
		else if(event == mStartEvent) {

			if(mPlotterProgramValue->get() == ""  
				|| mPlotterProgramValue->get().contains("internal", Qt::CaseInsensitive))
			{
				mRunningCalculator = mActiveCalculatorValue->get();
				
				prepareData(mRunningCalculator);
				
				emit startProcessing();
			}

		}
		else if(event == mFinishEvent){
			//if the calculator stopped running:	

			if(mPlotterProgramValue->get() == "" 
				|| mPlotterProgramValue->get().contains("internal", Qt::CaseInsensitive))
			{ 
					 emit finishedProcessing();
			}
		}
	}
				
	
	/**
	 * Prepares matrix with calculated data. Sends signal to GUI-thread to start printing the data.
	 * 
	 * 
	 * @param calculator Name of calculator that was executed.
	 */	
	void OnlinePlotter::prepareData(QString calculator)
	{

		ValueManager *vM = Core::getInstance()->getValueManager();
		QString pathToValues = "/DynamicsPlotters/" + calculator + "/";
		
		if(calculator == "") {
			return;
		}

		MatrixValue *dataMatrixValue = dynamic_cast<MatrixValue*>(vM->getValue(QString(pathToValues + QString("Internal/Data"))));	
			
		if(dataMatrixValue == 0){
			Core::log("OnlinePlotter: Couldn't find data Matrix or x/y label values.", true);
		}
		else {
			emit dataPrepared(pathToValues, dataMatrixValue, "", "");//calls OnlinePlotterWindow::printData(...)
		}


	}//printData
	
	
}



