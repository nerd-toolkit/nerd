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



#include "Properties.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QFileInfo>
#include <QDir>
#include <QListIterator>
#include "Core/PropertyChangedListener.h"

using namespace std;

namespace nerd {


/**
 * Constructor.
 */
Properties::Properties()
{
}


/**
 * Copy constructor. Creates a new Properties object with the same properties
 * as the original.
 * 
 * @param other the Properties object to copy.
 */
Properties::Properties(const Properties &other) {
	QList<QString> names = other.mProperties.keys();
	for(QListIterator<QString> i(names); i.hasNext();) {
		QString propName = i.next();
		mProperties.insert(propName, other.mProperties.value(propName));
	}
}

/**
 * Destructor.
 */
Properties::~Properties() {
}


/**
 * Sets the property with the given name to the new specified value.
 * If there is already a property with that name, its value is replaced
 * with the new value. If there is no such property, a new property with
 * the given name is created.
 *
 * @param key the property name whose content should be set.
 * @param value the new value of the property.
 */
void Properties::setProperty(const QString &name, const QString &value) {
	mProperties.insert(name, value);

	for(QListIterator<PropertyChangedListener*> i(mListeners); i.hasNext();) {
		i.next()->propertyChanged(this, name);
	}
}


/**
 * Returns the value of the property with the given name.
 * If no such key exists, an empty string is returned. Use hasProperty() to
 * check whether a property exists.
 *
 * @param key the name of the property to read.
 * @return the value of the property if it exists, otherwise the empty string.
 */
QString Properties::getProperty(const QString &name) const {
	if(mProperties.contains(name)) {
		return mProperties.value(name);
	}
	return "";
}


/**
 * Return ture if this Properties list contains a property with the given name.
 *
 * @param the name of the property.
 * @return true if the property exists, otherwise false.
 */
bool Properties::hasProperty(const QString &name) const {
	return mProperties.contains(name);
}


/**
 * Removes a property from the property list.
 * 
 * @param name the name of the property.
 */
void Properties::removeProperty(const QString &name) {
	mProperties.remove(name);

	for(QListIterator<PropertyChangedListener*> i(mListeners); i.hasNext();) {
		i.next()->propertyChanged(this, name);
	}
}


/**
 * Returns a list with all property names that exist in this Properties list.
 *
 * @return a list with all property names.
 */
const QList<QString> Properties::getPropertyNames() const {
	return mProperties.keys();
}


/**
 * Returns list with all properties as QString, containing all properties and their
 * values in a simple list format. Hereby each line starts with the name of the
 * property, followed by an <b>=</b> and the value of the property.
 *
 * @return a string containing a list of all properties and their values.
 */
QString Properties::getPropertyList() const {

	QString output;

	QList<QString> keys = mProperties.keys();
	for(int i = 0; i < keys.size(); ++i) {
		QString key = keys.at(i);
		output.append(key).append("=").append(mProperties.value(key)).append("\n");
	}
	return output;
}


/**
 * Saves the entire Properties list to a file, specified with fileName.
 * If the path for this file is not available this method tries to
 * create all required directories to fit the directory of the fileName
 * before the file is saved.
 *
 * If there is already a file with the given name, then it will be overwritten
 * without any notification.
 *
 * @param fileName the absolute or relative file name to store the properties to.
 * @return true if the list could be written successfully, otherwise false.
 */
bool Properties::saveToFile(const QString &fileName) {
	QFile file(fileName);

	if(!file.exists()) {
		QFileInfo info(file);
		QDir::current().mkpath(info.absolutePath());
	}

	if(file.open(QFile::WriteOnly | QFile::Truncate)) {
		QTextStream output(&file);
		output << getPropertyList();
		file.close();
		return true;
	}
	else {
		file.close();
		return false;
	}
}


/**
 * Loads a property list from a file.
 * Hereby existing properties are NOT deleted. However if there are
 * properties read from the file that already exist in the Properties list,
 * then the content of these properties will be replaced by the new values.
 *
 * This method assumes the content of the file to be in the format created by
 * getPropertyList() (or saveToFile()). Other content will not lead to
 * serious errors, but the content of the property list will be undefined.
 *
 * @param fileName the absolte or relative file name to read the properties from.
 * @return true if successful, otherwise false.
 */
bool Properties::loadFromFile(const QString &fileName) {
	QFile file(fileName);

	if(file.open(QFile::ReadOnly)) {
		QTextStream input(&file);
		while (!input.atEnd()) {
			QString line = input.readLine();

			int separatorIndex = line.indexOf("=");
			if(separatorIndex != -1) {
				setProperty(line.mid(0, separatorIndex),
								line.mid(separatorIndex + 1));
			}
		}
		file.close();
		return true;
	}
	else {
		file.close();
		return false;
	}
}

bool Properties::addPropertyChangedListener(PropertyChangedListener *listener) {
	if(listener == 0 || mListeners.contains(listener)) {
		return false;
	}
	mListeners.append(listener);
	return true;
}


bool Properties::removePropertyChangedListener(PropertyChangedListener *listener) {
	if(listener == 0 || !mListeners.contains(listener)) {
		return false;
	}
	mListeners.removeAll(listener);
	return true;
}


/**
 * Returns true if both Properties are equal, thus have the same properties set.
 */
bool Properties::equals(Properties *properties) const {
	if(properties == 0) {
		return false;
	}
	
	QList<QString> propertyNames = getPropertyNames();
	QList<QString> otherPropertyNames = properties->getPropertyNames();

	if(propertyNames.size() != otherPropertyNames.size()) {
		return false;
	}
	for(QListIterator<QString> i(propertyNames); i.hasNext();) {
		QString name = i.next();

		if(properties->hasProperty(name) == false) {
			return false;
		}
		if(getProperty(name) != properties->getProperty(name)) {
			return false;
		}
	}
	return true;
}



}


