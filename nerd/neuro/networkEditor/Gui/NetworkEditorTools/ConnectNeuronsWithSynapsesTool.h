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


#ifndef NERDConnectNeuronsWithSynapsesTool_H
#define NERDConnectNeuronsWithSynapsesTool_H

#include <QString>
#include <QHash>
#include "Gui/NetworkEditorTools/NetworkManipulationTool.h"
#include "Gui/NetworkEditor/VisualizationMouseListener.h"
#include "ModularNeuralNetwork/NeuronGroup.h"
#include "Value/StringValue.h"
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "Network/Neuron.h"

namespace nerd {

	struct SynapseSet;
	class NeuralNetworkToolbox;
	
	/**
	 * ConnectNeuronsWithSynapsesTool.
	 *
	 */
	class ConnectNeuronsWithSynapsesTool  : public NetworkManipulationTool, 
											public VisualizationMouseListener
	{
		Q_OBJECT
		
	public:
		
		static const int MODUS_BIDIRECTIONAL = 2;
		static const int MODUS_IGNORE_INTERFACES = 4;
		static const int MODUS_SINGLE_GROUP = 8;
		static const int MODUS_SELECTED_ELEMENTS = 16;
		static const int MODUS_SELECTED_TO_GROUP = 32;
		static const int MODUS_SELECTED_FROM_GROUP = 64;
		
	public:
		ConnectNeuronsWithSynapsesTool(NeuralNetworkEditor *editor, NeuralNetworkToolbox *owner, int modus);
		virtual ~ConnectNeuronsWithSynapsesTool();
		
		virtual void clear();
		virtual void activate(NetworkVisualization *visu);
		virtual QString getStatusMessage();
		
		virtual void mouseButtonPressed(NetworkVisualization *source, 
										QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseButtonReleased(NetworkVisualization *source, 
										 QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseDoubleClicked(NetworkVisualization *source,
										QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseDragged(NetworkVisualization *source, 
								  QMouseEvent *event, const QPointF &globalDistance);
		
		virtual void setModus(int modus);
		virtual void setBasicName(const QString &name);
		
		void interconnectSingleGroup(const QList<Neuron*> neurons);
		void connectElements(const QList<Neuron*> &source, const QList<SynapseTarget*> &target);
		
	private:
		void createInsertSynapseCommand(SynapseSet set);
		
	private:
		QString mBaseName;
		int mModus;
		NeuralNetworkEditor *mEditor;
		NeuronGroup *mGroup1;
		NeuronGroup *mGroup2;
		int mCurrentState;
		SynapseTarget *mTargetElement;
		QList<Neuron*> mSourceNeurons;
		QList<SynapseTarget*> mTargetElements;
	};
	
}

#endif



