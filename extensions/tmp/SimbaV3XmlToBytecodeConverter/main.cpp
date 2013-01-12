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

#include "Core/Core.h"
#include "PlugIns/PlugInManager.h"
#include "PlugIns/CommandLineArgument.h"
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QCoreApplication>
#include <iostream>
#include "IO/NeuralNetworkIO.cpp"
#include "Collections/StandardNeuralNetworkFunctions.h"
#include "Collections/StandardConstraintCollection.h"

using namespace nerd;
using namespace std;

int main(int argc, char *argv[])
{
	QCoreApplication *app = new QCoreApplication(argc, argv);

	Core::resetCore();

	CommandLineArgument *filesArgument = new CommandLineArgument("files", "f",
 			"<SourceFileName> <DestFileName>", 
			"Specifies name of the file to convert <SourceFileName> and the name of the destination "
			"file <DestFileName>", 2, 0, false);

	if(Core::getInstance()->init()) {
		// Check parameters
		if(filesArgument->getParameterValue()->get() != "") {
			//TODO unsafe, check size of parameter list before accessing .at(0) and .at(1)
			QStringList fileNames = filesArgument->getEntryParameters(0);

			StandardConstraintCollection();
			StandardNeuralNetworkFunctions();
			// Load net from file
			QString errorMsg;
			QList<QString> warnings;
			ModularNeuralNetwork* net = NeuralNetworkIOSimbaV3Xml::createASeriesNetFromFile(fileNames.at(0), &errorMsg, &warnings);
			if((net != 0) && (errorMsg.isEmpty())) {
				QListIterator<QString> warningsIt(warnings);
				while(warningsIt.hasNext()) {
					cout << "Warning: " << warningsIt.next().toStdString() << endl;
				}
				warnings.clear();

				// Write net to Bytecode file
				NeuralNetworkIO::createFileFromNetwork(fileNames.at(1), net, 
							NeuralNetworkIO::Bytecode, &errorMsg, &warnings);
				if(errorMsg.isEmpty()) {
					warningsIt.toFront();
					while(warningsIt.hasNext()) {
						cout << "Warning: " << warningsIt.next().toStdString() << endl;
					}
				} else {
					cout << errorMsg.toStdString() << endl;
				}
				delete net;
			} else {
				cout << errorMsg.toStdString() << endl;
			}
		} else {
			cout << "SimbaV3XmlToBytecodeConverter: No input and output files specified!" << endl
				<< "See SimbaV3XmlToBytecodeConverter --help for more information." << endl;
		}
	}

	Core::getInstance()->shutDown();
	Core::getInstance()->waitForAllThreadsToComplete();
	Core::resetCore();

	delete app;

	return 0;
}
