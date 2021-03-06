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



#ifndef NERDNetworkVisualization_H
#define NERDNetworkVisualization_H

#include <QString>
#include <QHash>
#include <QGLWidget>
#include "Core/ParameterizedObject.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Value/IntValue.h"
#include "Value/DoubleValue.h"
#include "Value/StringValue.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include "Gui/NetworkEditor/SelectionRectItem.h"
#include "Gui/NetworkEditor/ElementPairVisualization.h"
#include "Gui/NetworkEditor/VisualizationMouseListener.h"
#include "Gui/NetworkEditor/SelectionListener.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include <QPointF>
#include "Command/CommandExecutor.h"
#include "Gui/KeyListener.h"
#include <QTimer>
#include <QMutex>
#include "Value/ValueChangedListener.h"
#include "Core/PropertyChangedListener.h"

namespace nerd {

class NeuroModulatorItem;

	class NetworkVisualizationHandler;
	class NeuralNetworkEditor;

	/**
	 * NetworkVisualization.
	 *
	 */
	class NetworkVisualization : public QGLWidget,
				public ParameterizedObject,	public virtual EventListener, 
				public virtual ValueChangedListener, public virtual PropertyChangedListener
	{
	Q_OBJECT

	public:
		NetworkVisualization(const QString &name, NeuralNetworkEditor *owner);
		virtual ~NetworkVisualization();

		virtual QString getName() const;
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);
		virtual void propertyChanged(Properties *owner, const QString &property);

		virtual bool setNeuralNetwork(ModularNeuralNetwork *network);
		virtual ModularNeuralNetwork* getNeuralNetwork() const;

		bool setVisualizationHandler(NetworkVisualizationHandler *handler);
		NetworkVisualizationHandler* getVisualizationHandler() const;

		void invalidateItems();

		bool addPaintItem(PaintItem *item);
		bool removePaintItem(PaintItem *item);
		QList<PaintItem*> getPaintItems() const;

		ElementPairVisualization* getElementPairVisualization() const;

		void setSelectedItems(QList<PaintItem*> selectedItems);
		void selectAllItems(bool onlyVisible = false);
		QList<PaintItem*> getSelectedItems() const;
		PaintItem* getLastSelectedItem() const;
		void validateSelectedItems();

		bool addMouseListener(VisualizationMouseListener *listener);
		bool removeMouseListener(VisualizationMouseListener *listener);
		QList<VisualizationMouseListener*> getMouseListeners() const;
		void notifyMousePressedListeners(QMouseEvent *event, const QPointF &globalPosition);
		void notifyMouseReleasedListeners(QMouseEvent *event, const QPointF &globalPosition);
		void notifyMouseDoubleClickedListeners(QMouseEvent *evnet, const QPointF &globalPosition);
		void notifyMouseDraggedListeners(QMouseEvent *event, const QPointF &globalDistance);

		bool addSelectionListener(SelectionListener *listener);
		bool removeSelectionListener(SelectionListener *listener);
		QList<SelectionListener*> getSelectionListeners() const;
		void notifySelectionListeners();

		void notifyNeuralNetworkModified();
		void notifyNeuralNetworkParametersChanged();

		bool addKeyListener(KeyListener *listener);
		bool removedKeyListener(KeyListener *listener);
		QList<KeyListener*> getKeyListeners() const;

		void scaleVisualization(double factor);
		void setVisualizationScale(double scale);

		void addStatusMessage(const QString &message);
		void removeStatusMessage(const QString &message);

		CommandExecutor* getCommandExecutor() const;

		void sortItems();

		virtual void keyPressEvent(QKeyEvent *event);
		virtual void keyReleaseEvent(QKeyEvent *event);

		void invalidateNetwork();
		void clearSelection();

		QMutex* getSelectionMutex();
		double getScaling() const;
		void setScaling(double scaling);
		QPointF getVisualizationOffset() const;
		void setVisualizationOffset(QPointF offset);

		QList<PaintItem*> getPaintItems(QList<NeuralNetworkElement*> elements);

		virtual void paintNetworkView(QPainter &painter, QRectF visibleArea, double scaling, QPointF offset);

		NeuralNetworkEditor* getOwner() const;
		
		bool isVisualizingActivations() const;
		void setActivationVisualizationMode(bool visualize);

		void setCurrentNetworkFileName(const QString &fileName);
		QString getCurrentNetworkFileName() const;

		void setHomeBookmark(double x, double y, double scale);
		void setBookmark(int id, double x, double y, double scale);
		QHash<int, Vector3D> getBookmarks() const;
		
		void updateHiddenLayerMode();

	public slots:
		void paintTimerElapsed();	
		void setDefaultPaintIterval(int interval);
		void updateNetworkElementPositionProperties(QList<PaintItem*> affectedItems);
		void setVisualizationCenter(double x, double y);

	protected:
		virtual void mouseDoubleClickEvent(QMouseEvent *event);	
		virtual void mouseMoveEvent(QMouseEvent *event);
		virtual void mousePressEvent(QMouseEvent *event);
		virtual void mouseReleaseEvent(QMouseEvent *event);
		virtual void wheelEvent(QWheelEvent *event);
		

	 protected:
     	void paintEvent(QPaintEvent *event);

	protected slots:
		void closeEvent(QCloseEvent *e);
		void enableModifications(bool enable);
		void updateVisualizationHandler();

	signals:
		void neuralNetworkChanged(ModularNeuralNetwork *network);
		void neuralNetworkModified(ModularNeuralNetwork *network);
		void neuralNetworkParametersModified(ModularNeuralNetwork *network);
		void triggerUpdateVisualizationHandler();


	private:
		QTimer mPaintTimer;
		int mDefaultPaintIterval;
		int mInteractionPaintInterval;
		CommandExecutor *mCommandExecutor;
		QList<PaintItem*> mPaintItems;
		QPointF mLastLeftMousePressedPosition;
		QPointF mLastRightMousePressedPosition;
		QPointF mLastMidMousePressedPosition;
		QPointF mLastMousePosition;
		bool mMoveMode;
		bool mObjectDragMode;
		bool mCtrlButtonPressed;
		bool mAltButtonPressed;
		QList<PaintItem*> mSelectedItems;
		SelectionRectItem *mSelectionRectangle;
		ElementPairVisualization *mElementPairVisualization;
		QPointF mVisualizationOffset;
		double mScaling;
		double mAlternativeScaling;
		QHash<int, Vector3D> mBookmarks;
		QList<VisualizationMouseListener*> mMouseListeners;
		QList<SelectionListener*> mSelectionListeners;
		QList<KeyListener*> mKeyListeners;
		NetworkVisualizationHandler *mVisuHandler;
		double mScaleFactor;
		QList<QString> mStatusMessages;
		bool mNetworkInvalid;
		QMutex mSelectionMutex;
		bool mModificationsEnabled;
		BoolValue *mPerformanceMode;
		BoolValue *mStasisMode;
		Event *mNetworkStructureChangedEvent;
		Event *mNetworkParametersChangedEvent;
		Event *mClearCommandExecutorStack;
		NeuralNetworkEditor *mOwner;
		bool mActivationVisualiationMode;
		bool mInteractionMode;
		QString mCurrentNetworkFile;
		NeuroModulatorItem *mNeuroModulatorItem;
	};

}

#endif




