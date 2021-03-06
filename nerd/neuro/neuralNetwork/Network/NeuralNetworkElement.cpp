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



#include "NeuralNetworkElement.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"
#include <QStringList>
#include "Math/Math.h"
#include "Network/NeuralNetwork.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkElement.
 */
NeuralNetworkElement::NeuralNetworkElement()
	: mStartIteration(0), mRequiredIterations(1), mCopyPtr(0)
{
	addPropertyChangedListener(this);

	QList<QString> optionalPrefixes;
	optionalPrefixes.append("+");
	setOptionalHiddenPrefixes(optionalPrefixes);
}

NeuralNetworkElement::NeuralNetworkElement(const NeuralNetworkElement &other) 
	: Properties(other), mStartIteration(other.mStartIteration), 
	  mRequiredIterations(other.mRequiredIterations), mCopyPtr(0)
{
	updateIterationProperty();
	addPropertyChangedListener(this);
}

/**
 * Destructor.
 */
NeuralNetworkElement::~NeuralNetworkElement() {
}

void NeuralNetworkElement::setPosition(const Vector3D &position) {
	NeuralNetworkUtil::setNetworkElementLocationProperty(this, 
					position.getX(), position.getY(), position.getZ());
}


Vector3D NeuralNetworkElement::getPosition() const {
	QPointF pos = NeuralNetworkUtil::getPosition(this);
	return Vector3D(pos.x(), pos.y(), 0.0);
}


int NeuralNetworkElement::getRequiredIterations() const {
	return mRequiredIterations;
}

void NeuralNetworkElement::setRequiredIterations(int iterations) {
	mRequiredIterations = Math::max(1, iterations);
	updateIterationProperty();
}

int NeuralNetworkElement::getStartIteration() const {
	return mStartIteration;
}


// void NeuralNetworkElement::setStartIteration(int startIteration) {
// 	mStartIteration = startIteration;
// 	updateIterationProperty();
// }

void NeuralNetworkElement::propertyChanged(Properties *owner, const QString &property) {
	if(owner == this) {
		if(property == NeuralNetworkConstants::TAG_FAST_ITERATIONS 
			|| property == QString("+").append(NeuralNetworkConstants::TAG_FAST_ITERATIONS)
			|| property == NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT
			|| property == QString("+").append(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT)) //TODO maybe create a properties.isProperty() method.
		{
			if(!hasProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS)
				&& !hasProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT)) 
			{
				mRequiredIterations = 1;
				mStartIteration = 0;
			}
			else {
				QStringList entries;
				//prefer ODN property settings over FAST property (FAST is obolete)
				if(hasProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT)) {
					entries = getProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT).split(",");
				}
				else {
					 entries = getProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS).split(",");
				}
				int iterations = 1;
				int startIteration = 0;
				if(entries.size() == 1) {
					startIteration = entries.at(0).toInt();
				}
				else if(entries.size() == 2) {
					//note: here iterations is index 2 (format: start, iterations)
					iterations = entries.at(1).toInt();
					startIteration = entries.at(0).toInt();
				}
				mStartIteration = startIteration;
				if(iterations <= 0) {
					setRequiredIterations(iterations);
				}
				else {
					mRequiredIterations = iterations;
				}
			}
		}
	}
}

void NeuralNetworkElement::updateIterationProperty() {

	if(mRequiredIterations <= 1) {
		if(mStartIteration == 0) {
			removeProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS);
			removeProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT);
		}
		else {
// 			setProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS, 
// 					QString::number(mStartIteration) + "," + QString::number(mRequiredIterations));
			
			//set the property always as ODN (FAST is obsolete).
			setProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT, 
						QString::number(mStartIteration) + "," + QString::number(mRequiredIterations));
			
		}
	}
	else {
// 		if(mStartIteration == 0) {
// 			setProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS, 
// 						QString::number(mRequiredIterations));
// 		}
// 		else {
// 			setProperty(NeuralNetworkConstants::TAG_FAST_ITERATIONS, 
// 						QString::number(mStartIteration) + "," + QString::number(mRequiredIterations));
// 		}
		if(mStartIteration == 0) {
			setProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT, 
						QString::number(mRequiredIterations));
		}
		else {
			setProperty(NeuralNetworkConstants::TAG_NEURON_ORDER_DEPENDENT, 
						QString::number(mStartIteration) + "," + QString::number(mRequiredIterations));
		}
	}
}

}



