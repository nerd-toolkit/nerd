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


#include "httpwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextEdit>
#include <stdio.h>
#include "authdialog.h"

HttpWindow::HttpWindow(QWidget *) {

	// Widgets for selecting the file to upload
	QGroupBox *textBox = new QGroupBox("Text to send");
	mTextEdit = new QLineEdit("valuefile=/net/genome/work/output_test_php/testval.val&/ErstesValue/Value=127&/ZweitesValue=Hallo");
	mTextLabel = new QLabel("Text:");
	mTextLabel->setBuddy(mTextEdit);

	QHBoxLayout *textLayout = new QHBoxLayout();
	textLayout->addWidget(mTextLabel);
	textLayout->addWidget(mTextEdit);
	textBox->setLayout(textLayout);

	// Widgets for selecting the URL of the host
	QGroupBox *urlBox = new QGroupBox("Url of host");
//	mUrlEdit = new QLineEdit("http://localhost/~ferry/post_values.php");
	mUrlEdit = new QLineEdit("http://www.ikw.uni-osnabrueck.de/~neurokybernetik/protected/post_values.php");
	mUrlLabel = new QLabel("URL:");
	mUrlLabel->setBuddy(mUrlEdit);
	
	QHBoxLayout *urlLayout = new QHBoxLayout();
	urlLayout->addWidget(mUrlLabel);
	urlLayout->addWidget(mUrlEdit);
	urlBox->setLayout(urlLayout);
	
	// Widget for uploading the file
	mSendTextButton = new QPushButton("Send Text");
	connect(mSendTextButton, SIGNAL(clicked()), this, SLOT(sendText()));
	
	// Create the main layout
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(textBox);
	mainLayout->addWidget(urlBox);
	mainLayout->addWidget(mSendTextButton);

	setLayout(mainLayout);
	
	// Create the HTTP-object and connect it
	mHttp = new QHttp(this);
	connect(mHttp, SIGNAL(authenticationRequired(const QString&, quint16, QAuthenticator*)), this,
				SLOT(authenticationRequired(const QString&, quint16, QAuthenticator*)));
	connect(mHttp, SIGNAL(done(bool)), this, SLOT(done(bool)));

	// Set width
	setMinimumWidth(700);
}

void HttpWindow::sendText() {
	mSendTextButton->setEnabled(false);
	QByteArray textData = mTextEdit->text().toAscii();

	mUrl.setUrl(mUrlEdit->text());

	QHttpRequestHeader header("POST", mUrl.path());
	header.setValue("Host", mUrl.host());
	header.setContentType("application/x-www-form-urlencoded");
	QHttp::ConnectionMode mode = mUrl.scheme().toLower() == "https" ? QHttp::ConnectionModeHttps : QHttp::ConnectionModeHttp;

	mHttp->setHost(mUrl.host(), mode, mUrl.port() == -1 ? 0 : mUrl.port());
	mHttp->request(header, textData);
}

void HttpWindow::authenticationRequired(const QString &, quint16, QAuthenticator *authenticator) {

	AuthDialog dlg;
	if(dlg.exec() == QDialog::Accepted) {
		authenticator->setUser(dlg.mUserEdit->text());
		authenticator->setPassword(dlg.mPasswordEdit->text());
	}
}

void HttpWindow::done(bool error) {
	if(error) {
		QMessageBox::critical(this, "Error", mHttp->errorString());
	}
	QByteArray incomingData = mHttp->readAll();
	if(!incomingData.isEmpty()) {
		QMessageBox::critical(this, "Error", QString(incomingData));
	}
	mSendTextButton->setEnabled(true);
}
