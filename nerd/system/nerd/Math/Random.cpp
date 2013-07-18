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

#include "Random.h"
#include "MersenneTwister.h"
#include <stdlib.h>
#include <QTime>
#include <qcoreapplication.h>

namespace nerd {


bool Random::sInitialized = false;

Random::Random(): mFlapperJack(0) {
	mFlapperJack = new MTRand();
}

Random::~Random() {
	delete mFlapperJack;
}


void Random::mSetSeed(int seed) {
	mFlapperJack->seed(seed);
}



/**
 * Get next integer in [0,max]
 **/
int Random::mNextInt(int max){
	return mFlapperJack->randInt(max);
}


int Random::mNextInt(){
	return mFlapperJack->randInt();
}


/**
 * Get next double in [0,1]
 **/
double Random::mNextDouble(){
	return mFlapperJack->rand(); 
}


/**
 * Get next double in [min,max]
 **/
double Random::mNextDoubleBetween(double min, double max) {
	return min + mFlapperJack->randDblExc(max - min);
}


double Random::mNextSign() {
	if(mFlapperJack->randDblExc(2) >= 1) {
		return 1.0; 
	}
	else { 
		return -1.0;
	}
}


double Random::mNextGaussian(double mean, double stdDev) {
	return mFlapperJack->randNorm(mean, stdDev);
}



void Random::init() {
	srand(QTime::currentTime().msec() + (QCoreApplication::applicationPid() * 131));
	sInitialized = true;
}


void Random::setSeed(int seed) {
	srand(seed);
}


int Random::nextInt() {
	if(!sInitialized) {
		init();
	}
	return rand();
}

int Random::nextInt(int max) {
	if(!sInitialized) {
		init();
	}
	if(max <= 0) {
		return 0;
	}
	return rand() % max;
}


double Random::nextDouble() {
	if(!sInitialized) {
		init();
	}
	return ((double) rand()) / ((double) RAND_MAX);
}


/**
 * Return a random double between the given borders [min, max).
 * 
 * @param min Minimal value of the random double (Can be reached.).
 * @param max Maximal value of the random double (Can not be reached.).
 * @return Random double between the borders.
 */

double Random::nextDoubleBetween(double min, double max)
{
	// get random value between [0,1)
	double random = nextDouble();
				
	// normalize random value to [min,max)
	double norm = (min + ((max - min) * random));
	norm = (norm > max) ? max : norm;
	norm = (norm < min) ? min : norm;
	
	return norm;
}

double Random::nextSign() {
	if(nextInt(2) > 0) {
		return 1.0;
	}
	return -1.0;
}

}

