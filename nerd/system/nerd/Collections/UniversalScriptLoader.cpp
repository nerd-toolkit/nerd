/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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
 *   The NERD Toolkit is part of the EU project ALEAR                      *
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
 *   clearly by citing the NERD homepage and the NERD overview paper.      *  
 ***************************************************************************/



#include "UniversalScriptLoader.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "PlugIns/CommandLineArgument.h"
#include "Script/UniversalScriptingContext.h"
#include "PlugIns/PlugInManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new UniversalScriptLoader.
 */
UniversalScriptLoader::UniversalScriptLoader()
{

	CommandLineArgument *scriptLoaderArgument = new CommandLineArgument(
		"installScript", "is", "<ScriptName> [<ScriptFileName> <TriggerEvent> <ResetEvent>]", 
		QString("Installs a new script with name <ScriptName>, that loads file <ScriptFileName> ")
		+ "during the bind phase. The script is reset by <ResetEvent> and executed each "
		+ "time <TriggerEvent> is triggered.", 1, 3, true);

	QList<QString> scriptNames;

	int numberOfScriptsToInstall = scriptLoaderArgument->getNumberOfEntries();
	for(int i = 0; i < numberOfScriptsToInstall; ++i) {
		QList<QString> entryParams = scriptLoaderArgument->getEntryParameters(i);

		QString name = entryParams.at(0);
		QString fileName = "";
		if(entryParams.size() > 1) {
			fileName = entryParams.at(1);
		}
		QString triggerEvent = "";
		if(entryParams.size() > 2) {
			triggerEvent = entryParams.at(2);
		}
		QString resetEvent = "";
		if(entryParams.size() > 3) {
			resetEvent = entryParams.at(3);
		}

		if(name != "" && !scriptNames.contains(name)) {
			new UniversalScriptingContext(name, fileName, triggerEvent, resetEvent);
			scriptNames.append(name);
			Core::log("UniversalScriptLoader: Installed UniversalScriptingContext ["
						+ name + "]");
		}
	}
}




}



