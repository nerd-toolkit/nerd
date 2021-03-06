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



#include "ParameterEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditorCommands/ChangeValueCommand.h"
#include "Util/Tracer.h"
#include "Network/Neuro.h"
#include "Network/NeuralNetworkManager.h"
#include <QMutexLocker>


#define TRACE(message)
//#define TRACE(message) Tracer _ttt_(QString(message) + "[" + QString::number((long) QThread::currentThread()) + "]");


using namespace std;

namespace nerd {


/**
 * Constructs a new ParameterEditor.
 */
ParameterEditor::ParameterEditor(QWidget *owner, NeuralNetworkEditor *editor)
	: QWidget(owner), mScrollArea(0), mParameterWidget(0), mParameterLayout(0),
	   mParameterizedObject(0), mCurrentParameterizedObject(0), mEditor(editor)
{
	TRACE("ParameterEditor::ParameterEditor");

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	mScrollArea = new QScrollArea(this);
	mScrollArea->setWidgetResizable(true);
	mScrollArea->setMinimumSize(50, 50);
	layout->addWidget(mScrollArea);

	mParameterWidget = new QWidget(mScrollArea);
	mScrollArea->setWidget(mParameterWidget);
	mParameterLayout = new QGridLayout();
	mParameterWidget->setLayout(mParameterLayout);
	

	updateParameterView();
}



/**
 * Destructor.
 */
ParameterEditor::~ParameterEditor() {
	TRACE("ParameterEditor::~ParameterEditor");
}

void ParameterEditor::setParameterizedObject(ParameterizedObject *obj,
						QList<ParameterizedObject*> slaveObjects) 
{
	TRACE("ParameterEditor::setParameterizedObject");

	mSlaveObjects = slaveObjects;

	mParameterizedObject = obj;
	updateParameterView();
}


ParameterizedObject* ParameterEditor::getParameterizedObject() const {	
	TRACE("ParameterEditor::getParameterizedObject");

	return mParameterizedObject;
}


void ParameterEditor::invalidateListeners() {
	for(QListIterator<ParameterPanel*> i(mParameterPanels); i.hasNext();) {
		i.next()->invalidateListeners();
	}
}

void ParameterEditor::updateParameterView() {
	TRACE("ParameterEditor::updateParameterView");

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	QList<QString> names;
	QList<Value*> values;
	bool keepPanels = true;
	
	if(mParameterizedObject != 0) {
		names = mParameterizedObject->getParameterNames();
		values = mParameterizedObject->getParameters();
	}
	
	//Check if the parameter panel has to be rebuilt.
	if(mCurrentParameterizedObject != mParameterizedObject
		|| names.size() != mParameterPanels.size()
		|| values.size() != mParameterPanels.size())
	{
		keepPanels = false;
	}
	else {
		for(QListIterator<ParameterPanel*> i(mParameterPanels); i.hasNext();) {
			ParameterPanel *panel = i.next();

			int index = values.indexOf(panel->getParameterValue());
			if(index < 0 || names.indexOf(panel->getParameterName()) != index) {
				keepPanels = false;
			}
		}	
	}
	
	
	if(!keepPanels) {
		for(QListIterator<ParameterPanel*> i(mParameterPanels); i.hasNext();) {
			ParameterPanel *panel = i.next();
			disconnect(panel, SIGNAL(parameterChanged(Value*, const QString&, const QString&)),
					this, SLOT(parameterChanged(Value*, const QString&, const QString&)));
			panel->removeFromLayout(mParameterLayout);
			delete panel;
			//panel->deleteLater();
		}
		mParameterPanels.clear();
	
		for(int i = 0; i < names.size() && i < values.size(); ++i) {
			ParameterPanel *panel = new ParameterPanel(mParameterWidget, mParameterLayout, 
											names.at(i), values.at(i));

			connect(panel, SIGNAL(parameterChanged(Value*, const QString&, const QString&)),
					this, SLOT(parameterChanged(Value*, const QString&, const QString&)));

			mParameterPanels.append(panel);

		}
		mCurrentParameterizedObject = mParameterizedObject;
	}
}

void ParameterEditor::parameterChanged(Value *parameter, const QString &name, 
			const QString &content) 
{
	TRACE("ParameterEditor::parameterChanged");

	if(mEditor == 0 || parameter == 0) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu == 0) {
		return;
	}
	QList<Value*> values;
	QList<QString> contents;

	values.append(parameter);
	contents.append(content);


	for(QListIterator<ParameterizedObject*> i(mSlaveObjects); i.hasNext();) {
		ParameterizedObject *obj = i.next();
		Value *param = obj->getParameter(name);
		if(param != 0 && !values.contains(param)) {
			values.append(param);
			contents.append(content);
		}
	}

	Command *command = new ChangeValueCommand(visu, values, contents);

	visu->getCommandExecutor()->executeCommand(command);
}


}




