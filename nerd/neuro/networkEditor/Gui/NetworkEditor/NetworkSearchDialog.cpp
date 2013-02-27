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



#include "NetworkSearchDialog.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QLabel>
#include <QVBoxLayout>
#include "Network/NeuralNetworkManager.h"
#include "Network/Neuro.h"
#include <QMutexLocker>
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include <QRegExp>
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Gui/NetworkEditor/SynapseItem.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NetworkSearchDialog.
 */
NetworkSearchDialog::NetworkSearchDialog(NeuralNetworkEditor *owner, QWidget *parent)
	: QWidget(parent), mOwner(owner)
{
	setObjectName("NeuronSearchDialog");
	
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	QVBoxLayout *layout = new QVBoxLayout(this);	
	layout->setObjectName("MainLayout");
	setLayout(layout);

	QLabel *searchLabel = new QLabel("Search Phrase");
	layout->addWidget(searchLabel);

	mSearchTextField = new QLineEdit("");
	layout->addWidget(mSearchTextField);

	mSearchButton = new QPushButton("Search");
	layout->addWidget(mSearchButton);
	
	//TODO add "search only selected" checkbox

	connect(mSearchButton, SIGNAL(pressed()),
			this, SLOT(selectMatchingElements()));
	connect(mSearchTextField, SIGNAL(returnPressed()),
			this, SLOT(selectMatchingElements()));

	resize(200, 50);
}


/**
 * Destructor.
 */
NetworkSearchDialog::~NetworkSearchDialog() {
}

void NetworkSearchDialog::showWidget() {
	show();
	raise();
	mSearchTextField->setFocus();
}


void NetworkSearchDialog::selectMatchingElements() {

	if(mOwner == 0) {
		return;
	}

	NeuralNetworkManager *nmm = Neuro::getNeuralNetworkManager();
	QMutexLocker guard(nmm->getNetworkExecutionMutex());

	NetworkVisualization *visu = mOwner->getCurrentNetworkVisualization();

	if(visu == 0) {
		return;
	}

	{
		QMutexLocker guard2(visu->getSelectionMutex());
	
		ModularNeuralNetwork *net = visu->getNeuralNetwork();
		NetworkVisualizationHandler *handler = visu->getVisualizationHandler();
	
		if(net == 0 || handler == 0) {
			return;
		}
	
		QList<PaintItem*> selectedItems;
	
		QString regExpText = mSearchTextField->text();
		QString paramValue = "";

		qulonglong id = 0;
		bool searchForId = false;
		bool searchForModelFunction = false;
		bool searchForModelFunctionParameter = false;
		bool searchForModelFunctionParameterValue = false;
		
		if(regExpText.startsWith("$")) {
			//id mode
			id = regExpText.mid(1).toULongLong();
			searchForId = true;
		}
		else if(regExpText.startsWith("@@")) {
			regExpText = regExpText.mid(2);
			searchForModelFunctionParameter = true;
			if(regExpText.contains("=")) {
				searchForModelFunctionParameterValue = true;
				paramValue = regExpText.mid(regExpText.indexOf("=") + 1);
				regExpText = regExpText.mid(0, regExpText.indexOf("="));
			}
		}
		else if(regExpText.startsWith("@")) {
			regExpText = regExpText.mid(1);
			searchForModelFunction = true;
		}
		
		regExpText.replace("**", ".*");
		paramValue.replace("**", ".*");
		
		QRegExp expr(regExpText);
		expr.setCaseSensitivity(Qt::CaseInsensitive);
		
		QRegExp contentExpr(paramValue);
		contentExpr.setCaseSensitivity(Qt::CaseInsensitive);
	

		QList<Neuron*> neurons = net->getNeurons();
		for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
			Neuron *neuron = i.next();
	
			if((searchForId && id == neuron->getId()) 
				|| (!searchForId && expr.exactMatch(neuron->getNameValue().get()))
				|| (!searchForId 
					&& searchForModelFunction 
					&& ((expr.exactMatch(neuron->getTransferFunction()->getName()))
						|| (expr.exactMatch(neuron->getActivationFunction()->getName())))))
			{
				//add this neuron to the list of selected items
				PaintItem *item = handler->getNeuronItem(neuron);
				if(item != 0) {
					selectedItems.append(item);
				}
			}
			if(searchForModelFunction || searchForModelFunctionParameter) {
				if((matchFunctionParameters(neuron->getActivationFunction(), expr, contentExpr, 
											searchForModelFunctionParameterValue))
					||
					(matchFunctionParameters(neuron->getTransferFunction(), expr, contentExpr, 
											 searchForModelFunctionParameterValue)))
				{
					//add this neuron to the list of selected items
					PaintItem *item = handler->getNeuronItem(neuron);
					if(item != 0) {
						selectedItems.append(item);
					}
				}
			}
		}
		QList<Synapse*> synapses = net->getSynapses();
		for(QListIterator<Synapse*> i(synapses); i.hasNext();) {
			Synapse *synapse = i.next();
	
			if((searchForId && id == synapse->getId())
				|| (!searchForId && expr.exactMatch(synapse->getStrengthValue().getValueAsString()))
				|| (!searchForId && searchForModelFunction && expr.exactMatch(synapse->getSynapseFunction()->getName()))) 
			{
				//add this synapse to the list of selected items
				PaintItem *item = handler->getSynapseItem(synapse);
				if(item != 0) {
					selectedItems.append(item);
				}
			}
			if(searchForModelFunction || searchForModelFunctionParameter) {
				if(matchFunctionParameters(synapse->getSynapseFunction(), expr, contentExpr, 
											searchForModelFunctionParameterValue))
				{
					//add this neuron to the list of selected items
					//add this synapse to the list of selected items
					PaintItem *item = handler->getSynapseItem(synapse);
					if(item != 0) {
						selectedItems.append(item);
					}
				}
			}
		}
		QList<NeuronGroup*> groups = net->getNeuronGroups();
		for(QListIterator<NeuronGroup*> i(groups); i.hasNext();) {
			NeuronGroup *group = i.next();
	
			if((searchForId && group->getId() == id)
				|| (!searchForId && expr.exactMatch(group->getName()))) 
			{
				//add this group or module to the list of selected items
				NeuroModule *module = dynamic_cast<NeuroModule*>(group);
				if(module != 0) {
					PaintItem *item = handler->getModuleItem(module);
					if(item != 0) {
						selectedItems.append(item);
					}
				}
				else {
					PaintItem *item = handler->getGroupItem(group);
					if(item != 0) {
						selectedItems.append(item);
					}
				}
			}
		}

		visu->setSelectedItems(selectedItems);
	}

}

bool NetworkSearchDialog::matchFunctionParameters(ParameterizedObject *funct, QRegExp &nameRegExp, 
												  QRegExp &contentRegExp, bool matchContent)
{
	if(funct == 0) {
		return false;
	}
	QList<QString> paramNames = funct->getParameterNames();
	for(QListIterator<QString> k(paramNames); k.hasNext();) {
		QString name = k.next();
		if(nameRegExp.exactMatch(name)) {
			if(!matchContent
				|| contentRegExp.exactMatch(funct->getParameter(name)->getValueAsString()))
			{
				return true;
			}
		}
	}
	return false;
}


}



