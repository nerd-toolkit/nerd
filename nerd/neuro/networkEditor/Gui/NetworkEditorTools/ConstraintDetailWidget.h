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


#ifndef NERDConstraintDetailWidget_H
#define NERDConstraintDetailWidget_H

#include <QString>
#include <QHash>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include "Gui/NetworkEditor/SelectionListener.h"
#include <QLabel>
#include <QCheckBox>
#include "Gui/NetworkEditorTools/ParameterEditor.h"
#include "Gui/NetworkEditorTools/EditorToolWidget.h"

namespace nerd {

	class NeuralNetworkEditor;
	class ModularNeuralNetwork;
	class GroupConstraint;
	class NeuronGroup;

	/**
	 * ConstraintDetailWidget.
	 *
	 */
	class ConstraintDetailWidget : public EditorToolWidget, public virtual SelectionListener
	{
	Q_OBJECT
	public:
		ConstraintDetailWidget(NeuralNetworkEditor *owner);
		virtual ~ConstraintDetailWidget();

		virtual void selectionChanged(const QList<PaintItem*> &selectedItems);
		virtual void modificationPolicyChanged();

		virtual void invalidateView();

		QList<NeuronGroup*> getSelectedGroups() const;

	public slots:
		void currentEditorTabChanged(int index);
		void networkChanged(ModularNeuralNetwork *network);
		void networkModified(ModularNeuralNetwork *network);
		void constraintListSelectionChanged();
		void addButtonPressed();
		void removeButtonPressed();

	protected:
		void updateView();

	private:
		bool mInitialized;
		QLabel *mGroupInfoLabel;
		QListWidget *mConstraintList;
		QComboBox *mConstraintPoolSelector;
		ParameterEditor *mConstraintParameterEditor;
		QPushButton *mAddConstraintButton;
		QPushButton *mRemoveConstraintButton;
		QCheckBox *mApplyToAllSelectedCheckBox;

		NeuronGroup *mCurrentGroup;
		NeuronGroup *mPreviousGroup;
		QList<GroupConstraint*> mCurrentConstraints;
		QList<NeuronGroup*> mCurrentlySelectedGroups;
	};

}

#endif




