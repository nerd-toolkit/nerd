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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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



#include "NeuralNetworkEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include <QMenuBar>
#include <QMenu>
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "TransferFunction/TransferFunctionTanh.h"
#include "Core/Properties.h"
#include "IO/NeuralNetworkIO.h"
#include "IO/NeuralNetworkIOBytecode.h"
#include "IO/NeuralNetworkIOASeriesBDN.h"
#include "IO/NeuralNetworkIOMSeriesBDN.h"
#include "IO/DebugNeuralNetworkIOMSeriesComponentsBDN.h"
#include <QDockWidget>
#include "Gui/NetworkEditorTools/GroupSelectionWidget.h"
#include "Gui/NetworkEditorTools/PropertyDialog.h"
#include "Gui/NetworkEditorTools/NeuronDetailWidget.h"
#include "Gui/NetworkEditorTools/SynapseDetailWidget.h"
#include "Gui/NetworkEditorTools/ConstraintDetailWidget.h"
#include "Gui/NetworkEditorTools/NetworkDetailWidget.h"
#include "Gui/NetworkEditorTools/EditorMessageWidget.h"
#include "Gui/NetworkEditorTools/ModuleManagementWidget.h"
#include "Util/LoadNetworkIntoExistingNetwork.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutex>
#include <QMutexLocker>
#include "Gui/NetworkEditorTools/AddActivationPlotterAction.h"
#include "Gui/NetworkEditorTools/AddFirstReturnMapPlotterAction.h"
#include "Gui/Plotter/NeuronActivityPlotter.h"
#include "Gui/FirstReturnMap/FirstReturnMapPlotter.h"
#include "NerdConstants.h"
#include "Gui/NetworkEditorCommands/SetLocationPropertiesCommand.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Control/ControlInterface.h"
#include <QSvgGenerator>
#include <limits>
#include "Math/Math.h"
#include "Gui/NetworkEditor/NetworkSearchDialog.h"
#include "Util/NerdFileSelector.h"
#include "Gui/NetworkEditor/LoadRecentNetworkAction.h"
#include "NetworkEditorConstants.h"
#include "Value/ColorValue.h"
#include <PlugIns/CommandLineArgument.h>
#include <Gui/GuiManager.h>

using namespace std;

namespace nerd {


/**
 * Constructs a new NeuralNetworkEditor.
 */
NeuralNetworkEditor::NeuralNetworkEditor(QWidget *parent)
	: QMainWindow(parent), mMainPane(0),
	  mUndoAction(0), mRedoAction(0), mWindowToggleState(true), mViewModeMenu(0),
	  mToggleWindowArgument(0)
{	

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mMainPane = new QTabWidget(this);
	mMainPane->setFocus();

	connect(mMainPane, SIGNAL(currentChanged(int)),
			this, SLOT(currentTabChanged(int)));

	mUndoAction = new ActionWrapper("Undo", this);
	mRedoAction = new ActionWrapper("Redo", this);

	mRecentNetworksMenu = new QMenu("Load Recent Network ...");

	setCentralWidget(mMainPane);

	ValueManager *vm = Core::getInstance()->getValueManager();
	
	mDefaultPaintInterval = new IntValue(300);
	vm->addValue("/NetworkEditor/VisualizationUpdateInterval",
				mDefaultPaintInterval);
	mDefaultPaintInterval->addValueChangedListener(this);

	mAutoSaveNetworkDirectory = new StringValue(
			Core::getInstance()->getConfigDirectoryPath() + "/autoSave");
	mAutoSaveNetworkDirectory->useAsFileName(true);
	vm->addValue("/NetworkEditor/AutoSaveNetwork/Directory", mAutoSaveNetworkDirectory);

	mAutoSaveNetworkTimerSeconds = new IntValue(30);
	vm->addValue("/NetworkEditor/AutoSaveNetwork/IntervalSeconds", mAutoSaveNetworkTimerSeconds);
	mAutoSaveNetworkTimerSeconds->addValueChangedListener(this);
	
	vm->addValue(NetworkEditorConstants::VALUE_DOF_MODE_COLOR_NORMAL,
					new ColorValue(0, 0, 0, 255));
// 	vm->addValue(NetworkEditorConstants::VALUE_DOF_MODE_COLOR_HIGHLIGHT,
// 					new ColorValue(100,100,100,150));
	vm->addValue(NetworkEditorConstants::VALUE_DOF_MODE_COLOR_HIGHLIGHT,
					new ColorValue(130,130,130,255));
	vm->addValue(NetworkEditorConstants::VALUE_DOF_MODE_WIDTH_HIGHLIGHT,
					new IntValue(3));
	
	mClearSelectionsEvent = Core::getInstance()->getEventManager()
					->createEvent(NetworkEditorConstants::VALUE_EDITOR_CLEAR_ALL_SELECTIONS);
	mClearSelectionsEvent->addEventListener(this);
	

	connect(&mAutoSaveTimer, SIGNAL(timeout()),
			this, SLOT(autoSaveTimerExpired()));

	mSearchDialog = new NetworkSearchDialog(this);

	renameCurrentNetwork("");

	//restore the last window settings of this editor.
	restoreWidgetGeometries();
}



/**
 * Destructor.
 */
NeuralNetworkEditor::~NeuralNetworkEditor() {
}

void NeuralNetworkEditor::initialize() {
	setupMenuBar();
	setupTools();

	mToggleWindowArgument = Core::getInstance()->getValueManager()->getStringValue(
								"/CommandLineArguments/toggle");
	if(mToggleWindowArgument != 0) {
		mToggleWindowArgument->addValueChangedListener(this);
		connect(&mWindowToggleTimer, SIGNAL(timeout()),
					this, SLOT(toggleTimerExpired()));
		//update toggle state
		valueChanged(mToggleWindowArgument);
	}
	
	
	mShutDownEvent = Core::getInstance()->getShutDownEvent();
	mShutDownEvent->addEventListener(this);

	loadRecentNetworkFileNames();
}

int NeuralNetworkEditor::addNetworkVisualization(const QString &name) {
	NetworkVisualization *visu = new NetworkVisualization(name, this);
	visu->setVisualizationHandler(
			new SimpleNetworkVisualizationHandler(name + "/Handler", visu));
	int index = mMainPane->addTab(visu, name);
	mMainPane->setCurrentIndex(index);
	updateUndoRedoActions();
	connect(this, SIGNAL(enableNetworkModifications(bool)), 
			visu, SLOT(enableModifications(bool)));
	connect(this, SIGNAL(changeDefaultPaintInterval(int)),
			visu, SLOT(setDefaultPaintIterval(int)));
	visu->setNeuralNetwork(new ModularNeuralNetwork());

	mNetworkVisualizations.append(visu);

	emit changeDefaultPaintInterval(mDefaultPaintInterval->get());
	return index;
}

bool NeuralNetworkEditor::removeNetworkVisualization(int index) {
	if(index >= 0 && index < mMainPane->count()) {
		QWidget *visu = mMainPane->widget(index);
		mMainPane->removeTab(index);
		updateUndoRedoActions();
		disconnect(this, SIGNAL(enableNetworkModifications(bool)), 
				   visu, SLOT(enableModifications(bool)));
		disconnect(this, SIGNAL(changeDefaultPaintInterval(int)),
			visu, SLOT(setDefaultPaintIterval(int)));

		mNetworkVisualizations.removeAll(dynamic_cast<NetworkVisualization*>(visu));

		delete visu;
		return true;
	}
	return false;
}

int NeuralNetworkEditor::getNetworkVisualizationIndex(NetworkVisualization *visualization) {
	return mMainPane->indexOf(visualization);
}

NetworkVisualization* NeuralNetworkEditor::getCurrentNetworkVisualization() const {
	return dynamic_cast<NetworkVisualization*>(mMainPane->currentWidget());
}

NetworkVisualization* NeuralNetworkEditor::getNetworkVisualization(int index) const {
	if(index >= 0 && index < mMainPane->count()) {
		return dynamic_cast<NetworkVisualization*>(mMainPane->widget(index));
	}
	return 0;
}

QList<NetworkVisualization*> NeuralNetworkEditor::getNetworkVisualizations() const {
	return mNetworkVisualizations;
}


bool NeuralNetworkEditor::addNeuralNetworkObserver(NeuralNetworkObserver *observer) {
	if(observer == 0 || mNetworkObservers.contains(observer)) {
		return false;
	}
	mNetworkObservers.append(observer);
	return true;
}


bool NeuralNetworkEditor::removeNeuralNetworkObserver(NeuralNetworkObserver *observer) {
	if(observer == 0 || !mNetworkObservers.contains(observer)) {
		return false;
	}
	mNetworkObservers.removeAll(observer);
	return true;
}


QList<NeuralNetworkObserver*> NeuralNetworkEditor::getNeuralNetworkObservers() const {
	return mNetworkObservers;
}


void NeuralNetworkEditor::invalidateNeuralNetworkObservers() {
	for(QListIterator<NeuralNetworkObserver*> i(mNetworkObservers); i.hasNext();) {
		i.next()->invalidateView();
	}
}



EditorMessageWidget* NeuralNetworkEditor::getMessageWidget() const {
	return mMessageWidget;
}


void NeuralNetworkEditor::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	if(value == mDefaultPaintInterval) {
		emit changeDefaultPaintInterval(mDefaultPaintInterval->get());
	}
	else if(value == mAutoSaveNetworkTimerSeconds) {
		mAutoSaveTimer.setInterval(Math::max(5, mAutoSaveNetworkTimerSeconds->get()) * 1000);
	}
	else if(value == mToggleWindowArgument) {
		if(mToggleWindowArgument != 0) {
			QString entry = mToggleWindowArgument->get();
			if(entry.trimmed() == "{toggle}" || entry.trimmed() == "{}") {
				mWindowToggleTimer.setInterval(100);
				mWindowToggleTimer.start(100);
			}
			else {
				mWindowToggleTimer.stop();
			}
		}
	}
}


void NeuralNetworkEditor::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	if(event == mShutDownEvent) {
		saveRecentNetworkFileNames();
		storeWidgetGeometries();
	}
	else if(event == mClearSelectionsEvent) {
		clearAllSelections();
	}
}


QString NeuralNetworkEditor::getName() const {
	return "NeuralNetworkEditor";
}


void NeuralNetworkEditor::bringToolWidgetToFront(EditorToolWidget *widget) {
	if(widget == 0) {
		return;
	}
	QDockWidget *frontWidget = 0;
	for(QListIterator<QDockWidget*> i(mEditorToolWidgets); i.hasNext();) {
		QDockWidget *w = i.next();
		if(w->widget() == widget) {
			frontWidget = w;
			break;
		}	
	}
	if(frontWidget == 0) {
		return;
	}
	frontWidget->raise();
}

void NeuralNetworkEditor::triggerEnableNetworkModification(bool enable) {
	if(enable) {
		mAutoSaveTimer.setSingleShot(false),
		mAutoSaveTimer.start(Math::max(5, mAutoSaveNetworkTimerSeconds->get()) * 1000);
		//save current network immediately.
		autoSaveTimerExpired();
	}
	else {
		mAutoSaveTimer.stop();
	}

	emit enableNetworkModifications(enable);
}


void NeuralNetworkEditor::renameCurrentNetwork(const QString &name) {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->setCurrentNetworkFileName(name);
	}
	setWindowTitle("Network Editor [" + name + "]");
}

void NeuralNetworkEditor::updateRecentNetworkMenu(const QString &fileName) { 
	
	QList<QAction*> actions = mRecentNetworksMenu->actions();
	for(QListIterator<QAction*> i(actions); i.hasNext();) {
		LoadRecentNetworkAction *action = dynamic_cast<LoadRecentNetworkAction*>(i.next());
		if(action->getFileName() == fileName) {
			if(actions.size() <= 1) {
				return;
			}
			mRecentNetworksMenu->removeAction(action);
			mRecentNetworksMenu->insertAction(mRecentNetworksMenu->actions().first(), action);
			return;
		}
	}
	//if not found, then add a new loader action.
	LoadRecentNetworkAction *loadAction = new LoadRecentNetworkAction(fileName, this);
	if(mRecentNetworksMenu->actions().empty()) {
		mRecentNetworksMenu->addAction(loadAction);
		return;
	}
	else {
		mRecentNetworksMenu->insertAction(mRecentNetworksMenu->actions().first(), loadAction);
	}
	//check if there are networks to remove.
	while(mRecentNetworksMenu->actions().size() > 10) {
		QAction *action = mRecentNetworksMenu->actions().last();
		mRecentNetworksMenu->removeAction(action);
		delete action;
	}
}

bool NeuralNetworkEditor::isHiddenLayerModeEnabled() const {
	return mEnableHiddenLayersCheckbox->isChecked();
}

QMenu* NeuralNetworkEditor::getViewModeMenu() const {
	return mViewModeMenu;
}

void NeuralNetworkEditor::clearAllSelections() {
	for(QListIterator<NetworkVisualization*> i(mNetworkVisualizations); i.hasNext();) {
		NetworkVisualization *visu = i.next();
		if(visu != 0) {
			visu->setSelectedItems(QList<PaintItem*>());
		}
	}
}


void NeuralNetworkEditor::storeWidgetGeometries() {
// 	QSettings *settings = GuiManager::getGlobalGuiManager()->getWidgetSettings();
// 	if(settings != 0) {
// 		settings->setValue("network-editor/geometry", saveGeometry());
// 		settings->setValue("network-editor/pos", pos());
// 		settings->setValue("network-editor/size", size());
// 	}
}


void NeuralNetworkEditor::restoreWidgetGeometries() {
// 	QSettings *settings = GuiManager::getGlobalGuiManager()->getWidgetSettings();
// 	if(settings != 0) {
// 		restoreGeometry(settings->value("network-editor/geometry").toByteArray());
// 		QPoint pos = settings->value("network-editor/pos", QPoint(10, 10)).toPoint();
// 		QSize size = settings->value("network-editor/size", QSize(400, 400)).toSize();
// 		
// 		move(pos);
// 		resize(size);
// 	}
}


void NeuralNetworkEditor::undoCommand() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->getCommandExecutor()->undoLastCommand();
	}
}


void NeuralNetworkEditor::redoCommand() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->getCommandExecutor()->doNextCommand();
	}
}


void NeuralNetworkEditor::searchForNetworkElement() {
	mSearchDialog->showWidget();
}


void NeuralNetworkEditor::createTab() {
	addNetworkVisualization("New Tab");
}


void NeuralNetworkEditor::destroyTab() {
	removeNetworkVisualization(getNetworkVisualizationIndex(getCurrentNetworkVisualization()));
}

void NeuralNetworkEditor::saveNetwork(const QString &fileName, ModularNeuralNetwork *net, 
									  bool enableLogMessage, bool renameNetwork) 
{

	if(enableLogMessage) {
		mMessageWidget->clear();
	}

	BoolValue *pauseValue = Core::getInstance()->getValueManager()->getBoolValue(
						NerdConstants::VALUE_EXECUTION_PAUSE);

	bool paused = false;
	if(pauseValue != 0) {
		paused = pauseValue->get();
		pauseValue->set(true);
	}

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		if(net == 0) {
			nameOfFile = NerdFileSelector::getFileName("Save Neural Network", false, this);
		}
		else {
			nameOfFile = NerdFileSelector::getFileName("Save Neural Subnetwork", false, this);
		}
	}

	if(nameOfFile == "") {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	if(!nameOfFile.endsWith(".onn")) {
		nameOfFile.append(".onn");
	}

	ModularNeuralNetwork *network = net;
	NetworkVisualizationHandler *handler = 0;

	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0 
		&& visu->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(network == 0 && handler != 0) {
			network = handler->getNeuralNetwork();

			if(network != 0) {
				QPointF viewport = visu->getVisualizationOffset();
				network->setProperty("_Viewport", QString::number(viewport.x()) + ","
							+ QString::number(viewport.y()) + "," 
							+ QString::number(visu->getScaling()));
				
				//Store bookmarks in network XML.
				QString bookmarkString = "";
				QHash<int, Vector3D> bookmarks = visu->getBookmarks();
				for(int i = Qt::Key_F3; i <= Qt::Key_F8; ++i) {
					if(bookmarks.keys().contains(i)) {
						Vector3D bookmark = bookmarks.value(i);
						bookmarkString += QString::number(bookmark.getX()) + ","
											+ QString::number(bookmark.getY()) + ","
											+ QString::number(bookmark.getZ());
					}
					if(i != Qt::Key_F8) {
						bookmarkString += "|";
					}
				}
				if(bookmarkString != "") {
					network->setProperty("_Bookmarks", bookmarkString);
				}
			}
		}
	}

	if(network == 0 || handler == 0) {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	QString errorMessage;
	QList<QString> warnings;

	if(NeuralNetworkIO::createFileFromNetwork(nameOfFile, network, &errorMessage, &warnings)) {
		if(enableLogMessage) {
			mMessageWidget->addMessage("Successfully saved neural network to file \n[" + nameOfFile + "]");
		}
	}
	else {
		if(enableLogMessage) {
			mMessageWidget->addMessage("Failed saving neural network to file \n[" + nameOfFile + "]");
		}
	}

	if(errorMessage != "") {
		if(enableLogMessage) {
			mMessageWidget->addMessage(errorMessage);
		}
	}
	if(!warnings.empty() && enableLogMessage) {
		for(QListIterator<QString> j(warnings); j.hasNext();) {
			mMessageWidget->addMessage(j.next());
		}
	}
	
	if(renameNetwork) {
		renameCurrentNetwork(nameOfFile);
		updateRecentNetworkMenu(nameOfFile);
	}

	if(pauseValue != 0) {
		pauseValue->set(paused);
	}

	//TODO this is not neccessary any more since the positon information is now part of the network.
	//save the corresponding layout
// 	QString layoutFileName = nameOfFile.mid(0, nameOfFile.size() - 4).append(".lyt");
// 	saveLayout(layoutFileName, false, net);
}





void NeuralNetworkEditor::saveSubNetwork(const QString &fileName) {

	ModularNeuralNetwork *network = 0;
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	NetworkVisualizationHandler *handler = 0;

	if(visu != 0) 
	{
		handler = visu->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}
	
	//create network from selected objects
	visu->getSelectionMutex()->lock();
	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	ModularNeuralNetwork *net = new ModularNeuralNetwork(AdditiveTimeDiscreteActivationFunction(),
										TransferFunctionTanh(), SimpleSynapseFunction());

	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *item = i.next();
		NeuronItem *neuron = dynamic_cast<NeuronItem*>(item);
		if(neuron != 0) {
			net->addNeuron(neuron->getNeuron());
			continue;
		}
		ModuleItem *module = dynamic_cast<ModuleItem*>(item);
		if(module != 0) {
			net->addNeuronGroup(module->getNeuroModule());
			continue;
		}
		GroupItem *group = dynamic_cast<GroupItem*>(item);
		if(group != 0) {
			net->addNeuronGroup(group->getNeuronGroup());
			continue;
		}
	}

	saveNetwork(fileName, net, true, false);
	net->freeElements(false);

	delete net;

	visu->getSelectionMutex()->unlock();
	
}

void NeuralNetworkEditor::loadNetwork(const QString &fileName, bool addToNetwork) {

	mMessageWidget->clear();

	BoolValue *pauseValue = Core::getInstance()->getValueManager()->getBoolValue(
						NerdConstants::VALUE_EXECUTION_PAUSE);

	bool paused = false;
	if(pauseValue != 0) {
		paused = pauseValue->get();
		pauseValue->set(true);
	}

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		if(addToNetwork) {
			nameOfFile = NerdFileSelector::getFileName("Add Neural Network", true, this);
		}
		else {
			nameOfFile = NerdFileSelector::getFileName("Load Neural Network", true, this);
		}
	}

	if(nameOfFile == "") {	
		mMessageWidget->addMessage("Could not load network: File [" + nameOfFile 
									+ "] is not a valid network file!");

		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu == 0) {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	if(handler == 0) {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	ModularNeuralNetwork *currentNetwork = visu->getNeuralNetwork();

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();

	QMutex *mutex = nnm->getNetworkExecutionMutex();
	QMutex *selectionMutex = visu->getSelectionMutex();
	QList<NeuralNetworkElement*> newElements;

	{
		QMutexLocker locker(mutex);
		QMutexLocker selectionLocker(selectionMutex);

// 		if(!nnm->getNeuralNetworks().contains(currentNetwork)) {
// 			return;
// 		}
	
		QString errorMessage;
		QList<QString> warnings;
		NeuralNetwork *net = NeuralNetworkIO::createNetworkFromFile(nameOfFile, 
						&errorMessage, &warnings);
	
		if(errorMessage != "") {
			mMessageWidget->addMessage("Error while loading network file [" + nameOfFile + "]!");
			mMessageWidget->addMessage(errorMessage);

			if(pauseValue != 0) {
				pauseValue->set(paused);
			}

			return;
		}
		if(!warnings.empty()) {
			for(QListIterator<QString> j(warnings); j.hasNext();) {
				mMessageWidget->addMessage("Warning: " + j.next());
			}
		}
	
		ModularNeuralNetwork *newNet = dynamic_cast<ModularNeuralNetwork*>(net);
		if(newNet != 0) 
		{
			//remove currentNetwork from NetworkVisualization to prevent it from
 			//accessing deleted or invalid objects.
			visu->setNeuralNetwork(0);

			//Memorize controller and detach from currentNetwork.
			ControlInterface *controller = currentNetwork->getControlInterface();
			currentNetwork->setControlInterface(0);

			//clear current network to prepare it to be filled with the elements of newNet.
			if(!addToNetwork) {
				currentNetwork->freeElements(true);
	
				nnm->triggerNetworkStructureChangedEvent();
				visu->notifyNeuralNetworkModified();
			}

			QList<Neuron*> newNeurons = newNet->getNeurons();
			QList<NeuronGroup*> newGroups = newNet->getNeuronGroups();

			NeuronGroup *newDefaultGroup = newNet->getDefaultNeuronGroup();
			NeuronGroup *oldDefaultGroup = currentNetwork->getDefaultNeuronGroup();

			//update constraints of default group (because default group can not be copied directly)
			if(!addToNetwork) {
				QList<GroupConstraint*> newConstraints = newDefaultGroup->getConstraints();
				for(QListIterator<GroupConstraint*> i(newConstraints); i.hasNext();) {
					oldDefaultGroup->addConstraint(i.next());
				}
			}

			//update properties of default group
			if(!addToNetwork) {
				while(!oldDefaultGroup->getPropertyNames().empty()) {
					oldDefaultGroup->removeProperty(oldDefaultGroup->getPropertyNames().first());
				}
				QList<QString> properties = newDefaultGroup->getPropertyNames();
				for(QListIterator<QString> i(properties); i.hasNext();) {
					QString property = i.next();
					oldDefaultGroup->setProperty(property, newDefaultGroup->getProperty(property));
				}
			}

			//update all ids
			if(addToNetwork) {
				QHash<qulonglong, qulonglong> changedIds;

				QList<NeuralNetworkElement*> elements;
				newNet->getNetworkElements(elements);
				for(QListIterator<NeuralNetworkElement*> j(elements); j.hasNext();) {
					NeuralNetworkElement *elem = j.next();
					if(elem != newNet->getDefaultNeuronGroup()) {
						qulonglong newId = NeuralNetwork::generateNextId();
						changedIds.insert(elem->getId(), newId);
						elem->setId(newId);
						newElements.append(elem);
					}
				}
				newNet->notifyMemberIdsChanged(changedIds);
			}

			//free (but do not delete) all elements in newNet to prevent it from 
			//destroying its elements when newNet is deleted.
			newNet->freeElements(false);

			//copy properties
			if(!addToNetwork) {
				//keep all system properties, starting with "__".
				QList<QString> propertyNames = currentNetwork->getPropertyNames();
				for(QListIterator<QString> i(propertyNames); i.hasNext();) {
					QString property = i.next();
					if(!property.startsWith("__")) {
						currentNetwork->removeProperty(property);
					}
				}
				propertyNames = newNet->getPropertyNames();
				for(QListIterator<QString> i(propertyNames); i.hasNext();) {
					QString property = i.next();
					if(!property.startsWith("__")) {
						currentNetwork->setProperty(property, newNet->getProperty(property));
					}
				}
			}

			//update default functions
			if(!addToNetwork) {
				currentNetwork->setDefaultActivationFunction(*(newNet->getDefaultActivationFunction()));
				currentNetwork->setDefaultTransferFunction(*(newNet->getDefaultTransferFunction()));
				currentNetwork->setDefaultSynapseFunction(*(newNet->getDefaultSynapseFunction()));	
			}

			//move neurons from newNet to currentNetwork.
			for(QListIterator<Neuron*> i(newNeurons); i.hasNext();) {
				Neuron *neuron = i.next();
				currentNetwork->addNeuron(neuron);
			}	
			//move neuronGroups from newNet to currentNetwork (including constraints)
			for(QListIterator<NeuronGroup*> i(newGroups); i.hasNext();) {
				NeuronGroup *group = i.next();
				if(group != newNet->getDefaultNeuronGroup()) {
					currentNetwork->addNeuronGroup(group);
				}
			}

			//destroy the new net (all releavent objects have been moved to currentNetwork.
			delete newNet;

			//set the controller back in place to connect input and output neurons.
			currentNetwork->setControlInterface(controller);

			//update visualization
			visu->setNeuralNetwork(currentNetwork); 

// 			if(!addToNetwork && currentNetwork->hasProperty("_Viewport")) {
// 				//Try to set viewport
// 				QString viewport = currentNetwork->getProperty("_Viewport");
// 				QStringList viewportElements = viewport.split(",");
// 				if(viewportElements.size() == 3) {
// 					double x = viewportElements.at(0).toDouble();
// 					double y = viewportElements.at(1).toDouble();
// 					double scaling = viewportElements.at(2).toDouble();
// 					visu->setVisualizationOffset(QPointF(x, y));
// 					visu->setScaling(scaling);
// 				}
// 			}

			visu->getVisualizationHander()->rebuildView();
			nnm->triggerNetworkStructureChangedEvent();
			visu->notifyNeuralNetworkModified();
			nnm->triggerCurrentNetworksReplacedEvent();

			handler->markElementsAsSelected(newElements);
		}
		else {
			delete net;
		}

	}

	if(!addToNetwork) {
		renameCurrentNetwork(nameOfFile);
		updateRecentNetworkMenu(nameOfFile);
	}

	mMessageWidget->addMessage("Successfully loaded network from file [" + nameOfFile + "]!");

	//TODO automatic loading of a layout file is not required any more, because position info is part of the network.
	//load corresponding layout file
// 	int index = nameOfFile.lastIndexOf(".");
// 	if(index >= 0) {
// 		QString layoutFileName = nameOfFile.mid(0, index).append(".lyt");
// 		if(addToNetwork) {
// 			
// 		}
// 		else {
// 			loadLayout(layoutFileName, false);
// 		}
// 	}

	if(pauseValue != 0) {
		pauseValue->set(paused);
	}

}


void NeuralNetworkEditor::addSubNetwork(const QString &fileName) {
	loadNetwork(fileName, true);
}

void NeuralNetworkEditor::saveLayout(const QString &fileName, bool clearMessageArea, 
									 ModularNeuralNetwork *net) 
{

	if(clearMessageArea) {
		mMessageWidget->clear();
	}

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Save Layout", false, this);
	}

	if(nameOfFile == "") {
		return;
	}

	ModularNeuralNetwork *network = net;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
		&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(network == 0  && handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	QFile file(nameOfFile);
	if(file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream output(&file);

		QList<Neuron*> neurons = network->getNeurons();
		output << "#Neurons" << endl;
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			NeuronItem *item = handler->getNeuronItem(neuron);
			if(item != 0) {
				output << item->getLocalPosition().x() << "," << item->getLocalPosition().y() << "\n";
			}
		}

		output << "#Synapses" << endl;
		QList<Synapse*> synapses = network->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();
			SynapseItem *item = handler->getSynapseItem(synapse);
			if(item != 0) {
				output << item->getLocalPosition().x() << "," << item->getLocalPosition().y() << "\n";
			}
		}

		output << "#Modules" << endl;
		QList<NeuroModule*> modules = network->getNeuroModules();
		for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
			NeuroModule *module = i.next();
			ModuleItem *item = handler->getModuleItem(module);
			if(item != 0) {
				output << item->getLocalPosition().x() 
					   << "," << item->getLocalPosition().y() 
					   << "," << item->getSize().width()
					   << "," << item->getSize().height() << "\n";
			}
		}

		output << "#Groups" << endl;
		QList<NeuronGroup*> groups = network->getNeuronGroups();
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			NeuronGroup *group = i.next();
			if(dynamic_cast<NeuroModule*>(group) != 0) {
				//ignore modules
				continue;
			}
			GroupItem *item = handler->getGroupItem(group);
			if(item != 0) {
				output << item->getLocalPosition().x() 
					   << "," << item->getLocalPosition().y() 
					   << "\n";
			}
		}
		
		file.close();
		mMessageWidget->addMessage("Saved layout in file [" + nameOfFile + "]");
	}
	else {
		mMessageWidget->addMessage("Could not save layout to file [" + nameOfFile + "]");
	}
}

void NeuralNetworkEditor::loadLayout(const QString &fileName, bool clearMessageArea) {

	if(clearMessageArea) {
		mMessageWidget->clear();
	}

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Load Layout", true, this);
	}

	if(nameOfFile == "") {
		mMessageWidget->addMessage("Could not load layout: File [" + nameOfFile 
									+ "] is not a valid layout file!");
		return;
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
		&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	QFile file(nameOfFile);
	if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream input(&file);

		QString line = input.readLine();
		while(!line.trimmed().startsWith("#Neurons") && !input.atEnd()) {
			line = input.readLine();
		};

		QList<Neuron*> neurons = network->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
			NeuronItem *item = handler->getNeuronItem(neuron);
			if(item != 0) {
				line = input.readLine();
				if(line.startsWith("#Synapses")) {
					break;
				}
				QStringList coordinates = line.split(",");
				if(coordinates.size() != 2) {
					continue;
				}
				double x = coordinates.at(0).toDouble();
				double y = coordinates.at(1).toDouble();
				item->setLocalPosition(QPointF(x, y), true);
			}
		}

		while(!line.startsWith("#Synapses") && !input.atEnd()) {
			line = input.readLine();
		}

		QList<Synapse*> synapses = network->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();
			SynapseItem *item = handler->getSynapseItem(synapse);
			if(item != 0) {
				line = input.readLine();
				if(line.startsWith("#Modules")) {
					break;
				}
				QStringList coordinates = line.split(",");
				if(coordinates.size() != 2) {
					continue;
				}
				double x = coordinates.at(0).toDouble();
				double y = coordinates.at(1).toDouble();
				item->setLocalPosition(QPointF(x, y), true);
			}
		}

		while(!line.startsWith("#Modules") && !input.atEnd()) {
			line = input.readLine();
		}

		QList<NeuroModule*> modules = network->getNeuroModules();
		for(QListIterator<NeuroModule*> i(modules); i.hasNext();) {
			NeuroModule *module = i.next();
			ModuleItem *item = handler->getModuleItem(module);
			if(item != 0) {
				line = input.readLine();
				if(line.startsWith("#Groups")) {
					break;
				}
				QStringList data = line.split(",");
				if(data.size() != 4) {
					continue;
				}
				double x = data.at(0).toDouble();
				double y = data.at(1).toDouble();
				double width = data.at(2).toDouble();
				double height = data.at(3).toDouble();
				item->setLocalPosition(QPointF(x, y), true);
				item->setSize(QSize(width, height));
			}
		}

		while(!line.startsWith("#Groups") && !input.atEnd()) {
			line = input.readLine();
		}

		QList<NeuronGroup*> groups = network->getNeuronGroups();
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			NeuronGroup *group = i.next();
			if(dynamic_cast<NeuroModule*>(group) != 0) {
				continue;
			}
			GroupItem *item = handler->getGroupItem(group);
			if(item != 0) {
				line = input.readLine();
				QStringList data = line.split(",");
				if(data.size() != 2) {
					continue;
				}
				double x = data.at(0).toDouble();
				double y = data.at(1).toDouble();
				item->setLocalPosition(QPointF(x, y), true);
			}
		}
		
		file.close();
		mMessageWidget->addMessage("Successfully loaded layout from file [" + nameOfFile + "]!");
	}
	else {
		mMessageWidget->addMessage("Could not load layout from file [" + nameOfFile + "]!");
	}
	
}

void NeuralNetworkEditor::createMinimalNetwork() {
	mMessageWidget->clear();

	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	NetworkVisualizationHandler *handler = visu->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	NeuralNetworkManager *nnm = Neuro::getNeuralNetworkManager();

	QMutex *mutex = nnm->getNetworkExecutionMutex();
	QMutex *selectionMutex = visu->getSelectionMutex();
	QList<NeuralNetworkElement*> newElements;

	{
		QMutexLocker locker(mutex);
		QMutexLocker selectionLocker(selectionMutex);

		ModularNeuralNetwork *currentNetwork = visu->getNeuralNetwork();

		ControlInterface *controller = currentNetwork->getControlInterface();

		if(controller == 0) {
			return;
		}

		visu->setNeuralNetwork(0); 
		currentNetwork->setControlInterface(0);
		
		currentNetwork->freeElements(true);

		int numberOfOutputs = controller->getOutputValues().size();
		for(int i = 0; i < numberOfOutputs; ++i) {
			Neuron *neuron = new Neuron("Input", 
						*(currentNetwork->getDefaultTransferFunction()),
						*(currentNetwork->getDefaultActivationFunction()));
			neuron->setProperty(Neuron::NEURON_TYPE_INPUT);
			currentNetwork->addNeuron(neuron);
		}
		int numberOfInputs = controller->getInputValues().size();
		for(int i = 0; i < numberOfInputs; ++i) {
			Neuron *neuron = new Neuron("Output", 
						*(currentNetwork->getDefaultTransferFunction()),
						*(currentNetwork->getDefaultActivationFunction()));
			neuron->setProperty(Neuron::NEURON_TYPE_OUTPUT);
			currentNetwork->addNeuron(neuron);
		}

		//set the controller back in place to connect input and output neurons.
		currentNetwork->setControlInterface(controller);

		//update visualization
		visu->setNeuralNetwork(currentNetwork); 

		visu->getVisualizationHander()->rebuildView();
		nnm->triggerNetworkStructureChangedEvent();
		visu->notifyNeuralNetworkModified();

		handler->markElementsAsSelected(newElements);

	}

	mMessageWidget->addMessage("Created minimal network from agent interface!");
}


void NeuralNetworkEditor::exportByteCode(const QString &fileName) {

	mMessageWidget->clear();

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Export as A-Series Bytecode", false, this);
	}

	if(nameOfFile == "") {
		return;
	}

	if(!nameOfFile.endsWith(".mef")) {
		nameOfFile.append(".mef");
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
		&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	QString errorMessage;

	if(!NeuralNetworkIOBytecode::createMotionEditorFileFromNetwork(nameOfFile, network, &errorMessage)) {
		mMessageWidget->addMessage("Could not export neural network as bytecode and motion editor file."
									" [" + nameOfFile + "]");
	}
	else {
		mMessageWidget->addMessage("Successfully exported neural network to bytecode "
								   "and motion editor file. [" + nameOfFile + "]");
	}

	if(errorMessage != "") {
		mMessageWidget->addMessage(errorMessage);
	}
}

void NeuralNetworkEditor::exportASeriesBDN(const QString &fileName) {

	mMessageWidget->clear();

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Export as A-Series BrainDesigner Library", false, this);
	}

	if(nameOfFile == "") {
		return;
	}

	if(!nameOfFile.endsWith(".bdp")) {
		nameOfFile.append(".bdp");
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
			&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	QString errorMessage;

	if(!NeuralNetworkIOASeriesBDN::createFileFromNetwork(nameOfFile, network, &errorMessage))
	{
		mMessageWidget->addMessage("Could not export neural network as BrainDesigner Library."
				" [" + nameOfFile + "]");		
	}
	
	if(errorMessage != "") {
		mMessageWidget->addMessage(errorMessage);
	}
}


void NeuralNetworkEditor::exportMSeriesBDN(const QString &fileName) {

	mMessageWidget->clear();

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Export as M-Series BrainDesigner Library", false, this);
	}

	if(nameOfFile == "") {
		return;
	}

	if(!nameOfFile.endsWith(".bdp")) {
		nameOfFile.append(".bdp");
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
			&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}
	
	//additionally save network as onn file.
	QString networkFileName = nameOfFile + ".onn";
	saveNetwork(networkFileName, network, true, false);

	QString errorMessage;

	if(!NeuralNetworkIOMSeriesBDN::createFileFromNetwork(nameOfFile, network, &errorMessage))
	{
		mMessageWidget->addMessage("Could not export neural network as BrainDesigner Library.\n"
				" [" + nameOfFile + "]");		
	}
	else {
		mMessageWidget->addMessage("Successfully exported neural network as BrainDesigner Library.\n"
				" [" + nameOfFile + "]");	
	}
	
	if(errorMessage != "") {
		mMessageWidget->addMessage(errorMessage);
	}
	
	/*
	if(!NeuralNetworkIOBytecode::createMotionEditorFileFromNetwork(nameOfFile, network, &errorMessage)) {
		mMessageWidget->addMessage("Could not export neural network as bytecode and motion editor file."
				" [" + nameOfFile + "]");
	}
	else {
		mMessageWidget->addMessage("Successfully exported neural network to bytecode "
				"and motion editor file. [" + nameOfFile + "]");
	}
	*/

	
}

void NeuralNetworkEditor::exportDebugMSeriesComponentsBDN(const QString &fileName) {

	mMessageWidget->clear();

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Export as M-Series BrainDesigner Library", false, this);
	}

	if(nameOfFile == "") {
		return;
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualizationHandler *handler = 0;

	if(getCurrentNetworkVisualization() != 0 
			&& getCurrentNetworkVisualization()->getVisualizationHander() != 0) 
	{
		handler = getCurrentNetworkVisualization()->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		return;
	}

	QString errorMessage;

	if(!DebugNeuralNetworkIOMSeriesComponentsBDN::createFileFromNetwork(nameOfFile, network, &errorMessage))
	{
		mMessageWidget->addMessage("Could not export neural network as BrainDesigner Library."
				" [" + nameOfFile + "]");		
	}

	if(errorMessage != "") {
		mMessageWidget->addMessage(errorMessage);
	}
}


void NeuralNetworkEditor::saveNetworkAsSvgGraphics(const QString &fileName) {
	
	mMessageWidget->clear();

	BoolValue *pauseValue = Core::getInstance()->getValueManager()->getBoolValue(
						NerdConstants::VALUE_EXECUTION_PAUSE);

	bool paused = false;
	if(pauseValue != 0) {
		paused = pauseValue->get();
		pauseValue->set(true);
	}

	QString nameOfFile = fileName;
	if(nameOfFile == "") {
		nameOfFile = NerdFileSelector::getFileName("Export as SVG Graphics", false, this);
	}

	if(nameOfFile == "") {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	if(!nameOfFile.endsWith(".svg")) {
		nameOfFile.append(".svg");
	}

	ModularNeuralNetwork *network = 0;
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	NetworkVisualizationHandler *handler = 0;

	if(visu != 0 
		&& visu->getVisualizationHander() != 0) 
	{
		handler = visu->getVisualizationHander();
		if(handler != 0) {
			network = handler->getNeuralNetwork();
		}
	}

	if(network == 0 || handler == 0) {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	//determine graphics size
	//always paint the entire network
	double minX = numeric_limits<double>::max();
	double minY = numeric_limits<double>::max();
	double maxX = -1 * numeric_limits<double>::max();
	double maxY = -1 * numeric_limits<double>::max();

	QList<PaintItem*> items = visu->getPaintItems();

	if(items.empty()) {
		if(pauseValue != 0) {
			pauseValue->set(paused);
		}
		return;
	}

	for(QListIterator<PaintItem*> i(items); i.hasNext();) {
		PaintItem *item = i.next();
		QRectF boundingBox = item->getPaintingBox();
		if((boundingBox.width() > 10000) || (boundingBox.height() > 10000)) {
			continue;
		}
		minX = Math::min(minX, boundingBox.x());
		minY = Math::min(minY, boundingBox.y());
		maxX = Math::max(maxX, boundingBox.x() + boundingBox.width());
		maxY = Math::max(maxY, boundingBox.y() + boundingBox.height());
	}

	double border = 0.0;
	minX -= border;
	minY -= border;
	maxX += border;
	maxY += border;
	
	//double width = maxX - minX;
	//double height = maxY - minY;

	QSvgGenerator generator;
	generator.setFileName(nameOfFile);
	//generator.setSize(QSize(width, height));
	//generator.setViewBox(QRect(0, 0, width, height));
	generator.setTitle(tr("NERD Neural Network"));
	generator.setDescription(tr(""));
	
	QPainter painter;
	painter.begin(&generator);

	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setRenderHint(QPainter::TextAntialiasing, false);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
	painter.setRenderHint(QPainter::HighQualityAntialiasing, false);
	painter.setRenderHint(QPainter::NonCosmeticDefaultPen, false);

	QRectF visibleArea(minX, minY, maxX - minX, maxY - minY);

	visu->paintNetworkView(painter, visibleArea, 1.0, QPointF(-minX, -minY));

	painter.end();

	mMessageWidget->addMessage(QString("Successfully exported network to Scalable Vector Graphics (SVG)."));

	if(pauseValue != 0) {
		pauseValue->set(paused);
	}
}


void NeuralNetworkEditor::currentTabChanged(int index) {
	updateUndoRedoActions();
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->setFocus();
		visu->updateHiddenLayerMode();
		setWindowTitle("Network Editor [" + visu->getCurrentNetworkFileName() + "]");
	}
	emit tabSelectionChanged(index);
}


void NeuralNetworkEditor::toggleTimerExpired() {
	if(!isVisible()) {
		return;
	}
	if(mWindowToggleState) {
		move(pos() + QPoint(1, 0));
	}
	else {
		move(pos() + QPoint(-1, 0));
	}
	mWindowToggleState = !mWindowToggleState;
}

void NeuralNetworkEditor::autoSaveTimerExpired() {

	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker guard(nmm->getNetworkExecutionMutex());

	Core::getInstance()->enforceDirectoryPath(mAutoSaveNetworkDirectory->get());
	
	int index = 0;
	for(QListIterator<NetworkVisualization*> i(mNetworkVisualizations); i.hasNext();) {
		NetworkVisualization *visu = i.next();
		QMutexLocker guard2(visu->getSelectionMutex());

		QString fileName = mAutoSaveNetworkDirectory->get() 
							+ "/autosave" + QString::number(index) + ".onn";

		ModularNeuralNetwork *net = visu->getNeuralNetwork();
		if(net != 0) {
			saveNetwork(fileName, net, false, false);
		}
		index++;
	}
}

void NeuralNetworkEditor::hiddenLayerStateChanged() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->updateHiddenLayerMode();
	}
}

void NeuralNetworkEditor::selectAllItems() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->selectAllItems();
	}
}


void NeuralNetworkEditor::selectAllVisibleItems() {
	cerr << "huhu" << endl;
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu != 0) {
		visu->selectAllItems(true);
	}
}


void NeuralNetworkEditor::setupMenuBar() {
	addEditMenu();
	addPlotterMenu();
	addViewModeMenu();
}

QMenu* NeuralNetworkEditor::addEditMenu() {

	QMenuBar *mainMenu = menuBar();

	//Edit Menu
	QMenu *editMenu = new QMenu("Edit", mainMenu);

	editMenu->addAction(mUndoAction);
	mUndoAction->setShortcut(tr("Ctrl+z"));
	connect(mUndoAction, SIGNAL(triggered()),
			this, SLOT(undoCommand()));

	editMenu->addAction(mRedoAction);
	mRedoAction->setShortcut(tr("Ctrl+y"));
	connect(mRedoAction, SIGNAL(triggered()),
			this, SLOT(redoCommand()));  

	editMenu->addSeparator();

	QAction *findAction = editMenu->addAction("&Find");
	findAction->setShortcut(tr("Ctrl+f"));
	connect(findAction, SIGNAL(triggered()),
			this, SLOT(searchForNetworkElement()));
			
	QAction *selectAllAction = editMenu->addAction("Select All");
	connect(selectAllAction, SIGNAL(triggered()),
			this, SLOT(selectAllItems()));
			
	QAction *selectAllHiddenAction = editMenu->addAction("Select All Visible");
	connect(selectAllHiddenAction, SIGNAL(triggered()),
			this, SLOT(selectAllVisibleItems()));

	editMenu->addSeparator();
	
	mEnableHiddenLayersCheckbox = editMenu->addAction("Enable Hidden &Layers");
	mEnableHiddenLayersCheckbox->setCheckable(true);
	mEnableHiddenLayersCheckbox->setChecked(true);
	mEnableHiddenLayersCheckbox->setShortcut(tr("Ctrl+l"));
	connect(mEnableHiddenLayersCheckbox, SIGNAL(changed()),
			this, SLOT(hiddenLayerStateChanged()));

	mainMenu->addMenu(editMenu);

	return editMenu;
}


QMenu* NeuralNetworkEditor::addPlotterMenu() {
	QMenuBar *mainMenu = menuBar();

	//Edit Menu
	QMenu *plotterMenu = new QMenu("Plotter", mainMenu);

	AddActivationPlotterAction *addOutputPlotterAction = 
						new AddActivationPlotterAction("Add Output Plotter", "Output",
									this, NeuronActivityPlotter::PLOT_NEURON_OUTPUT, plotterMenu);
	plotterMenu->addAction(addOutputPlotterAction);

	AddActivationPlotterAction *addActivationPlotterAction = 
						new AddActivationPlotterAction("Add Activation Plotter", "Activation",
									this, NeuronActivityPlotter::PLOT_NEURON_ACTIVATION, plotterMenu);
	plotterMenu->addAction(addActivationPlotterAction);
	
	plotterMenu->addSeparator();
	
	AddActivationPlotterAction *addWeightPlotterAction = 
						new AddActivationPlotterAction("Add Weight and Bias Plotter", "Weight / Bias",
									this, NeuronActivityPlotter::PLOT_SYNAPSE_WEIGHTS, plotterMenu);
	plotterMenu->addAction(addWeightPlotterAction);
	
	AddActivationPlotterAction *addParameterPlotterAction = 
						new AddActivationPlotterAction("Add Parameter Plotter", "Parameter",
									this, NeuronActivityPlotter::PLOT_PARAMETERS, plotterMenu);
	plotterMenu->addAction(addParameterPlotterAction);
	
	plotterMenu->addSeparator();

	AddFirstReturnMapPlotterAction *addFirstReturnMapPlotterAction =
						new AddFirstReturnMapPlotterAction("Add FirstReturnMap Plotter",
									this, FirstReturnMapPlotter::PLOT_NEURON_OUTPUT, plotterMenu);
	plotterMenu->addAction(addFirstReturnMapPlotterAction);

	mainMenu->addMenu(plotterMenu);

	return plotterMenu;
}

QMenu* NeuralNetworkEditor::addViewModeMenu() {
	QMenuBar *mainMenu = menuBar();

	//Edit Menu
	mViewModeMenu = new QMenu("View", mainMenu);

	mainMenu->addMenu(mViewModeMenu);

	return mViewModeMenu;
}


void NeuralNetworkEditor::setupTools() {

	mEditorToolWidgets.clear();

	setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	//allow position selection via command line argument.
	CommandLineArgument *moveNeuronDetailWidgetToTheLeft 
			= new CommandLineArgument("editorWidgetPositions", "ewp", "<widget1Pos1>,<widget2Pos2>...",
									  "Starts the editor with custom default locations for the tool "
									  "widgets.\n     Positions: lrtb (left, right, top, bottom)"
									  "\n    Widgets: gncsmtpa (group, neuron, constraints, synapses, "
									  "\n    modules, neTwork, properties, message Area"
									  "\n    Example: nt,pb,ab,sr", 1, 0, true, false);
	
	QHash<QString, Qt::DockWidgetArea> availablePositions;
	QHash<QString, Qt::DockWidgetArea> widgetPositions;
	
	availablePositions.insert("t", Qt::TopDockWidgetArea);
	availablePositions.insert("b", Qt::BottomDockWidgetArea);
	availablePositions.insert("l", Qt::LeftDockWidgetArea);
	availablePositions.insert("r", Qt::RightDockWidgetArea);
	
	widgetPositions.insert("g", Qt::RightDockWidgetArea);
	widgetPositions.insert("n", Qt::RightDockWidgetArea);
	widgetPositions.insert("c", Qt::RightDockWidgetArea);
	widgetPositions.insert("s", Qt::RightDockWidgetArea);
	widgetPositions.insert("m", Qt::RightDockWidgetArea);
	widgetPositions.insert("t", Qt::RightDockWidgetArea);
	widgetPositions.insert("p", Qt::RightDockWidgetArea);
	widgetPositions.insert("a", Qt::BottomDockWidgetArea);
	
	if(!moveNeuronDetailWidgetToTheLeft->getEntries().empty()) {
		QString entry = moveNeuronDetailWidgetToTheLeft->getEntries().at(0);
		QStringList posInfo = entry.split(",");

		bool ok = true;
		
		for(int i = 0; i < posInfo.size(); ++i) {
			QString pos = posInfo.at(i);
			
			if(pos.length() != 2) {
				ok = false;
			}
			else {
				if(!availablePositions.keys().contains(pos.mid(1, 1))
					|| !widgetPositions.keys().contains(pos.mid(0, 1)))
				{
					ok = false;
				}
				else {
					widgetPositions.insert(pos.mid(0, 1), availablePositions.value(pos.mid(1, 1)));
				}
			}
		}
		if(!ok) {
			Core::log("Warning: Position entry [" + entry + "] for command line parameter -ewp "
						"is not in the correct format!", true);
		}
	}

	QDockWidget *groupDock = new QDockWidget("Groups", this);
	groupDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	groupDock->setWidget(new GroupSelectionWidget(this));
	//addDockWidget(Qt::RightDockWidgetArea, groupDock);
	addDockWidget(widgetPositions.value("g"), groupDock);
	mEditorToolWidgets.append(groupDock);
	
	QDockWidget *neuronDock = new QDockWidget("Neuron", this);
	neuronDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	neuronDock->setWidget(new NeuronDetailWidget(this));
	addDockWidget(widgetPositions.value("n"), neuronDock);
	mEditorToolWidgets.append(neuronDock);

	QDockWidget *constraintDock = new QDockWidget("Constraints", this);
	constraintDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	constraintDock->setWidget(new ConstraintDetailWidget(this));
	addDockWidget(widgetPositions.value("c"), constraintDock);
	mEditorToolWidgets.append(constraintDock);

	QDockWidget *synapseDock = new QDockWidget("Synapse", this);
	synapseDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	synapseDock->setWidget(new SynapseDetailWidget(this));
	addDockWidget(widgetPositions.value("s"), synapseDock);
	mEditorToolWidgets.append(synapseDock);

	QDockWidget *moduleDock = new QDockWidget("Neuro-Modules", this);
	moduleDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	moduleDock->setWidget(new ModuleManagementWidget(this));
	addDockWidget(widgetPositions.value("m"), moduleDock);
	mEditorToolWidgets.append(moduleDock);

	QDockWidget *networkDock = new QDockWidget("Network", this);
	networkDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	networkDock->setWidget(new NetworkDetailWidget(this));
	addDockWidget(widgetPositions.value("t"), networkDock);
	mEditorToolWidgets.append(networkDock);

	QDockWidget *propertiesDock = new QDockWidget("Properties", this);
	propertiesDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	propertiesDock->setWidget(new PropertyDialog(this));
	addDockWidget(widgetPositions.value("p"), propertiesDock);
	mEditorToolWidgets.append(propertiesDock);


	if(widgetPositions.value("g") == widgetPositions.value("m")) {
		tabifyDockWidget(groupDock, moduleDock);
	}
	if(widgetPositions.value("m") == widgetPositions.value("s")) {
		tabifyDockWidget(moduleDock, synapseDock);
	}
	else if(widgetPositions.value("g") == widgetPositions.value("s")) {
		tabifyDockWidget(groupDock, synapseDock);
	}
	
	if(widgetPositions.value("p") == widgetPositions.value("t")) {
		tabifyDockWidget(propertiesDock, networkDock);
	}
	if(widgetPositions.value("t") == widgetPositions.value("c")) {
		tabifyDockWidget(networkDock, constraintDock);
	}
	else if(widgetPositions.value("p") == widgetPositions.value("c")) {
		tabifyDockWidget(propertiesDock, constraintDock);
	}
	if(widgetPositions.value("c") == widgetPositions.value("n")) {
		tabifyDockWidget(constraintDock, neuronDock);
	}
	else if(widgetPositions.value("t") == widgetPositions.value("n")) {
		tabifyDockWidget(networkDock, neuronDock);
	}
	else if(widgetPositions.value("p") == widgetPositions.value("n")) {
		tabifyDockWidget(propertiesDock, neuronDock);
	}
	else if(widgetPositions.value("n") == widgetPositions.value("s")) {
		tabifyDockWidget(synapseDock, neuronDock);
	}

	
// 	tabifyDockWidget(propertiesDock, synapseDock);

	mMessageWidget = new EditorMessageWidget(this);
	QDockWidget *messageDock = new QDockWidget("Messages", this);
	messageDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	messageDock->setWidget(mMessageWidget);
	addDockWidget(widgetPositions.value("a"), messageDock);
	messageDock->resize(100, 100);
	mMessageWidget->resize(100, 100);
	mEditorToolWidgets.append(messageDock);
}


void NeuralNetworkEditor::updateUndoRedoActions() {
	NetworkVisualization *visu = getCurrentNetworkVisualization();
	if(visu == 0) {
		mUndoAction->setWrappedAction(0);
		mRedoAction->setWrappedAction(0);
	}
	else {
		mUndoAction->setWrappedAction(visu->getCommandExecutor()->getUndoAction());
		mRedoAction->setWrappedAction(visu->getCommandExecutor()->getRedoAction());
	}
}



void NeuralNetworkEditor::keyPressEvent(QKeyEvent *event) {
		NetworkVisualization *visu = getCurrentNetworkVisualization();
		if(visu != 0) {
			visu->keyPressEvent(event);
		}
}

void NeuralNetworkEditor::keyReleaseEvent(QKeyEvent *event) {
		NetworkVisualization *visu = getCurrentNetworkVisualization();
		if(visu != 0) {
			visu->keyReleaseEvent(event);
		}
}

void NeuralNetworkEditor::saveRecentNetworkFileNames() {
	QString filePrefix = Core::getInstance()->getConfigDirectoryPath() + "/properties";
	Core::getInstance()->enforceDirectoryPath(filePrefix);
	QList<QAction*> loadRecentActions = mRecentNetworksMenu->actions();
	if(!loadRecentActions.empty()) {
		QFile file(filePrefix + "/recentNetworks.txt");
		if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			file.close();
			return;
		}
		QTextStream output(&file);
		for(QListIterator<QAction*> i(loadRecentActions); i.hasNext();) {
			LoadRecentNetworkAction *action = dynamic_cast<LoadRecentNetworkAction*>(i.next());
			if(action != 0) {
				output << action->getFileName() << endl;
			}
		}
		file.close();
	}
}


void NeuralNetworkEditor::loadRecentNetworkFileNames() {
	QString filePrefix = Core::getInstance()->getConfigDirectoryPath() + "/properties";
	QFile file(filePrefix + "/recentNetworks.txt");
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return;
	}
	QList<QString> fileNames;

	QTextStream input(&file);
	while (!input.atEnd()) {
		fileNames.append(input.readLine());
	}
	file.close();
	
	for(int i = fileNames.size() - 1; i >= 0; --i) {
		updateRecentNetworkMenu(fileNames.at(i));
	}
}




// QString NeuralNetworkEditor::getFileName(const QString &fileChooserTitle, bool existingFilesOnly) {
// 	Properties &props = Core::getInstance()->getProperties();
// 	QString fileChooserDirectory = ".";
// 	if(props.hasProperty("CurrentFileChooserPath")) {
// 		fileChooserDirectory = props.getProperty("CurrentFileChooserPath");
// 	}
// 
// 	QString fileName;
// 	if(existingFilesOnly) {
// 		fileName = QFileDialog::getOpenFileName(this, fileChooserTitle,	fileChooserDirectory);
// 	}
// 	else {
// 		fileName = QFileDialog::getSaveFileName(this, fileChooserTitle,	fileChooserDirectory);
// 	}
// 
// 	if(fileName == "") {
// 		return "";
// 	}
// 
// 	fileChooserDirectory = fileName.mid(0, fileName.lastIndexOf("/"));
// 	props.setProperty("CurrentFileChooserPath", fileChooserDirectory);
// 
// 	return fileName;
// }

}




