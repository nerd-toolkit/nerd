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



#include "EvolutionMainControlParameterPanel.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include "Event/EventManager.h"
#include "Value/ValueManager.h"
#include "EvolutionConstants.h"
#include "Value/ChangeValueTask.h"
#include "Gui/EvolutionParameterPanel/ParameterControlWidget.h"
#include "Gui/FitnessPlotter/OnlineFitnessPlotter.h"
#include "Gui/EvolutionParameterPanel/ParameterControlWidget.h"
#include "NerdConstants.h"
#include "Value/DoubleValue.h"


using namespace std;

namespace nerd {




/**
 * Constructs a new EvolutionMainControlParameterPanel.
 * This QWidget has to be created during or after the binding phase!
 */
EvolutionMainControlParameterPanel::EvolutionMainControlParameterPanel(const QString &worldName)
	: mGenerationCompletedEvent(0), mShutDownEvent(0), mWorldName(worldName)
{

	EventManager *em = Core::getInstance()->getEventManager();
	mShutDownEvent = em->registerForEvent(NerdConstants::EVENT_NERD_SYSTEM_SHUTDOWN, this, false);

	ValueManager *vm = Core::getInstance()->getValueManager();
	mCurrentGenerationValue = vm->getIntValue(EvolutionConstants::VALUE_EVO_CURRENT_GENERATION_NUMBER);
	mCurrentIndividualValue = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_INDIVIDUAL);
	mCurrentTryValue = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_CURRENT_TRY);
	mNumberOfTriesValue = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_TRIES);
	mNumberOfStepsValue = vm->getIntValue(EvolutionConstants::VALUE_EXECUTION_NUMBER_OF_STEPS);

	if(mCurrentGenerationValue != 0) {
		mCurrentGenerationValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mCurrentGenerationValue.");
	}
	if(mCurrentIndividualValue != 0) {
		mCurrentIndividualValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mCurrentIndividualValue.");
	}
	if(mCurrentTryValue != 0) {
		mCurrentTryValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mCurrentTryValue.");
	}
	if(mNumberOfTriesValue != 0) {
		mNumberOfTriesValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mNumberOfTriesValue.");
	}
	if(mNumberOfStepsValue != 0) {
		mNumberOfStepsValue->addValueChangedListener(this);
	}
	else {
		Core::log("EvolutionMainControlParameterPanel: Could not find mNumberOfStepsValue.");
	}


	//Setup GUI
	QVBoxLayout *mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	//Status Area
	QFrame *statusFrame = new QFrame();
	QVBoxLayout *statusLayout = new QVBoxLayout();
	statusFrame->setLayout(statusLayout);
	statusFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	statusFrame->setLineWidth(1);
	
	mCurrentGenerationLabel = new QLabel("Gen: 0");
	mCurrentIndividualLabel = new QLabel("Ind: 0");
	mCurrentTryLabel = new QLabel("Try: 0");

	QHBoxLayout *currentStateLayout = new QHBoxLayout();
	currentStateLayout->addWidget(mCurrentGenerationLabel);
	currentStateLayout->addWidget(mCurrentIndividualLabel);
	currentStateLayout->addWidget(mCurrentTryLabel);
	statusLayout->addLayout(currentStateLayout);

	//Parameter area
	QWidget *parameterPanel = new QWidget();
	QVBoxLayout *parameterLayout = new QVBoxLayout();
	parameterPanel->setLayout(parameterLayout);

	//Add control objects
	QHBoxLayout *runParamsLayout = new QHBoxLayout();
	runParamsLayout->addWidget(new ParameterControlWidget("Steps", mNumberOfStepsValue, 120));
	runParamsLayout->addWidget(new ParameterControlWidget("Tries", mNumberOfTriesValue, 120));
	
	runParamsLayout->addStretch(5);
	parameterLayout->addLayout(runParamsLayout);
	addPopulationParameters(parameterLayout);
	parameterLayout->addStretch(5);

	QScrollArea *scrollArea = new QScrollArea();
	scrollArea->setWidgetResizable(true);
	scrollArea->setContentsMargins(1, 0, 1, 0);
	parameterPanel->show();
	scrollArea->setWidget(parameterPanel);

	//FitnessPlotter
	OnlineFitnessPlotter *plotter = new OnlineFitnessPlotter(false);
	plotter->getPlotter()->show();
	plotter->getPlotter()->hideHandle(true);
	plotter->getPlotter()->setPlotterRange(500);
	plotter->getPlotter()->setMinimumHeight(200);
	plotter->bind();
	if(plotter->getPlotter()->getValuePlotter() != 0 
			&& plotter->getPlotter()->getValuePlotter()->getPlotterWidget() != 0) 
	{
		plotter->getPlotter()->getValuePlotter()->getPlotterWidget()->showValuesInLegend(true);
	}

	mainLayout->addWidget(statusFrame);
	mainLayout->addWidget(plotter->getPlotter());
	mainLayout->addWidget(scrollArea);
}


/**
 * Destructor.
 */
EvolutionMainControlParameterPanel::~EvolutionMainControlParameterPanel() {
}

QString EvolutionMainControlParameterPanel::getName() const {
	return "EvolutionMainControlParameterPanel";
}


void EvolutionMainControlParameterPanel::valueChanged(Value *value) {
	if(value == 0) {
		return;
	}
	else if(value == mCurrentTryValue || value == mNumberOfTriesValue) {
		if(mNumberOfTriesValue == 0 || mCurrentTryValue == 0) {
			return;
		}
		mCurrentTryLabel->setText("Try: " + mCurrentTryValue->getValueAsString()
					+ " / " + mNumberOfTriesValue->getValueAsString());
	}
	else if(value == mCurrentIndividualValue) {
		mCurrentIndividualLabel->setText("Ind: " + mCurrentIndividualValue->getValueAsString());
	}
	else if(value == mCurrentGenerationValue) {
		mCurrentGenerationLabel->setText("Gen: " + mCurrentGenerationValue->getValueAsString());
	}
}


void EvolutionMainControlParameterPanel::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mShutDownEvent) {
		//deregister from all values and events
		if(mCurrentIndividualValue != 0) {
			mCurrentIndividualValue->removeValueChangedListener(this);
			mCurrentIndividualValue = 0;
		}
		if(mCurrentTryValue != 0) {
			mCurrentTryValue->removeValueChangedListener(this);
			mCurrentTryValue = 0;
		}
		if(mCurrentGenerationValue != 0) {
			mCurrentGenerationValue->removeValueChangedListener(this);
			mCurrentGenerationValue = 0;
		}
		if(mNumberOfStepsValue != 0) {
			mNumberOfStepsValue->removeValueChangedListener(this);
			mNumberOfStepsValue = 0;
		}
		if(mNumberOfTriesValue != 0) {
			mNumberOfTriesValue->removeValueChangedListener(this);
			mNumberOfTriesValue = 0;
		}
	}
}

void EvolutionMainControlParameterPanel::selectionMethodIndexChanged(int index) {
	if(index >= 0 && index < mSelectionProportions.size()) {
		for(int i = 0; i < mSelectionProportions.size(); ++i) {
			DoubleValue *propVal = mSelectionProportions.at(i);
			if(i == index) {
				propVal->set(1.0);
			}
			else {
				propVal->set(0.0);
			}
		}
	}
}



void EvolutionMainControlParameterPanel::addPopulationParameters(QVBoxLayout *layout) {
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	QString prefix = "/Evo/" + mWorldName + "/Pop/";
	QStringList desiredPopNames = vm->getValueNamesMatchingPattern(prefix + ".*/DesiredPopulationSize");

	for(QListIterator<QString> i(desiredPopNames); i.hasNext();) {
		QString desiredPopName = i.next();
		QString name = desiredPopName.mid(prefix.size());
		name = name.mid(0, name.indexOf("/"));

		{
			QVBoxLayout *mainLayout = new QVBoxLayout();
			QFrame *populationFrame = new QFrame();
			populationFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
			populationFrame->setLineWidth(1);
			populationFrame->setContentsMargins(1, 1, 1, 1);
			QHBoxLayout *nameLayout = new QHBoxLayout();
			populationFrame->setLayout(nameLayout);
	
			//Add Population basic parameters.
			QLabel *nameLabel = new QLabel("Population: " + name);
			nameLayout->addWidget(nameLabel);
	
			mainLayout->addWidget(populationFrame);
	
			QHBoxLayout *populationLayout = new QHBoxLayout();
			
			IntValue *desPopSize = vm->getIntValue(prefix + name + "/DesiredPopulationSize");
			if(desPopSize != 0) {
				populationLayout->addWidget(new ParameterControlWidget("DesiredPopSize", desPopSize, 120));
			}
			IntValue *numPreservedParents = vm->getIntValue(prefix + name + "/NumberOfPreservedParents");
			if(numPreservedParents != 0) {
				populationLayout->addWidget(new ParameterControlWidget(
								"PreservedParents", numPreservedParents, 120));
			}
			populationLayout->addStretch(4);
			mainLayout->addLayout(populationLayout);
			layout->addLayout(mainLayout);
		}

		mSelectionMethodBox = new QComboBox();
		QLabel *selectionMethodLabel = new QLabel("Selection Method");
		QHBoxLayout *selectionBoxLayout = new QHBoxLayout();
		QFrame *selectionBoxFrame = new QFrame();
		selectionBoxFrame->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
		selectionBoxFrame->setLineWidth(1);
		selectionBoxFrame->setContentsMargins(1, 1, 1, 1);
		selectionBoxFrame->setLayout(selectionBoxLayout);
		selectionBoxLayout->addWidget(selectionMethodLabel);
		selectionBoxLayout->addWidget(mSelectionMethodBox);
		selectionBoxLayout->addStretch(5);

		layout->addWidget(selectionBoxFrame);

		mSelectionMethodParamLayout = new QStackedLayout();
		layout->addLayout(mSelectionMethodParamLayout);

		int selectedIndex = 0;
		int currentIndex = 0;

		//Add Selection parameters
		QStringList selectionNames = vm->getValueNamesMatchingPattern(
						prefix + name + "/Selection/.*/PopulationProportion");
		for(QListIterator<QString> j(selectionNames); j.hasNext();) {
			QString selectionName = j.next();
	
			selectionName = selectionName.mid(QString(prefix + name + "/Selection/").size());
			selectionName = selectionName.mid(0, selectionName.indexOf("/"));

			mSelectionMethodBox->addItem(selectionName);

			{	
				QGridLayout *selectionLayout = new QGridLayout();
				int parameterWidgetWidth = 120;
	
				QString selectionPrefix = prefix + name + "/Selection/" + selectionName;

				DoubleValue *popProp = vm->getDoubleValue(selectionPrefix + "/PopulationProportion");
				if(popProp != 0) {
					mSelectionProportions.append(popProp);
					if(popProp->get() > 0.0) {
						selectedIndex = currentIndex;
					}
				}

				QStringList otherProps = vm->getValueNamesMatchingPattern(selectionPrefix + "/.*");
				int propIndex = 0;
				for(QListIterator<QString> m(otherProps); m.hasNext();) {
					QString propName = m.next();
					QString propShortName = propName.mid(selectionPrefix.size() + 1);

					if(propShortName == "PopulationProportion") {
						continue;
					}
					
					Value *value = vm->getValue(propName);
					if(value != 0) {
						selectionLayout->addWidget(new ParameterControlWidget(propShortName, value, 120),
											propIndex / 4, propIndex % 4);
						propIndex++;
					}
				}
				int horSize = propIndex % 4;
				while(horSize < 4) {
					QWidget *placeHolder = new QWidget();
					placeHolder->setFixedWidth(parameterWidgetWidth);
					selectionLayout->addWidget(placeHolder, 0, horSize);
					++horSize;
				}
				
				QWidget *selectionWidget = new QWidget();
				selectionWidget->setLayout(selectionLayout);
				mSelectionMethodParamLayout->addWidget(selectionWidget);
			}
			currentIndex++;
		}

		connect(mSelectionMethodBox, SIGNAL(currentIndexChanged(int)),
             	mSelectionMethodParamLayout, SLOT(setCurrentIndex(int)));
		connect(mSelectionMethodBox, SIGNAL(currentIndexChanged(int)),
             	this, SLOT(selectionMethodIndexChanged(int)));

		mSelectionMethodBox->setCurrentIndex(selectedIndex);
	}
}


}



