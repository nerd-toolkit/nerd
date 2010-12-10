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

#ifndef NERDHTTPValueAccess_H
#define NERDHTTPValueAccess_H

#include "Value/Value.h"
#include <QWidget>
#include <QUrl>
#include <QString>
#include <QList>
#include <QHttpRequestHeader>
#include <QAuthenticator>
#include <QMutex>
#include <QWaitCondition>
#include <QProgressDialog>

namespace nerd {

	/**
	 * HTTPValueAccess
	 */
	class HTTPValueAccess : public QObject {
	Q_OBJECT

	public:
		HTTPValueAccess(QWidget *parent);
		virtual ~HTTPValueAccess();

		void getValues(QUrl scriptUrl, QString valueFilePath, QString valueTypeFilePath, 
						const QList<QString> *valueNames = 0);
		void postValues(QUrl scriptUrl, QString valueFilePath, const QMap<QString, 
						QString> *valueNamesAndContents);

	private:
		enum State {
			None,
			GetValueRequestStarted,
			GetValueTypeRequestStarted,
			PostValueRequestStarted
		};

	private:
		State mState;
		QWidget *mParent;
		QHttpRequestHeader mHeader;
		QHttp *mHttp;
		QByteArray mRequestValueString;
		QByteArray mGetValueData;
		QByteArray mGetValueTypeData;
		QString mValueFilePath;
		QString mValueTypeFilePath;

	private:
		void interpretGetValuesData();
		void getValuesError(QString mErrorString);
		void postValuesError(QString mErrorString);

	private slots:
		void authenticationRequired(const QString &hostname, quint16 port, QAuthenticator *authenticator); 
		void done(bool error);

	signals:
		void getValuesFinished(bool error);
		void postValuesFinished(bool error);
	};

}

#endif
