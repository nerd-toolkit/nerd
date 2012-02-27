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


#include "NeuroAndSimEvaluationStandardGuiApplication.h"
#include "PlugIns/CommandLineArgument.h"
#include "PlugIns/PlugInManager.h"

namespace nerd {

NeuroAndSimEvaluationStandardGuiApplication::NeuroAndSimEvaluationStandardGuiApplication(
												bool enableControl, bool enableDebugging)
		: NeuroAndSimEvaluationBaseApplication(), mEnableGui(true), mGuiMainWindow(0), 
		  mEnableControl(enableControl), mEnableDebugging(enableDebugging)
{
	
}

NeuroAndSimEvaluationStandardGuiApplication::~NeuroAndSimEvaluationStandardGuiApplication() {
}

QString NeuroAndSimEvaluationStandardGuiApplication::getName() const {
	return "NeuroAndSimEvaluationStandardGuiApplication";
}

bool NeuroAndSimEvaluationStandardGuiApplication::setupGui() {
	//Register command line argument descriptions to the PlugInManager to support
	//switching the GUI on and off 
	CommandLineArgument *guiArgument = 
			new CommandLineArgument(
				"enableGui", "gui", "",
				"Starts the application with graphical user interface.",
				0, 0,
				true);
	CommandLineArgument *noGuiArgument = 
			new CommandLineArgument(
				"disableGui", "nogui", "",
				"Starts the application without graphical user interface.",
				0, 0,
				true);

	if(noGuiArgument->getParameterValue()->get() != "") {
		mEnableGui = false;
	}
	if(guiArgument->getParameterValue()->get() != "") {
		mEnableGui = true;
	}

	if(mEnableGui) {
		mGuiMainWindow = new GuiMainWindow(mEnableControl, mEnableDebugging);
		connect(this, SIGNAL(showGui()), mGuiMainWindow, SLOT(showWindow()));
	}
	return true;
}


bool NeuroAndSimEvaluationStandardGuiApplication::runApplication() {
	NeuroAndSimEvaluationBaseApplication::runApplication();
	return true;
}

}


