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


#include "AddActivationPlotterAction.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/Plotter/NeuronActivityPlotter.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include "Network/Neuron.h"
#include "Value/DoubleValue.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new AddActivationPlotterAction.
 */
AddActivationPlotterAction::AddActivationPlotterAction(const QString &name, 
					const QString &titleName, NeuralNetworkEditor *editor, 
					int plotMode, QObject *owner)
	: QAction(name, owner), mEditor(editor), mPlotMode(plotMode), mTitleName(titleName)
{
	connect(this, SIGNAL(triggered()),
			this, SLOT(addPlotter()));
}



/**
 * Destructor.
 */
AddActivationPlotterAction::~AddActivationPlotterAction() {
}


void AddActivationPlotterAction::addPlotter() {
	if(mEditor == 0) {
		return;
	}

	QList<PaintItem*> selectedItems;

	NetworkVisualization *visu = mEditor->getCurrentNetworkVisualization();
	if(visu != 0) {
		selectedItems = visu->getSelectedItems();
	}

	NeuronActivityPlotter *plotter = new NeuronActivityPlotter(mTitleName, mPlotMode, mEditor, 0, 0);
	//TODO organize destruction.


	//fill with selected neurons
	for(QListIterator<PaintItem*> i(selectedItems); i.hasNext();) {
		PaintItem *paintItem = dynamic_cast<PaintItem*>(i.next());
		if(paintItem != 0) {
			plotter->addPlottedNetworkElement(paintItem->getNetworkElement());
		}
	}

	plotter->showWidget();
}


}



