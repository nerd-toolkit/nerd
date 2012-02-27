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

#include "ControllerFitnessFunctionParser.h"
#include "Core/Core.h"
#include "Fitness/Fitness.h"
#include "Fitness/FitnessManager.h"
#include "Fitness/FitnessFunction.h"
#include "Fitness/ControllerFitnessFunction.h"
#include <QStringList>

namespace nerd {


ControllerFitnessFunctionParser::ControllerFitnessFunctionParser(
		ControllerProvider *provider) 
{
	Core::getInstance()->addSystemObject(this);
	if(provider == 0) {
		Core::log("ControllerFitnessFunctionParser: Provided ControllerProvider is "
			"empty!");
	}
	mControllerProvider = provider;
	mFitnessArgumentDescription = new CommandLineArgument(
		"loadFitness", "fit", "<ControllerName> <PrototypeName> <FitnessName> <Prefix>",
		"Load a fitness function and connects it with a controller. <ControllerName> identifies the "
		"ControllerInterface to connect the FitneeFunction to. The <Prefix> can be used to change the "
		"parameter path of the new FitnessFunction."
		"prototype-information.", 2, 2, true);
}

ControllerFitnessFunctionParser::~ControllerFitnessFunctionParser() {
}

bool ControllerFitnessFunctionParser::init() {

	return true;
}

bool ControllerFitnessFunctionParser::bind() {

	QStringList params = mFitnessArgumentDescription->getParameters();

	for(int i = 0; i < params.size(); ++i) {
		QStringList paramSet = params.at(i).split(" ");
		if(paramSet.size() < 2 || paramSet.size() > 4) {
			Core::log("ControllerFitnessFunctionParser: Could not interpret command line "
				"argument.");
			continue;
		}
		QString controllerName = paramSet.at(0);
		QString fitnessPrototypeName = paramSet.at(1);
		QString fitnessName = fitnessPrototypeName;
		QString prefix = "";
		if(paramSet.size() > 2) {
			fitnessName = paramSet.at(2);
		}
		if(paramSet.size() > 3) {
			prefix = paramSet.at(3);
		}
		createAndAddFitnessFunction(controllerName, fitnessPrototypeName, fitnessName, prefix);
	}
	return true;
}

bool ControllerFitnessFunctionParser::cleanUp() {
	return true;
}

void ControllerFitnessFunctionParser::createAndAddFitnessFunction(const QString &controllerName, 
					const QString &prototype, const QString &fitnessName, const QString &prefix) 
{
	
	ControlInterface *controller = mControllerProvider->getControllerInterface(controllerName);
	if(controller == 0) {
		Core::log(QString("ControllerFitnessFunctionParser: Controller "
			"[").append(controllerName).append("] does not exist. Fitness function could "
			"not be loaded!"));
		return;
	}

	FitnessManager *fManager = Fitness::getFitnessManager();
	FitnessFunction *prototypeCopy = fManager
			->createFitnessFunctionFromPrototype(prototype, fitnessName);

	if(prototypeCopy == 0) {
		Core::log(QString("ControllerFitnessFunctionParser: Could not create copy of "
			"fitness prototype [").append(prototype).append("]. Fitness function could "
			"not be loaded!"));
		return;
	}

	ControllerFitnessFunction *function = 
		dynamic_cast<ControllerFitnessFunction*>(prototypeCopy);

	if(function == 0) {	
		Core::log(QString("ControllerFitnessFunctionParser: Could not create fitness "
					"function [").append(prototype).append("]. The prototype does not represent a "
					"ControllerFitnessFunction!"));
		delete prototypeCopy;
		return;
	}

	if(prefix != "") {
		//set custom prefix
		QString prefixString = prefix;
		if(prefixString.lastIndexOf("/") != prefixString.size() - 1) {
			prefixString.append("/");
		}
		function->setPrefix(prefixString + fitnessName + "/");
	}

	function->setControlInterface(controller);
	function->attachToSystem();
	fManager->addFitnessFunction(function);

	Core::log(QString("ControllerFitnessFunctionParser: Created fitness function from "
		"prototype [").append(prototype).append("] to control agent "
		"[").append(controllerName).append("] ").append(" and saved as "
		"[").append(fitnessName).append("]!"));
}

QString ControllerFitnessFunctionParser::getName() const {
	return "ControllerFitnessFunctionParser";
}

}
