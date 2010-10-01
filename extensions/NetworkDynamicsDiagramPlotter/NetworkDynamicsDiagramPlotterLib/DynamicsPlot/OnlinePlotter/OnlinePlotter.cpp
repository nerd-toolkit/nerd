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



#include "OnlinePlotter.h"
#include <iostream>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/MatrixValue.h"
#include "Event/EventManager.h"

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
		 
// 		mNextStepEvent = em->getEvent(NerdConstants::EVENT_EXECUTION_NEXT_STEP); //das benutzen?
		mStartEvent = em->getEvent("calculatorStarts");
		mFinishEvent = em->getEvent("calculatorFinishes");
		
		mVM = Core::getInstance()->getValueManager();
		mActiveCalculatorValue = mVM->getValue("/DynamicsPlotters/ActiveCalculator");
		mPlotterProgramValue = mVM->getValue("/DynamicsPlotters/PlotterProgram");		

		if(em != 0){
			em->registerForEvent("calculatorStarts", this);
			em->registerForEvent("calculatorFinishes", this);
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
		}else if(event == mStartEvent) {
			if(mPlotterProgramValue->getValueAsString().contains("inbuilt", Qt::CaseInsensitive)){
				mRunningCalculator = dynamic_cast<StringValue*>(mActiveCalculatorValue)->get();//vM->getValue("/DynamicsPlotters/ActiveCalculator")->getValueAsString();
			}

		}else if(event == mFinishEvent){
			if(mPlotterProgramValue->getValueAsString().contains("inbuilt", Qt::CaseInsensitive)){//if the calculator stopped running:			 
				if(qPrintable(mRunningCalculator) != 0){
					 prepareData(mRunningCalculator);
				}
				
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
		if(calculator == 0)return;
		else if(calculator == "StandardBifurcation_Calculator" || calculator == "Transients_Calculator" || calculator == "Isoperiod_Calculator"){
 			MatrixValue *dataMatrixValue = dynamic_cast<MatrixValue*>(vM->getValue(QString(pathToValues + QString("Data"))));
			if(dataMatrixValue == 0){
				Core::log("OnlinePlotter: Couldn't find data Matrix");
			}
			emit dataPrepared(pathToValues, dataMatrixValue);//supposed to call OnlinePlotterWindow::printData(...)
		}else{
			Core::log("OnlinePlotter: Unknown calculator name: " + calculator, true);
		}

	}//printData
	
	
}



