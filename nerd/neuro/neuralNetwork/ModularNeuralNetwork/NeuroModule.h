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

#ifndef NERDNeuroModule_H
#define NERDNeuroModule_H

#include "ModularNeuralNetwork/NeuronGroup.h"
#include <QSizeF>

namespace nerd {

	/**
	 * NeuroModule.
	 */
	class NeuroModule : public NeuronGroup {
	public:
		NeuroModule(const QString &name, qulonglong id = 0);
		NeuroModule(const NeuroModule &other);
		virtual ~NeuroModule();

		virtual NeuronGroup* createCopy() const;
		virtual NeuroModule* createDeepCopy() const;
		virtual void setId(qulonglong id);
	
		virtual bool setOwnerNetwork(ModularNeuralNetwork *network);

		virtual bool addInputNeuron(Neuron *neuron);
		virtual bool removeInputNeuron(Neuron *neuron);
		virtual const QList<Neuron*> getInputNeurons(int startLevel = 0) const;

		virtual bool addOutputNeuron(Neuron *neuron);
		virtual bool removeOutputNeuron(Neuron *neuron);
		virtual const QList<Neuron*> getOutputNeurons(int startLevel = 0) const;

		virtual bool addSubModule(NeuroModule *module);
		virtual bool removeSubModule(NeuroModule *module);
		virtual void removeAllSubModules();
		virtual const QList<NeuroModule*>& getSubModules() const;

		virtual NeuroModule* getParentModule() const;

		virtual bool addNeuron(Neuron *neuron);
		virtual bool removeNeuron(Neuron *neuron);
		virtual void removeAllNeurons();

// 		virtual QList<Neuron*> getAllEnclosedNeurons() const;
// 		virtual QList<NeuroModule*> getAllEnclosedModules() const;
		virtual QList<Synapse*> getAllEnclosedSynapses() const;

// 		virtual bool moveGroupToNetworkConst(ModularNeuralNetwork const *oldNetwork, 
// 										ModularNeuralNetwork *newNetwork);

		virtual bool moveGroupToLocation(double x, double y, double z);
		virtual bool replaceNeuron(Neuron *oldNeuron, Neuron *newNeuron);

		virtual void setSize(const QSizeF &size);
		virtual QSizeF getSize() const;

		virtual bool equals(NeuronGroup *group);

		void renewIds();
		QList<NeuralNetworkElement*> getAllEnclosedNetworkElements() const;

// 		virtual bool resolveConstraints();
// 		virtual bool extendModule();
// 		virtual bool pruneModule();
// 		virtual bool mutateModule();
		
	protected:		
		NeuroModule *mParentModule;
	};

}

#endif


