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


#include "EditorMessageWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new EditorMessageWidget.
 */
EditorMessageWidget::EditorMessageWidget(NeuralNetworkEditor *owner)
	: EditorToolWidget(owner), mInitialized(false)
{

	QVBoxLayout *layout = new QVBoxLayout(this);
	
	setLayout(layout);

	mTextArea = new QTextEdit(this);
	mTextArea->document()->setMaximumBlockCount(30);
	//mTextArea->setReadOnly(true);
	mTextArea->setWordWrapMode(QTextOption::NoWrap);
	mTextArea->resize(100, 50);
	resize(100, 50);
	layout->addWidget(mTextArea);

	if(mEditor != 0) {
		connect(mEditor, SIGNAL(tabSelectionChanged(int)),
				this, SLOT(currentEditorTabChanged(int)));
	}


}


/**
 * Destructor.
 */
EditorMessageWidget::~EditorMessageWidget() {
}


void EditorMessageWidget::modificationPolicyChanged() {
}


void EditorMessageWidget::currentEditorTabChanged(int) {
	if(mEditor == 0) {
			return;
	}
}


void EditorMessageWidget::clear() {
	mTextArea->document()->clear();
}


void EditorMessageWidget::addMessage(const QString &message) {
	mTextArea->append(message);
}



void EditorMessageWidget::setMessage(const QString &message) {
	mTextArea->document()->clear();
	mTextArea->append(message);
}




}


