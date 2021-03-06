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



#ifndef NERDNeuralNetworkToolbox_H
#define NERDNeuralNetworkToolbox_H

#include <QString>
#include <QHash>
#include <QObject>
#include <QMenu>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "Gui/NetworkEditorTools/NetworkManipulationTool.h"
#include "Gui/NetworkEditorTools/InsertNeuronNetworkTool.h"
#include "Gui/NetworkEditorTools/InsertModuleNetworkTool.h"
#include "Gui/NetworkEditorTools/InsertSynapseNetworkTool.h"
#include "Gui/NetworkEditorTools/RemoveObjectsNetworkTool.h"
#include "Gui/NetworkEditorTools/RemoveSelectedObjectsNetworkTool.h"
#include "Gui/NetworkEditorTools/GrabNetworkElementIdTool.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditorTools/ShowElementPairTool.h"
#include "Gui/NetworkEditorTools/CopyPasteNetworkTool.h"
#include "Gui/NetworkEditorTools/ReconnectSynapseNetworkTool.h"
#include "Gui/NetworkEditorTools/ReplaceModuleTool.h"
#include "Gui/NetworkEditorTools/GuessModulePairs.h"
#include "Gui/NetworkEditorTools/GuessModulePairsByPosition.h"
#include "Gui/NetworkEditorTools/ConnectNeuronsWithSynapsesTool.h"


namespace nerd {

	/**
	 * NeuralNetworkToolbox.
	 *
	 */
	class NeuralNetworkToolbox : public QObject {
	Q_OBJECT
	public:
		enum {NoTool, InsertNeuronTool, RemoveNeuronTool, InsertSynapseTool};
		enum {ALIGN_TYPE_HORIZONTAL, ALIGN_TYPE_VERTICAL, ALIGN_MODE_LEFT, ALIGN_MODE_RIGHT, 
				ALIGN_MODE_TOP, ALIGN_MODE_BOTTOM, ALIGN_MODE_CENTER, ALIGN_MODE_SPACING, 
				ALIGN_MODE_DISTANCE};

	public:
		NeuralNetworkToolbox(NeuralNetworkEditor *owner);
		virtual ~NeuralNetworkToolbox();

	public slots:
		void editorTabChanged(int index);
		void toolIsDone();
		void useInsertNeuronTool();
		void useInsertModuleTool();
		void useInsertSynapseTool();
		void useRemoveObjectsTool();
		void useRemoveSelectedObjectsTool();
		void useGrabIdTool();
		void useCopyPasteTool();
		void resetCurrentNetwork();
		void enableModifications(bool enable);
		void alignAllSynapses();
		void alignSelectedSynapses();
		void selectSynapsesOfMarkedNeurons();
		void setAllLocationProperties();
		void alignNetworkElementsAccordingToLocationProperties();
		void alignNetworkElementsCenterHoriztonally();
		void alignNetworkElementsCenterVertically();
		void alignNetworkElementsDistanceHorizontally();
		void alignNetworkElementsDistanceVertically();
		void alignNetworkElementsSpacingHorizontally();
		void alignNetworkElementsSpacingVertically();
		void alignModuleSizeHorizontally();
		void alignModuleSizeVertically();
		void alignNetworkElementsLeft();
		void alignNetworkElementsRight();
		void alignNetworkElementsTop();
		void alignNetworkElementsBottom();
		void useVisualizeElementPairTool();
		void useReconnectSynapseTool();
		void useReplaceModuleTool();
		void useGuessGroupIdsTool();
		void useGuessGroupIdsByPositionTool();
		void updateViewMode();
		void fullyConnectGroupsBidirectional();
		void fullyConnectGroupsUniDirectional();
		void fullyConnectGroupsBidirectionalIgnoreInterfaces();
		void fullyConnectGroupsUniDirectionalIgnoreInterfaces();
		void fullyConnectSelectedNeurons();
		void fullyConnectSelectedNeuronsIgnoreInterfaces();
		void fullyConnectSingleGroup();
		void fullyConnectSelectedNeuronsToTarget();
		void fullyConnectTargetToSelectedElements();
		void grabIdsOfSelectedElements();
		void storeInitialOutput();
		void clearInitialOutput();

	protected:
		virtual void addNetworkMenu();
		void setTool(NetworkManipulationTool *currentTool);

	private:
		void alignNetworkElements(int type, int mode);
		void alignModuleSize(int type);
// 		void setLocation(PaintItem *item, const QString &location);
// 		void setLocationSize(ModuleItem *item, const QString &location);

	private:
		NeuralNetworkEditor *mOwner;
		NetworkManipulationTool *mCurrentTool;
		InsertNeuronNetworkTool *mInsertNeuronTool;
		InsertModuleNetworkTool *mInsertModuleTool;
		InsertSynapseNetworkTool *mInsertSynapseTool;	
		RemoveObjectsNetworkTool *mRemoveObjectsTool;
		RemoveSelectedObjectsNetworkTool *mRemoveSelectedObjectsTool;
		GrabNetworkElementIdTool *mGrabIdTool;
		ShowElementPairTool *mShowElementPairTool;
		CopyPasteNetworkTool *mCopyPasteTool;
		ReconnectSynapseNetworkTool *mReconnectSynapseTool;
		bool mModificationsEnabled;
		QMenu *mNetworkMenu;
		QMenu *mToolsMenu;
		QMenu *mViewMenu;
		ReplaceModuleTool *mReplaceModuleTool;
		GuessModulePairs *mGuessGroupPairsTool;
		GuessModulePairsByPosition *mGuessGroupPairsByPositionTool;
		ConnectNeuronsWithSynapsesTool *mConnectNeuronsWithSynapsesTool;
		
	};

}

#endif




