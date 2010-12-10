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

 

#include "PropertyDialog.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"
#include "Constraints/GroupConstraint.h"
#include "Util/Tracer.h"
#include <QThread>
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>
#include "NeuralNetworkConstants.h"
#include "Evolution/EvolutionManager.h"
#include "Evolution/World.h"
#include "Evolution/Population.h"
#include "Evolution/Individual.h" 
#include "Network/NeuroTagManager.h"

#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");
//#define pTRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");


using namespace std;

namespace nerd {


/**
 * Constructs a new PropertyDialog.
 */
PropertyDialog::PropertyDialog(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false), mCurrentProperties(0), 
	  mPropertyList(0), mListScrollArea(0),
	  mSetButton(0), mChangeButton(0), mRemoveButton(0), mPropertySelectionField(0),
	  mValueEditField(0)
{
	setObjectName("PropertyDialog");

	QVBoxLayout *layout = new QVBoxLayout(this);	
	layout->setObjectName("MainLayout");
	setLayout(layout);

	QLayout *nameLayout = new QHBoxLayout();
	layout->addLayout(nameLayout);

	QLayout *listLayout = new QHBoxLayout();
	listLayout->setObjectName("ListLayout");
	layout->addLayout(listLayout);

	QHBoxLayout *propertySelectLayout = new QHBoxLayout();	
	layout->addLayout(propertySelectLayout);

	QHBoxLayout *valueEditLayout = new QHBoxLayout();
	layout->addLayout(valueEditLayout);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->setObjectName("ButtonLayout");
	layout->addLayout(buttonLayout);

	QHBoxLayout *autoSelectLayout = new QHBoxLayout();
	autoSelectLayout->setObjectName("AutoSelectLayout");
	layout->addLayout(autoSelectLayout);

	QHBoxLayout *typeSelectionLayout = new QHBoxLayout();
	layout->addLayout(typeSelectionLayout);
	QHBoxLayout *typeSelectionLayoutLower = new QHBoxLayout();
	layout->addLayout(typeSelectionLayoutLower);


	mNameLabel = new QLabel("");
	nameLayout->addWidget(mNameLabel);

	mPropertyList = new QListWidget(this);
	mListScrollArea = new QScrollArea(this);
	mListScrollArea->setWidget(mPropertyList);
	mListScrollArea->setWidgetResizable(true);
	listLayout->addWidget(mListScrollArea);

	mPropertySelectionField = new QComboBox(this);
	mPropertySelectionField->setEditable(true);
	addStandardPropertyTemplates();
	propertySelectLayout->addWidget(mPropertySelectionField);
	
	mValueEditField = new QLineEdit();
	valueEditLayout->addWidget(mValueEditField);

	mSetButton = new QPushButton("Set", this);
	mSetButton->setContentsMargins(2, 2, 2, 2);
	mSetButton->setMinimumWidth(15);
	mChangeButton = new QPushButton("Change", this);
	mChangeButton->setContentsMargins(2, 2, 2, 2);
	mChangeButton->setMinimumWidth(15);
	mRemoveButton = new QPushButton("Delete", this);
	mRemoveButton->setContentsMargins(2, 2, 2, 2);
	mRemoveButton->setMinimumWidth(15);
	mApplyToAllIndividuals = new QCheckBox("All", this);
	mApplyToAllIndividuals->setChecked(false);
	mApplyToAllIndividuals->setToolTip("Apply changes to all individuals (Evolution only)");
	mShowAllProperties = new QCheckBox("H", this);
	mShowAllProperties->setChecked(false),
	mShowAllProperties->setToolTip("Show also hidden properties");

	buttonLayout->addWidget(mSetButton);
	buttonLayout->addWidget(mChangeButton);
	buttonLayout->addWidget(mRemoveButton);
	buttonLayout->addWidget(mApplyToAllIndividuals);
	buttonLayout->addWidget(mShowAllProperties);

	mSelectMatchingElementsButton = new QPushButton("Select Matching");
	mForceExactMatchForSelectionCheckBox = new QCheckBox("Exact");
	mForceExactMatchForSelectionCheckBox->setChecked(false);
	mShowNonMatchingElementsCheckBox = new QCheckBox("Negate");
	mShowNonMatchingElementsCheckBox->setChecked(false);

	autoSelectLayout->addWidget(mSelectMatchingElementsButton);
	autoSelectLayout->addWidget(mForceExactMatchForSelectionCheckBox);
	autoSelectLayout->addWidget(mShowNonMatchingElementsCheckBox);

	mConsiderNeurons = new QCheckBox("N");
	mConsiderSynapses = new QCheckBox("S");
	mConsiderGroups = new QCheckBox("G");
	mConsiderModules = new QCheckBox("M");

	mHelpButton = new QPushButton("Help");
	mHelpButton->setContentsMargins(2, 2, 2, 2);
	mHelpButton->setMinimumWidth(15);

	mConsiderNeurons->setMinimumWidth(10);
	mConsiderSynapses->setMinimumWidth(10);
	mConsiderGroups->setMinimumWidth(10);
	mConsiderModules->setMinimumWidth(10);

	typeSelectionLayout->addWidget(mConsiderNeurons);
	typeSelectionLayout->addWidget(mConsiderSynapses);
	typeSelectionLayout->addWidget(mConsiderGroups);
	typeSelectionLayout->addWidget(mConsiderModules);
	typeSelectionLayout->addWidget(mHelpButton);
	


	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}
	connect(mPropertyList, SIGNAL(currentRowChanged(int)),
			this, SLOT(propertyRowChanged(int)));

	connect(mSetButton, SIGNAL(pressed()),
			this, SLOT(setButtonPressed()));
	connect(mChangeButton, SIGNAL(pressed()),
			this, SLOT(changeButtonPressed()));
	connect(mRemoveButton, SIGNAL(pressed()),
			this, SLOT(removeButtonPressed()));
	connect(mShowAllProperties, SIGNAL(clicked()),
			this, SLOT(updatePropertyList()));
	connect(mSelectMatchingElementsButton, SIGNAL(pressed()),
			this, SLOT(updateElementSelection()));
	connect(mHelpButton, SIGNAL(pressed()),
			this, SLOT(showHelpDialog()));
	connect(mValueEditField, SIGNAL(returnPressed()),
			this, SLOT(changeButtonPressed()));

	mEvolutionManager = dynamic_cast<EvolutionManager*>(Core::getInstance()
					->getGlobalObject(NeuralNetworkConstants::OBJECT_EVOLUTION_MANAGER));
	if(mEvolutionManager == 0){
		mApplyToAllIndividuals->setChecked(false);
		mApplyToAllIndividuals->setEnabled(false);
	}

}

/**
 * Destructor.
 */
PropertyDialog::~PropertyDialog() {
	TRACE("PropertyDialog::~PropertyDialog");
}


void PropertyDialog::selectionChanged(QList<PaintItem*>) {
	TRACE("PropertyDialog::selectionChanged");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<Properties*> properties = getSelectedProperties();

	bool considerAll = !mConsiderNeurons->isChecked()
						&& !mConsiderSynapses->isChecked()
						&& !mConsiderGroups->isChecked()
						&& !mConsiderModules->isChecked();

	mCurrentProperties = 0;
	for(int i = properties.size() - 1; i >= 0; --i) {
		Properties *p = properties.at(i);
		mCurrentProperties = p;
		if(considerAll) {
			break;
		}
		if(mConsiderNeurons->isChecked() && dynamic_cast<Neuron*>(p) != 0) {
			break;
		}
		if(mConsiderSynapses->isChecked() && dynamic_cast<Synapse*>(p) != 0) {
			break;
		}
		if(mConsiderGroups->isChecked() && dynamic_cast<NeuronGroup*>(p) != 0) {
			break;
		}
		if(mConsiderModules->isChecked() && dynamic_cast<NeuroModule*>(p) != 0) {
			break;
		}

		mCurrentProperties = 0;
	}

	updatePropertyList();
}


void PropertyDialog::modificationPolicyChanged() {
	mPropertySelectionField->setEnabled(mModificationsEnabled);
	mValueEditField->setReadOnly(!mModificationsEnabled);
	mSetButton->setEnabled(mModificationsEnabled);
	mChangeButton->setEnabled(mModificationsEnabled);
	mRemoveButton->setEnabled(mModificationsEnabled);
}


QList<Properties*> PropertyDialog::getSelectedProperties() const {	
	TRACE("PropertyDialog::getSelectedProperties");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<Properties*> properties;

	if(mEditor == 0 || mEditor->getCurrentNetworkVisualization() == 0) {
		return properties;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	bool allTypes = !mConsiderNeurons->isChecked()
						&& !mConsiderSynapses->isChecked()
						&& !mConsiderGroups->isChecked()
						&& !mConsiderModules->isChecked();
	
	QString idString = "";
	QString name = "";

	QList<PaintItem*> selectedItems = visu->getSelectedItems();
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *p = i.next();

		if((allTypes || mConsiderNeurons->isChecked()) && dynamic_cast<NeuronItem*>(p) != 0) {
			Neuron *neuron = dynamic_cast<NeuronItem*>(p)->getNeuron();
			properties.append(neuron);
			idString = QString::number(neuron->getId());
			name = neuron->getNameValue().get();
		}
		else if((allTypes || mConsiderSynapses->isChecked()) && dynamic_cast<SynapseItem*>(p) != 0) {
			Synapse *synapse = dynamic_cast<SynapseItem*>(p)->getSynapse();
			properties.append(synapse);
			idString = QString::number(synapse->getId());
		}
		else if((allTypes || mConsiderGroups->isChecked()) && dynamic_cast<GroupItem*>(p) != 0) {
			NeuronGroup *group = dynamic_cast<GroupItem*>(p)->getNeuronGroup();
			properties.append(group);
			idString = QString::number(group->getId());
			name = group->getName();
		}
		else if((allTypes || mConsiderModules->isChecked()) && dynamic_cast<ModuleItem*>(p) != 0) {
			NeuroModule *module = dynamic_cast<ModuleItem*>(p)->getNeuroModule();
			properties.append(module);
			idString = QString::number(module->getId());
			name = module->getName();
		}
	}
	if(properties.empty() && allTypes) {
		ModularNeuralNetwork *network = visu->getNeuralNetwork();
		if(network != 0) {
			mNameLabel->setText("[NeuralNetwork]");
			properties.append(network);
		}
	}
	else {
		if(name.size() > 20) {
			name = QString("..").append(name.mid(name.size() - 20));
		}
		mNameLabel->setText(QString("N-ID: ") + idString + " [" + name + "]");
	}

	return properties;
}


void PropertyDialog::addPropertyTemplate(const QString &property) {
	TRACE("PropertyDialog::addPropertyTemplate");

	if(mPropertySelectionField->findText(property) == -1) {
		mPropertySelectionField->addItem(property);
	}
}


QList<QString> PropertyDialog::getPropertyTemplates() const {
	TRACE("PropertyDialog::getPropertyTemplates");

	QList<QString> props;
	for(int i = 0; i < mPropertySelectionField->count(); ++i) {
		props.append(mPropertySelectionField->itemText(i));
	}
	return props;
}


void PropertyDialog::addStandardPropertyTemplates() {
	TRACE("PropertyDialog::addStandardPropertyTemplates");

	addPropertyTemplate("ConnectionTargetClass");
	addPropertyTemplate("ConnectionSourceClass");
}


void PropertyDialog::invalidateView() {

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	EditorToolWidget::invalidateView();
	mCurrentProperties = 0;
}


void PropertyDialog::enableDialog(bool enable) {
	mListScrollArea->setEnabled(enable);
	mSetButton->setEnabled(enable);
	mChangeButton->setEnabled(enable);
	mRemoveButton->setEnabled(enable);
}

void PropertyDialog::currentEditorTabChanged(int) {
	TRACE("PropertyDialog::addStandardPropertyTemplates");

	if(mEditor == 0) {
			return;
	}
	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		connect(visu, SIGNAL(neuralNetworkChanged(ModularNeuralNetwork*)),
				this, SLOT(networkChanged(ModularNeuralNetwork*)));
		
		visu->addSelectionListener(this);
	}
	
}


void PropertyDialog::networkChanged(ModularNeuralNetwork*) {
	TRACE("PropertyDialog::networkChanged");

	if(!mInitialized) {
		mInitialized = true;
		//grab tags from NeuroTagManager
		addPropertyTemplate("-----------------");
		QList<NeuroTag> tags = NeuroTagManager::getInstance()->getTags();
		for(QListIterator<NeuroTag> i(tags); i.hasNext();) {
			NeuroTag tag = i.next();
			addPropertyTemplate(tag.mTagName);
		}
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mNetworkInvalid = false;

	mCurrentProperties = 0;
	updatePropertyList();
}


void PropertyDialog::propertyRowChanged(int row) {
	TRACE("PropertyDialog::propertyRowChanged");

	if(mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QListWidgetItem *item = mPropertyList->item(row);

	if(item != 0) {
		QString line = item->text();
		int index = line.indexOf(" = ");
		if(index > -1) {
			mPropertySelectionField->lineEdit()->setText(line.mid(0, index).trimmed());
			mValueEditField->setText(line.mid(index + 3).trimmed());
		}
	}
}


void PropertyDialog::setButtonPressed() {	
	TRACE("PropertyDialog::setButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<Properties*> properties = getSelectedProperties();

	QString prop = mPropertySelectionField->lineEdit()->text().trimmed();

	if(prop != "" && !properties.empty()) {
		addPropertyTemplate(prop);
		
		if(mApplyToAllIndividuals->isChecked()) {
			QList<ModularNeuralNetwork*> elements = getMatchingNetworks();
			for(QListIterator<ModularNeuralNetwork*> i(elements); i.hasNext();) {
				ModularNeuralNetwork *net = i.next();
	
				QList<NeuralNetworkElement*> netElements;
				net->getNetworkElements(netElements);
				for(QListIterator<Properties*> p(properties); p.hasNext();) {
					NeuralNetworkElement *elem = dynamic_cast<NeuralNetworkElement*>(p.next());

					if(elem != 0) {
						Properties *partnerElem = dynamic_cast<Properties*>(
							NeuralNetwork::selectNetworkElementById(elem->getId(), netElements));

						if(partnerElem != 0) {
							partnerElem->setProperty(prop, mValueEditField->text().trimmed());
						}
					}
				}
			}
		}
		else {
			for(QListIterator<Properties*> i(properties); i.hasNext();) {
				Properties *p = i.next();
				p->setProperty(prop, mValueEditField->text().trimmed());
			}
		}
		updatePropertyList();
	}
}


void PropertyDialog::changeButtonPressed() {
	TRACE("PropertyDialog::changeButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<Properties*> properties = getSelectedProperties();

	QString prop = mPropertySelectionField->lineEdit()->text().trimmed();

	if(prop != "" && !properties.empty()) {
		addPropertyTemplate(prop);

		if(mApplyToAllIndividuals->isChecked()) {
			QList<ModularNeuralNetwork*> elements = getMatchingNetworks();
			for(QListIterator<ModularNeuralNetwork*> i(elements); i.hasNext();) {
				ModularNeuralNetwork *net = i.next();
	
				QList<NeuralNetworkElement*> netElements;
				net->getNetworkElements(netElements);
				for(QListIterator<Properties*> p(properties); p.hasNext();) {
					NeuralNetworkElement *elem = dynamic_cast<NeuralNetworkElement*>(p.next());

					if(elem != 0) {
						Properties *partnerElem = dynamic_cast<Properties*>(
							NeuralNetwork::selectNetworkElementById(elem->getId(), netElements));

						if(partnerElem != 0) {
							if(partnerElem->hasProperty(prop)) {
								partnerElem->setProperty(prop, mValueEditField->text().trimmed());
							}
						}
					}
				}
			}
		}
		else {
			for(QListIterator<Properties*> i(properties); i.hasNext();) {
				Properties *p = i.next();
				if(p->hasProperty(prop)) {
					p->setProperty(prop, mValueEditField->text().trimmed());
				}
			}
		}
		updatePropertyList();
	}
}


void PropertyDialog::removeButtonPressed() {
	TRACE("PropertyDialog::removeButtonPressed");

	if(mEditor == 0 || !mModificationsEnabled || mNetworkInvalid) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<Properties*> properties = getSelectedProperties();

	QString prop = mPropertySelectionField->lineEdit()->text().trimmed();

	if(prop != "" && !properties.empty()) {
		addPropertyTemplate(prop);

		if(mApplyToAllIndividuals->isChecked()) {
			QList<ModularNeuralNetwork*> elements = getMatchingNetworks();
			for(QListIterator<ModularNeuralNetwork*> i(elements); i.hasNext();) {
				ModularNeuralNetwork *net = i.next();
	
				QList<NeuralNetworkElement*> netElements;
				net->getNetworkElements(netElements);
				for(QListIterator<Properties*> p(properties); p.hasNext();) {
					NeuralNetworkElement *elem = dynamic_cast<NeuralNetworkElement*>(p.next());

					if(elem != 0) {
						Properties *partnerElem = dynamic_cast<Properties*>(
							NeuralNetwork::selectNetworkElementById(elem->getId(), netElements));

						if(partnerElem != 0) {
							partnerElem->removeProperty(prop);
						}
					}
				}
			}
		}
		else {
			for(QListIterator<Properties*> i(properties); i.hasNext();) {
				Properties *p = i.next();
				if(p != 0) {
					p->removeProperty(prop);
				}
			}
		}
		updatePropertyList();
	}
}


void PropertyDialog::updatePropertyList() {
	TRACE("PropertyDialog::updatePropertyList");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	mPropertyList->clear();
	if(mCurrentProperties != 0) {

		QList<QString> propertyNames = mCurrentProperties->getPropertyNames();
		for(QListIterator<QString> i(propertyNames); i.hasNext();) {
			QString name = i.next();
			if(!mShowAllProperties->isChecked() && name.startsWith("_")) {
				//do not show hidden properties if ShowAllProp checkbox is not selected.
				//all properties starting with _ will not be shown.
				continue;
			}
			QString line = name + " = " + mCurrentProperties->getProperty(name);
			new QListWidgetItem(line, mPropertyList);
		}

		enableDialog(true);
	}
	else {
		enableDialog(false);
	}
}


void PropertyDialog::updateElementSelection() {

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	if(mNetworkInvalid || mEditor == 0) {
		return;
	}

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();

	if(visu == 0) {
		return;
	}	
	
	ModularNeuralNetwork *net = visu->getNeuralNetwork();

	if(net == 0) {
		return;
	}

	QList<NeuralNetworkElement*> elements;
	net->getNetworkElements(elements);

	QList<NeuralNetworkElement*> selectedElements;

	QString property = mPropertySelectionField->currentText().trimmed();
	QString value = mValueEditField->text().trimmed();
	bool forceExactness = mForceExactMatchForSelectionCheckBox->isChecked();
	bool negate = mShowNonMatchingElementsCheckBox->isChecked();

	for(QListIterator<NeuralNetworkElement*> i(elements); i.hasNext();) {
		NeuralNetworkElement *elem = i.next();
		Properties *prop = dynamic_cast<Properties*>(elem);

		if(prop != 0) {
			if(prop->hasProperty(property)) {
				if((forceExactness && !(prop->getProperty(property).trimmed() == value))
					|| negate) 
				{
					continue;
				}
				selectedElements.append(elem);
			}
			else if(negate) {
				selectedElements.append(elem);
			}
		}
	}

	QMutexLocker lockSelection(visu->getSelectionMutex());

	//collect visualization elements
	QList<PaintItem*> selectedItems = visu->getPaintItems(selectedElements);

	//make sure that the currently selected items remains the last item in the list.
	QList<PaintItem*> currentlySelectedItems = visu->getSelectedItems();
	if(!currentlySelectedItems.empty()) {
		PaintItem *current = currentlySelectedItems.last();
		if(selectedItems.contains(current)) {
			selectedItems.removeAll(current);
			selectedItems.append(current);
		}
	}

	visu->setSelectedItems(selectedItems);
}

//Temporarily until a better documentation mechanism is found.
void PropertyDialog::showHelpDialog() {

	QDialog *dialog = new QDialog(this);
	dialog->setWindowTitle("Property Help");
	dialog->setAttribute(Qt::WA_QuitOnClose, false);

	QVBoxLayout *layout = new QVBoxLayout();	
	layout->setObjectName("HelpLayout");

	QTextEdit *edit = new QTextEdit();
	edit->setReadOnly(true);
	//edit->setLineWrapMode(QTextEdit::NoWrap);

	layout->addWidget(edit);
	
	dialog->setLayout(layout);

	QString helpMessage;

	NeuroTagManager *ntm = NeuroTagManager::getInstance();
	QList<NeuroTag> tags = ntm->getTags();

	for(QListIterator<NeuroTag> i(tags); i.hasNext();) {
		NeuroTag tag = i.next();
		helpMessage.append(tag.mTagName).append(" [").append(tag.mType).append("]\n")
				.append(tag.mDescription).append("\n\n");
	}

	edit->setText(helpMessage);

	dialog->setVisible(true);
}


//TODO 
//Move this method outside of the NetworkEditor package.
QList<ModularNeuralNetwork*> PropertyDialog::getMatchingNetworks() 
{

	QList<ModularNeuralNetwork*> networks;
	if(mEvolutionManager == 0 || mEditor == 0 || mEditor->getCurrentNetworkVisualization() == 0) {
		return networks;
	}
	
	ModularNeuralNetwork *currentNet = mEditor->getCurrentNetworkVisualization()->getNeuralNetwork();

	QList<World*> worlds = mEvolutionManager->getEvolutionWorlds();
	for(QListIterator<World*> i(worlds); i.hasNext();) {
		World *world = i.next();

		QList<Population*> populations = world->getPopulations();
		for(QListIterator<Population*> j(populations); j.hasNext();) {
			Population *pop = j.next();

			QList<Individual*> individuals = pop->getIndividuals();
			for(QListIterator<Individual*> k(individuals); k.hasNext();) {
				Individual *ind = k.next();
				ModularNeuralNetwork *network = dynamic_cast<ModularNeuralNetwork*>(ind->getGenome());
				if(network == currentNet) {
					for(QListIterator<Individual*> l(individuals); l.hasNext();) {
						ModularNeuralNetwork *n = dynamic_cast<ModularNeuralNetwork*>(
								l.next()->getGenome());
						if(n != 0) {
							networks.append(n);
						}
					}
					return networks;
				}
			}
		}
	}

	return networks;
	
}


}




