/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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



#include "OrcsModelOptimizerApplication.h"

#include "OrcsModelOptimizerConstants.h"

#include <iostream>
#include "Core/Core.h"
#include "Collections/ODE_Physics.h"
#include "PlugIns/StepsPerSecondCounter.h"
#include "PlugIns/CommandLineArgument.h"
#include "Physics/Physics.h"
#include "Gui/Control/BoolValueSwitcherAction.h"
#include "Optimization/Tools.h"
#include "Value/BoolValue.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new OrcsModelOptimizerApplication.
 * Calls the constructor of the BaseApplication.
 */
OrcsModelOptimizerApplication::OrcsModelOptimizerApplication() 
	: BaseApplication(), mEnableGui(true)
{
  mEnableGui = true;
  mEnableFps = false;
  mConfigFile = "standart_config.xml";
}

/**
 * Destructor.
 * Does Nothing.
 */
OrcsModelOptimizerApplication::~OrcsModelOptimizerApplication() 
{
  
}

/**
 * Returns the name of the object.
 */
QString OrcsModelOptimizerApplication::getName() const {
	return "OrcsModelOptimizerApplication";
}


/**
 * Set up the GUI elements of the application.
 * The following elements are used: GuiMainWindow, StepsPerSecondCounter
 */
bool OrcsModelOptimizerApplication::setupGui() 
{
  // handle gui - commandline arguments 
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

  if(noGuiArgument->getParameterValue()->get() != "")
    mEnableGui = false;

  if(guiArgument->getParameterValue()->get() != "")
    mEnableGui = true;
  
  
  // handle fps - commandline argument
  
  CommandLineArgument *fpsArgument = 
      new CommandLineArgument(
                              "showFps", "fps", "",
                              "Shows the fps in the gui.",
                              0, 0,
                              true);
  
  if(fpsArgument->getParameterValue()->get() != "")
    mEnableFps = true;

  if(mEnableGui) 
  {
    // setup GUI
    
    mGuiMainWindow = new GuiMainWindow(false, true);
    connect(this, SIGNAL(showGui()), mGuiMainWindow, SLOT(showWindow()));
        
    mGuiMainWindow->setWindowTitle("ORCS Model Optimizer");
    
    BoolValueSwitcherAction *runOptimizationChkBox = new BoolValueSwitcherAction(
        "&Run Optimization",
        OrcsModelOptimizerConstants::VM_RUNOPTIMIZATION_VALUE);
    mGuiMainWindow->getMenu("Optimization")->addAction(runOptimizationChkBox);
    
    BoolValueSwitcherAction *pauseOptimizationChkBox = new BoolValueSwitcherAction(
        "&Pause Optimization",
        OrcsModelOptimizerConstants::VM_PAUSEOPTIMIZATION_VALUE);
    mGuiMainWindow->getMenu("Optimization")->addAction(pauseOptimizationChkBox);
    
    BoolValueSwitcherAction *shutdownOptimizationChkBox = new BoolValueSwitcherAction(
        "&Close Application after Optimization",
        OrcsModelOptimizerConstants::VM_SHUTDOWNAFTEROPTIMIZATION_VALUE);
    mGuiMainWindow->getMenu("Optimization")->addAction(shutdownOptimizationChkBox);

    if(mEnableFps) {
      new StepsPerSecondCounter();
    }  
  }

  
  return true;
}

/**
 * Set up the components of the application.
 * The following components are used: ODE_Physics, SimpleObjectFileParser,
 * StepsPerSecondCounter 
 */
bool OrcsModelOptimizerApplication::setupApplication() 
{  
  bool ok = true;
  
  // handle config file commandline argument
  CommandLineArgument *configFileArgument = 
      new CommandLineArgument(
                              "configuration", "config", "<configuration file>",
                              "Configuration file for the optimization process.",
                              1, 0,
                              true);
  
  if(configFileArgument->getNumberOfEntries() == 1)
    mConfigFile = configFileArgument->getEntries()[0];
    
  
  //install ODE PhysicsLayer
  ODE_Physics();
  
  mOptimizationManager = new OptimizationManager(mConfigFile);
  
  // get values which controll optimization run
  ValueManager * vm = Core::getInstance()->getValueManager();
  
  BoolValue *runOptimizationValue = Tools::getOrAddBoolValue(vm, 
                    OrcsModelOptimizerConstants::VM_RUNOPTIMIZATION_VALUE,
                    false,
                    this->getName(),
                    ok,
                    false);
  
  BoolValue *pauseOptimizationValue = Tools::getOrAddBoolValue(vm, 
                    OrcsModelOptimizerConstants::VM_PAUSEOPTIMIZATION_VALUE,
                    false,
                    this->getName(),
                    ok,
                    false);
  
  BoolValue *shutDownAfterOptimization = Tools::getOrAddBoolValue(vm, 
                    OrcsModelOptimizerConstants::VM_SHUTDOWNAFTEROPTIMIZATION_VALUE,
                    false,
                    this->getName(),
                    ok,
                    false);
  
  
  if(mEnableGui)
  {
    // setup application to run in window
    // -> start directly optimization and close not after optimization
    runOptimizationValue->set(true);
    pauseOptimizationValue->set(false);
    shutDownAfterOptimization->set(false);
  }
  else
  {
    // setup application to run in shell
    // -> start directly optimization and close after optimization
    runOptimizationValue->set(true);
    pauseOptimizationValue->set(false);
    shutDownAfterOptimization->set(true);
  }
  
  return ok;
}


bool OrcsModelOptimizerApplication::runApplication() 
{
  mOptimizationManager->start();
	
	return true;
}

}



