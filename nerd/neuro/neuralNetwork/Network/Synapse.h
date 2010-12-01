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


#ifndef NERDSynapse_H
#define NERDSynapse_H

#include "Network/SynapseTarget.h"
#include "Value/DoubleValue.h"
#include "Value/BoolValue.h"

namespace nerd {

	class SynapseFunction;
	class Neuron;

	/**
	 * Synapse
	 */
	class Synapse : public SynapseTarget
	{
	public:
		Synapse(Neuron *source, SynapseTarget *target, double strength, 
				const SynapseFunction &synapseFunction, qulonglong id = 0);
		Synapse(const Synapse &other);
		virtual ~Synapse();

		static Synapse* createSynapse(Neuron *source, SynapseTarget *target, double strength,
				const SynapseFunction &synapseFunction, qulonglong id = 0);

		//TODO createcopy
		virtual Synapse* createCopy() const;

		virtual void setId(qulonglong id);
		virtual qulonglong getId() const;	

		virtual void reset();
		virtual double calculateActivation();
		virtual double getActivation();
		virtual DoubleValue& getStrengthValue();
		virtual BoolValue& getEnabledValue();

		virtual bool addSynapse(Synapse *synapse);
		virtual bool removeSynapse(Synapse *synapse);
		virtual QList<Synapse*> getSynapses() const;

		virtual void setSource(Neuron *source);
		virtual Neuron* getSource();
		virtual void setTarget(SynapseTarget *target);
		virtual SynapseTarget* getTarget();

		virtual void setSynapseFunction(const SynapseFunction &synapseFunction);
		virtual SynapseFunction* getSynapseFunction();

		virtual bool equals(Synapse *synapse);

		virtual void centerPosition();

	protected:
		Neuron *mSourceNeuron;
		SynapseTarget *mTarget;
		QList<Synapse*> mSynapses;
		double mCurrentActivation;
		SynapseFunction *mSynapseFunction;

	private:		
		qulonglong mId;
		DoubleValue mStrengthValue;
		BoolValue mEnabledValue;

	};

}

#endif



