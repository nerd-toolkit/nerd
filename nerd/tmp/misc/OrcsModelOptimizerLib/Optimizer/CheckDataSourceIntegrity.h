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

// GNU General Public License Agreement
// Copyright (C) 2004-2007 CodeCogs, Zyba Ltd, Broadwood, Holford, TA5 1DU, England.
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by CodeCogs. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

#ifndef NERD_CheckDataSourceIntegrity_H
#define NERD_CheckDataSourceIntegrity_H


#include "ModelOptimizer.h"
#include <QList>

class QString;
namespace nerd { class TrainSet; }
namespace nerd { class DoubleValue; } 
namespace nerd { class IntValue; } 

namespace nerd {
 
struct ChannelDescr
{
  int id;
  QString name;
};  
  
struct SourceDescr
{
  int id;
  QString name;
  QList<ChannelDescr> channels;
}; 
  
struct MotionDescr
{
  int id;
  QString name;
  QList<SourceDescr> sources;
};  
  
  
 /**
 * CheckDataSourceIntegrity
 *   
 */
class CheckDataSourceIntegrity : public virtual ModelOptimizer {
  public:
    CheckDataSourceIntegrity(const QString &model, TrainSet *trainSet);
    
    ~CheckDataSourceIntegrity();

    virtual QString getName() const;
    
    virtual bool init();
    
    virtual bool optimizationStart();

    virtual bool doNextOptimizationStep();
    
    virtual bool optimizationEnd();
    
  protected:
    
    DoubleValue *mTolerance;
    
    IntValue *mTimeTolerance;
    
    virtual bool writeReportStartEntry();
    
    virtual bool writeReportEndEntry();
    
    virtual bool saveErrorStatistic(QList<MotionDescr> &incoherentData);
};

} // namespace nerd
#endif
