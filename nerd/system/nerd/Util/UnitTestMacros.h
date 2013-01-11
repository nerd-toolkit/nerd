/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
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



#ifndef NERD_UNIT_TEST_MACROS_H
#define NERD_UNIT_TEST_MACROS_H

#include "Core/Core.h"
#include <iostream>
#include <QtTest/QtTest>
#include <QList>
#include <stdlib.h>
#include <algorithm>
#include <QCoreApplication>

using namespace nerd;
using namespace std;


//Required for summary message.
int error = 0;
int numberOfTestedClasses = 0;
QList<QObject*> mTestObjects;
int numberOfIterations = 1;
int testSeed = -1;
int numberOfKnownTests = 0;

void runShuffledTests(int loops, long seed) {
	for(int j = 0; j < loops; ++j) {
		if(j > 0 && j % 10 == 0) {
			cerr << "Finished " << j << " executions." << endl;
		}
		int randSeed = seed;
		if(randSeed == -1) {
			randSeed = rand();
		}
		std::cout << endl << endl
			<< "************* RUN NUMBER " << j << " with RAND: " << "**********************"
			<< endl << randSeed << endl << " *******************"
			<< "**************************************************** RUN " << j
			<< endl;
		srand(randSeed);
		random_shuffle(mTestObjects.begin(), mTestObjects.end());
		for(QList<QObject*>::iterator i = mTestObjects.begin();
												i != mTestObjects.end(); ++i)
		{
			int failed = QTest::qExec(*i);
			if(failed > 0) {
				std::cout << endl
						<< "******************************* FAIL ***********************"
						<< "******************************* FAIL ***********************"
						<< endl << endl;
				error = error + failed;
			}
		}
	}
	cout << "Tested Classes: [" << mTestObjects.size() << "/" << numberOfKnownTests
		 << "]   \tRandomized Loops: [" << loops 
		 << "]   \tErrors: [" << error << "]\n          Seed: [" << seed << "]" << endl;
	cerr << "Tested Classes: [" << mTestObjects.size() << "/" << numberOfKnownTests
		 << "]   \tRandomized Loops: [" << loops 
		 << "]   \tErrors: [" << error << "]\n          Seed: [" << seed << "]" << endl;

	//delete test cases
	for(int i = 0; i < mTestObjects.size(); ++i) {
		delete mTestObjects.at(i);
	}
	mTestObjects.clear();
	Core::resetCore();
}



#define TEST_SIMPLE(Testclass) { Testclass obj; \
	numberOfTestedClasses++; \
	int failed = QTest::qExec(&obj); \
	if(failed > 0) { \
		std::cout << "*********************************** FAIL ****************************" \
				<< "*********************************** FAIL ****************************" \
				<< std::endl; } \
		error = error + failed; \
	}



#define TEST(Testclass) { mTestObjects.append(new Testclass()); }


#define TEST_START(name, iterations, seed, noOfKnownTests) int main(int argc, char *argv[]) { \
	argc = argc; \
	argv[0] = argv[0]; \
	error = 0; \
	numberOfTestedClasses = 0; \
	srand(time(0)); \
	numberOfIterations = iterations; \
	testSeed = seed; \
	numberOfKnownTests = noOfKnownTests; \
	QCoreApplication _app(argc, argv); \
	cout << "Running Testcases [" << name << "]" << endl; \
	cout << "---------------------------------------------------------------------" << endl; \
	cerr << "Running Testcases [" << name << "]" << endl; \
	cerr << "---------------------------------------------------------------------" << endl;


#define TEST_END runShuffledTests(numberOfIterations, testSeed); \
	if(error > 0) { \
		cerr << "ERRORS DETECTED: See logfile testLog.txt for details." << endl; \
	} \
	cerr << endl; \
	cout << endl; \
	return error; \
};


#endif

