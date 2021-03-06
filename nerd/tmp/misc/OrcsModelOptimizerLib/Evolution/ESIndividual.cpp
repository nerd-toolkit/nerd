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

#include "ESIndividual.h"

#include <QString>



namespace nerd {
	
unsigned long ESIndividual::IDCounter = 0;

ESIndividual::ESIndividual(QList<OptimizationDouble> objectParameters, 
													 QList<OptimizationDouble> strategyParameters, 
													 double fitness) 
	: mID(IDCounter++),
	  mObjectParameters(objectParameters),
	  mStrategyParameters(strategyParameters),
	  mFitness(fitness)										 
{
}

QList<OptimizationDouble> ESIndividual::getObjectParameters() const {
	return mObjectParameters;
}

QList<OptimizationDouble> ESIndividual::getStrategyParameters() const {
	return mStrategyParameters;
}

double ESIndividual::getFitness() const {
	return mFitness;
}

unsigned long ESIndividual::getID() const{
	return mID;
}

void ESIndividual::setObjectParameters(QList<OptimizationDouble> parameters) 
{
	mObjectParameters = QList<OptimizationDouble>(parameters);
}

void ESIndividual::setStrategyParameters(QList<OptimizationDouble> parameters) 
{
	mStrategyParameters = QList<OptimizationDouble>(parameters);
}

void ESIndividual::setFitness(double fitness) {
	mFitness = fitness;
}

QString ESIndividual::toString() const
{
	QString str;
	
	str += "Individual: ";
	
	str += "ObjParas: ";
	for(int i = 0; i < mObjectParameters.size();i++ ){
		
		if(i > 0){
			str += " ; ";
		}
		
		str += QString::number(mObjectParameters.at(i).value);
	}
	str += " ";
	
	str += "StrategyParas: ";
	for(int i = 0; i < mStrategyParameters.size();i++ ){
		
		if(i > 0){
			str += " ; ";
		}
		
		str += QString::number(mStrategyParameters.at(i).value);
	}
	str += " ";
	
	str += "Fitness: " + QString::number(mFitness);
	
	return str;
}


} // namespace nerd
