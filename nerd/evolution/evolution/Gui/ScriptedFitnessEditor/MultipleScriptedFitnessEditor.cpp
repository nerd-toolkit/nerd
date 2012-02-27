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



#include "MultipleScriptedFitnessEditor.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include "Value/Value.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include "Gui/ScriptedFitnessEditor/ScriptedFitnessEditor.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new MultipleScriptedFitnessEditor.
 */
MultipleScriptedFitnessEditor::MultipleScriptedFitnessEditor(const QString &fitnessNamePattern, 
							bool initOnConstruction)
	: mInitialized(false), mFitnessValueNamePattern(fitnessNamePattern)
{
	setWindowTitle("Scripted Fitness Editor");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	if(initOnConstruction) {
		initialize();
	}
}



/**
 * Destructor.
 */
MultipleScriptedFitnessEditor::~MultipleScriptedFitnessEditor() {
}

void MultipleScriptedFitnessEditor::initialize() {

	ValueManager *vm = Core::getInstance()->getValueManager();

	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	QTabWidget *tabWidget = new QTabWidget();

	//QStringList fitnessFunctions = vm->getValueNamesMatchingPattern("/Evo/Pop/.*/Fitness/.*/Code");
	QStringList fitnessFunctions = vm->getValueNamesMatchingPattern(mFitnessValueNamePattern);
	for(QListIterator<QString> i(fitnessFunctions); i.hasNext();) {
		QString name = i.next();

		QString editorValuePath = name;
		editorValuePath = editorValuePath.mid(0, editorValuePath.size() - 5);

		QString fitnessName = editorValuePath;
		fitnessName = fitnessName.mid(fitnessName.lastIndexOf("/") + 1);

		QString populationName = name;
		populationName = populationName.mid(9);
		populationName = populationName.mid(0, populationName.indexOf("/"));

		

		tabWidget->addTab(new ScriptedFitnessEditor(editorValuePath), populationName + "/" + fitnessName);
	}

	layout->addWidget(tabWidget);

	mInitialized = true;
}

void MultipleScriptedFitnessEditor::showWidget() {
	if(!mInitialized) {
		initialize();
	}
	show();
}


}



