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



#include "HtmlInfoWidget.h"
#include <QVBoxLayout>
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>

namespace nerd {


/**
 * Construcor of the HtmlInfoWidget.
 *
 * @param documentFileName the name of the file to be loaded when the window is shown.
 * @param actionName the name of the associated QAction object.
 * @param parent a parent widget. If this is 0 then HtmlInfoWidget is shown in its own window.
 */
HtmlInfoWidget::HtmlInfoWidget(const QString &documentFileName, const QString &actionName,
				QWidget *parent)
	: QWidget(parent), mDocumentFileName(documentFileName), mDocumentBrowser(0)
{
	//initialize widget properties for the case where this widget is used as window.
	setWindowTitle("Simulator Short Documentation");
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	resize(600,400);

	//create html text browser
	mDocumentBrowser = new QTextBrowser(this);
	mDocumentBrowser->setReadOnly(true);

	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);

	layout->addWidget(mDocumentBrowser);

	//create activation QAction.
	mActivationAction = new QAction(actionName, this);
	connect(mActivationAction, SIGNAL(triggered()),
			this, SLOT(showInfoWindow()));
}


/**
 * Does nothing.
 */
HtmlInfoWidget::~HtmlInfoWidget() {
}


/**
 * Returns the QAction object that updates and shows the HTML document
 * when triggered.
 *
 * @return the activation action.
 */
QAction* HtmlInfoWidget::getAction() const {
	return mActivationAction;
}


/**
 * This slot updates the document and shows the HTML window.
 */
void HtmlInfoWidget::showInfoWindow() {
	updateDocument();
	show();
}


/**
 * Loads the document specified by name in the constructor and displays it
 * in the text browser area.
 * If the specified file can not be found, then a failure message will be
 * shown instead of the HTML text.
 */
void HtmlInfoWidget::updateDocument() {

	QString documentText = "";
	QFile docFile(mDocumentFileName);

	if(!docFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		documentText.append("Could not find text file [").append(mDocumentFileName)
				.append("]!");
	}
	else {
		QTextStream in(&docFile);
		QString nextLine = in.readAll();
		documentText.append(nextLine);
	}
	mDocumentBrowser->setText(documentText);
	docFile.close();
}


}





