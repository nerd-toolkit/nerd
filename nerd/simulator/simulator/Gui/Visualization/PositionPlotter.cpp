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


#include <iostream>
#include "PositionPlotter.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimBody.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include "Gui/Visualization/OpenGLVisualization.h"
#include "Physics/Physics.h"
#include "Gui/GuiManager.h"

namespace nerd{

PositionPlotter::PositionPlotter() {
	setWindowTitle("Position Plotter");
	mPosX = 0;
	mPosY = 0;

	GuiManager::getGlobalGuiManager()->addWidget("PositionPlotter", this);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mWindowLayout = new QGridLayout();
	mWindowLayout->setMargin(1);
	setLayout(mWindowLayout);

	QGroupBox *simBodyBox = new QGroupBox();
	mWindowLayout->addWidget(simBodyBox, 0,0);
	mObjectSelectionBox = new QComboBox();
	simBodyBox->setTitle("Sim Body");
	simBodyBox->setWhatsThis("This is the physical body the position of which "
			"should be plotted.");
	QHBoxLayout *simBodyBoxLayout = new QHBoxLayout();
	simBodyBox->setLayout(simBodyBoxLayout);
	simBodyBoxLayout->addWidget(mObjectSelectionBox);

	mRemoveObject = new QPushButton("Remove sim object");
	mAddObject = new QPushButton("Add sim object");

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	mWindowLayout->addLayout(buttonLayout, 1, 0);
	buttonLayout->setMargin(1);
	buttonLayout->addWidget(mAddObject);
	buttonLayout->setAlignment(mAddObject, Qt::AlignRight);
	buttonLayout->addWidget(mRemoveObject);
	buttonLayout->setAlignment(mRemoveObject, Qt::AlignLeft);

	QGroupBox *objectsToPlot = new QGroupBox();
	objectsToPlot->setTitle("Objects to plot");
	mObjectList = new QListWidget(this);
	mObjectList->setSelectionMode(QAbstractItemView::MultiSelection);
	QHBoxLayout *objectsToPlotLayout = new QHBoxLayout();
	objectsToPlot->setLayout(objectsToPlotLayout);
	objectsToPlotLayout->setMargin(1);
	objectsToPlotLayout->addWidget(mObjectList);

	mWindowLayout->addWidget(objectsToPlot);
	mObjectList->hide();
	mObjectList->setWhatsThis("List of all sim bodies being plotted.");

	mActivateButton = new QPushButton("Activate Plotter");
	mDeactivateButton = new QPushButton("Deactivate Plotter");
	mDeactivateButton->setDisabled(true);
	QHBoxLayout *controlLayout = new QHBoxLayout();
	mWindowLayout->addLayout(controlLayout, 7, 0);
	controlLayout->setMargin(1);
	controlLayout->addWidget(mActivateButton);
	controlLayout->setAlignment(mActivateButton, Qt::AlignRight);
	controlLayout->addWidget(mDeactivateButton);
	controlLayout->setAlignment(mDeactivateButton, Qt::AlignLeft);

	connect(mRemoveObject, SIGNAL(clicked()),
			this, SLOT(removeSelectedObjects()));
    connect(mAddObject, SIGNAL(clicked()),
            this, SLOT(addObjectToList()));

	connect(mActivateButton, SIGNAL(clicked()),
			this, SLOT(plotterActivateAction()));
    connect(mDeactivateButton, SIGNAL(clicked()),
            this, SLOT(plotterDeactivateAction()));

    mPlotterActive = false;

}

PositionPlotter::~PositionPlotter() {
}

void PositionPlotter::addObjectToList() {
    if(!mPlotterActive) {
        QString objectName = mObjectSelectionBox->currentText();
        if(!mObjectNameList.contains(objectName)) {
            mObjectNameList.append(objectName);
            QListWidgetItem *item = new QListWidgetItem(objectName);
            mObjectList->addItem(item);
        }
        std::cout << "Objects in internal list:" << std::endl;
        for(int i=0; i<mObjectNameList.size(); ++i) {
            std::cout << mObjectNameList.at(i).toStdString() << std::endl;
        }
        if(mObjectList->isHidden()) {
            mObjectList->show();
            mActivateButton->setEnabled(true);
        }
    }
}

void PositionPlotter::removeSelectedObjects() {
    if(!mPlotterActive) {
        QList<QListWidgetItem*> objects = mObjectList->selectedItems();
        while(objects.size() > 0) {
            QString name = objects.at(0)->text();
            mObjectNameList.removeAll(name);
            mObjectList->removeItemWidget(objects.at(0));
            mObjectList->update();
            QListWidgetItem *tmpItem = objects.at(0);
            objects.pop_front();
            delete tmpItem;
        }
        if(mObjectList->count() == 0) {
            mActivateButton->setDisabled(true);
            mObjectList->hide();
        }
        std::cout << "Objects in internal list:" << std::endl;
        for(int i=0; i<mObjectNameList.size(); ++i) {
            std::cout << mObjectNameList.at(i).toStdString() << std::endl;
        }
    }
}

void PositionPlotter::plotterActivateAction() {
    QString names = mObjectNameList.join(",");
    mPlotterActive = true;
    mRemoveObject->setDisabled(true);
    mAddObject->setDisabled(true);
    mActivateButton->setDisabled(true);
    mDeactivateButton->setEnabled(true);
    emit activatePlotter(names);
}

void PositionPlotter::plotterDeactivateAction() {
    emit deactivatePlotter();
    mAddObject->setEnabled(true);
    mRemoveObject->setEnabled(true);
    mDeactivateButton->setDisabled(true);
    mActivateButton->setEnabled(true);
    mPlotterActive = false;
}

void PositionPlotter::showWindow() {
	if(isHidden()) {
		show();
		fillComboBox();
	}
	else {
		mPosX = x();
		mPosY = y();
		hide();
	}
}

void PositionPlotter::fillComboBox() {
	PhysicsManager *pManager = Physics::getPhysicsManager();
	mObjectSelectionBox->clear();
	//mObjectSelectionBox->addItem("None");
	QList<SimBody*> bodies = pManager->getSimBodies();
	for(int i = 0; i < bodies.size(); i++) {
		QString name = bodies.at(i)->getAbsoluteName();
        mObjectSelectionBox->addItem(name);
	}
}

}
