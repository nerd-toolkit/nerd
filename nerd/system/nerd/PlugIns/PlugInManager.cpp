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

#include "PlugInManager.h"
#include <iostream>
#include <QListIterator>
#include "Core/Core.h"
#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>
#include "PlugIns/PlugIn.h"

using namespace std;

namespace nerd {


/**
 * Constructor.
 */
PlugInManager::PlugInManager()
	: mArgHelp(0)
{
	mCommandLineInput = QCoreApplication::instance()->arguments();
	//remove application file name.
	if(!mCommandLineInput.empty()) {
		mCommandLineInput.removeFirst();
	}
}


/**
 * Destructor.
 * Added CommandLineArguments will be destroyed here.
 */
PlugInManager::~PlugInManager() {
	while(!mArgumentDescriptions.empty()) {
		CommandLineArgument *c = mArgumentDescriptions.at(0);
		mArgumentDescriptions.removeAll(c);
		delete c;
	}

	//unload and delete all PlugInLoaders
	while(!mPlugInLoaders.empty()) {
		QPluginLoader *l = mPlugInLoaders.at(0);
		mPlugInLoaders.removeAll(l);
		l->unload();
		delete l;
	}
}



/** 
 * Loads all PlugIns from shared libraries that can be found in the directory plugInDirName.
 * @param plugInDirName the name of the plugIn directory.
 * @return true if successful, otherwise false.
 */
bool PlugInManager::loadPlugIns(const QString plugInDirName) {

	QDir plugInDir(plugInDirName);

	Core::log(QString("PlugInManager: Loading PlugIns from directory [")
			.append(plugInDirName).append("]."));

	QList<QString> plugInFileNames = plugInDir.entryList(QDir::Files);
	
	for(QListIterator<QString> i(plugInFileNames); i.hasNext();) {
		QString fileName = i.next();
		QPluginLoader *loader = new QPluginLoader(plugInDir.absoluteFilePath(fileName));
		QObject *plugin = loader->instance();
		bool ok = true;
		if(plugin != 0) {
			PlugIn *p = qobject_cast<nerd::PlugIn*>(plugin);
			if(p != 0) {
				QString statusMessage(QString("PlugInManager: Loading PlugIn [")
						.append(p->getName()).append("]."));
				if(!p->install()) {
					Core::log(statusMessage.append(" Installation failed!"));
				}
				else {
					Core::log(statusMessage.append(" Success!"), true);
				}
			}
			else {
				Core::log(QString("PlugInManager: File [")
					.append(fileName).append("] was not a PlugIn!"));
				ok = false;
			}
		}
		else {
			Core::log(QString("PlugInManager: File [")
					.append(fileName).append("] was not a PlugIn !"));
			ok = false;
		}

		if(ok) {
			mPlugInLoaders.append(loader);
		}
		else {
			loader->unload();
			delete loader;
		}
	}
	Core::log("PlugInManager: Done loading PlugIns.");
	return true;
}

bool PlugInManager::getNumberOfLoadedPlugIns() const {
	return mPlugInLoaders.size();
}


/**
 * Checks if the help message was requrested with --help or -h. If so, it prints
 * out the help message and returnes true. Otherwise it returns false.
 *
 * @return true if help was requested, otherwise false.
 */
bool PlugInManager::checkHelpMessageRequest() {
	//add help argument (is done here instead of constructor because the argument
	//will automatically register at the PlugInManager, so it has to be constructed first.

	if(mArgHelp == 0) {
		mArgHelp = new CommandLineArgument("help", "h", "<type>", "Shows this help message.\n"
						 "The shown command line arguments can be modified with an optional "
						 "argument <type>, which indicates the desired type of arguments."	
						 "\nIf <type> is empty then only 'common' arguments are shown; "
						 "to see all arguments set <type> to 'all'",
						0, 1, true, true);

		bool printedHelp = false;

		QStringList arguments =mArgHelp->getEntries();
		for(QListIterator<QString> i(arguments); i.hasNext();) {
			QString type = i.next().trimmed();
			printCommandLineHelpMessage(type);
			printedHelp = true;
		}
		return printedHelp;
	}
	return false;
}

/**
 * Adds a CommandLineArgument to support a new set of command line arguments.
 * CommandLineArguments added to the PlugInManager will be destroyed by the 
 * PlugInManager during shutdown!.
 *
 * @param desc the description to add.
 * @return true if successful, otherwise false.
 */
bool PlugInManager::addCommandLineArgument(CommandLineArgument *desc) {
	if(desc == 0) {
		return false;
	}
	for(QListIterator<CommandLineArgument*> i(mArgumentDescriptions); i.hasNext();) {
		CommandLineArgument *c = i.next();
		if(desc->getFullArgumentName() != "" 
			&& c->getFullArgumentName() == desc->getFullArgumentName()) 
		{
			Core::log(QString("PlugInManager::addCommandLineArgument: ")
					  .append("There was a name conflict of argument with full name [")
					  .append(c->getFullArgumentName())
					  .append("] ! [SKIPPING] "));
			return false;
		}
		if(desc->getShortArgumentName() != ""
			&& c->getShortArgumentName() == desc->getShortArgumentName()) 
		{
			Core::log(QString("PlugInManager::addCommandLineArgument: ")
					  .append("There was a name conflict of argument with short name [")
					  .append(c->getShortArgumentName())
					  .append("] ! [SKIPPING] "));
			return false;
		}
	}
	mArgumentDescriptions.append(desc);

	//check if there is an entry in the command line input.
	parseCommandLine();
	
	return true;
}



/**
 * Returns a list with all known CommandLineArguments.
 *
 * @return a list with all CommandLineArguments.
 */
QList<CommandLineArgument*>
PlugInManager::getCommandLineArguments() const {
	return mArgumentDescriptions;
}



/**
 * Returns the CommandLineArgument with the given full name. 
 * 
 * @param name the full name of the CommandLineArgument.
 * @return a pointer to the specified description object, if found, otherwise NULL.
 */
CommandLineArgument* 
PlugInManager::getCommandLineArgument(const QString &name) {
	for(QListIterator<CommandLineArgument*> i(mArgumentDescriptions); i.hasNext();) {
		CommandLineArgument *c = i.next();
		if(c->getFullArgumentName() == name) {
			return c;
		}
	}
	return 0;
}


/**
 * Parses the command line attributes and fills all CommandLineArguments
 * that have not been processed so far. Adding a CommandLineArgument automatically
 * triggers this parsing function. Previously added descriptions hereby get only updated
 * once. So multiple calls of this method does not affect the content of previously
 * processed CommandLineArguments.
 *
 * @return true if there was no error during parsing, otherwise false.
 */
bool PlugInManager::parseCommandLine() 
{
	
	CommandLineArgument *currentDescription = 0;
	int parameterCounter = 0;
	QString parameterList;

	for(int i = 0; i < mCommandLineInput.size(); i++) {
		QString token = mCommandLineInput.at(i);
		if(token.startsWith("-")) {
			//try to finalize current argument.
			if(currentDescription != 0) {
				//add parameters to the parameter value of the current argument.
				if(!currentDescription->isProcessed()) { 
					StringValue *params = currentDescription->getParameterValue();
					params->set(params->get().append("{")
								.append(parameterList.trimmed()).append("}"));
				}
				currentDescription = 0;
				parameterCounter = 0;
				parameterList = "";
			}

			//detect new argument (based on full or short name)
			if(token.startsWith("--")) {
				//full name detected
				QString name = token.mid(2);
				for(QListIterator<CommandLineArgument*> j(mArgumentDescriptions); 
					j.hasNext();)
				{
					CommandLineArgument *desc = j.next();
					if(desc->getFullArgumentName() == name) {
						currentDescription = desc;
						parameterCounter = 0;
						parameterList = "";
						break;
					}
				}
			}
			else {
				//short name detected
				QString name = token.mid(1);
				for(QListIterator<CommandLineArgument*> j(mArgumentDescriptions); 
					j.hasNext();)
				{
					CommandLineArgument *desc = j.next();
					if(desc->getShortArgumentName() == name) {
						currentDescription = desc;
						parameterCounter = 0;
						parameterList = "";
						break;
					}
				}
			}
		}
		else {
			parameterList.append(" ").append(token);
			parameterCounter++;
		}
	}	
	if(currentDescription != 0) {
		//add parameters to the parameter value of the current argument.
		if(!currentDescription->isProcessed() && !currentDescription->isDeactivated()) { 
			StringValue *params = currentDescription->getParameterValue();
			params->set(params->get().append("{").append(parameterList.trimmed()).append("}"));
			currentDescription = 0;
			parameterCounter = 0;
			parameterList = "";
		}
	}

	//mark all available descriptions as beeing processed to protect them from beeing 
	//filled with more data again.
	for(int i = 0; i < mArgumentDescriptions.size(); ++i) {
		mArgumentDescriptions.at(i)->markAsProcessed(true);
	}

	return true;
}


/**
 * Checks whether the command line arguments are valid. Therefore only command line
 * arguments described by an added CommandLineArgument are considered to 
 * be valid arguments. As each argument can have several parameters (key words not
 * starting with a '-'), the number of parameters are also checked for correctness.
 * The number of required parameters has to be matched and may not exceed this number
 * plus the number of optional parameters. 
 * 
 * @return true if the command line arguments seem to be valid, otherwise false.
 */
bool PlugInManager::validateCommandLineSyntax() {

	CommandLineArgument *currentDescription = 0;
	int parameterCounter = 0;

	if(mCommandLineErrors != "") {
		cerr << "PlugInManager: Wrong usage of command line arguments:" << endl;
		cerr << mCommandLineErrors.toStdString().c_str() << endl;
		return false;
	}

	for(int i = 0; i < mCommandLineInput.size(); i++) {
		QString token = mCommandLineInput.at(i);
		if(token.startsWith("-")) {
			//try to finalize current argument.
			if(currentDescription != 0) {
				if(parameterCounter < currentDescription->getNumberOfRequiredParameters()
					|| parameterCounter > (currentDescription->getNumberOfRequiredParameters()
										+ currentDescription->getNumberOfOptionalParameters()))
				{
					//simple synatax check failed!
					cerr << "PlugInManager: Number of parameters of command line argument ["
						<< currentDescription->getFullArgumentName().toStdString().c_str() 
						<< "] was not valid! [TERMINATING]" << endl;
					Core::log(QString("PlugInManager: Number of parameters [")
						.append(QString::number(parameterCounter))
						.append("] of command line argument [") 
						.append(currentDescription->getFullArgumentName()) 
						.append("] was not valid!"));
					return false;
				}
				currentDescription = 0;
				parameterCounter = 0;
			}

			//detect new argument (based on full or short name)
			if(token.startsWith("--")) {
				//full name detected
				QString name = token.mid(2);
				for(QListIterator<CommandLineArgument*> j(mArgumentDescriptions); 
					j.hasNext();)
				{
					CommandLineArgument *desc = j.next();
					if(desc->getFullArgumentName() == name) {
						currentDescription = desc;
						parameterCounter = 0;
						break;
					}
				}
			}
			else {
				//short name detected
				QString name = token.mid(1);
				for(QListIterator<CommandLineArgument*> j(mArgumentDescriptions); 
					j.hasNext();)
				{
					CommandLineArgument *desc = j.next();
					if(desc->getShortArgumentName() == name) {
						currentDescription = desc;
						parameterCounter = 0;
						break;
					}
				}
			}
		} 
		else {
			parameterCounter++;
		}

		if(currentDescription == 0) {
			//detected unknown token (parsing failed)
			cerr << "PlugInManager: Failed to detect command line argument ["
					<< token.toStdString().c_str() << "]!  [TERMINATING]" << endl;
			return false;
		}
	}	
	if(currentDescription != 0) {
		if(parameterCounter < currentDescription->getNumberOfRequiredParameters()
			|| parameterCounter > (currentDescription->getNumberOfRequiredParameters()
								+ currentDescription->getNumberOfOptionalParameters()))
		{
			//simple synatax check failed!
			cerr << "PlugInManager: Number of parameters of command line argument ["
					<< currentDescription->getFullArgumentName().toStdString().c_str() 
					<< "] was not valid! [TERMINATING]" << endl;

			Core::log(QString("PlugInManager: Number of parameters [")
						.append(QString::number(parameterCounter))
						.append("] of command line argument [") 
						.append(currentDescription->getFullArgumentName()) 
						.append("] was not valid!"));
			return false;
		}
		currentDescription = 0;
		parameterCounter = 0;
	}

	return true;
}


/**
 * Returns a string with the help message explaining all available command line arguments
 * and their parameters. This message can be displayed at the console to give the user
 * an overview on all supported command line arguments.
 *
 * @return the help messsage string. 
 */
QString PlugInManager::getCommandLineHelpMessage(const QString &type) const {
	QString selectedType = type;
	if(selectedType == "") {
		selectedType = "common";
	}
	QString message = "Supported Command Line Options [" + selectedType + "]:\n"
					+ "-----------------------------------------------";
	for(QListIterator<CommandLineArgument*> i(mArgumentDescriptions); i.hasNext();) {
		CommandLineArgument *c = i.next();
		if(c->hasProperty(selectedType)) {
			message.append("\n");
			if(c->isDeactivated()) {
				message.append("[deactivated] ");
			}
			message.append(" -").append(c->getShortArgumentName());
			message.append(" [--").append(c->getFullArgumentName()).append("]");
			
			message.append(" " + c->getParameterDescription());
			message.append("\n" + c->getDescription() + "\n");
		}
	}
	return message;
}


/**
 * Prints the help messge to the console (standard error stream) to give the user an 
 * overview about all supported command line arguments and their parameters.
 */
void PlugInManager::printCommandLineHelpMessage(const QString &type) const {
	cerr << endl << getCommandLineHelpMessage(type).toStdString().c_str() << endl;
}


/**
 * Transforms the content of the StringValue of a CommandLineArgument 
 * into a list that contains a single string for each occurence of that command line
 * argument. So if the application was started with two occurences of argument -arg param1 param2
 * then the returned list contains two strings of the form "param1 param2".
 *
 * @parameterValue the StringValue containing the entries of a CommandLineArgument.
 * @return a list with the single parameter entries of each occurence of the argumennt. 
 */
// QStringList PlugInManager::getCommandLineArgumentParameters(StringValue *parameterValue) {
// 	if(parameterValue == 0) {
// 		return QStringList();
// 	}
// 	QStringList parameters = parameterValue->get().split("{");
// 	QStringList prunnedParams;
// 
// 	for(int i = 0; i < parameters.size(); ++i) {
// 		QString param = parameters.at(i).mid(0, parameters.at(i).size() - 1);
// 		if(param.trimmed() != "") {
// 			prunnedParams.append(param);
// 		}
// 	}
// 	return prunnedParams;
// }


/**
 * Adds an errorMessage to the command line error buffer that will be displayed when the
 * validity of the command line arguments is checked. 
 *
 * @param errorMessage the message to add.
 */
void PlugInManager::reportCommandLineError(const QString &errorMessage) {
	mCommandLineErrors.append("> ").append(errorMessage).append("\n");
}


}



