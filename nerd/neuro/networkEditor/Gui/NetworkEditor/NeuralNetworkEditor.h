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



#ifndef NERDNeuralNetworkEditor_H
#define NERDNeuralNetworkEditor_H

#include <QString>
#include <QHash>
#include <QMainWindow>
#include "Command/CommandExecutor.h"
#include <QFileDialog>
#include <QTabWidget>
#include "Gui/Containers/ActionWrapper.h"
#include "Gui/NetworkEditorTools/EditorMessageWidget.h"
#include "Gui/NetworkEditor/NeuralNetworkObserver.h"
#include "Value/ValueChangedListener.h"
#include <QTimer>
#include "Event/Event.h"
#include "Value/BoolValue.h"
#include "Value/IntValue.h"
#include "Value/StringValue.h"
#include "Event/EventListener.h"
#include "Event/Event.h"


namespace nerd {
	
	class NetworkVisualization;
	class ModularNeuralNetwork;
	class NetworkSearchDialog;

	/**
	 * NeuralNetworkEditor.
	 *
	 */
	class NeuralNetworkEditor : public QMainWindow, public virtual ValueChangedListener,
								public virtual EventListener 
	{
	Q_OBJECT
	public:
		NeuralNetworkEditor(QWidget *parent);
		virtual ~NeuralNetworkEditor();

		virtual void initialize();

		virtual int addNetworkVisualization(const QString &name);
		virtual bool removeNetworkVisualization(int index);
		int getNetworkVisualizationIndex(NetworkVisualization *visualization);
		NetworkVisualization* getCurrentNetworkVisualization() const;
		NetworkVisualization* getNetworkVisualization(int index) const;
		const QList<NetworkVisualization*>& getNetworkVisualizations() const;

		bool addNeuralNetworkObserver(NeuralNetworkObserver *observer);
		bool removeNeuralNetworkObserver(NeuralNetworkObserver *observer);
		const QList<NeuralNetworkObserver*>& getNeuralNetworkObservers() const;
		void invalidateNeuralNetworkObservers();

		EditorMessageWidget* getMessageWidget() const;

		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);
		virtual QString getName() const;

		void bringToolWidgetToFront(EditorToolWidget *widget);

		void triggerEnableNetworkModification(bool enable);

		virtual void renameCurrentNetwork(const QString &name);
		virtual void updateRecentNetworkMenu(const QString &fileName);

	signals:
		void tabSelectionChanged(int index);
		void enableNetworkModifications(bool enable);
		void changeDefaultPaintInterval(int interval);

	public slots:
		void undoCommand();
		void redoCommand();
		void searchForNetworkElement();
		void createTab();
		void destroyTab();
		void saveNetwork(const QString &fileName = "", ModularNeuralNetwork *net = 0, 
						 bool enableLogMessage = true, bool renameNetwork = true);
		void saveSubNetwork(const QString &fileName = "");
		void loadNetwork(const QString &fileName = "", bool addToNetwork = false);
		void addSubNetwork(const QString &fileName = "");
		void saveLayout(const QString &fileName = "", bool clearMessageArea = true, 
													 ModularNeuralNetwork *net = 0);
		void loadLayout(const QString &fileName = "", bool clearMessageArea = true);
		void createMinimalNetwork();
		void exportByteCode(const QString &fileName = "");
		void exportASeriesBDN(const QString &fileName = "");
		void exportMSeriesBDN(const QString &fileName = "");
		void exportDebugMSeriesComponentsBDN(const QString &fileName = "");
		void saveNetworkAsSvgGraphics(const QString &fileName = "");

		void currentTabChanged(int index);

		void toggleTimerExpired();
		void autoSaveTimerExpired();

		

	protected:
		virtual void setupMenuBar();
		virtual QMenu* addEditMenu();
		virtual QMenu* addPlotterMenu();
		virtual void setupTools();
		void updateUndoRedoActions();


		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);

	private:
		//QString getFileName(const QString &fileChooserTitle, bool existingFilesOnly);
		void saveRecentNetworkFileNames();
		void loadRecentNetworkFileNames();
		
	protected:
		QTabWidget *mMainPane;
		QList<NetworkVisualization*> mNetworkVisualizations;
		ActionWrapper *mUndoAction;
		ActionWrapper *mRedoAction;
		EditorMessageWidget *mMessageWidget;
		QList<NeuralNetworkObserver*> mNetworkObservers;
		IntValue *mDefaultPaintInterval;
		QList<QDockWidget*> mEditorToolWidgets;
		bool mWindowToggleState;
		QTimer mWindowToggleTimer;
		QTimer mAutoSaveTimer;
		NetworkSearchDialog *mSearchDialog;
		IntValue *mAutoSaveNetworkTimerSeconds;
		StringValue *mAutoSaveNetworkDirectory;
		QString mCurrentNetworkFileName;
		QMenu *mRecentNetworksMenu;
		Event *mShutDownEvent;
		
	};

}

#endif




