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



#ifndef NERDPropertyDialog_H
#define NERDPropertyDialog_H

#include <QString>
#include <QHash>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include "Gui/NetworkEditor/SelectionListener.h"
#include "Core/Properties.h"
#include <QLabel>
#include <QCheckBox>
#include "Gui/NetworkEditorTools/EditorToolWidget.h"


namespace nerd {

	class NeuralNetworkEditor;
	class ModularNeuralNetwork;
	class PaintItem;
	class EvolutionManager;
	class NetworkElement;

	/**
	 * PropertyDialog.
	 *
	 */
	class PropertyDialog : public EditorToolWidget, public virtual SelectionListener {
	Q_OBJECT
	public:
		PropertyDialog(NeuralNetworkEditor *owner);
		virtual ~PropertyDialog();

		virtual void selectionChanged(QList<PaintItem*> selectedItems);
		virtual void modificationPolicyChanged();

		QList<Properties*> getSelectedProperties() const;

		void addPropertyTemplate(const QString &property);
		QList<QString> getPropertyTemplates() const;
		void addStandardPropertyTemplates();

		virtual void invalidateView();

		void enableDialog(bool enable);

	public slots:
		void currentEditorTabChanged(int index);
		void networkChanged(ModularNeuralNetwork *network);
		void propertyRowChanged(int row);
		void setButtonPressed();
		void changeButtonPressed();
		void removeButtonPressed();
		void updatePropertyList();
		void updateElementSelection();
		void showHelpDialog();

	private:
		QList<ModularNeuralNetwork*> getMatchingNetworks();

	private:
		bool mInitialized;
		Properties *mCurrentProperties;
		QListWidget *mPropertyList;
		QScrollArea *mListScrollArea;
		QPushButton *mSetButton;
		QPushButton *mChangeButton;
		QPushButton *mRemoveButton;
		QComboBox *mPropertySelectionField;
		QLineEdit *mValueEditField;
		QLabel *mNameLabel;
		QCheckBox *mConsiderNeurons;
		QCheckBox *mConsiderSynapses;
		QCheckBox *mConsiderGroups;
		QCheckBox *mConsiderModules;
		QPushButton *mHelpButton;
		QCheckBox *mApplyToAllIndividuals;
		QCheckBox *mShowAllProperties;
		QPushButton *mSelectMatchingElementsButton;
		QCheckBox *mForceExactMatchForSelectionCheckBox;
		QCheckBox *mShowNonMatchingElementsCheckBox;
		EvolutionManager *mEvolutionManager;
	};

}

#endif




