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
#include "ImageExporter.h"
#include "Core/Core.h"
#include <QFileDialog>
#include <QErrorMessage>
#include <QHBoxLayout>
#include "Gui/Visualization/OpenGLVisualization.h"
#include "Gui/GuiManager.h"

namespace nerd{

ImageExporter::ImageExporter() {
	setWindowTitle("Image Exporter");
	mPosX = 0;
	mPosY = 0;

	GuiManager::getGlobalGuiManager()->addWidget("ImageExporter", this);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);

	mWindowLayout = new QGridLayout();
	setLayout(mWindowLayout);

	mSizeBox = new QGroupBox(tr("Custom Image Size"));
	mSizeBox->setCheckable(true);
	mSizeBox->setChecked(false);
    QHBoxLayout *hbox = new QHBoxLayout();
    QLabel *widthLabel = new QLabel("Width");
    mWidthEdit = new QLineEdit("0");
    mWidthEdit->setInputMask("D9999");
    //mWidthEdit->setFixedWidth(5);
    QLabel *heightLabel = new QLabel("Height");
    mHeightEdit = new QLineEdit("0");
    mHeightEdit->setInputMask("D9999");
    //mHeightEdit->setFixedWidth(5);
    hbox->addWidget(widthLabel);
    hbox->addWidget(mWidthEdit);
    hbox->addWidget(heightLabel);
    hbox->addWidget(mHeightEdit);
    hbox->addStretch(1);
    hbox->setContentsMargins(5,5,5,5);
    mSizeBox->setLayout(hbox);

    mHideTimeBox = new QCheckBox("Hide current simulation time information on image");
    mHideTimeBox->setChecked(true);

	mSaveButton = new QPushButton("Save image");

    mWindowLayout->setContentsMargins(11, 11, 11, 11);
    mWindowLayout->addWidget(mSizeBox);
    mWindowLayout->addWidget(mHideTimeBox);
	mWindowLayout->addWidget(mSaveButton);
	connect(mSaveButton, SIGNAL(clicked()),
            this, SLOT(chooseFile()));

}

ImageExporter::~ImageExporter() {
}

void ImageExporter::chooseFile() {
    mFileName = QFileDialog::getSaveFileName(this,
                        tr("Export Image"),
                        QString(),
                        tr("All Files (*)"));
    if(!mFileName.isEmpty()) {
        QString extension = ".png";
        if(!mFileName.endsWith(extension)) {
            mFileName.append(extension);
        }
        int w, h = 0;
        if(mSizeBox->isChecked()) {
            w = mWidthEdit->text().toInt();
            h = mHeightEdit->text().toInt();
        }
        emit exportImage(mFileName, w, h, mHideTimeBox->isChecked());
    }
}

void ImageExporter::imageExported(bool success) {
    QErrorMessage *message = new QErrorMessage(this);
    if(success) {
        message->showMessage(QString("Image successfully exported to file ").
                                    append(mFileName), "success");
    } else {
        message->showMessage("Image could not be saved.", "fail");
    }
}


void ImageExporter::showWindow() {
	if(isHidden()) {
		show();
	}
	else {
		mPosX = x();
		mPosY = y();
		hide();
	}
}

}
