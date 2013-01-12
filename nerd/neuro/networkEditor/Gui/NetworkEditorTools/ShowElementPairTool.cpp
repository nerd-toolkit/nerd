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



#include "ShowElementPairTool.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Network/Neuro.h"
#include "Network/Neuron.h"
#include "Network/NeuralNetwork.h"
#include "Network/Synapse.h"
#include "Network/NeuralNetworkElement.h"
#include <QApplication>
#include <QClipboard>

using namespace std;

namespace nerd {


/**
 * Constructs a new ShowElementPairTool.
 */
ShowElementPairTool::ShowElementPairTool(QObject *owner)
	: NetworkManipulationTool(owner)
{
}


/**
 * Destructor.
 */
ShowElementPairTool::~ShowElementPairTool() {
}

void ShowElementPairTool::clear() {
	if(mVisuContext != 0) {
		mVisuContext->removeMouseListener(this);
		mVisuContext->getElementPairVisualization()->setElementPairString("");
	}
	NetworkManipulationTool::clear();
}


void ShowElementPairTool::activate(NetworkVisualization *visu) {

	NetworkManipulationTool::activate(visu);
	if(mVisuContext != 0) {
		mVisuContext->addMouseListener(this);
		QString chainText = QApplication::clipboard()->text();
		updateStatusMessage(QString("Show Element Pairs: ").append(chainText));
		mVisuContext->getElementPairVisualization()->setElementPairString(chainText);
	}
	
	
}

QString ShowElementPairTool::getStatusMessage() {
	return QString("Show Element Pairs");
}

void ShowElementPairTool::mouseButtonPressed(NetworkVisualization *source, 
					QMouseEvent *e, const QPointF&)
{
	if(mVisuContext == 0 || mVisuContext != source || !(e->buttons() & Qt::RightButton)) {
		return;
	}

	QMutexLocker locker(Neuro::getNeuralNetworkManager()->getNetworkExecutionMutex());

	NetworkVisualizationHandler *handler = mVisuContext->getVisualizationHander();
	if(handler == 0) {
		return;
	}

	emit done();

}


void ShowElementPairTool::mouseButtonReleased(NetworkVisualization*, 
					QMouseEvent*, const QPointF&) 
{
}

void ShowElementPairTool::mouseDoubleClicked(NetworkVisualization*,
					QMouseEvent*, const QPointF&)
{

}


void ShowElementPairTool::mouseDragged(NetworkVisualization*, 
					QMouseEvent*, const QPointF&)
{
}



}



