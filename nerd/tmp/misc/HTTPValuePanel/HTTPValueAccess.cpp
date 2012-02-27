/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   HTTPValuePanel by Ferry Bachmann (bachmann@informatik.hu-berlin.de)   *
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

#include "HTTPValueAccess.h"
#include "HTTPAuthDialog.h"
#include "HTTPValuePanel.h"
#include <QMessageBox>
#include <QDebug>
#include "Core/Core.h"
#include "Value/ValueManager.h"

namespace nerd {

HTTPValueAccess::HTTPValueAccess(QWidget *parent)
	: mState(None), mParent(parent)
{
	mHttp = new QHttp(parent);

	connect(mHttp, SIGNAL(authenticationRequired(const QString&, quint16, QAuthenticator*)), this,
				SLOT(authenticationRequired(const QString&, quint16, QAuthenticator*)));
	connect(mHttp, SIGNAL(done(bool)), this, SLOT(done(bool)));
}

HTTPValueAccess::~HTTPValueAccess() {
}

/**
 * Loads values from a valuefile on a server with the help of a serverscript.
 * The same script is used twice. First to get the values and their contents
 * as strings from the valuefile. Second to get the type and properties of
 * the values as strings from the valuetypefile.
 *
 * All values loaded that are not already added to the ValueManager are added
 * to the ValueManager with a prefix to mark them as remote values and to
 * avoid conflicts with local values.
 * All values requested are expected to have this prefix and the prefix is cut
 * of the value names before sending the request.
 *
 * If a loaded value is already part of the ValueManager, its content is updated.
 *
 * The function is asynchronous and returns immediately. When the request is
 * finished, the requestFinished signal is emitted.
 *
 * If there is some error or warning, the function shows a message box. In case
 * of an error, the error paramater of the requestFinished signal is true.
 *
 * @param scriptUrl The URL of the serverscript.
 * @param valueFilePath The path of the valuefile on the server.
 * @param valueTypeFilePath The path of the valuetypefile on the server.
 * @param valueNames A pointer to the list of the valuenames to load. If the pointer is 0, all available values are loaded from the valuefile. If the list is empty, the function does nothing. If the list is non-empty, the values with the names from this list are loaded and their contents are updated.
 */
void HTTPValueAccess::getValues(QUrl scriptUrl, QString valueFilePath, QString valueTypeFilePath, 
								const QList<QString> *valueNames) 
{
	if((valueNames == 0) || !valueNames->isEmpty()) {
		mValueFilePath = valueFilePath;
		mValueTypeFilePath = valueTypeFilePath;

		// Create HTTP-Post-Header
		// Cannot use QHttp::post because of Content Type
		mHeader.setRequest("POST", scriptUrl.path());
		mHeader.setValue("Host", scriptUrl.host());
		mHeader.setContentType("application/x-www-form-urlencoded");

		// Set HTTP-Host
		QHttp::ConnectionMode mode = scriptUrl.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
		mHttp->setHost(scriptUrl.host(), mode, scriptUrl.port() == -1 ? 0 : scriptUrl.port());

		// Create valuefile-request
		QByteArray textData = QString("valuefile=%1").arg(valueFilePath).toAscii();
		mRequestValueString.clear();
		if(valueNames != 0) {
			for(int i = 0; i < valueNames->size(); ++i) {
				QString valueName = valueNames->at(i);
				// Cut of the prefix
				QString trimmedName = valueName.right(valueName.length() - HTTPValuePanel::mValuePrefix.length());
				// Append TrimmedValueName to the request string as HTTP post paramater
				mRequestValueString.append(QString("&%1=").arg(trimmedName));
			}
			textData.append(mRequestValueString);
		}

		// Start request
		mState = GetValueRequestStarted;
		mHttp->request(mHeader, textData);
	}
}

/**
 * Loads values from a valuefile on a server with the help of a serverscript.
 * The same script is used twice. First to get the values and their contents
 * as strings from the valuefile. Second to get the type and properties of
 * the values as strings from the valuetypefile.
 *
 * All values loaded that are not already added to the ValueManager are added
 * to the ValueManager with a prefix to mark them as remote values and to
 * avoid conflicts with local values.
 * All values requested are expected to have this prefix and the prefix is cut
 * of the value names before sending the request.
 *
 * If a loaded value is already part of the ValueManager, its content is updated.
 *
 * The function is asynchronous and returns immediately. When the request is
 * finished, the requestFinished signal is emitted.
 *
 * If there is some error or warning, the function shows a message box. In case
 * of an error, the error paramater of the requestFinished signal is true.
 *
 * @param scriptUrl The URL of the serverscript.
 * @param valueFilePath The path of the valuefile on the server.
 * @param valueNamesAndContents A pointer to the list of the valuenames to load. If the pointer is 0, all available values are loaded from the valuefile. If the list is empty, the function does nothing. If the list is non-empty, the values with the names from this list are loaded and their contents are updated.
 */
void HTTPValueAccess::postValues(QUrl scriptUrl, QString valueFilePath, const QMap<QString, QString> *valueNamesAndContents) {
	if((valueNamesAndContents != 0) && !valueNamesAndContents->isEmpty()) {
		mValueFilePath = valueFilePath;
		// Create HTTP-Post-Header
		// Cannot use QHttp::post because of Content Type
		mHeader.setRequest("POST", scriptUrl.path());
		mHeader.setValue("Host", scriptUrl.host());
		mHeader.setContentType("application/x-www-form-urlencoded");

		// Set HTTP-Host
		QHttp::ConnectionMode mode = scriptUrl.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;
		mHttp->setHost(scriptUrl.host(), mode, scriptUrl.port() == -1 ? 0 : scriptUrl.port());

		// Create valuefile-request
		QByteArray textData = QString("valuefile=%1").arg(valueFilePath).toAscii();
		mRequestValueString.clear();
		if(valueNamesAndContents != 0) {
			QMap<QString, QString>::const_iterator it;
			for(it = valueNamesAndContents->constBegin(); it != valueNamesAndContents->constEnd(); ++it) {
				QString valueName = it.key();
				// Cut of the prefix
				QString trimmedName = valueName.right(valueName.length() - HTTPValuePanel::mValuePrefix.length());
				QString valueContent = it.value();
				// Append TrimmedValueName and ValueContent to the request string as HTTP post paramater
				mRequestValueString.append(QString("&%1=%2").arg(trimmedName).arg(valueContent));
			}
			textData.append(mRequestValueString);
		}

		// Start request
		mState = PostValueRequestStarted;
		mHttp->request(mHeader, textData);
	}
}

void HTTPValueAccess::getValuesError(QString mErrorString) {
	QMessageBox::critical(mParent, "Error", mErrorString);
	mState = None;
	emit getValuesFinished(true);
}

void HTTPValueAccess::postValuesError(QString mErrorString) {
	QMessageBox::critical(mParent, "Error", mErrorString);
	mState = None;
	emit postValuesFinished(true);
}

void HTTPValueAccess::authenticationRequired(const QString &, quint16, QAuthenticator *authenticator) {
	HTTPAuthDialog dlg;
	if(dlg.exec() == QDialog::Accepted) {
		authenticator->setUser(dlg.mUserEdit->text());
		authenticator->setPassword(dlg.mPasswordEdit->text());
	}
}

void HTTPValueAccess::done(bool error) {
	if(error) {
		getValuesError(mHttp->errorString());
	} else {
		QHttpResponseHeader responseHeader = mHttp->lastResponse();
		if(responseHeader.statusCode() != 200) {
			getValuesError(responseHeader.reasonPhrase());
		} else {
			// Get all data received
			QByteArray incomingData = mHttp->readAll();

			switch(mState) {
				case GetValueRequestStarted:
					// If it's a Get-Request, data starting with "Error:" is an error.
					if(incomingData.startsWith("Error:")) {
						getValuesError(incomingData);
					} else {
						// Copy received data to buffer
						mGetValueData = incomingData;
						// Request the Value Types and Properties
						QByteArray textData = QString("valuefile=%1").arg(mValueTypeFilePath).toAscii();
						textData.append(mRequestValueString);
						mState = GetValueTypeRequestStarted;
						mHttp->request(mHeader, textData);
					}
					break;
				case GetValueTypeRequestStarted:
					// If it's a Get-Request, data starting with "Error:" is an error.
					if(incomingData.startsWith("Error:")) {
						getValuesError(incomingData);
					} else {
						mGetValueTypeData = incomingData;
						mState = None;
						// Interpret the received Value and Value Type data
						interpretGetValuesData();
						emit getValuesFinished(false);
					}
					break;
				case PostValueRequestStarted:
					// If it's a Post-Request, any received data is an error.
					if(!incomingData.isEmpty()) {
						postValuesError(incomingData);
					} else {
						mState = None;
						emit postValuesFinished(false);
					}
					break;
				case None:
				default: 
					//TODO (@Ferry: add a warning here?)
					break;
			}
		}
	}
}

void HTTPValueAccess::interpretGetValuesData() {
	ValueManager *vm = Core::getInstance()->getValueManager();

	QMap<QString, QString> valuesFound;
	QMap<QString, QString> valuesTypesFound;

	// Read all values with content from mGetValueData
	QTextStream getValueStream(mGetValueData);
	while(!getValueStream.atEnd()) {
		QString line = getValueStream.readLine();
		line = line.trimmed();

		if(line.startsWith("#")) {
			continue;
		}
		int sepIndex = line.indexOf("=");
		if(sepIndex == -1) {
			continue;
		}
		QString name = line.left(sepIndex);
		QString valueContent = line.right(line.length() - sepIndex - 1);
		valuesFound[name] = valueContent;
	}
	// Read all value type and properties information from mGetValueTypeData
	QTextStream getValueTypeStream(mGetValueTypeData);
	while(!getValueTypeStream.atEnd()) {
		QString line = getValueTypeStream.readLine();
		line = line.trimmed();

		if(line.startsWith("#")) {
			continue;
		}
		int sepIndex = line.indexOf("=");
		if(sepIndex == -1) {
			
			continue;
		}
		QString name = line.left(sepIndex);
		QString valueType = line.right(line.length() - sepIndex - 1);
		valuesTypesFound[name] = valueType;
	}

	// Iterate over Values and set the content if value is already
	// added to the ValueManager.
	// Otherwise add it!
	QMapIterator<QString, QString> itValues(valuesFound);
	while(itValues.hasNext()) {
		itValues.next();
		QString name = itValues.key();
		QString content = itValues.value();
		Value *value = vm->getValue(HTTPValuePanel::mValuePrefix + name);
		if(value == 0) {	// Value not added to the ValueManager yet
			// Get type and properties information
			if(!valuesTypesFound.contains(name)) {
				QMessageBox::warning(mParent, "Warning", QString("No type information found for new value \"%1\". Value ignored!").arg(name));
				continue;
			}
			QString typeInformation = valuesTypesFound[name];
			QStringList splitList = typeInformation.split("=");
			if(splitList.size() != 2) {
				QMessageBox::warning(mParent, "Warning", QString("Could not parse type and properties information of value \"%1\". Value ignored!").arg(name));
				continue;
			}
			// Create value from type information
			value = vm->createCopyOfPrototype(splitList.at(0));
			if(value == 0) {
				QMessageBox::warning(mParent, "Warning", QString("Unknown type of value \"%1\". Value ignored!").arg(name));
				continue;
			}
			// Set properties from properties information
			if(!value->setProperties(splitList.at(1))) {
				QMessageBox::warning(mParent, "Warning", QString("Failed to set properties of value \"%1\". Value ignored!").arg(name));
				delete value;
				continue;
			}
			// Set value from value content string
			if(!value->setValueFromString(content)) {
				QMessageBox::warning(mParent, "Warning", QString("Failed to set content of value \"%1\". Value ignored!").arg(name));
				delete value;
				continue;
			}
			// Add value to the ValueManager with prefix
			if(!vm->addValue(HTTPValuePanel::mValuePrefix + name, value)) {
				QMessageBox::warning(mParent, "Warning", QString("Failed to add value \"%1\" to ValueManager!").arg(name));
				delete value;
				continue;
			}
		} else {	// Value already added to the ValueManager
			// Set value from value content string
			if(!value->setValueFromString(content)) {
				QMessageBox::warning(mParent, "Warning", QString("Failed to set content of existing value \"%1\"!").arg(name));
			}
		}
	}
}



}
