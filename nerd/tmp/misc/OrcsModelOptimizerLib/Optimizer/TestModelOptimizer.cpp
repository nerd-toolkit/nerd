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


#include "TestModelOptimizer.h"

#include "Optimization/TrainSet.h"
#include "Optimization/MotionData.h"
#include "Optimization/SimulationData.h"
#include "Optimization/PhysicalData.h"


#include <QFile>
#include <QTextStream>

namespace nerd 
{

TestModelOptimizer::TestModelOptimizer(const QString &model, TrainSet *trainSet) 
 : ModelOptimizer::ModelOptimizer(model, trainSet)  
{ }
 
QString TestModelOptimizer::getName() const{
  return "TestModelOptimizer";
}

bool TestModelOptimizer::doNextOptimizationStep()
{
  mTrainSet->collectNewSimData();
  
  for(int motion = 0; motion < mTrainSet->getMotionCount(); motion++)
  {
    QFile file ("motion_" + QString::number(motion) + ".data");
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
      
    QTextStream out(&file);
    
    MotionData *motionData = mTrainSet->getMotionData(motion);
    
    for(int channel = 0; channel < motionData->getChannelCount(); channel++)
    {
      out << QString("# Simulation Data %1\n").arg(QString::number(channel + 1));
      out << QString("# Physical Data %1\n").arg(QString::number(channel+ 1));
      out << QString("# Physical Data %1 -STD\n").arg(QString::number(channel+ 1));
    }
    out << "\n";
      
    for(int i = 0; i < motionData->getMotionLength(); i++ )
    {
      for(int channel = 0; channel < motionData->getChannelCount(); channel++)
      {
        if(channel > 0)
          out << ";";
        double test = motionData->getSimulationData()->getData(channel,i);
        out << QString::number(motionData->getSimulationData()->getData(channel,i));
        out << ";";
        out << QString::number(motionData->getPhysicalData()->getMeanData(channel,i));
        out << ";";
        out << QString::number(motionData->getPhysicalData()->getStandartDeviation(channel,i));
      }
      out << "\n";
    }
  }
  
  return true;
}
                                              
                                     

} // namespace nerd