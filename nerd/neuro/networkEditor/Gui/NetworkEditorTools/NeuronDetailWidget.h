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



#ifndef NERDNeuronDetailWidget_H
#define NERDNeuronDetailWidget_H

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
#include "Value/ValueChangedListener.h"
#include "Gui/NetworkEditorTools/ParameterEditor.h"
#include "Gui/NetworkEditorTools/EditorToolWidget.h"
#include "Gui/NetworkEditor/VisualizationMouseListener.h"

namespace nerd {

	class NeuralNetworkEditor;
	class ModularNeuralNetwork;
	class Neuron;

	/**
	 * NeuronDetailWidget.
	 *
	 */
	class NeuronDetailWidget : public EditorToolWidget, public virtual SelectionListener,
								public virtual ValueChangedListener,
								public virtual VisualizationMouseListener
	{
	Q_OBJECT
	public:
		NeuronDetailWidget(NeuralNetworkEditor *owner);
		virtual ~NeuronDetailWidget();

		virtual void selectionChanged(QList<PaintItem*> selectedItems);

		virtual void valueChanged(Value *value);
		virtual QString getName() const;

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
		void changeNeuronName();
		void changeBias();
		void changeActivity();
		void changeOutput();
		void enableActivityObservation(int state);
		void enableBiasObservation(int state);
		void enableOutputObservation(int state);
		void transferFunctionSelectionChanged();
		void activationFunctionSelectionChanged();

    private slots:
        void markNameFieldAsEdited();
        void markBiasFieldAsEdited();
        void markActivationFieldAsEdited();
        void markOutputFieldAsEdited();
        void markAsValueEdited(QLineEdit *edit);
		void markAsValueUpdated(QLineEdit *edit);

	signals:
		void setBiasText(const QString &text);
		void setActivationText(const QString &text);
		void setOutputText(const QString &text);

	protected:
		void updateNeuronView();
		void udpateFunctionViews();

	private:
		bool mInitialized;
		QLineEdit *mNeuronNameField;
		QLineEdit *mActivityField;
		QLineEdit *mBiasField;
		QLineEdit *mOutputField;
		QCheckBox *mObserveActivityField;
		QCheckBox *mObserveBiasField;
		QCheckBox *mObserveOutputField;
		QComboBox *mTransferFunctionSelector;
		ParameterEditor *mTransferFunctionParameterEditor;
		QComboBox *mActivationFunctionSelector;
		ParameterEditor *mActivationFunctionParameterEditor;

		Neuron *mNeuron;
		DoubleValue *mActivationValue;
		DoubleValue *mBiasValue;
		DoubleValue *mOutputValue;
		bool mUpdatingChangedSelection;
		
		QList<Neuron*> mSlaveNeurons;

	};

}

#endif




