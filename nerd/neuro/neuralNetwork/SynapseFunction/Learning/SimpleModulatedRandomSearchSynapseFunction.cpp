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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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

#include "SimpleModulatedRandomSearchSynapseFunction.h"
#include <iostream>
#include <Math/Math.h>
#include <Math/Random.h>
#include "Network/Neuron.h"
#include <QStringList>
#include <Core/Core.h>

using namespace std;

namespace nerd {
	
	
	/**
	* Constructor.
	*/
	SimpleModulatedRandomSearchSynapseFunction::SimpleModulatedRandomSearchSynapseFunction()
		: NeuroModulatorSynapseFunction("MRS1"), mCurrentNetwork(0)
	{
		mTypeParameters = new StringValue("1, 1.0, 0.2, 0");
		mTypeParameters->setDescription("Parameter Settings: NM-type, change probability, "
										"disable probability, mode, opt1, opt2, opt3|<second entry>|<third>|...");

		mProbabilityForChange = new DoubleValue(0.5);
		mProbabilityForChange->setDescription("Reference probability for changes.");
		
		mInactive = new BoolValue(false);
		mInactive->setDescription("If true, then the synapse has no effect and does also not change.");

		addParameter("Settings", mTypeParameters);
		addParameter("Probability", mProbabilityForChange);
		addParameter("Inactive", mInactive);
		
		updateSettings();
	}


	/**
	* Copy constructor.
	*/
	SimpleModulatedRandomSearchSynapseFunction::SimpleModulatedRandomSearchSynapseFunction(
											const SimpleModulatedRandomSearchSynapseFunction &other)
		: Object(), ValueChangedListener(), NeuroModulatorSynapseFunction(other), 
			mCurrentNetwork(0)
	{
		mTypeParameters = dynamic_cast<StringValue*>(getParameter("Settings"));
		mProbabilityForChange = dynamic_cast<DoubleValue*>(getParameter("Probability"));
		mInactive = dynamic_cast<BoolValue*>(getParameter("Inactive"));

		for(QHashIterator<QString, Value*> i(other.mObservableOutputs); i.hasNext();) {
			i.next();
			addObserableOutput(i.key(), i.value()->createCopy());
		}
		updateSettings();
	}


	/**
	* Destructor.
	*/
	SimpleModulatedRandomSearchSynapseFunction::~SimpleModulatedRandomSearchSynapseFunction() {
		mParameters.clear();
		mObservableOutputs.clear();
		while(!mObservables.empty()) {
			DoubleValue *observable = mObservables.front();
			mObservables.removeAll(observable);
			delete observable;
		}
	}

	SynapseFunction* SimpleModulatedRandomSearchSynapseFunction::createCopy() const {
			return new SimpleModulatedRandomSearchSynapseFunction(*this);
	}

	void SimpleModulatedRandomSearchSynapseFunction::valueChanged(Value *value) {
		NeuroModulatorSynapseFunction::valueChanged(value);
		if(value == 0) {
			return;
		}
		else if(value == mTypeParameters) {
			updateSettings();
		}
	}


	/**
	* Does nothing in this implementation.
	*/
	void SimpleModulatedRandomSearchSynapseFunction::reset(Synapse *synapse) {
		mCurrentNetwork = 0;
		updateSettings();
	}


	/**
	* Returns the strength of the owner synapse.
	* 
	* @param owner the owner of this SynapseFunction.
	* @return the strength of the owner.
	*/
	double SimpleModulatedRandomSearchSynapseFunction::calculate(Synapse *owner) {
		mCurrentNetwork = 0;
		if(owner == 0 || owner->getSource() == 0) {
			return 0.0;
		}
		mCurrentNetwork = owner->getSource()->getOwnerNetwork();
		if(mCurrentNetwork == 0) {
			return owner->getStrengthValue().get();
		}
		
		for(int i = 0; i < mParameters.size(); ++i) {
		
			SimpleModulatedRandomSearchParameters &params = mParameters[i];

			switch(params.mMode) {
				case 0:
				default:
					randomSearchMode0(owner, params);
			}
		}
		
		if(mInactive->get()) {
			return 0.0;
		}
		
		return owner->getStrengthValue().get();
	}

	bool SimpleModulatedRandomSearchSynapseFunction::equals(SynapseFunction *synapseFunction) const {
		if(NeuroModulatorSynapseFunction::equals(synapseFunction) == false) {
			return false;
		}

		SimpleModulatedRandomSearchSynapseFunction *sf = dynamic_cast<SimpleModulatedRandomSearchSynapseFunction*>(synapseFunction);

		if(sf == 0) {
			return false;
		}
		if(!sf->mTypeParameters->equals(mTypeParameters)) {
			return false;
		}
		if(!sf->mProbabilityForChange->equals(mProbabilityForChange)) {
			return false;
		}
		return true;
	}



	void SimpleModulatedRandomSearchSynapseFunction::updateSettings() {
		
		for(QListIterator<DoubleValue*> i(mObservables); i.hasNext();) {
			i.next()->set(0.0);
		}
		mParameters.clear();
		
		QStringList entries = mTypeParameters->get().split("|");
		
		for(int i = 0; i < entries.size(); ++i) {
			QString entry = entries.at(i);
			QStringList paramStrings = entry.split(",");
			
			if(paramStrings.length() < 4) {
				Core::log("SimpleModulatedRandomSearchSynapseFunction: Too few parameters in ["
						+ entry + "]", true);
				continue;
			}
			
			bool ok = true;
			
			SimpleModulatedRandomSearchParameters params;
			
			params.mType = paramStrings.at(0).toInt(&ok);
			if(!ok) {
				Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse type (1) in ["
						+ entry + "]", true);
				continue;
			}
			params.mChangeProbability = paramStrings.at(1).toDouble(&ok);
			if(!ok) {
				Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse change probability (2) in ["
						+ entry + "]", true);
				continue;
			}
			params.mDisableProbability = paramStrings.at(2).toDouble(&ok);
			if(!ok) {
				Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse disable probability (3) in ["
						+ entry + "]", true);
				continue;
			}
			params.mMode = paramStrings.at(3).toInt(&ok);
			if(!ok) {
				Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse mode (4) in ["
						+ entry + "]", true);
				continue;
			}
			if(paramStrings.size() > 4) {
				params.mParam1 = paramStrings.at(4).toDouble(&ok);
				if(!ok) {
					Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse optional parameter 1 (5) in ["
							+ entry + "]", true);
							continue;
				}
			}
			if(paramStrings.size() > 5) {
				params.mParam2 = paramStrings.at(5).toDouble(&ok);
				if(!ok) {
					Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse optional parameter 2 (6) in ["
							+ entry + "]", true);
					continue;
				}
			}
			if(paramStrings.size() > 6) {
				params.mParam3 = paramStrings.at(6).toDouble(&ok);
				if(!ok) {
					Core::log("SimpleModulatedRandomSearchSynapseFunction: Could not parse optional parameter 3 (7) in ["
							+ entry + "]", true);
					continue;
				}
			}
			if(paramStrings.size() > 7) {
				Core::log("Warning: SimpleModulatedRandomSearchSynapseFunction: There are too many parameters in ["
						+ entry + "]. Ignoring superfluous entries.", true);	
			}
			
			if(i < mObservables.size()) {
				params.mObservable = mObservables.at(i);
			}
			else {
				DoubleValue *newObservable = new DoubleValue();
				params.mObservable = newObservable;
				addObserableOutput("Observable" + QString::number(i) + "_T" + QString::number(params.mType), newObservable);
				mObservables.append(newObservable);
			}
			mParameters.append(params);
		}
	}
	
	
	void SimpleModulatedRandomSearchSynapseFunction::randomSearchMode0(
						Synapse *owner, SimpleModulatedRandomSearchParameters &params) 
	{
		if(owner == 0) {
			return;
		}
		double concentration = NeuroModulator::getConcentrationInNetworkAt(
						params.mType, owner->getPosition(), mCurrentNetwork);
		
		double disableProbability = Math::max(0.0, Math::min(1.0, 
						concentration * params.mDisableProbability * mProbabilityForChange->get()));
		
		double changeProbability = Math::max(0.0, Math::min(1.0, 
						concentration * params.mChangeProbability * mProbabilityForChange->get()));
		
		//cerr << "Got: " << disableProbability << " and " << changeProbability << " of " << params.mType << endl;
		
		if(Random::nextDouble() < disableProbability) {
			mInactive->set(!mInactive->get());
			
			bool ok = true;
			int countDisables = owner->getProperty("MRS-D").toInt(&ok);
			if(!ok) {
				countDisables = 0;
			}
			owner->setProperty("MRS-E", QString::number(countDisables));
		}
		if(!mInactive->get() && (Random::nextDouble() < changeProbability)) {
			
			double variance = params.mParam1;
			double min = params.mParam2;
			double max = params.mParam3;
			
			double newWeight = Math::max(min, Math::min(max, 
								owner->getStrengthValue().get() 
									+ (Random::nextDoubleBetween(-variance, variance))));
			owner->getStrengthValue().set(newWeight);
			
			bool ok = true;
			Core::log("Prop: " + owner->getProperty("MRS-M"), true);
			int countChanges = owner->getProperty("MRS-M").toInt(&ok);
			if(!ok) {
				countChanges = 0;
			}
			owner->setProperty("MRS-M", QString::number(countChanges));
		}
		if(params.mObservable != 0) {
			params.mObservable->set(concentration);
		}
	}
	
}


