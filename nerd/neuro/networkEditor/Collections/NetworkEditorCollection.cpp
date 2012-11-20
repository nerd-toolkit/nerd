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



#include "NetworkEditorCollection.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QAction>
#include "Gui/NetworkEditor/EvaluationNetworkEditor.h"
#include "Gui/NetworkEditorTools/ChangeViewModeTool.h"
#include "Gui/NetworkEditorTools/NeuralNetworkToolbox.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new NetworkEditorCollection. 
 */
NetworkEditorCollection::NetworkEditorCollection(QMenu *targetMenu, const QString &name, 
												 bool allowManualStasisControl)
{
	EvaluationNetworkEditor *editor = new EvaluationNetworkEditor(allowManualStasisControl);
	editor->resize(1000, 700);
	editor->initialize();

	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_INPUT_OUTPUT, 
						   "Show Motor / Sensor Neurons", Qt::Key_I);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_MODULE_INPUT_OUTPUT, 
						   "Show Module Input/Output", Qt::Key_M, false, true);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_ACTIVATION_FLIPPED, 
						   "Show Neuron Flipped State", Qt::Key_F);
	
	new ChangeViewModeTool(editor, ChangeViewModeTool::MENU_SEPARATOR);
	
	new ChangeViewModeTool(editor, PaintItem::SHOW_MODULE_NAMES, 
						   "Show Module Names", Qt::Key_O, false, true);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_NAME, 
						   "Show Neuron Names", Qt::Key_N , false, true);
	new ChangeViewModeTool(editor, PaintItem::HIDE_WEIGHTS,
						   "Show Synapse Weights", Qt::Key_W, true, true);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_BIAS_AS_NUMBER, 
						   "Show Bias Values", Qt::Key_V);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURON_BIAS, 
						   "Highlight Biased Neurons", Qt::Key_B);
	
	new ChangeViewModeTool(editor, ChangeViewModeTool::MENU_SEPARATOR);
	
	new ChangeViewModeTool(editor, PaintItem::HIDE_UNSELECTED,
							"Hide Unselected Elements", Qt::Key_H);
	
	
	new ChangeViewModeTool(editor, ChangeViewModeTool::MENU_SEPARATOR);
	
	new ChangeViewModeTool(editor, PaintItem::SHOW_MODULE_HANDLES,
							"Hide Module Handles", Qt::Key_1, true);
	new ChangeViewModeTool(editor, PaintItem::SHOW_MODULE_BACKGROUND,
							"Hide Module Background", Qt::Key_2, true);
	new ChangeViewModeTool(editor, PaintItem::USE_SYNAPSE_TYPE_SYMBOLS,
							"Use Only Synapse Arrows", Qt::Key_E, true);
	new ChangeViewModeTool(editor, PaintItem::INCREASE_READABILITY,
							"Increase Text Readability", Qt::Key_R, false, true);
	
	new ChangeViewModeTool(editor, ChangeViewModeTool::MENU_SEPARATOR);
	
	new ChangeViewModeTool(editor, PaintItem::SHOW_ELEMENT_SLAVE_STATUS,
							"Show Degrees of Freedom", Qt::Key_S);
	new ChangeViewModeTool(editor, PaintItem::SHOW_NEURO_MODULATORS,
						   "Show Neuro-Modulators", Qt::Key_T, false, false);
	new ChangeViewModeTool(editor, PaintItem::HIGHLIGHT_WEIGHT_CHANGES,
						   "Highlight Weight Changes", Qt::Key_T, false, false);
	new ChangeViewModeTool(editor, PaintItem::DRAW_NAMES_SOLID,
							"Draw all Names Black", Qt::Key_9, false, true);
// 	new ChangeViewModeTool(editor, PaintItem::USE_COSMETIC_LINES,
// 							"Draw Cosmetic Lines", Qt::Key_C);
	new NeuralNetworkToolbox(editor);

	if(targetMenu != 0) {
		QAction *action = targetMenu->addAction(name);
		action->setShortcut(Qt::Key_F4);
		QObject::connect(action, SIGNAL(triggered()),
						 editor, SLOT(show()));
	}
}






}



