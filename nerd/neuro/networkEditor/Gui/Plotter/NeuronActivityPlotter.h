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



#ifndef NERDNeuronActivityPlotter_H
#define NERDNeuronActivityPlotter_H

#include <QString>
#include <QHash>
#include "Gui/ValuePlotter/ValuePlotterWidget.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QPushButton>

namespace nerd {

	/**
	 * NeuronActivityPlotter.
	 *
	 */
	class NeuronActivityPlotter : public ValuePlotterWidget, public virtual EventListener
	{
	Q_OBJECT
	public:
		enum {PLOT_NEURON_ACTIVATION, PLOT_NEURON_OUTPUT, PLOT_SYNAPSE_WEIGHTS, PLOT_PARAMETERS};

	public:
		NeuronActivityPlotter(const QString &name, int plotMode, NeuralNetworkEditor *editor, 
							  int activeTab = 0, QWidget *parent = 0);
		virtual ~NeuronActivityPlotter();

		virtual void addPlottedNetworkElement(NeuralNetworkElement *element);

		virtual QString getName() const;
		virtual void eventOccured(Event *event);

	public slots:
		void currentTabChanged(int);
		void neuralNetworkModified(ModularNeuralNetwork *network);
		void neuralNetworkChanged(ModularNeuralNetwork *network);
		void addSelectedNeuronsButtonPressed();
		void removeNeuronsButtonPressed();

	private:
		void removeAllNetworkElements();
		QString getSynapseName(Synapse *synapse) const;

	private:
		NeuralNetworkEditor *mEditor;
		int mPlotMode;
		Event *mShutDownEvent;
		Event *mNetworksReplacedEvent;
		Event *mNetworksModifiedEvent;
		QList<NeuralNetworkElement*> mPlottedNetworkElements;
		QHash<NeuralNetworkElement*, QList<Value*> > mPlottedParameters;
		QPushButton *mAddSelectedButton;
		QPushButton *mRemoveNeuronsButton;
		
	};

}

#endif



