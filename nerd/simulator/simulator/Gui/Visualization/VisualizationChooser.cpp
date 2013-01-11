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



#include "VisualizationChooser.h"
#include "Core/Core.h"
#include "Physics/PhysicsManager.h"
#include "Physics/SimBody.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include "Gui/Visualization/OpenGLVisualization.h"
#include "Physics/Physics.h"
#include "Gui/GuiManager.h"

namespace nerd{

VisualizationChooser::VisualizationChooser() {
	setWindowTitle("Camera Manager");
	mPosX = 0;
	mPosY = 0;
	
	GuiManager::getGlobalGuiManager()->addWidget("CameraManager", this);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	
	mWindowLayout = new QGridLayout();
	mWindowLayout->setMargin(1);
	setLayout(mWindowLayout);
	
	QGroupBox *cameraMountBox = new QGroupBox();
	mWindowLayout->addWidget(cameraMountBox, 0,0);
	mObjectSelectionBox = new QComboBox();
	cameraMountBox->setTitle("Camera Mount");
	cameraMountBox->setWhatsThis("This is the physical body where the "
			"camera should be attached to. If the camera is controllable, "
			"then it can be moved relative to the mount point. "
			"This allows the attachment of a camera to an object, so that it "
			"automatically follows the object.");
	QHBoxLayout *cameraMountBoxLayout = new QHBoxLayout();
	cameraMountBox->setLayout(cameraMountBoxLayout);
	cameraMountBoxLayout->addWidget(mObjectSelectionBox);
	

	QGroupBox *position = new QGroupBox();
	position->setTitle("Position");
	position->setWhatsThis("The position offset that will be added to "
			"the position of the reference body.");
	QHBoxLayout *positionLayout = new QHBoxLayout();
	position->setLayout(positionLayout);	
	positionLayout->setMargin(1);
	mPositionTextX = new QLineEdit("0.0");
	mPositionTextY = new QLineEdit("0.0");
	mPositionTextZ = new QLineEdit("0.0");
	
	positionLayout->addWidget(mPositionTextX);
	positionLayout->addWidget(mPositionTextY);
	positionLayout->addWidget(mPositionTextZ);
	
	mWindowLayout->addWidget(position, 1, 0	);

	QGroupBox *orientation = new QGroupBox();
	orientation->setTitle("Orientation");
	orientation->setWhatsThis("The orientation offset that will be added to "
			"the orientation of the reference body. "
			"The orientation is described in degree.");
	QHBoxLayout *orientationLayout = new QHBoxLayout();
	orientation->setLayout(orientationLayout);	
	orientationLayout->setMargin(1);

	mOrientationTextX = new QLineEdit("0.0");
	mOrientationTextY = new QLineEdit("180.0");
	mOrientationTextZ = new QLineEdit("0.0");
	
	orientationLayout->addWidget(mOrientationTextX);
	orientationLayout->addWidget(mOrientationTextY);
	orientationLayout->addWidget(mOrientationTextZ);
	
	mWindowLayout->addWidget(orientation, 2, 0);

	QGroupBox *cameraSettings = new QGroupBox();
	cameraSettings->setTitle("Camera Settings");
	QHBoxLayout *cameraSettingsLayout = new QHBoxLayout();
	cameraSettings->setLayout(cameraSettingsLayout);
	cameraSettingsLayout->setMargin(1);

	mOpeningAngle = new QLineEdit(QString::number(OpenGLVisualization::getDefaultOpeningAngle()));
	mMinCutoff = new QLineEdit(QString::number(OpenGLVisualization::getDefaultMinCutoff()));
	mMaxCutoff = new QLineEdit(QString::number(OpenGLVisualization::getDefaultMaxCutoff()));
	mOpeningAngle->setToolTip("Opening angle of the camera.");
	mMinCutoff->setToolTip("Minimum cutoff border of the camera.");
	mMaxCutoff->setToolTip("Maximum cutoff border of the camera.");

	cameraSettingsLayout->addWidget(mOpeningAngle);
	cameraSettingsLayout->addWidget(mMinCutoff);
	cameraSettingsLayout->addWidget(mMaxCutoff);

	mWindowLayout->addWidget(cameraSettings, 3, 0	);
	
	QWidget *options = new QWidget();
	QHBoxLayout *optionsLayout = new QHBoxLayout();
	options->setLayout(optionsLayout);
	optionsLayout->setMargin(1);
	
	QGroupBox *controllableCheckBoxNamer = new QGroupBox();
	controllableCheckBoxNamer->setTitle("Controllable");
	mIsManipulatable = new QCheckBox();
	mIsManipulatable->setChecked(true);
	mCreateVisualization = new QPushButton("Create Visualization");
	QHBoxLayout *controlCheckBoxLayout = new QHBoxLayout();
	controllableCheckBoxNamer->setLayout(controlCheckBoxLayout);
	controlCheckBoxLayout->addWidget(mIsManipulatable);
	controlCheckBoxLayout->setMargin(1);

	
	QGroupBox *checkBoxNamer = new QGroupBox();
	checkBoxNamer->setTitle("Translational");
	mIsTranslational = new QCheckBox();
	mIsTranslational->setChecked(false);
	QHBoxLayout *checkBoxLayout = new QHBoxLayout();
	checkBoxNamer->setLayout(checkBoxLayout);
	checkBoxLayout->setMargin(1);
	checkBoxLayout->addWidget(mIsTranslational);

	mVisualizationName = new QLineEdit("");
	QGroupBox *nameBox = new QGroupBox();
	nameBox->setTitle("Name");
	QHBoxLayout *nameBoxLayout = new QHBoxLayout();
	nameBox->setLayout(nameBoxLayout);
	nameBoxLayout->setMargin(1);
	nameBoxLayout->addWidget(mVisualizationName);
	
	optionsLayout->addWidget(controllableCheckBoxNamer);
	optionsLayout->addWidget(checkBoxNamer);
	optionsLayout->addWidget(nameBox);

	mWindowLayout->addWidget(options);

	QGroupBox *activeVisualizations = new QGroupBox();
	activeVisualizations->setTitle("Active Visualizations");
	mVisualizationList = new QListWidget(this);
	mVisualizationList->setSelectionMode(QAbstractItemView::MultiSelection);
	QHBoxLayout *activeVisualizationsLayout = new QHBoxLayout();
	activeVisualizations->setLayout(activeVisualizationsLayout);
	activeVisualizationsLayout->setMargin(1);	
	activeVisualizationsLayout->addWidget(mVisualizationList);

	mWindowLayout->addWidget(activeVisualizations);
	mVisualizationList->hide();
	mVisualizationList->setWhatsThis("List of all active visualization. \n"
						"Double click: hide/show visualization. \nSingle click: "
						"Bring visualization to the front.");

	mRemoveVisualization = new QPushButton("Remove Visualizations");
	
	QHBoxLayout *buttonLayout = new QHBoxLayout();
	mWindowLayout->addLayout(buttonLayout, 6, 0);
	buttonLayout->setMargin(1);
	buttonLayout->addWidget(mRemoveVisualization);
	buttonLayout->setAlignment(mRemoveVisualization, Qt::AlignLeft);
	buttonLayout->addWidget(mCreateVisualization);
	buttonLayout->setAlignment(mCreateVisualization, Qt::AlignRight);
	
	connect(mRemoveVisualization, SIGNAL(clicked()), 
			this, SLOT(removeSelectedVisualization()));

	connect(mVisualizationList, SIGNAL(itemClicked(QListWidgetItem*)), this,
 			SLOT(moveVisualizationToFront(QListWidgetItem*)));
	connect(mVisualizationList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), 
			this, SLOT(changeVisualizationVisibility(QListWidgetItem*)));

	connect(mCreateVisualization, SIGNAL(clicked()), 
			this, SLOT(createNewVisualization()));
	connect(mCreateVisualization, SIGNAL(clicked()), 
			this, SLOT(clearLineEditFields()));
}

VisualizationChooser::~VisualizationChooser() {
  while (!mActiveVisualizations.empty()) {
		OpenGLVisualization *tmp = mActiveVisualizations.begin().value();
		mActiveVisualizations.erase(mActiveVisualizations.begin());
		delete tmp;
   }
}

void VisualizationChooser::showWindow() {
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

void VisualizationChooser::fillComboBox() {
	PhysicsManager *pManager = Physics::getPhysicsManager();
	mObjectSelectionBox->clear();
	mObjectSelectionBox->addItem("None");
	QList<SimBody*> bodies = pManager->getSimBodies();
	for(int i = 0; i < bodies.size(); i++) {
		mObjectSelectionBox->addItem(bodies.at(i)->getAbsoluteName());
	}
}

void VisualizationChooser::createNewVisualization() {
	QString objectName = mObjectSelectionBox->currentText();
	QString position = "(";
	position.append(mPositionTextX->text()).append(",").append(mPositionTextY->text())
			.append(",").append(mPositionTextZ->text()).append(")");
	QString orientation = "(";
	orientation.append(mOrientationTextX->text()).append(",").append(mOrientationTextY->text())
			.append(",").append(mOrientationTextZ->text()).append(")");

	bool isManipulatable = mIsManipulatable->checkState();

	SimBody *referenceBody;
	if(objectName.compare("None") == 0) {
		referenceBody = 0;
	}
	else {
		referenceBody = Physics::getPhysicsManager()->getSimBody(objectName);
		
		if(referenceBody == 0) {
			Core::log("VisualizationChooser: Could not get reference body with name [" + objectName + "]!", true);
		}
	}	
	QString title;
	if(mVisualizationName->text().compare("") == 0) {
		title = "";
		QString bodyName;
		if(referenceBody != 0) {
			title.append(referenceBody->getName());
		}
		else {
			title.append("None");
		}
		title.append("  - Camera");
	}
	else {
		title = mVisualizationName->text();
		title.append(" ");
	}

	if(mActiveVisualizations.contains(title)) {
		QMap<QString, OpenGLVisualization*>::iterator i = mActiveVisualizations.end()-1;
 		while(i != mActiveVisualizations.begin()) {
     if(i.key().startsWith(title)) {	
				if(i.key().compare(title) == 0) {
					title.append(" " + QString::number(1));
				}else{
					QString listEntry = i.key();
					QString number = listEntry.remove(title);
					int index = number.toInt();
					title.append(" " + QString::number(index + 1));
				}
			}
		--i;
		}
		if(i.key().startsWith(title)) {	
				if(i.key().compare(title) == 0) {
					title.append(" " + QString::number(1));
				}else{
					QString listEntry = i.key();
					QString number = listEntry.remove(title);
					int index = number.toInt();
					title.append(" " + QString::number(index + 1));
				}
			}
	}

	OpenGLVisualization *newVisualization = new OpenGLVisualization(isManipulatable,
					referenceBody, title, false);
	dynamic_cast<Vector3DValue*>(newVisualization->getParameter("StartPosition"))
			->setValueFromString(position);
	dynamic_cast<Vector3DValue*>(newVisualization->getParameter("StartOrientation"))
			->setValueFromString(orientation);
	dynamic_cast<BoolValue*>(newVisualization->getParameter("Translational"))
			->set(mIsTranslational->checkState());

	double openingAngle = mOpeningAngle->text().toDouble();
	double minCut = mMinCutoff->text().toDouble();
	double maxCut = mMaxCutoff->text().toDouble();

	if(openingAngle >= 0 && openingAngle <= 180) {
		dynamic_cast<DoubleValue*>(newVisualization->getParameter("OpeningAngle"))
			->setValueFromString(mOpeningAngle->text());
	}
	if(minCut < 0) {	
		minCut = dynamic_cast<DoubleValue*>(newVisualization->getParameter("MinCutoff"))->getValueAsString().toDouble();
	}

	if(maxCut < 0) {	
		maxCut = dynamic_cast<DoubleValue*>(newVisualization->getParameter("MaxCutoff"))->getValueAsString().toDouble();
	}
	if(minCut < maxCut) {
		dynamic_cast<DoubleValue*>(newVisualization->getParameter("MinCutoff"))
				->setValueFromString(QString::number(minCut));
	
		dynamic_cast<DoubleValue*>(newVisualization->getParameter("MaxCutoff"))
				->setValueFromString(QString::number(maxCut));			
	}
	
	mMaxCutoff->setText(dynamic_cast<DoubleValue*>(newVisualization->getParameter("MaxCutoff"))->getValueAsString());
	mMinCutoff->setText(dynamic_cast<DoubleValue*>(newVisualization->getParameter("MinCutoff"))->getValueAsString());
	
	mOpeningAngle->setText(dynamic_cast<DoubleValue*>(newVisualization->getParameter("OpeningAngle"))->getValueAsString());

	newVisualization->resetViewport();
	newVisualization->show();

	QListWidgetItem *item = new QListWidgetItem(title);
	mVisualizationList->addItem(item);

	mActiveVisualizations[title] = newVisualization;	

	if(mVisualizationList->isHidden()){
		mVisualizationList->show();
	}
}

void VisualizationChooser::changeVisualizationVisibility(QListWidgetItem *item) {
	QString name = item->text();
	OpenGLVisualization *tmp = mActiveVisualizations.value(name);
	if(tmp != 0) {
		tmp->changeVisibility();
	}
}

void VisualizationChooser::moveVisualizationToFront(QListWidgetItem *item) {
QString name = item->text();
	OpenGLVisualization *tmp = mActiveVisualizations.value(name);
	if(tmp != 0) {
		tmp->raise();
	}
}

void VisualizationChooser::clearLineEditFields() {
// 	mOrientationTextX->setText("0.0");
// 	mOrientationTextY->setText("0.0");
// 	mOrientationTextZ->setText("0.0");
// 	mPositionTextX->setText("0.0");
// 	mPositionTextY->setText("180.0");
// 	mPositionTextZ->setText("0.0");

	mVisualizationName->setText("");
// 	mIsManipulatable->setChecked
}

void VisualizationChooser::removeSelectedVisualization() {
// 	QListWidgetItem *current = mVisualizationList->currentItem();
// 	if(current != 0) {
// 		QString name = current->text();
// 		OpenGLVisualization *tmp = mActiveVisualizations.value(name);
// 		mActiveVisualizations.remove(name);
// 		mVisualizationList->removeItemWidget(current);
// 		mVisualizationList->repaint();
// 		delete tmp;
// 		delete current;
// 	}

	QList<QListWidgetItem*> currentSelection = mVisualizationList->selectedItems();
	while(currentSelection.size() > 0) {
		QString name = currentSelection.at(0)->text();
		OpenGLVisualization *tmp = mActiveVisualizations.value(name);
		mActiveVisualizations.remove(name);
		mVisualizationList->removeItemWidget(currentSelection.at(0));
		//mVisualizationList->repaint();
		mVisualizationList->update();
		tmp->unpublishAllParameters();
		delete tmp;
		QListWidgetItem *tmpItem = currentSelection.at(0);
		currentSelection.pop_front();
		delete tmpItem;
	}
	if(mVisualizationList->count() == 0) {
		mVisualizationList->hide();
	}

}

}
