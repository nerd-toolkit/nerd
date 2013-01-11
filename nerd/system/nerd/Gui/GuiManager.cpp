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



#include "GuiManager.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "NerdConstants.h"
#include <QCoreApplication>

using namespace std;

namespace nerd {

GuiManager *GuiManager::mGlobalGuiManager = 0;


/**
 * Constructs a new GuiManager.
 */
GuiManager::GuiManager()
	: mWidgetSettings(0)
{
	if(!Core::getInstance()->isUsingReducedFileWriting()) {
		mWidgetSettings = new QSettings("NERD-Toolkit", QCoreApplication::instance()->applicationName());
	}
}


/**
 * Copy constructor. 
 * 
 * @param other the GuiManager object to copy.
 */
GuiManager::GuiManager(const GuiManager&) {
}

/**
 * Destructor.
 */
GuiManager::~GuiManager() {
	mGlobalGuiManager = 0;
}

QString GuiManager::getName() const {
	return "GuiManager";
}


bool GuiManager::init() {
	return true;
}


bool GuiManager::bind() {
	return true;
}


bool GuiManager::cleanUp() {
	return true;
}


GuiManager* GuiManager::getGlobalGuiManager() {
	if(mGlobalGuiManager == 0) {
		GuiManager *gm = new GuiManager();
		if(gm->registerAsGlobalObject()) {
			mGlobalGuiManager = gm;
		}
	}
	return mGlobalGuiManager;
}

bool GuiManager::registerAsGlobalObject() {
	Core::getInstance()->addGlobalObject(
		NerdConstants::OBJECT_GUI_MANAGER, this);

	return Core::getInstance()->getGlobalObject(
		NerdConstants::OBJECT_GUI_MANAGER) == this;
}

bool GuiManager::addWidget(const QString &name, QWidget *widget) {
	if(mWidgets.keys().contains(name)) {
		return false;
	}
	mWidgets.insert(name, widget);
	return true;
}


bool GuiManager::removeWidget(QWidget *widget) {
	QList<QString> keys = mWidgets.keys();
	for(QListIterator<QString> i(keys); i.hasNext();) {
		QString name = i.next();
		if(mWidgets.value(name) == widget) {
			mWidgets.remove(name);
		}
	}
	return true;
}


bool GuiManager::removeWidget(const QString &name) {
	if(mWidgets.keys().contains(name)) {
		mWidgets.remove(name);
		return true;
	}
	return false;
}

QWidget* GuiManager::getWidget(const QString &name) const {
	return mWidgets.value(name);
}


/**
 * Returns the globale QSettings object of this application.
 * If no GUI is used or if file access should be minimized (--disableLogging),
 * then this method returns 0.
 */
QSettings* GuiManager::getWidgetSettings() const {
	return mWidgetSettings;
}


}



