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



#include "ChangeValueCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/Neuro.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new ChangeValueCommand.
 */
ChangeValueCommand::ChangeValueCommand(NetworkVisualization *context, QList<Value*> values, 
							QList<QString> contents, const QString &label)
	: Command("Change Parameter"), mVisuContext(context), mValues(values), mNewContents(contents)
{
	if(label != "") {
		setLabel(label);
	}
}



/**
 * Destructor.
 */
ChangeValueCommand::~ChangeValueCommand() {
}



bool ChangeValueCommand::isUndoable() const {
	return true;
}



bool ChangeValueCommand::doCommand() {
	if(mVisuContext == 0 || mNewContents.size() != mValues.size()) {
		return true;
	}
	QMutexLocker guard(mVisuContext->getSelectionMutex());

	mOldContents.clear();
	for(int i = 0; i < mValues.size() && i < mNewContents.size(); ++i) {
		Value *value = mValues.at(i);
		if(value == 0) {
			continue;
		}
		mOldContents.append(value->getValueAsString());
	
		value->setValueFromString(mNewContents.at(i));
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisuContext->notifyNeuralNetworkModified();

	return true;
}


bool ChangeValueCommand::undoCommand() {
	if(mVisuContext == 0 || mNewContents.size() != mValues.size() 
		|| mOldContents.size() != mValues.size()) 
	{
		return true;
	}
	QMutexLocker guard(mVisuContext->getSelectionMutex());
	
	for(int i = 0; i < mValues.size() && i < mOldContents.size(); ++i) {
		Value *value = mValues.at(i);
		if(value == 0) {
			continue;
		}
		value->setValueFromString(mOldContents.at(i));
	}

	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisuContext->notifyNeuralNetworkModified();

	return true;
}


}




