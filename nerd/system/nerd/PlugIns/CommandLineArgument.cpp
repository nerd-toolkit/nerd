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

#include "CommandLineArgument.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "PlugIns/PlugInManager.h"

namespace nerd {


/**
 * Constructs a new command line argument description.
 *
 * @param fullName the long name of the command line argument (without '--' prefix).
 * @param shortName the short name of the argument (without '-' prefix).
 * @param parameterDescription a string specifying the parameter names.
 * @param description a short description of the argument function and its parameters. 
 * @param numberOfRequiredParameters the number of required parameters of the argument.
 * @param numberOfOptionalParameters the number of additional optional parameters.
 * @param publishInRepository determines whether to publish the argument in the ValueManager.
 */
CommandLineArgument::CommandLineArgument(
						const QString &fullName,
						const QString &shortName,
						const QString &parameterDescription,
						const QString &description,
						int numberOfRequiredParameters,
						int numberOfOptionalParameters,
						bool publishInRepository,
						bool isCommonArgument)
	: Properties(), mFullName(fullName), mShortName(shortName), mDescription(description),
	  mParameterDescription(parameterDescription),
	  mNumberOfRequiredParameters(numberOfRequiredParameters), 
	  mNumberOfOptionalParameters(numberOfOptionalParameters),
	  mParameterContainer(0), mProcessed(false), mArgumentPublished(publishInRepository),
	  mDeactivated(false)
{
	if(isCommonArgument) {
		setProperty("common");
	}
	setProperty("all");
	mParameterContainer = new StringValue("");
	if(publishInRepository) {
		Core::getInstance()->getValueManager()->addValue(
				QString("/CommandLineArguments/").append(mFullName), mParameterContainer);
	}
	Core::getInstance()->getPlugInManager()->addCommandLineArgument(this);
}


/**
 * Destroys the CommandLineArgument. If the argument was published in the 
 * ValueManager it will not be destroyed as the ValueManager will do this. If not 
 * published the argument's StringValue will be destroyed here.
 */
CommandLineArgument::~CommandLineArgument() {
	//delete mParameterContainer only if the parameter was not registered at the
	//ValueManager.
// 	if(Core::getInstance()->getValueManager()->getValue(
// 			QString("/CommandLineArguments/").append(mFullName)) != mParameterContainer)
// 	{
// 		delete mParameterContainer;
// 	}
	if(!mArgumentPublished) {
		delete mParameterContainer;
	}
}


/**
 * Returns the full name of the command line argument. The name does not contain the 
 * leading '--' full name prefix. The name can be empty to signal that no full name is 
 * available.
 *
 * @return the full name of the command line argument.
 */
QString CommandLineArgument::getFullArgumentName() const {
	return mFullName;
}


/**
 * Returns the short name of the command line argument. The name does not contain the
 * leading '-' short name prefix. The short name can be empty to signal that no short name 
 * is available.
 * 
 * @return the short name of the command line argument.
 */
QString CommandLineArgument::getShortArgumentName() const {
	return mShortName;
}


/**
 * Returns the number of required parameters of the command line argument. 
 * This number is used for a rough automated syntax test.
 * 
 * @return the number of required parameters.
 */
int CommandLineArgument::getNumberOfRequiredParameters() const {
	return mNumberOfRequiredParameters;
}


/**
 * Returns the number of optional parameters in addition to the required 
 * argument parameters.  This number is used for a rough automated syntax test. 
 *
 * @return the number of optional parameters.
 */
int CommandLineArgument::getNumberOfOptionalParameters() const {
	return mNumberOfOptionalParameters;
}


/**
 * Returns a short description of all required and optional argument parameters.
 * Although this is free text, the author should follow some simple guide lines:
 * 
 * <pre>
 * nameOfParam1 nameOfParam2 [nameOfOptionalParam1]
 *   nameOfParam1: Description of param1.
 *   nameOfParam2: Description of param2.
 *   nameOfOptionalParam1: Description of optional param1.
 * </pre>
 * 
 * @return a short multi-line description of all parameters and their meaning.
 */
QString CommandLineArgument::getParameterDescription() const {
	return mParameterDescription;
}


/**
 * Returns a short description of the arguments' function. This description 
 * usually is used in the help text of the application to describe which
 * command line arguments are available.
 * 
 * @return a short description of the argument.
 */
QString CommandLineArgument::getDescription() const {
	return mDescription;
}


/**
 * Returns a StringValue containing all detected parameters of the argument.
 * Multiple occurences of an argument are separated in this StringValue with 
 * curled braces '{}'. So the number of occurences and each content can be 
 * replicated by expanding the braces. If a command line parameter with zero
 * required parameters occured, then a set of empty braces is appended to the 
 * StringValue. To check if such a parameter was specified at command line
 * the user has to check if the ParameterValue is emptry or not.
 *
 * If the CommandLineArgument was created with an active publish flag, then
 * the returned StringValue is the one published at the ValueManager.
 *
 * @return a pointer to the ParameterValue containing all parameters of each occurence.
 */
StringValue* CommandLineArgument::getParameterValue() {
	return mParameterContainer;
}


void CommandLineArgument::markAsProcessed(bool processed) {
	mProcessed = processed;
}


bool CommandLineArgument::isProcessed() const {
	return mProcessed;
}

QStringList CommandLineArgument::getParameters() {
	QStringList parameters = mParameterContainer->get().split("{");
	QStringList prunnedParams;

	for(int i = 0; i < parameters.size(); ++i) {
		QString param = parameters.at(i).mid(0, parameters.at(i).size() - 1);
		if(param.trimmed() != "") {
			prunnedParams.append(param);
		}
	}
	return prunnedParams;
}

int CommandLineArgument::getNumberOfEntries() const {
	return getEntries().size();
}


QStringList CommandLineArgument::getEntries() const {
	if(mParameterContainer == 0) {
		return QStringList();
	}
	QStringList parameters = mParameterContainer->get().split("{");
	QStringList prunnedParams;

	for(int i = 0; i < parameters.size(); ++i) {
		if(parameters.at(i).trimmed() == "") {
			continue;
		}
		QString param = parameters.at(i).mid(0, parameters.at(i).size() - 1);
		prunnedParams.append(param);
	}
	return prunnedParams;
}

QStringList CommandLineArgument::getEntryParameters(int entry) const {
	QList<QString> entries = getEntries();

	if(entry < 0 || entry >= entries.size()) {
		return QList<QString>();
	}
	return entries.at(entry).split(" ");
}


void CommandLineArgument::deactivate() {
	mDeactivated = true;
	//erase content.
	mParameterContainer->set("");
}


bool CommandLineArgument::isDeactivated() {
	return mDeactivated;
}



}


