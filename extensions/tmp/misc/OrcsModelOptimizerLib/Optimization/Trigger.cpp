/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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

#include "Trigger.h"

#include "OrcsModelOptimizerConstants.h"

#include <QString>

#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "Tools.h"
#include "Normalization.h"

namespace nerd
{

const QString Trigger::MODUS_BIGGER_THEN = "ValueBiggerThen";
const QString Trigger::MODUS_SMALLER_THEN = "ValueSmallerThen";
const QString Trigger::MODUS_BIGGER_EQUAL_THEN = "ValueBiggerEqualThen";
const QString Trigger::MODUS_SMALLER_EQUAL_THEN = "ValueSmallerEqualThen";

const QString Trigger::PARAVALTYPE_NORMALIZED = "Normalized";
const QString Trigger::PARAVALTYPE_SIMULATION = "Simulation";
const QString Trigger::PARAVALTYPE_PHYSICAL = "Physical";

Trigger::Trigger()
  : mIsActive(false),
		mModusName(0), 
		mModus(UNKNOWN),
    mParameterValueType(0), 
    mParameterNormalization(0), 
    mTriggerValueName(0), 
    mParameter(0)
{ }


Trigger::Trigger(const QStringList &triggerConfigPaths)
	: mIsActive(true),
		mTriggerConfigPaths(triggerConfigPaths), 
		mModusName(0), 
		mModus(UNKNOWN),
    mParameterValueType(0), 
    mParameterNormalization(0), 
    mTriggerValueName(0), 
    mParameter(0)
{ }

Trigger::~Trigger()
{
  if(mParameterNormalization != 0){
    delete mParameterNormalization;
    mParameterNormalization = 0;
  }
}

bool Trigger::init()
{
  bool ok = true;
	
	if(mIsActive == true)
	{
		///////////////////////////////////////////////
		// Load Values
		///////////////////////////////////////////////
	
		ValueManager *vm = Core::getInstance()->getValueManager();
	
		mTriggerValueName = Tools::getStringValue(vm,
																							mTriggerConfigPaths,
																							OrcsModelOptimizerConstants::VM_VALUE_VALUE,
																							Tools::EM_ERROR,
																							getName(),
																							ok);
	
		mModusName = Tools::getStringValue(vm,
																			mTriggerConfigPaths,
																			OrcsModelOptimizerConstants::VM_MODUS_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
		ok &=readModusFromConfig();
	
		mParameter = Tools::getDoubleValue(vm,
																			mTriggerConfigPaths,
																			OrcsModelOptimizerConstants::VM_PARAMETER_VALUE,
																			Tools::EM_ERROR,
																			getName(),
																			ok);
	
		mParameterValueType = Tools::getOrAddStringValue(vm,
														mTriggerConfigPaths,
														OrcsModelOptimizerConstants::VM_PARAMETERVALUETYPE_VALUE,
														false,
														getName(),
														ok,
														Trigger::PARAVALTYPE_NORMALIZED);
	
		ok &= readParameterNormalizationFromConfig();
	}

  return ok;
}

QString Trigger::getName() const {
  return "Trigger";
}

/**
 * Returns the name of the Value which is used to check if the trigger gets active
 * for their values.
 *
 * @return Name of the Value.
 */
QString Trigger::getTriggerValueName() const
{
	if(mIsActive == true){
  	return mTriggerValueName->get();
	}
	else {
		return "";
	}
}

/**
 * Checks if the trigger is activated by the given value.
 *
 * Important: The given value must be normalized.
 *
 * @param checkValue Normalized value which is used to check if the trigger gets active.
 * @return TRUE if the trigger gets active, otherwise FALSE.
 */
bool Trigger::checkTrigger(double checkValue)
{
	if(mIsActive == false){
		return true;
	}
	
  switch(mModus)
  {
    case BIGGER_THEN:
      return mParameterNormalization->normalize( mParameter->get() ) < checkValue;
    case SMALLER_THEN:
      return mParameterNormalization->normalize( mParameter->get() ) > checkValue;
    case BIGGER_EQUAL_THEN:
      return mParameterNormalization->normalize( mParameter->get() ) <= checkValue;
    case SMALLER_EQUAL_THEN:
      return mParameterNormalization->normalize( mParameter->get() ) >= checkValue;
    default:
    {
      Core::log("Trigger::checkTrigger(): Unknown modus \"" + QString::number(mModus) +
          "\" defined!");
      return false;
    }
  }
}

/**
 * Reads from mModusName which modus should be used and sets the mModus.
 * VM_PHYSICALDATA_PATH
 * @return TRUE if successfully, otherwise FALSE.
 */
bool Trigger::readModusFromConfig()
{
  bool ok = true;

  if(mModusName == 0){
    Core::log("Trigger: Value [mModusName] is zero!");
    mModus = UNKNOWN;
    return false;
  }

  if(mModusName->get() == Trigger::MODUS_BIGGER_THEN) {
    mModus = BIGGER_THEN;
  }
  else if(mModusName->get() == Trigger::MODUS_SMALLER_THEN) {
    mModus = SMALLER_THEN;
  }
  else if(mModusName->get() == Trigger::MODUS_SMALLER_EQUAL_THEN) {
    mModus = SMALLER_EQUAL_THEN;
  }
  else if(mModusName->get() == Trigger::MODUS_BIGGER_EQUAL_THEN) {
    mModus = BIGGER_EQUAL_THEN;
  }
  else {
    mModus = UNKNOWN;
    ok = false;
    Core::log("Trigger: Unknow modus \"" + mModusName->get() + "\"! " +
        "Possible modus are: " +
        "\"" + Trigger::MODUS_BIGGER_THEN + "\", " +
        "\"" + Trigger::MODUS_BIGGER_EQUAL_THEN + "\", " +
        "\"" + Trigger::MODUS_SMALLER_THEN + "\", " +
        "\"" + Trigger::MODUS_SMALLER_EQUAL_THEN + "\". ");
  }

  return ok;
}

/**
 * Reads from the mParameterValueType which normalization should be used for the parameter
 * and sets mParameterNormalization.
 *
 * @return TRUE if successfully, otherwise FALSE.
 */
bool Trigger::readParameterNormalizationFromConfig()
{
  bool ok = true;

  // delete old mParameterNormalization if one exists
  if(mParameterNormalization != 0)
  {
    delete mParameterNormalization;
    mParameterNormalization = 0;
  }

  if(mParameterValueType == 0){
    Core::log("Trigger: Value [mParameterValueType] is zero!");
    return false;
  }

  QStringList pathList;

 if(mParameterValueType->get() == Trigger::PARAVALTYPE_SIMULATION)
 {
   for(int i = 0; i < mTriggerConfigPaths.size(); i++){
     pathList += mTriggerConfigPaths.at(i) +
                 OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
                 OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;
   }

   pathList += OrcsModelOptimizerConstants::VM_BASE_PATH +
       OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
       OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
       OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;

  }
  else if(mParameterValueType->get() == Trigger::PARAVALTYPE_PHYSICAL)
  {
    for(int i = 0; i < mTriggerConfigPaths.size(); i++){
      pathList += mTriggerConfigPaths.at(i) +
          OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
          OrcsModelOptimizerConstants::VM_SIMULATIONDATA_PATH;
    }

    pathList += OrcsModelOptimizerConstants::VM_BASE_PATH +
        OrcsModelOptimizerConstants::VM_TRAINDATA_PATH +
        OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH +
        OrcsModelOptimizerConstants::VM_PHYSICALDATA_PATH;
  }
  else if(mParameterValueType->get() != Trigger::PARAVALTYPE_NORMALIZED){
    ok = false;
    Core::log("Trigger: Unknow parameterNormalization \"" + mParameterValueType->get() + "\"! " +
        "Possible parameterNormalizations are: " +
        "\"" + Trigger::PARAVALTYPE_NORMALIZED + "\", " +
        "\"" + Trigger::PARAVALTYPE_SIMULATION + "\", " +
        "\"" + Trigger::PARAVALTYPE_PHYSICAL + "\".");
  }

  if(ok == true)
  {
    mParameterNormalization = new Normalization(pathList);

    ok &= mParameterNormalization->init();
  }

  return ok;
}


} // namespace nerd
