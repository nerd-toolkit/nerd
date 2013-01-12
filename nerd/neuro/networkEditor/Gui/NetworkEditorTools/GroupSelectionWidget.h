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


#ifndef NERDGroupSelectionWidget_H
#define NERDGroupSelectionWidget_H

#include <QString>
#include <QHash>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLineEdit>
#include "Gui/NetworkEditor/SelectionListener.h"
#include <QLabel>
#include <QCheckBox>
#include "Gui/NetworkEditorTools/EditorToolWidget.h"
#include "Gui/NetworkEditor/VisualizationMouseListener.h"

namespace nerd {

	class NeuralNetworkEditor;
	class ModularNeuralNetwork;
	class NeuronGroup;

	/**
	 * GroupSelectionWidget.
	 *
	 */
	class GroupSelectionWidget : public EditorToolWidget, public virtual SelectionListener,
								 public virtual VisualizationMouseListener  
	{
	Q_OBJECT
	public:
		GroupSelectionWidget(NeuralNetworkEditor *owner);
		virtual ~GroupSelectionWidget();

		virtual void selectionChanged(QList<PaintItem*> selectedItems);
		virtual void modificationPolicyChanged();

		virtual void invalidateView();

		virtual void mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseButtonReleased(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseDoubleClicked(NetworkVisualization *source,
					QMouseEvent *event, const QPointF &globalPosition);
		virtual void mouseDragged(NetworkVisualization *source, 
					QMouseEvent *event, const QPointF &globalDistance);

	public slots:
		void currentEditorTabChanged(int index);
		void networkChanged(ModularNeuralNetwork *network);
		void networkModified(ModularNeuralNetwork *network);
		void addButtonPressed();
		void removeButtonPressed();
		void renameButtonPressed();
		void addItemsButtonPressed();
		void removeItemsButtonPressed();
		void showMembersButtonPressed();
		void groupSelectionChanged();

	protected:
		void addAndRemoveItems(bool addToTarget);
		void updateGroupList();
		void clearGroupList();

	private:
		QListWidget *mGroupList;
		QPushButton *mAddItemsButton;
		QPushButton *mRemoveItemsButton;
		QPushButton *mShowMembersButton;
		QPushButton *mRenameButton;
		QPushButton *mAddButton;
		QPushButton *mRemoveButton;
		QLineEdit *mGroupNameField;

		QHash<QListWidgetItem*, NeuronGroup*> mNetworkGroups;
	};

}

#endif




