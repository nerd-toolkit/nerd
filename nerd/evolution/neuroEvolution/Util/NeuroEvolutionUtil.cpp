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



#include "NeuroEvolutionUtil.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {


/**
 * Generates a local evolution setting from a given string. 
 * The string can be a fixed number, a percentage (ends with %) 
 * or a global network variable (starts with $).
 * A percentage term is relative to the setting given with parameter currentSetting.
 * If allowVars is active, then a global variable may be used (property of the network).
 * If the settingString could not be interpreted, then the returned value is the currentSetting.
 */
double NeuroEvolutionUtil::getLocalNetworkSetting(const QString &settingString, 
				double currentSetting, ModularNeuralNetwork *network, bool *ok, bool allowVars)
{
	if(ok != 0) {
		*ok = true;
	}
	if(settingString.trimmed() == "") {
		return currentSetting;
	}
	if(settingString != "") {
		bool isOk = true;
		if(settingString.startsWith("%")){
			double percentage = settingString.mid(1).toDouble(&isOk);
			if(isOk) {
				return currentSetting * (percentage / 100.0);
			}
		}
		else if(settingString.endsWith("%")){
			double percentage = settingString.mid(0, settingString.length() - 1).toDouble(&isOk);
			if(isOk) {
				return currentSetting * (percentage / 100.0);
			}
		}
		else if(settingString.startsWith("$")) {
			if(allowVars) {
				QString propertyName = settingString.mid(1);
	
				if(network != 0) {
					if(network->hasProperty(propertyName)) {
						bool isOk = true;
						double setting = NeuroEvolutionUtil::getLocalNetworkSetting(
									network->getProperty(propertyName), currentSetting,
									network, &isOk, false);
						if(isOk) {
							return setting;
						}
					}
				}
			}
		}
		else {
			bool isOk = true;
			double prob = settingString.toDouble(&isOk);
			if(isOk) {
				return prob;
			}
		}
	}
	if(ok != 0) {
		*ok = false;
	}
	return currentSetting;
}

}



