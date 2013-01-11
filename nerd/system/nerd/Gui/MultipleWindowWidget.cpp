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



#include "MultipleWindowWidget.h"
#include <QHBoxLayout>
#include <QAction>
#include <iostream>

using namespace std;

namespace nerd {

MultipleWindowWidget::MultipleWindowWidget(const QString &name, QWidget *parent)
	: PositionMemoryWidget(name, parent)
{
	mAddTabButton = new QPushButton("+");
	mAddTabButton->setWhatsThis("Add a new tag to the dialog.");
	mRemoveTabButton = new QPushButton("-");
	mRemoveTabButton->setWhatsThis("Removes a tab from the dialog.");
	mRemoveTabButton->setEnabled(false);

	mAddTabButton->setFixedWidth(30);
	mRemoveTabButton->setFixedWidth(30);

	mTabWidget = new QTabWidget();

	QWidget *mainWidget = centralWidget();
	QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
	mainWidget->setLayout(mainLayout);

	QHBoxLayout *upperLayout = new QHBoxLayout();
	upperLayout->setMargin(1);
	mainLayout->addLayout(upperLayout);
	mainLayout->setAlignment(upperLayout, Qt::AlignTop);

	upperLayout->addStretch(1000);
	upperLayout->addWidget(mAddTabButton);
	upperLayout->addWidget(mRemoveTabButton);

	mainLayout->addWidget(mTabWidget);

	connect(mAddTabButton, SIGNAL(clicked()),
			this, SLOT(addButtonPressed()));
	connect(mRemoveTabButton, SIGNAL(clicked()),
			this, SLOT(removeButtonPressed()));
}


MultipleWindowWidget::~MultipleWindowWidget() {
	while(mTabWidget->count() > 0) {
		QWidget *current = mTabWidget->currentWidget();
		mTabWidget->removeTab(mTabWidget->currentIndex());
		if(current != 0) {
			delete current;
		}
	}
}


void MultipleWindowWidget::addButtonPressed() {
	addTab();
	updateButtons();
}

void MultipleWindowWidget::updateButtons() {
	if(mTabWidget->count() > 0) {
		mTabWidget->setCurrentIndex(mTabWidget->count() - 1);
	}
	if(mTabWidget->count() > 1) {
		mRemoveTabButton->setEnabled(true);
	}
}


void MultipleWindowWidget::removeButtonPressed() {
	if(mTabWidget->count() > 1) {
		QWidget *current = mTabWidget->currentWidget();
		mTabWidget->removeTab(mTabWidget->currentIndex());
		if(current != 0) {
			delete current;
		}
	}
	if(mTabWidget->count() <= 1) {
		mRemoveTabButton->setEnabled(false);
	}
}

QTabWidget* MultipleWindowWidget::getTabWidget() {
	return mTabWidget;
}


}


