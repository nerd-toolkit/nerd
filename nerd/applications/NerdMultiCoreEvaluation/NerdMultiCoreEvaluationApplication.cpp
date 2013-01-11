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


#include "NerdMultiCoreEvaluationApplication.h"
#include "Core/Core.h"
#include "Value/InterfaceValue.h"
#include <iostream>
#include "Event/Event.h"
#include "Event/EventManager.h"


#include "NerdConstants.h"
#include "Statistics/Statistics.h"
#include "PlugIns/CommandLineArgument.h"
#include <QApplication>
#include "Statistics/Statistics.h"
#include "Statistics/StatisticsLogger.h"
#include <Command/Command.h>
#include <QStringList>

using namespace std;

using namespace nerd;


NerdMultiCoreEvaluationApplication::NerdMultiCoreEvaluationApplication()
		: BaseApplication()
{
	mEvaluationName = "Evaluation";
}

NerdMultiCoreEvaluationApplication::~NerdMultiCoreEvaluationApplication()
{
}

QString NerdMultiCoreEvaluationApplication::getName() const
{
	return "NerdMultiCoreEvaluationApplication";
}

bool NerdMultiCoreEvaluationApplication::runApplication() {
	
	mRunner->start();

	return true;
}

bool NerdMultiCoreEvaluationApplication::setupGui() {
	//no GUI support
	return true;
}

bool NerdMultiCoreEvaluationApplication::setupApplication()
{
	bool ok = true;
	
	CommandLineArgument *evalNameArg = new CommandLineArgument("name", "name", "<evalName>",
							 "The name of this evaluation process.", 1, 0, true, true);

	if(evalNameArg->getEntries().size() > 0) {
		QStringList params = evalNameArg->getEntryParameters(0);
		mEvaluationName = params.at(0);
	}
	
	mRunner = new MultiCoreEvaluationRunner();
	
	Core::log("EvaluationName: " + mEvaluationName, true);

	return ok;
}



