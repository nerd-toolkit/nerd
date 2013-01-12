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


#ifndef NERDCommandLineArgument_H
#define NERDCommandLineArgument_H

#include "Value/StringValue.h"
#include "Core/Properties.h"

namespace nerd {

	/**
	 * CommandLineArgument.
	 * Describes the syntax of a valid command line argument provided by an 
	 * object (like a PlugIn). 
	 * 
	 * The description contains the name of the command line argument and a short name
	 * for the argument. The full name is specified at the command line with a 
	 * leading '--' before the name, while the short name is prefixed with a single '-'.
	 * The '-' are not part of the name, so the command line argument 'help' with short 
	 * name 'h' is either specified with --help or with -h at the command line.
	 * 
	 * The command line parser will do a rough syntax check based on the number of
	 * parameters following an argument (strings not starting with a '--' or a '-'.
	 * If the number of arguments is too low or too large, then the parser will
	 * inform the user about the usage of the program. 
	 *
	 * For the usage message the descriptions of the arguments and their parameters
	 * are displayed at the command line. The description of the argument briefly
	 * explains what the purpose of the argument is. The arguments' parameters
	 * are described in a separate string. Both descriptions should give a user 
	 * sufficient informaiton to use the argument correctly. 
	 *
	 * CommandLineArguments are used by PlugIns and other objects in the
	 * system to add command line support to the application. Thus when a specific 
	 * PlugIn was loaded at startup it will automatically provide its comamnd line
	 * arguments to the PlugInManager and therefore allows parsing of the command
	 * line argument and a help message text. 
	 *
	 * When a command line argument was found while parsing the command line 
	 * list then the corresponding argument parameters are appended to the string
	 * contained in the StringValue accessible via getParameterValue().
	 * The parameters of each occurrence are surrounded by curled braces, so multiple
	 * occurences of each argument are possible. If an argument has no required 
	 * parameters, then an empty set of curled braces will be added.
	 *
	 * Example of a ParameterValue content: {}{optionalParmeter1, optionalParameter2}
	 * 
	 * The parameter value (getParameterValue()) can be published at the ValueManager.
	 * The standard path for command line arguments is "/CommandLineArguments/argumentFullName".
	 * The publication of the parameter value enables setting command line arguments 
	 * with the standard pre-init value loading mechanism from a file instead of the 
	 * command line.
	 */
	class CommandLineArgument : public Properties {
	public:
		CommandLineArgument(const QString &fullName,
									   const QString &shortName,
									   const QString &parameterDescription,
									   const QString &description,
									   int numberOfRequiredParameters,
									   int numberOfOptionalParameters,
									   bool publishInRepository,
									   bool isCommonArgument = true);
		virtual ~CommandLineArgument();
		
		QString getFullArgumentName() const;
		QString getShortArgumentName() const;

		int getNumberOfRequiredParameters() const;
		int getNumberOfOptionalParameters() const;

		QString getParameterDescription() const;
		QString getDescription() const;

		StringValue* getParameterValue();

		void markAsProcessed(bool processed);
		bool isProcessed() const;

		QStringList getParameters();

		int getNumberOfEntries() const;
		QStringList getEntries() const;
		QStringList getEntryParameters(int entry) const;
		
		void deactivate();
		bool isDeactivated();

	private:
		QString mFullName;
		QString mShortName;
		QString mDescription;
		QString mParameterDescription;
		int mNumberOfRequiredParameters;
		int mNumberOfOptionalParameters;
		StringValue *mParameterContainer;
		bool mProcessed;
		bool mArgumentPublished;
		bool mDeactivated;
	};

}

#endif


