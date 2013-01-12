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


#include "Normalization.h"

#include "OrcsModelOptimizerConstants.h"

#include <QString>

#include "Core/Core.h"
#include "Value/DoubleValue.h"
#include "Tools.h"

namespace nerd
{


const double Normalization::StdNormalizedMin = 0.0;
const double Normalization::StdNormalizedMax = 1.0;


Normalization::Normalization()
  : mIsActive(false),
    mOriginalMin(0),
    mOriginalMax(0),
    mNormalizedMin(0),
    mNormalizedMax(0)
{}


/**
 * Create a Normalization object which loads it configuration from the given paths.
 */
Normalization::Normalization(const QStringList &configPaths)
  : mConfigPaths(configPaths),
	mIsActive(true),
    mOriginalMin(0),
    mOriginalMax(0),
    mNormalizedMin(0),
    mNormalizedMax(0)
{
  if(mConfigPaths.size() <= 0) {
    mIsActive = false;
  }
}

bool Normalization::init()
{
  bool ok = true;

  if(mIsActive)
  {
    ///////////////////////////////////////////////
    // Load Values
    ///////////////////////////////////////////////

    ValueManager *vm = Core::getInstance()->getValueManager();

    mOriginalMin = Tools::getDoubleValue( vm,
                                          mConfigPaths,
                                          OrcsModelOptimizerConstants::VM_ORGMIN_VALUE,
                                          Tools::EM_ERROR,
                                          getName(),
                                          ok );


    mOriginalMax = Tools::getDoubleValue( vm,
                                          mConfigPaths,
                                          OrcsModelOptimizerConstants::VM_ORGMAX_VALUE,
                                          Tools::EM_ERROR,
                                          getName(),
                                          ok );

    mNormalizedMin = Tools::getOrAddDoubleValue( vm,
                                                 mConfigPaths,
                                                 OrcsModelOptimizerConstants::VM_NORMMIN_VALUE,
                                                 Tools::EM_ERROR,
                                                 getName(),
                                                 ok,
                                                 StdNormalizedMin);

    mNormalizedMax = Tools::getOrAddDoubleValue(vm,
                                                mConfigPaths,
                                                OrcsModelOptimizerConstants::VM_NORMMAX_VALUE,
                                                Tools::EM_ERROR,
                                                getName(),
                                                ok,
                                                StdNormalizedMax);



    ///////////////////////////////////////////////
    // Check if values are valid
    ///////////////////////////////////////////////

    if(ok == true) // check if all values are initialized
    {
      if(mOriginalMin->get() >= mOriginalMax->get())
      {
        Core::log("Normalization: ERROR - Value [" +
            OrcsModelOptimizerConstants::VM_ORGMAX_VALUE + "] (\"" +
            QString::number(mOriginalMin->get()) + "\") must be smaller than Value [" +
            OrcsModelOptimizerConstants::VM_ORGMIN_VALUE + "] (\"" +
            QString::number(mOriginalMax->get()) +
            "\")!");
        ok = false;
      }

      if(mNormalizedMin->get() >= mNormalizedMax->get())
      {
        Core::log("Normalization: ERROR - Value ["  +
            OrcsModelOptimizerConstants::VM_NORMMAX_VALUE + "] (\"" +
            QString::number(mNormalizedMin->get()) + "\") must be smaller than Value [" +
            OrcsModelOptimizerConstants::VM_NORMMIN_VALUE + "] (\"" +
            QString::number(mNormalizedMax->get()) +
            "\")!");
        ok = false;
      }
    }
  }

  return ok;
}

QString Normalization::getName() const {
  return "Normalization";
}

double Normalization::normalize(double orgVal) const
{
  if(mIsActive)
  {
    if(mOriginalMin->get() < mOriginalMax->get() &&
       mNormalizedMin->get() < mNormalizedMax->get())
    {
      double ret = (mNormalizedMin->get() + (((mNormalizedMax->get() - mNormalizedMin->get())
            / (mOriginalMax->get() - mOriginalMin->get())) * (orgVal - mOriginalMin->get())));
      ret = (ret > mNormalizedMax->get()) ? mNormalizedMax->get() : ret;
      ret = (ret < mNormalizedMin->get()) ? mNormalizedMin->get() : ret;
      return ret;
    }

    return 0.0;
  }
  else
    return orgVal;
}

} // namespace nerd
