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



#include "NetworkVisualization.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QPainter>
#include <QMouseEvent>
#include <QRectF>
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Math/Math.h"
#include <QThread>
#include <QApplication>
#include "Util/Tracer.h"
#include <QMutexLocker>
#include "Value/ValueManager.h"
#include "NerdConstants.h"
#include "NeuralNetworkConstants.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Network/Neuro.h"
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"


using namespace std;


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(message);

namespace nerd {



/**
 * Constructs a new NetworkVisualization.
 */
NetworkVisualization::NetworkVisualization(const QString &name, NeuralNetworkEditor *owner)
	: QGLWidget(),  ParameterizedObject(name), mDefaultPaintIterval(300),
	  mInteractionPaintInterval(50),
	  mCommandExecutor(0), mLastLeftMousePressedPosition(0, 0), 
	  mLastRightMousePressedPosition(0, 0), mLastMidMousePressedPosition(0, 0),
	  mLastMousePosition(0, 0),
	  mMoveMode(false), mObjectDragMode(false),
	  mVisualizationOffset(QPointF(0.0, 0.0)), mScaling(1.0), mAlternativeScaling(0.2), mVisuHandler(0),
	  mScaleFactor(0.9), mNetworkInvalid(true), mSelectionMutex(QMutex::Recursive),
	  mModificationsEnabled(true), mPerformanceMode(0), mStasisMode(0),
	  mNetworkStructureChangedEvent(0), mClearCommandExecutorStack(0), mOwner(owner),
	  mActivationVisualiationMode(true), mInteractionMode(false)
{
	TRACE("NetworkVisualization::NetworkVisualization");
	mSelectionRectangle = new SelectionRectItem();
	addPaintItem(mSelectionRectangle);

	mElementPairVisualization = new ElementPairVisualization(2000);
	mElementPairVisualization->setOwner(this);
	addPaintItem(mElementPairVisualization);

	mCommandExecutor = new CommandExecutor();

	mPaintTimer.start(mDefaultPaintIterval);

	connect(&mPaintTimer, SIGNAL(timeout()),
			this, SLOT(paintTimerElapsed()));
	connect(this, SIGNAL(triggerUpdateVisualizationHandler()),
			this, SLOT(updateVisualizationHandler()));

	ValueManager *vm = Core::getInstance()->getValueManager();
	mPerformanceMode = vm->getBoolValue(NerdConstants::VALUE_RUN_IN_PERFORMANCE_MODE);
	if(mPerformanceMode != 0) {
		mPerformanceMode->addValueChangedListener(this);
	}
	mStasisMode = vm->getBoolValue(NeuralNetworkConstants::VALUE_EVO_STASIS_MODE);

	Neuro::getNeuralNetworkManager();
	mNetworkStructureChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
			NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_STRUCTURES_CHANGED, this);
	mNetworkParametersChangedEvent = Core::getInstance()->getEventManager()->registerForEvent(
		NeuralNetworkConstants::EVENT_NNM_CURRENT_NETWORK_PARAMETERS_CHANGED, this);
	mClearCommandExecutorStack = Core::getInstance()->getEventManager()->getEvent(
			NeuralNetworkConstants::EVENT_NETWORK_CLEAR_MODIFICATION_STACKS, true);
	if(mClearCommandExecutorStack != 0) {
		mClearCommandExecutorStack->addEventListener(this);
	}
}


/**
 * Destructor.
 */
NetworkVisualization::~NetworkVisualization() {

	TRACE("NetworkVisualization::~NetworkVisualization");

	mPaintTimer.stop();

	//allow observers to clear and release all references on this editor.
	mSelectedItems.clear();
	notifySelectionListeners();

	if(mVisuHandler != 0) {
		mVisuHandler->setNeuralNetwork(0);
		delete mVisuHandler;
	}

	//TODO check what to destroy
	delete mCommandExecutor;

	if(mPerformanceMode != 0) {
		mPerformanceMode->removeValueChangedListener(this);
	}

	//destroy local visus.
	delete mElementPairVisualization;
	delete mSelectionRectangle;
}


QString NetworkVisualization::getName() const {
	TRACE("NetworkVisualization::getName");
	return ParameterizedObject::getName();
}


void NetworkVisualization::eventOccured(Event *event){
	TRACE("NetworkVisualization::eventOccured");
	if(event == 0) {
		return;
	}
	else if(event == mNetworkStructureChangedEvent) {
		//make sure that changes that did not origin from the command executor
		notifyNeuralNetworkModified();
	}
	else if(event == mNetworkParametersChangedEvent) {
		notifyNeuralNetworkParametersChanged();
	}
	else if(event == mClearCommandExecutorStack) {
		mCommandExecutor->clearCommandStacks();
	}
}


void NetworkVisualization::valueChanged(Value *value) {
	TRACE("NetworkVisualization::valueChanged");
	if(value == 0) {
		return;
	}
	if(value == mPerformanceMode) {
		if(mPerformanceMode->get()) {
			update();
			//repaint();
			mPaintTimer.stop();
		}
		else {
			update();
			//repaint();
			if(mActivationVisualiationMode || mInteractionMode) {
				mPaintTimer.start();
			}
		}
	}
}

void NetworkVisualization::propertyChanged(Properties *owner, const QString &property) {
	if(owner == 0) {
		return;
	}
	if(property == NeuralNetworkConstants::TAG_HIDE_LAYERS) {
		updateHiddenLayerMode();
	}
}

bool NetworkVisualization::setNeuralNetwork(ModularNeuralNetwork *network) {
	TRACE("NetworkVisualization::setNeuralNetwork");
	QMutexLocker lock(&mSelectionMutex);

	mNetworkInvalid = true;

	if(mVisuHandler == 0) {
		return false;
	}
	if(QThread::currentThread() != QApplication::instance()->thread()) {
		cerr << "Wrong thread" << endl;
		return true;
	}

	mCommandExecutor->clearCommandStacks();
	clearSelection();

	//TODO check if this was important!! This made problems when loading a net and triggering networkReplacedEvent.
// 	if(network == mVisuHandler->getNeuralNetwork()) {
// 		if(network != 0) {
// 			mNetworkInvalid = false;
// 		}
// 		return true;
// 	}

	clearSelection();
	
	NeuralNetwork *oldNetwork = mVisuHandler->getNeuralNetwork();
	if(oldNetwork != 0) {
		oldNetwork->removePropertyChangedListener(this);
	}

	mNetworkInvalid = !(mVisuHandler->setNeuralNetwork(network));
	
	if(!mNetworkInvalid && network != 0) {
		network->addPropertyChangedListener(this);
	}

	clearSelection();

	mCurrentNetworkFile = "";

	emit neuralNetworkChanged(network);
	return mNetworkInvalid;
}


ModularNeuralNetwork* NetworkVisualization::getNeuralNetwork() const {
	TRACE("NetworkVisualization::getNeuralNetwork");
	if(mVisuHandler == 0) {
		return 0;
	}
	return mVisuHandler->getNeuralNetwork();
}


bool NetworkVisualization::setVisualizationHandler(NetworkVisualizationHandler *handler) {
	TRACE("NetworkVisualization::setVisualizationHandler");
	if(mVisuHandler != 0) {
		delete mVisuHandler;
		mVisuHandler = 0;
	}
	mVisuHandler = handler;
	return true;
}


NetworkVisualizationHandler* NetworkVisualization::getVisualizationHander() const {
	TRACE("NetworkVisualization::getVisualizationHander");
	return mVisuHandler;
}

void NetworkVisualization::invalidateItems() {
	TRACE("NetworkVisualization::invalidateItems");
	for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {	
		i.next()->invalidate();
	}
}



void NetworkVisualization::paintEvent(QPaintEvent*) {
	TRACE("NetworkVisualization::paintEvent");

	if(!isVisible()) {
		return;
	}

	QMutexLocker lock(&mSelectionMutex); 

	QPainter painter;
	painter.begin(this);
	//painter.setRenderHint(QPainter::Antialiasing);

	if(mNetworkInvalid) {
		painter.fillRect(0, 0, width(), height(), QBrush(QColor(255, 255, 255)));
		painter.end();
		return;
	}

	painter.setPen(QColor(0, 0, 0));

	if(mPerformanceMode != 0 && mPerformanceMode->get()) {
		painter.fillRect(0, 0, width(), height(), QBrush(QColor(0, 255, 255)));
		painter.end();
		return;
	}


	bool requiresSort = false;
	int minLevel = -100000;


	for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
		PaintItem *item = i.next();
		if(item->getPaintLevel() < minLevel) {
			requiresSort = true;
			break;
		}
		else {
			minLevel = item->getPaintLevel();
		}
	}

	QRectF visibleArea(-mVisualizationOffset.x(), -mVisualizationOffset.y(), 
						width() / mScaling, height() / mScaling);

	paintNetworkView(painter, visibleArea, mScaling, mVisualizationOffset);

	if(!mStatusMessages.empty()) {

		QList<QString> messages = mStatusMessages;
		QString message;
		for(int i = 0; i < messages.size(); ++i) {
			if(i != 0) { 
				message += " | ";
			}
			message += messages.at(i);
		}
		painter.setPen(QPen(QColor(255, 0, 0)));
		painter.drawText(10.0, height() - 10.0, message);
	}

	if(mStasisMode != 0 && mStasisMode->get()) {
		painter.fillRect(0, 0, width(), 10, QColor("red"));
		painter.fillRect(0, 0, 10, height(), QColor("red"));
		painter.fillRect(width() - 10, 0, width(), height(), QColor("red"));
		painter.fillRect(0, height() - 10, width(), 10, QColor("red"));
	}

	painter.end();

	if(requiresSort) {
		sortItems();
	}
}


/**
 * Paints the network graph with the QPainter, hereby only painting the visibleArea measured in 
 * network coordinates, given a specific scaling and painting offset.
 */
void NetworkVisualization::paintNetworkView(QPainter &painter, QRectF visibleArea, 
								double scaling, QPointF offset) 
{
	if(scaling == 0.0) {
		return;
	}

	QMutexLocker lock(&mSelectionMutex);

	QRect clearArea((visibleArea.x() + offset.x()) * scaling, (visibleArea.y() + offset.y()) * scaling, 
					visibleArea.width() * scaling, visibleArea.height() * scaling);

	painter.fillRect(clearArea, QBrush(QColor(255, 255, 255)));

	painter.scale(scaling, scaling);
	painter.translate(offset);

	for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
		PaintItem *item = i.next();
		QRectF paintingBox = item->getPaintingBox();
		if(paintingBox.intersects(visibleArea)) {
			item->paintSelf(&painter);
		}
	}

	painter.translate(-offset);
	painter.scale(1.0 / scaling, 1.0 / scaling);
}

NeuralNetworkEditor* NetworkVisualization::getOwner() const {
	return mOwner;
}

bool NetworkVisualization::isVisualizingActivations() const {
	return mActivationVisualiationMode;
}

void NetworkVisualization::setActivationVisualizationMode(bool visualize) {
	mActivationVisualiationMode = visualize;
}

void NetworkVisualization::setCurrentNetworkFileName(const QString &fileName) {
	mCurrentNetworkFile = fileName;
}

QString NetworkVisualization::getCurrentNetworkFileName() const {
	return mCurrentNetworkFile;
}

/**
 * The HOME bookmark is the viewport position specified by the optional _Viewport property of the current network.
 */
void NetworkVisualization::setHomeBookmark(double x, double y, double scale) {
	mBookmarks.insert(Qt::Key_F8, Vector3D(x, y, scale));	
}

void NetworkVisualization::setBookmark(int id, double x, double y, double scale) {
	mBookmarks.insert(Qt::Key_F3 + id, Vector3D(x, y, scale));
}

QHash<int, Vector3D> NetworkVisualization::getBookmarks() const {
	return mBookmarks;
}

void NetworkVisualization::updateHiddenLayerMode() {

	if(mOwner == 0 || mVisuHandler == 0 || mVisuHandler->getNeuralNetwork() == 0) {
		return;
	}
	
	ModularNeuralNetwork *net = mVisuHandler->getNeuralNetwork();
	if(net != 0 && mOwner->isHiddenLayerModeEnabled()) {
		QList<QString> _hiddenLayers = net->getProperty(NeuralNetworkConstants::TAG_HIDE_LAYERS).split(",");
		QList<QString> hiddenLayers;
		for(QListIterator<QString> i(_hiddenLayers); i.hasNext();) {
			QString layer = i.next().trimmed();
			if(layer != "") {
				hiddenLayers.append(layer);
			}
		}
		for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
			PaintItem *item = i.next();
			Properties *props = item->getEncapsulatedProperties();
			if(props == 0 || hiddenLayers.empty()) {
				item->setToHiddenLayer(false);
				continue;
			}
			QStringList layers = props->getProperty(NeuralNetworkConstants::TAG_LAYER_IDENTIFIER).split(",");
			bool hidden = false;
			for(QListIterator<QString> j(layers); j.hasNext();) {
				QString layer = j.next();
				if(layer != "" && hiddenLayers.contains(layer)) {
					hidden = true;
					break;
				}
			}
			item->setToHiddenLayer(hidden);
		}
		removeStatusMessage("Hidden Layers");
		addStatusMessage("Hidden Layers");
	}
	else {
		for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
			PaintItem *item = i.next();
			item->setToHiddenLayer(false);
		}
		removeStatusMessage("Hidden Layers");
	}
}

void NetworkVisualization::closeEvent(QCloseEvent*) {
	TRACE("NetworkVisualization::closeEvent");
	Core::getInstance()->scheduleTask(new ShutDownTask());
}

void NetworkVisualization::enableModifications(bool enable) {
	TRACE("NetworkVisualization::addPaintItem");
	mCommandExecutor->clearCommandStacks();
	mModificationsEnabled = enable;
}


void NetworkVisualization::updateVisualizationHandler() {
	if(mVisuHandler != 0) {
		mVisuHandler->rebuildView();
	}
}


bool NetworkVisualization::addPaintItem(PaintItem *item) {
	TRACE("NetworkVisualization::addPaintItem");
	if(item == 0 || mPaintItems.contains(item)) {
		return false;
	}

	for(QList<PaintItem*>::iterator i = mPaintItems.begin(); i != mPaintItems.end(); ++i) {
		if((*i)->getPaintLevel() > item->getPaintLevel()) {
			mPaintItems.insert(i, item);
			return true;
		}
	}
	mPaintItems.append(item);

	return true;
}


bool NetworkVisualization::removePaintItem(PaintItem *item) {
	TRACE("NetworkVisualization::removePaintItem");
	if(item == 0 || !mPaintItems.contains(item)) {
		return false;
	}
	mSelectedItems.removeAll(item);
	mPaintItems.removeAll(item);

	return true;
}



QList<PaintItem*> NetworkVisualization::getPaintItems() const {
	TRACE("NetworkVisualization::getPaintItems");
	return mPaintItems;
}


ElementPairVisualization* NetworkVisualization::getElementPairVisualization() const {
	return mElementPairVisualization;
}


void NetworkVisualization::setSelectedItems(QList<PaintItem*> selectedItems) {
	TRACE("NetworkVisualization::setSelectedItems");
	if(mNetworkInvalid) {
		clearSelection();
		return;
	}

	for(QListIterator<PaintItem*> i(mSelectedItems); i.hasNext();) {
		i.next()->setSelected(false);
	}
	mSelectedItems = selectedItems;
	for(QListIterator<PaintItem*> i(mSelectedItems); i.hasNext();) {
		i.next()->setSelected(true);
	}
	notifySelectionListeners();
}


void NetworkVisualization::selectAllItems(bool onlyVisible) {
	QList<PaintItem*> selectedItems;
	for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
		PaintItem *item = i.next();
		if(dynamic_cast<NeuronItem*>(item) != 0 
			|| dynamic_cast<SynapseItem*>(item) != 0
			|| dynamic_cast<ModuleItem*>(item) != 0
			|| dynamic_cast<GroupItem*>(item) != 0)
		{
			if(!onlyVisible || (!item->isHidden() && !item->isInHiddenLayer())) {
				selectedItems.append(item);
			}
		}
	}
	setSelectedItems(selectedItems);
}

QList<PaintItem*> NetworkVisualization::getSelectedItems() const {
	TRACE("NetworkVisualization::getSelectedItems");
	return mSelectedItems;
}


PaintItem* NetworkVisualization::getLastSelectedItem() const {
	TRACE("NetworkVisualization::getLastSelectedItem");
	if(mSelectedItems.empty()) {
		return 0;
	}
	return mSelectedItems.last();
}

void NetworkVisualization::validateSelectedItems() {
	if(mVisuHandler == 0) {
		return;
	}
	ModularNeuralNetwork *network = mVisuHandler->getNeuralNetwork();

	if(network == 0) {
		mSelectedItems.clear();
		return;
	}
	QList<Neuron*> neurons = network->getNeurons();
	QList<Synapse*> synapses = network->getSynapses();
	QList<NeuronGroup*> groups = network->getNeuronGroups();
	QList<NeuroModule*> modules = network->getNeuroModules();

	bool changed = false;

	//remove all items that are not valid any more.
	QList<PaintItem*> items = mSelectedItems;
	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			if(neuron == 0 || !neurons.contains(neuron)) {
				mSelectedItems.removeAll(item);
				changed = true;
				continue;
			}
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			Synapse *synapse = synapseItem->getSynapse();
			if(synapse == 0 || !synapses.contains(synapse)) {
				mSelectedItems.removeAll(item);
				changed = true;
				continue;
			}
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			if(module == 0 || !modules.contains(module)) {
				mSelectedItems.removeAll(item);
				changed = true;
				continue;
			}
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			NeuronGroup *group = groupItem->getNeuronGroup();
			if(group == 0 || !groups.contains(group)) {
				mSelectedItems.removeAll(item);
				changed = true;
				continue;
			}
		}
	}
	if(changed) {
		notifySelectionListeners();
	}
}


bool NetworkVisualization::addMouseListener(VisualizationMouseListener *listener) {
	TRACE("NetworkVisualization::addMouseListener");
	if(listener == 0 || mMouseListeners.contains(listener)) {
		return false;
	}
	mMouseListeners.append(listener);
	return true;
}


bool NetworkVisualization::removeMouseListener(VisualizationMouseListener *listener) {
	TRACE("NetworkVisualization::removeMouseListener");
	if(listener == 0 || !mMouseListeners.contains(listener)) {
		return false;
	}
	mMouseListeners.removeAll(listener);
	return true;
}


QList<VisualizationMouseListener*> NetworkVisualization::getMouseListeners() const {
	TRACE("NetworkVisualization::getMouseListeners");
	return mMouseListeners;
}


void NetworkVisualization::notifyMousePressedListeners(QMouseEvent *event, 
													   const QPointF &globalPosition)
{
	TRACE("NetworkVisualization::notifyMousePressedListeners");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}
	for(QListIterator<VisualizationMouseListener*> i(mMouseListeners); i.hasNext();) {
		i.next()->mouseButtonPressed(this, event, globalPosition);
	}
}


void NetworkVisualization::notifyMouseReleasedListeners(QMouseEvent *event, 
														const QPointF &globalPosition)
{	
	TRACE("NetworkVisualization::notifyMouseReleasedListeners");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}
	for(QListIterator<VisualizationMouseListener*> i(mMouseListeners); i.hasNext();) {
		i.next()->mouseButtonReleased(this, event, globalPosition);
	}
}


void NetworkVisualization::notifyMouseDoubleClickedListeners(QMouseEvent *event, 
															const QPointF &globalPosition) 
{
	TRACE("NetworkVisualization::notifyMouseReleasedListeners");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}
	for(QListIterator<VisualizationMouseListener*> i(mMouseListeners); i.hasNext();) {
		i.next()->mouseDoubleClicked(this, event, globalPosition);
	}
}

void NetworkVisualization::notifyMouseDraggedListeners(QMouseEvent *event, 
													   const QPointF &globalDistance)
{
	TRACE("NetworkVisualization::notifyMouseDraggedListeners");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}
	for(QListIterator<VisualizationMouseListener*> i(mMouseListeners); i.hasNext();) {
		i.next()->mouseDragged(this, event, globalDistance);
	}
}


bool NetworkVisualization::addSelectionListener(SelectionListener *listener) {	
	TRACE("NetworkVisualization::addSelectionListener");
	if(listener == 0 || mSelectionListeners.contains(listener)) {
		return false;
	}
	mSelectionListeners.append(listener);
	return true;
}


bool NetworkVisualization::removeSelectionListener(SelectionListener *listener) {
	TRACE("NetworkVisualization::removeSelectionListener");
	if(listener == 0 || !mSelectionListeners.contains(listener)) {
		return false;
	}
	mSelectionListeners.removeAll(listener);
	return true;
}


QList<SelectionListener*> NetworkVisualization::getSelectionListeners() const {
	TRACE("NetworkVisualization::getSelectionListeners");
	return mSelectionListeners;
}

void NetworkVisualization::notifySelectionListeners() {
	TRACE("NetworkVisualization::notifySelectionListeners");

	if(mNetworkInvalid) {
		return;
	}	

	QMutexLocker locker(&mSelectionMutex);

	for(QListIterator<SelectionListener*> i(mSelectionListeners); i.hasNext();) {
		i.next()->selectionChanged(mSelectedItems);
	}
}


void NetworkVisualization::notifyNeuralNetworkModified() {
	TRACE("NetworkVisualization::notifyNeuralNetworkModified");

	emit triggerUpdateVisualizationHandler();
	emit neuralNetworkModified(getNeuralNetwork());
	//repaint();
	update();
}

void NetworkVisualization::notifyNeuralNetworkParametersChanged() {
	emit neuralNetworkParametersModified(getNeuralNetwork());
	//update();
}

bool NetworkVisualization::addKeyListener(KeyListener *listener) {
	TRACE("NetworkVisualization::addKeyListener");
	if(listener == 0 || mKeyListeners.contains(listener)) {
		return false;
	}
	mKeyListeners.append(listener);
	return true;
}


bool NetworkVisualization::removedKeyListener(KeyListener *listener) {	
	TRACE("NetworkVisualization::removedKeyListener");
	if(listener == 0 || !mKeyListeners.contains(listener)) {
		return false;
	}
	mKeyListeners.removeAll(listener);
	return true;
}

QList<KeyListener*> NetworkVisualization::getKeyListeners() const {
	TRACE("NetworkVisualization::getKeyListeners");
	return mKeyListeners;
}


void NetworkVisualization::scaleVisualization(double factor) {
	TRACE("NetworkVisualization::scaleVisualization");
	double oldScaling = mScaling;
	mScaling = Math::max(mScaling * factor, 0.01);

	double halfWidth = ((double) width()) / 2.0;
	double halfHeight = ((double) height()) / 2.0;

	mVisualizationOffset = QPointF(
			(mVisualizationOffset.x() - (halfWidth / oldScaling)) + (halfWidth / mScaling),
			(mVisualizationOffset.y() - (halfHeight / oldScaling)) + (halfHeight / mScaling));

	//repaint();
	update();
}

void NetworkVisualization::setVisualizationScale(double scale) {

	double oldScaling = mScaling;
	mScaling = Math::max(scale, 0.01);

	double halfWidth = ((double) width()) / 2.0;
	double halfHeight = ((double) height()) / 2.0;

	mVisualizationOffset = QPointF(
			(mVisualizationOffset.x() - (halfWidth / oldScaling)) + (halfWidth / mScaling),
			(mVisualizationOffset.y() - (halfHeight / oldScaling)) + (halfHeight / mScaling));

	//repaint();
	update();

}



void NetworkVisualization::addStatusMessage(const QString &message) {
	TRACE("NetworkVisualization::addStatusMessage");
	if(!mStatusMessages.contains(message)) {
		mStatusMessages.append(message);
	}
}


void NetworkVisualization::removeStatusMessage(const QString &message) {
	TRACE("NetworkVisualization::removeStatusMessage");
	mStatusMessages.removeAll(message);
}


CommandExecutor* NetworkVisualization::getCommandExecutor() const  {	
	TRACE("NetworkVisualization::getCommandExecutor");
	return mCommandExecutor;
}


void NetworkVisualization::sortItems() {
	TRACE("NetworkVisualization::sortItems");

	QList<PaintItem*> items = mPaintItems;
	mPaintItems.clear();

	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		addPaintItem(i.next());
	}
}


void NetworkVisualization::mouseDoubleClickEvent(QMouseEvent *event) {
	TRACE("NetworkVisualization::mouseDoubleClickEvent");
	QMutexLocker lock(&mSelectionMutex);

	notifyMouseDoubleClickedListeners(event, (event->pos() / mScaling) - mVisualizationOffset);
}


void NetworkVisualization::mouseMoveEvent(QMouseEvent *event) {
	TRACE("NetworkVisualization::mouseMoveEvent");
	QMutexLocker lock(&mSelectionMutex);

	mLastMousePosition = event->posF();

	if(mNetworkInvalid) {
		return;
	}

	if(mMoveMode) {
		//Move offset of viewpoint
		QPointF move = (event->pos() - mLastMidMousePressedPosition) / mScaling;

		notifyMouseDraggedListeners(event, move);

		mVisualizationOffset = mVisualizationOffset + move;
		mLastMidMousePressedPosition = event->pos();
	}
	else if(event->buttons() & Qt::LeftButton) {
		QPointF move = (event->pos() - mLastLeftMousePressedPosition) / mScaling;

		notifyMouseDraggedListeners(event, move);

		if(mSelectionRectangle->isHidden()) {

			mObjectDragMode = true;
		
			for(QListIterator<PaintItem*> i(mSelectedItems); i.hasNext();) {
				PaintItem *item = i.next();
				if(item->getParent() != 0 && mSelectedItems.contains(item->getParent())) {
					continue;
				}
				item->mouseMoved(move, event->buttons());
			}
		}

		mLastLeftMousePressedPosition = event->pos();
	}
	else if(event->buttons() & Qt::RightButton) {

		QPointF move = event->pos() - mLastRightMousePressedPosition;
		notifyMouseDraggedListeners(event, move);

		if(!mSelectionRectangle->isHidden()) {

			QPointF globalPos = mSelectionRectangle->getGlobalPosition();
			mSelectionRectangle->setSize(move.x() / mScaling, move.y() / mScaling);
			QRectF rect(globalPos.x(),
					   globalPos.y(),
					   mSelectionRectangle->getWidth(),
					   mSelectionRectangle->getHeight());

			for(QListIterator<PaintItem*> i(mPaintItems); i.hasNext();) {
				PaintItem *item = i.next();

				bool selectionChanged = false;

				if(mSelectedItems.contains(item)) {
					if((event->modifiers() & Qt::ControlModifier) != Qt::ControlModifier) {
						if(!item->isActive() || item->isHidden() || item->isInHiddenLayer()
							|| !rect.contains(item->getBoundingBox()))
						{
							item->setSelected(false);	
							mSelectedItems.removeAll(item);
							selectionChanged = true;
						}
					}
				}
				else {
					if(item->isActive() && !item->isHidden() && !item->isInHiddenLayer() 
						&& item != mSelectionRectangle
						&& rect.contains(item->getBoundingBox()))
					{
						item->setSelected(true);
						mSelectedItems.append(item);
						selectionChanged = true;
					}
				}
				if(selectionChanged) {
					notifySelectionListeners();
				}
			}
		}
	}
}


void NetworkVisualization::mousePressEvent(QMouseEvent *event) {
	TRACE("NetworkVisualization::mousePressEvent");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}

	mInteractionMode = true;
	mPaintTimer.setInterval(mInteractionPaintInterval);

	if(!mPaintTimer.isActive()) {
		mPaintTimer.start();
	}

	setFocus();

	notifyMousePressedListeners(event, (event->pos() / mScaling) - mVisualizationOffset);

	if((event->buttons() & Qt::LeftButton && event->buttons() & Qt::RightButton)
		|| event->buttons() & Qt::MidButton) 
	{
		//enable viewpoint movement
		mLastMidMousePressedPosition = event->pos();
		mMoveMode = true;
	}
	else if(event->buttons() & Qt::LeftButton) {
		mLastLeftMousePressedPosition = event->pos();
		
		bool selectionChanged = false;

		for(int i = mPaintItems.size() - 1; i >= 0; --i) {
			PaintItem *item = mPaintItems.at(i);
			if(item->isActive() && !item->isHidden() 
				&& item->isHit((event->pos() / mScaling) - mVisualizationOffset, 
								event->buttons(), mScaling)) 
			{
				if((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
					if(mSelectedItems.contains(item)) {
						mSelectedItems.removeAll(item);
						item->setSelected(false);
						selectionChanged = true;
					}
					else {
						mSelectedItems.append(item);
						item->setSelected(true);
						selectionChanged = true;
					}
				}
				else if(!mSelectedItems.contains(item)) {
					clearSelection();
					mSelectedItems.append(item);
					item->setSelected(true);
					selectionChanged = true;
				}
				else {
					mSelectedItems.removeAll(item);
					mSelectedItems.append(item); //move this item to the end.
					item->setSelected(true);
					selectionChanged = true;
				}
				
				notifySelectionListeners();

				//repaint();
				update();
				return;
			}
		}
		if((event->modifiers() & Qt::ControlModifier) != Qt::ControlModifier) {
			clearSelection();
			selectionChanged = true;
		}

		if(selectionChanged) {
			notifySelectionListeners();
		}

		//enable viewpoint movement
		mLastMidMousePressedPosition = event->pos();
		mMoveMode = true;
	}
	else if(event->buttons() & Qt::RightButton) {
		mLastRightMousePressedPosition = event->pos();

		mSelectionRectangle->setLocalPosition(
				(-mVisualizationOffset + (mLastRightMousePressedPosition / mScaling)));
		mSelectionRectangle->setSize(0, 0);
		mSelectionRectangle->setHidden(false);

	}
	update();
	//repaint();
}


void NetworkVisualization::mouseReleaseEvent(QMouseEvent *event) {
	TRACE("NetworkVisualization::mouseReleaseEvent");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}

	//udpate object positions.
	if(mObjectDragMode) {
		updateNetworkElementPositionProperties(mSelectedItems);
	}
	mObjectDragMode = false;

	notifyMouseReleasedListeners(event, (event->pos() / mScaling) - mVisualizationOffset);

	if(!(event->buttons() & Qt::RightButton)) {
		mSelectionRectangle->setHidden(true);
	}
	mMoveMode = false;
	
	mInteractionMode = false;

	if(mActivationVisualiationMode) {
		mPaintTimer.setInterval(mDefaultPaintIterval);
	}
	else {
		mPaintTimer.stop();
	}
	
}



void NetworkVisualization::wheelEvent(QWheelEvent *event) {
	TRACE("NetworkVisualization::wheelEvent");

	double factor = event->delta() > 0 ? 1.0 / mScaleFactor : mScaleFactor;
	
	scaleVisualization(factor);
}


void NetworkVisualization::keyPressEvent(QKeyEvent *event) {	
	TRACE("NetworkVisualization::keyPressEvent");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}

	if(event->key() == Qt::Key_Up) {
		mVisualizationOffset += QPointF(0.0, 10.0);
	}
	else if(event->key() == Qt::Key_Down) {
		mVisualizationOffset += QPointF(0.0, -10.0);
	}
	else if(event->key() == Qt::Key_Left) {
		mVisualizationOffset += QPointF(10.0, 0.0);
	}
	else if(event->key() == Qt::Key_Right) {
		mVisualizationOffset += QPointF(-10.0, 0.0);
	}
	else if(event->key() == Qt::Key_Plus) {
		scaleVisualization(1.0 / mScaleFactor);
	}
	else if(event->key() == Qt::Key_Minus) {
		scaleVisualization(mScaleFactor);
	}
	else if(event->key() == Qt::Key_F2) {
		//cerr << "Mousepos: " << mLastMousePosition.x() << "," << mLastMousePosition.y() << " " << mScaling << "  Off: " << mVisualizationOffset.x() << "," << mVisualizationOffset.y() <<  endl;
		//mVisualizationOffset += (mLastMousePosition * mScaling);
		double scaling = mAlternativeScaling;
		mAlternativeScaling = mScaling;
		setVisualizationScale(scaling);
	}
	else if(event->key() >= Qt::Key_F3 && event->key() <= Qt::Key_F8) {
		if((event->modifiers() & Qt::ShiftModifier) != 0) {
			//store bookmark
			mBookmarks.insert(event->key(), 
				Vector3D((double) mVisualizationOffset.x(), (double) mVisualizationOffset.y(), mScaling));
		}
		else {
			if(mBookmarks.keys().contains(event->key())) {
				Vector3D bookmark = mBookmarks.value(event->key());
				mVisualizationOffset = QPointF((float) bookmark.getX(),
											   (float) bookmark.getY());
				mScaling = bookmark.getZ();
				update();
				//repaint();
			}
		}
	}
	else {
		event->ignore();
	}

	//notify other listeners
	for(QListIterator<KeyListener*> i(mKeyListeners); i.hasNext();) {
		i.next()->keyPressed(event);
	}
}


void NetworkVisualization::keyReleaseEvent(QKeyEvent *event) {
	TRACE("NetworkVisualization::keyReleaseEvent");
	QMutexLocker lock(&mSelectionMutex);

	if(mNetworkInvalid) {
		return;
	}

	//notify other listeners
	for(QListIterator<KeyListener*> i(mKeyListeners); i.hasNext();) {
		i.next()->keyReleased(event);
	}
}


void NetworkVisualization::invalidateNetwork() {
	TRACE("NetworkVisualization::invalidateNetwork");

	mSelectionMutex.lock();
	mNetworkInvalid = true;
	clearSelection();
	mSelectionMutex.unlock();
}

void NetworkVisualization::paintTimerElapsed() {
	TRACE("NetworkVisualization::paintTimerElapsed");
	update();
	//repaint();

	//restart paint timer so that the new interval starts after painting.
	if(mActivationVisualiationMode || mInteractionMode) {
		mPaintTimer.start();
	}
	else {
		mPaintTimer.stop();
	}
}


void NetworkVisualization::setDefaultPaintIterval(int interval) {
	TRACE("NetworkVisualization::setDefaultPaintIterval");

	mDefaultPaintIterval = interval;
	mPaintTimer.setInterval(mDefaultPaintIterval);
}


void NetworkVisualization::updateNetworkElementPositionProperties(QList<PaintItem*> affectedItems) {

	for(QListIterator<PaintItem*> i(affectedItems); i.hasNext();) {
		PaintItem *item = i.next();

		NeuronItem *neuronItem = dynamic_cast<NeuronItem*>(item);
		if(neuronItem != 0) {
			Neuron *neuron = neuronItem->getNeuron();
			neuron->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
			continue;
		}
		SynapseItem *synapseItem = dynamic_cast<SynapseItem*>(item);
		if(synapseItem != 0) {
			Synapse *synapse = synapseItem->getSynapse();
			synapse->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
			continue;
		}
		ModuleItem *moduleItem = dynamic_cast<ModuleItem*>(item);
		if(moduleItem != 0) {
			NeuroModule *module = moduleItem->getNeuroModule();
			module->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
			module->setProperty(NeuralNetworkConstants::TAG_MODULE_SIZE,
					QString::number(moduleItem->getSize().width()) + ","
					  + QString::number(moduleItem->getSize().height()));

			QList<PaintItem*> children = moduleItem->getMemberItems();
			if(!children.empty()) {
				updateNetworkElementPositionProperties(children);
			}
			continue;
		}
		GroupItem *groupItem = dynamic_cast<GroupItem*>(item);
		if(groupItem != 0) {
			NeuronGroup *group = groupItem->getNeuronGroup();
			group->setProperty(NeuralNetworkConstants::TAG_ELEMENT_LOCATION, 
					QString::number(item->getGlobalPosition().x()) + "," 
					  + QString::number(item->getGlobalPosition().y()) + ",0");
			continue;
		}
	}

}

void NetworkVisualization::setVisualizationCenter(double x, double y) {
	
// 	double halfWidth = ((double) width()) / 2.0;
// 	double halfHeight = ((double) height()) / 2.0;

// 	mVisualizationOffset = QPointF(
// 			(mVisualizationOffset.x() - (halfWidth / oldScaling)) + (halfWidth / mScaling),
// 			(mVisualizationOffset.y() - (halfHeight / oldScaling)) + (halfHeight / mScaling));
	setVisualizationOffset(QPointF(
			(mVisualizationOffset.x() + (x / mScaling)),
			(mVisualizationOffset.y() + (y / mScaling))));
// 			(mVisualizationOffset.y() - (halfHeight / oldScaling)) + (halfHeight / mScaling));
}


void NetworkVisualization::clearSelection() {
	TRACE("NetworkVisualization::clearSelection");

	QMutexLocker locker(&mSelectionMutex);

	for(QListIterator<PaintItem*> k(mSelectedItems); k.hasNext();) {
		k.next()->setSelected(false);
	}
	mSelectedItems.clear();
	notifySelectionListeners();
}

QMutex* NetworkVisualization::getSelectionMutex() {
	return &mSelectionMutex;
}


double NetworkVisualization::getScaling() const {
	return mScaling;
}

void NetworkVisualization::setScaling(double scaling) {
	mScaling = scaling;
}

QPointF NetworkVisualization::getVisualizationOffset() const {
	return mVisualizationOffset;
}

void NetworkVisualization::setVisualizationOffset(QPointF offset) {
	mVisualizationOffset = offset;
}


QList<PaintItem*> NetworkVisualization::getPaintItems(QList<NeuralNetworkElement*> elements) {
	QList<PaintItem*> items;

	for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		Neuron *neuron = dynamic_cast<Neuron*>(elem);
		if(neuron != 0) {
			PaintItem *item = mVisuHandler->getNeuronItem(neuron);
			if(item != 0) {
				items.append(item);
			}
			continue;
		}
		Synapse *synapse = dynamic_cast<Synapse*>(elem);
		if(synapse != 0) {
			PaintItem *item = mVisuHandler->getSynapseItem(synapse);
			if(item != 0) {
				items.append(item);
			}
			continue;
		}
		NeuroModule *module = dynamic_cast<NeuroModule*>(elem);
		if(module != 0) {
			PaintItem *item = mVisuHandler->getModuleItem(module);
			if(item != 0) {
				items.append(item);
			}
			continue;
		}
		NeuronGroup *group = dynamic_cast<NeuronGroup*>(elem);
		if(group != 0) {
			PaintItem *item = mVisuHandler->getGroupItem(group);
			if(item != 0) {
				items.append(item);
			}
			continue;
		}
	}
	return items;
}




}




