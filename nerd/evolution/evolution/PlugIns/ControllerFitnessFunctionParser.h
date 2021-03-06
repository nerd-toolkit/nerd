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

#ifndef ControllerFitnessFunctionParser_H_
#define ControllerFitnessFunctionParser_H_

#include "Core/SystemObject.h"
#include "PlugIns/CommandLineArgument.h"
#include "Control/ControllerProvider.h"

namespace nerd {


/**
 * ControllerFitnessFunctionParser. Commandline parser to load ControllerFitnessFunctions into the FitnessManager. 
 * To load a fitness function use the command: -fit <controllerInterface> <prototypeName> {<fitnessName>}
 * The controllerInterface specifies the name of the controller, which this function will use during evaluation.
 * The prototypeName defines the fitness function prototype that is to be used, to create the new fitness function. So, currently only previously implemented fitness function, which are integrated into the application can be used. 
 * The fitnessName is optional and can be used to store the new created fitness function copy with the name fitnessName at the fitness manager. 
 * Note that the fitness manager checks the names and requires unique fitness function names!
**/

class ControllerFitnessFunctionParser : public virtual SystemObject {

	public:
		ControllerFitnessFunctionParser(ControllerProvider *provider);
		~ControllerFitnessFunctionParser();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;
		
	private:
		void createAndAddFitnessFunction(const QString &controller, const QString &prototype,
										 const QString &fitnessName, const QString &prefix);

	private:
		CommandLineArgument *mFitnessArgumentDescription;
		ControllerProvider *mControllerProvider;
};
}
#endif

