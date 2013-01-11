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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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

#include "StatisticTools.h"
#include "MotionData.h"
#include "PhysicalData.h"
#include "SimulationData.h"
#include "TrainSet.h"
#include "Core/Core.h"

#include <cmath>

using namespace nerd;

/**
 * Calculates the mean of data points.
 * 
 * @param data Vector with data points.
 * @return Mean of the data points.
 */
double StatisticTools::calculateMean(QVector<double> data)
{
  // avoid division through zero
  if(data.count() <= 0)
    return 0.0;
  
  double sum = 0.0;
  
  for(int i = 0; i < data.count(); i++)
  {
    sum += data.at(i);
  }
  
  return sum / data.count();
}


/**
 * Calculates the variance of data points.
 * 
 * Note: Use calculateVariance(QVector<double> &data, double mean) if you 
 * calculated already the mean.
 * 
 * @param data Vector with data points.
 * @return Variance of the data points.
 */
double StatisticTools::calculateVariance(QVector<double> data)
{
  double mean = StatisticTools::calculateMean(data);
  
  return StatisticTools::calculateVariance(data,mean); 
}

/**
 * Calculates the variance of data points with the provided mean.
 * 
 * Note: If the mean is already calculated provides this mehod a faster
 * calculation than the calculateVariance(QVector<double> &data). 
 * 
 * @param data Vector with data points.
 * @param mean Mean of the provided data points
 * @return Variance of the data points.
 */
double StatisticTools::calculateVariance(QVector<double> data, double mean)
{
  // avoid division by zero
  if(data.count() < 2)
    return 0.0;
  
  double meanDeviationSum = 0;
  
  for(int i = 0; i < data.count(); i++)
  {
    meanDeviationSum += pow(data.at(i) - mean, 2);
  }
  
  return meanDeviationSum * ( 1.0 / (data.count() - 1.0));
}

/**
 * Calculates the standard deviation of data points.
 * 
 * Note: Use calculateStandardDeviation(QVector<double> &data, double mean) 
 * if you calculated already the mean.
 * 
 * @param data Vector with data points.
 * @return Variance of the data points.
 */
double StatisticTools::calculateStandardDeviation(QVector<double> data)
{
  double mean = StatisticTools::calculateMean(data);
  
  return StatisticTools::calculateStandardDeviation(data,mean); 
}

/**
 * Calculates the  standard deviation of data points with the provided mean.
 * 
 * Note: If the mean is already calculated provides this mehod a faster
 * calculation than the calculateStandardDeviation(QVector<double> &data). 
 * 
 * @param data Vector with data points.
 * @param mean Mean of the provided data points
 * @return Variance of the data points.
 */
double StatisticTools::calculateStandardDeviation(QVector<double> data, 
                                                  double mean)
{
  return pow(StatisticTools::calculateVariance(data, mean), 0.5);
}


/**
 * Calculates the quadratic error between the SimulationData and the
 * PhysicalData of a MotionData object for a specific channel.
 * 
 * e = (1 / N) * sum{t = 1} {N} (1/2 * (simulationData(channel, t) - physicalData(channel, t)) )
 * 
 * @param motionData MotionData object with the SimulationData and PhysicalData which should
 * be used.
 * @param channel Channel from which the data should be used to calculate the error.
 * @return Positive quadratic error. -1 if an error occured.
 */
double StatisticTools::calculateQuadraticError(MotionData *motionData, uint channel)
{
  if(motionData == 0) {
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given MotionData is not initialized!");
    return -1;
  }
  
  // avoid division by zero
  if(motionData->getMotionLength() <= 0){
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given MotionData contains no data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int t = 0; t < motionData->getMotionLength(); t++)
  {
    double curPhysicalData = motionData->getPhysicalData()->getMeanData(channel, t);
    double curSimulationData = motionData->getSimulationData()->getData(channel, t);
    
    sum += 0.5 * pow(curPhysicalData - curSimulationData, 2);
  }
  
  return sum / (double)motionData->getMotionLength();
}


/**
 * Calculates the quadratic error between the SimulationData and the
 * PhysicalData of a MotionData object for all channels.
 * 
 * @param motionData MotionData object with the SimulationData and PhysicalData which should
 * be used.
  * @return Positive quadratic error. -1 if an error occured.
 */
double StatisticTools::calculateQuadraticError(MotionData *motionData)
{
  if(motionData == 0) {
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given MotionData is not initialized!");
    return -1;
  }
  
  // avoid division by zero 
  if(motionData->getChannelCount() <= 0){
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given MotionData contains no data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int channel = 0; channel < motionData->getChannelCount(); channel++)
  {
    double error = StatisticTools::calculateQuadraticError(motionData, channel);
    
    if(error < 0){
      return -1;
    }
    
    sum += error;
  }
  
  return sum / (double)motionData->getChannelCount();
}

/**
 * Calculates the quadratic error between the SimulationData and the
 * PhysicalData of all MotionData objects in the given TrainSet.
 * 
 * @param trainSet TrainSet object with the MotionData objects which should be used.
 * @return Positive quadratic error. -1 if an error occured.
 */
double StatisticTools::calculateQuadraticError(TrainSet *trainSet)
{
  if(trainSet == 0) {
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given TrainSet is not initialized!");
    return -1;
  }
  
  // avoid division by zero 
  if(trainSet->getMotionCount()<= 0){
    Core::log("StatisticTools - calculateQuadraticError: ERROR - The given TrainSet contains no motion data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int motion = 0; motion < trainSet->getMotionCount(); motion++)
  {
    double error = StatisticTools::calculateQuadraticError(trainSet->getMotionData(motion));
    
    if(error < 0){
      return -1;
    }
    
    sum += error;
  }
  
  return sum / (double)trainSet->getMotionCount();
}



/**
 * Calculates the  chi square error between the SimulationData and the
 * PhysicalData of a MotionData object for a specific channel.
 * 
 * e = sum{t = 1} {N} (((simulationData(channel, t) - physicalData(channel, t)) / standardDeviation )^2)
 * 
 * @param motionData MotionData object with the SimulationData and PhysicalData which should
 * be used.
 * @param channel Channel from which the data should be used to calculate the error.
 * @return Positive  chi square error. -1 if an error occured.
 */
double StatisticTools::calculateChiSquareError(MotionData *motionData, uint channel)
{
  if(motionData == 0) {
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given MotionData is not initialized!");
    return -1;
  }
  
  // avoid division by zero
  if(motionData->getMotionLength() <= 0){
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given MotionData contains no data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int t = 0; t < motionData->getMotionLength(); t++)
  {
    double curPhysicalData = motionData->getPhysicalData()->getMeanData(channel, t);
    double curSimulationData = motionData->getSimulationData()->getData(channel, t);
    double curStandardDeviation = motionData->getPhysicalData()->getStandartDeviation(channel, t);
    
    sum += pow( (curPhysicalData - curSimulationData) / curStandardDeviation, 2);
  }
  
  return sum;
}


/**
 * Calculates the  chi square error between the SimulationData and the
 * PhysicalData of a MotionData object for all channels.
 * 
 * @param motionData MotionData object with the SimulationData and PhysicalData which should
 * be used.
 * @return Positive  chi square error. -1 if an error occured.
 */
double StatisticTools::calculateChiSquareError(MotionData *motionData)
{
  if(motionData == 0) {
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given MotionData is not initialized!");
    return -1;
  }
  
  // avoid division by zero 
  if(motionData->getChannelCount() <= 0){
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given MotionData contains no data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int channel = 0; channel < motionData->getChannelCount(); channel++)
  {
    double error = StatisticTools::calculateChiSquareError(motionData, channel);
    
    if(error < 0){
      return -1;
    }
    
    sum += error;
  }
  
  return sum;
}

/**
 * Calculates the  chi square error between the SimulationData and the
 * PhysicalData of all MotionData objects in the given TrainSet.
 * 
 * @param trainSet TrainSet object with the MotionData objects which should be used.
 * @return Positive  chi square error. -1 if an error occured.
 */
double StatisticTools::calculateChiSquareError(TrainSet *trainSet)
{
  if(trainSet == 0) {
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given TrainSet is not initialized!");
    return -1;
  }
  
  // avoid division by zero 
  if(trainSet->getMotionCount()<= 0){
    Core::log("StatisticTools - calculateChiSquareError: ERROR - The given TrainSet contains no motion data!");
    return -1;
  }
  
  double sum = 0;
  
  for(int motion = 0; motion < trainSet->getMotionCount(); motion++)
  {
    double error = StatisticTools::calculateChiSquareError(trainSet->getMotionData(motion));
    
    if(error < 0){
      return -1;
    }
    
    sum += error;
  }
  
  return sum;
}
