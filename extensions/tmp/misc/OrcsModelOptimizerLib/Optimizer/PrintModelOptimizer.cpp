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
 *   Copyright (C) 2008 - 2010 by the Neurocybernetics Groups Osnabrück    *
 *   Contact: Christian Rempis                                             *
 *   christian.rempis@uni-osnabrueck.de                                    *
 *   Contributors: see contributors.txt in the nerd main directory.        *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD Kit is part of the EU project ALEAR                          *
 *   (Artificial Language Evolution on Autonomous Robots) www.ALEAR.eu     *
 *   This work is funded by EU-Project Number ICT 214856                   *
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


#include "PrintModelOptimizer.h"
#include "OrcsModelOptimizerConstants.h"

#include "Core/Core.h"
#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/SimulationData.h"
#include "Optimization/PhysicalData.h"
#include "Optimization/Tools.h"
#include "Optimization/StatisticTools.h"
#include "Value/Value.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"




#include <QFile>
#include <QTextStream>

namespace nerd
{

PrintModelOptimizer::PrintModelOptimizer(const QString &model, TrainSet *trainSet)
 : ModelOptimizer::ModelOptimizer(model, trainSet)
{ }

QString PrintModelOptimizer::getName() const{
  return "PrintModelOptimizer";
}


ParameterDescription* PrintModelOptimizer::createParameterDescription(
    const QString &parameterConfigPath, bool *ok)
{
  PrintParameterDescription* paraDescr = new PrintParameterDescription();
  paraDescr->value = 0;

  ValueManager *vm = Core::getInstance()->getValueManager();

  DoubleValue *curStart = Tools::getDoubleValue(vm,
      parameterConfigPath
          + OrcsModelOptimizerConstants::VM_START_VALUE,
          Tools::EM_ERROR,
          this->getName(),
                        *ok);

  DoubleValue *curEnd = Tools::getDoubleValue(vm,
                                              parameterConfigPath
                                                  + OrcsModelOptimizerConstants::VM_END_VALUE,
                                                  Tools::EM_ERROR,
                                                      this->getName(),
                                                          *ok);

  DoubleValue *curStep = Tools::getDoubleValue(vm,
                                               parameterConfigPath
                                                   + OrcsModelOptimizerConstants::VM_STEP_VALUE,
                                                   Tools::EM_ERROR,
                                                       this->getName(),
                                                           *ok);


  paraDescr->start = curStart;
  paraDescr->end = curEnd;
  paraDescr->step = curStep;

  return paraDescr;
}


bool PrintModelOptimizer::doNextOptimizationStep()
{
  QFile file ("optimization_print.data");
  file.open(QIODevice::WriteOnly | QIODevice::Truncate);

  QTextStream out(&file);

  if(mToOptimizeParameters.size() == 1)
  {
    printParameter(out,(PrintParameterDescription*)mToOptimizeParameters.at(0));
  }
  else if(mToOptimizeParameters.size() == 2)
  {
    PrintParameterDescription *parameter = (PrintParameterDescription*)mToOptimizeParameters.at(0);

    for(double curVal = parameter->start->get();
        curVal <= parameter->end->get();
        curVal += parameter->step->get())
    {
      ((DoubleValue*)parameter->value)->set(curVal);

      printParameter(out,(PrintParameterDescription*)mToOptimizeParameters.at(1));
    }
  }
  file.close();

  return true;
}

bool PrintModelOptimizer::printParameter(QTextStream &out, PrintParameterDescription *parameter)
{
  for(double curVal = parameter->start->get();
      curVal <= parameter->end->get();
      curVal += parameter->step->get())
  {
    ((DoubleValue*)parameter->value)->set(curVal);

    mTrainSet->collectNewSimData();

    double curError = StatisticTools::calculateQuadraticError(mTrainSet);

    if(curError < 0){
      return false;
    }

    out << QString::number(curError) << " ";
  }

  out << "\n";
  return true;
}



} // namespace nerd
