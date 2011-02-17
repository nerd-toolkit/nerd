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



#include "NetworkVisualizationHandler.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"
#include "NeuralNetworkConstants.h"
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new NetworkVisualizationHandler.
 */
NetworkVisualizationHandler::NetworkVisualizationHandler(const QString &name, NetworkVisualization *owner)
	: ParameterizedObject(name, QString("/Visualization/Network/").append(name)),
	  mOwner(owner), mNeuralNetwork(0)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the NetworkVisualizationHandler object to copy.
 */
NetworkVisualizationHandler::NetworkVisualizationHandler(const NetworkVisualizationHandler &other) 
	: ParameterizedObject(other), mNeuralNetwork(0)
{
}

/**
 * Destructor.
 */
NetworkVisualizationHandler::~NetworkVisualizationHandler() {
}

QString NetworkVisualizationHandler::getName() const {
	return "NetworkVisualizationHandler";
}


bool NetworkVisualizationHandler::setNeuralNetwork(ModularNeuralNetwork *network) {
	if(mOwner != 0 && mOwner->getOwner() != 0 && network != mNeuralNetwork) {
		mOwner->getOwner()->renameCurrentNetwork("");
	}
	mNeuralNetwork = network;

	if(mNeuralNetwork != 0 && mNeuralNetwork->hasProperty("_Bookmarks") && mOwner != 0) {
		//Try to recover bookmarks
		QString bookmarks = mNeuralNetwork->getProperty("_Bookmarks");
		QStringList bookmarkEntries = bookmarks.split("|");
		for(int j = 0; j < bookmarkEntries.size(); ++j) {
			QStringList bookmarkElements = bookmarkEntries.at(j).split(",");
			if(bookmarkElements.size() == 3) {
				double x = bookmarkElements.at(0).toDouble();
				double y = bookmarkElements.at(1).toDouble();
				double scaling = bookmarkElements.at(2).toDouble();
				mOwner->setBookmark(j, x, y, scaling);
			}
		}
	}
	if(mNeuralNetwork != 0 && mNeuralNetwork->hasProperty("_Viewport") && mOwner != 0) {
		//Try to set viewport (and overwrite the F8 bookmark if a viewport is found).
		QString viewport = mNeuralNetwork->getProperty("_Viewport");
		QStringList viewportElements = viewport.split(",");
		if(viewportElements.size() == 3) {
			double x = viewportElements.at(0).toDouble();
			double y = viewportElements.at(1).toDouble();
			double scaling = viewportElements.at(2).toDouble();
			mOwner->setHomeBookmark(x, y, scaling);
		}
	}
	return true;
}

ModularNeuralNetwork* NetworkVisualizationHandler::getNeuralNetwork() const {
	return mNeuralNetwork;
}

//TODO provide own, faster implementation
void NetworkVisualizationHandler::rebuildView() {
	setNeuralNetwork(mNeuralNetwork);
}

}




