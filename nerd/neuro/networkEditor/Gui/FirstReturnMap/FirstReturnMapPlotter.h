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


#ifndef NERDFirstReturnMapPlotter_H
#define NERDFirstReturnMapPlotter_H

#include <QString>
#include <QHash>
#include "Gui/ValuePlotter/ValuePlotterWidget.h"
#include "Network/Neuron.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QPushButton>
#include <QWidget>

namespace nerd {

	/**
	 * FirstReturnMapPlotter.
	 *
	 */
	class FirstReturnMapPlotter : public QWidget, public virtual EventListener {
	Q_OBJECT
	public:
		enum {PLOT_NEURON_ACTIVATION, PLOT_NEURON_OUTPUT};
	public:
		FirstReturnMapPlotter(const QString &name, int plotMode, 
							  NeuralNetworkEditor *editor, int activeTab, QWidget *parent = 0);
		virtual ~FirstReturnMapPlotter();

		virtual QString getName() const;
		virtual void eventOccured(Event *event);

		virtual void addPlottedNeuron(Neuron *neuron);

		ValuePlotter* getValuePlotter() const;

		void setPlotterRange(int range);
		void setTriggerEventName(const QString &eventName);
		void hideHandle(bool hide);

	signals:
		void updateSupportedTriggerEvents();
		void setPlotterRange(const QString&);

	public slots:
		void showWidget();
		void plotterRangeChanged();
		void plotterItemCollectionChanged();
		void showLegendCheckBoxChanged(bool checked);
		void supportedTriggerEventsChanged();
		void triggerEventSelected(int index);
		void selectAllItems(int state);
		void currentTabChanged(int);
		void neuralNetworkModified(ModularNeuralNetwork *network);
		void neuralNetworkChanged(ModularNeuralNetwork *network);
		void addSelectedNeuronsButtonPressed();
		void removeNeuronsButtonPressed();
		void changeLineWidth();
		void enablePointConnections();
		void clearHistory();

	private:
		void removeAllNeurons();

	private:
		NeuralNetworkEditor *mEditor;
		int mPlotMode;
		Event *mShutDownEvent;
		Event *mNetworksReplacedEvent;
		Event *mNetworksModifiedEvent;
		QList<Neuron*> mPlottedNeurons;
		QPushButton *mAddSelectedButton;
		QPushButton *mRemoveNeuronsButton;
		QPushButton *mClearHistoryButton;

		ValuePlotter *mValuePlotter;
		QLineEdit *mHistorySizeField;
		QScrollArea *mPlotterItemsScroller;
		QVBoxLayout *mPlotterItemScrollerLayout;
		QVector<PlotterItemControlPanel*> mPlotterItemPanels;
		QCheckBox *mShowLegendCheckBox;
		QLineEdit *mLineWidthField;
		QCheckBox *mConnectLinesCheckBox;
		QComboBox *mUpdateEventSelector;
		QTabWidget *mControlArea;
		QCheckBox *mSelectAllCheckBox;
		QSplitter *mSplitter;
	};

}

#endif



