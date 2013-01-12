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

#include "AboutInfoWidget.h"
#include "Core/Core.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QTextBrowser>
#include <QScrollArea>

namespace nerd{

AboutInfoWidget::AboutInfoWidget(const QString &actionName) {
	setWindowTitle("About NERD");
	mChangeVisibilityAction = new QAction(actionName, this);

	mAboutWindowTabWidget = new QTabWidget(this);

	mWindowLayout = new QVBoxLayout;
	mWindowLayout->setMargin(1);
	setLayout(mWindowLayout);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
	resize(700,400);

	QHBoxLayout *titleLayout = new QHBoxLayout();
	titleLayout->setMargin(1);

	QImage jpgImage;
	QString name = ":/resources/pics/orcsLogo.png";
	if(!jpgImage.load(name)) {
		Core::log("Could not load Window-Logo");
	}
	else{
		mOrcsIcon = QPixmap::fromImage(jpgImage);
		QIcon orcsIcon(mOrcsIcon);
		setWindowIcon(orcsIcon);
		mChangeVisibilityAction->setIcon(orcsIcon);
	}

	name = ":/resources/pics/orcsLogoSmall.png";
	if(!jpgImage.load(name)) {
		Core::log("Could not load orcs-Logo");
	}
	else{
		mSmallOrcsLogo = QPixmap::fromImage(jpgImage);
	}

	name = ":/resources/pics/uofmLogo.png";

	if(!jpgImage.load(name)) {
		Core::log("Could not load UOFM-Logo");
	}
	else{
		mUosIcon = QPixmap::fromImage(jpgImage);
		mUosIcon = mUosIcon.scaled(200, 60, Qt::KeepAspectRatio);
	}
	name = ":/resources/pics/alearLogo2.png";
	if(!jpgImage.load(name)) {
		Core::log("Could not load ALEAR-Logo");
	}
	else{
		mAlearIcon = QPixmap::fromImage(jpgImage);
		mAlearIcon = mAlearIcon.scaled(200, 60, Qt::KeepAspectRatio);
	}

	name = ":/resources/pics/euLogo.png";
	if(!jpgImage.load(name)) {
		Core::log("Could not load EU-Logo");
	}
	else{
		mEuIcon = QPixmap::fromImage(jpgImage);
		mEuIcon = mEuIcon.scaled(75, 75, Qt::KeepAspectRatio);
	}

	if(!mOrcsIcon.isNull()){
		QLabel *orcsIconLabel = new QLabel(this);
		orcsIconLabel->setPixmap(mSmallOrcsLogo);
		orcsIconLabel->setMaximumSize(25, 25);
		titleLayout->addWidget(orcsIconLabel);
	}

	QLabel *mTitle = new QLabel(this);
	QFont titleFont = mTitle->font();
	titleFont.setPointSize(14);
	mTitle->setFont(titleFont);
	mTitle->setText("<b>NERD Kit" + Core::getVersionString() + "</b>");
	titleLayout->addWidget(mTitle);
	titleLayout->setAlignment(Qt::AlignCenter | Qt::AlignTop);

	QHBoxLayout *logoLayout = new QHBoxLayout();
	logoLayout->setMargin(1);
	logoLayout->setAlignment(Qt::AlignHCenter);

	if(!mUosIcon.isNull()){
		QLabel *uosIconLabel = new QLabel(this);
		uosIconLabel->setPixmap(mUosIcon);
		uosIconLabel->setTextFormat(Qt::RichText);
		uosIconLabel->setOpenExternalLinks(true);
		uosIconLabel->setMaximumSize(200, 60);
		logoLayout->addWidget(uosIconLabel);
		logoLayout->setAlignment(uosIconLabel, Qt::AlignLeft | Qt::AlignBottom);
	}

	if(!mAlearIcon.isNull()){
		logoLayout->addStretch(1000);
		QLabel *alearIconLabel = new QLabel(this);
		alearIconLabel->setPixmap(mAlearIcon);
		alearIconLabel->setTextFormat(Qt::RichText);
		alearIconLabel->setOpenExternalLinks(true);
		alearIconLabel->setMaximumSize(200, 60);
		logoLayout->addWidget(alearIconLabel);
		logoLayout->setAlignment(alearIconLabel, Qt::AlignCenter | Qt::AlignBottom);
	}

	if(!mEuIcon.isNull()){
		logoLayout->addStretch(1000);
		QLabel *euIconLabel = new QLabel(this);
		euIconLabel->setPixmap(mEuIcon);
		euIconLabel->setTextFormat(Qt::RichText);
		euIconLabel->setOpenExternalLinks(true);
		euIconLabel->setMaximumSize(75, 60);
		logoLayout->addWidget(euIconLabel);
		logoLayout->setAlignment(euIconLabel, Qt::AlignRight | Qt::AlignBottom);
	}

	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->setMargin(1);

	mCloseButton = new QPushButton("Close", this);
	buttonLayout->addLayout(logoLayout);
	buttonLayout->addWidget(mCloseButton);
	buttonLayout->setAlignment(mCloseButton, Qt::AlignBottom | Qt::AlignRight);

	createAboutTab();
	createContributorsTab();
	createCreditsTab();
	createLicenseTab();

	mWindowLayout->addSpacing(10);
	mWindowLayout->addLayout(titleLayout);
	mWindowLayout->addSpacing(20);
	mWindowLayout->setAlignment(titleLayout, Qt::AlignTop | Qt::AlignCenter);
	mWindowLayout->setStretchFactor(titleLayout,0);
	mWindowLayout->addWidget(mAboutWindowTabWidget);
	mWindowLayout->setStretchFactor(mAboutWindowTabWidget, 1);

	mWindowLayout->addSpacing(5);
	mWindowLayout->setStretchFactor(logoLayout, 0);
	mWindowLayout->addSpacing(0);
	mWindowLayout->addLayout(buttonLayout);
	mWindowLayout->setStretchFactor(buttonLayout, 0);

	connect(mChangeVisibilityAction, SIGNAL(triggered()), this, SLOT(changeVisibility()));
	connect(mCloseButton, SIGNAL(clicked()), this, SLOT(close()));

}

AboutInfoWidget::~AboutInfoWidget() {
}

void AboutInfoWidget::changeVisibility() {
	if(isHidden()){
		mAboutWindowTabWidget->setCurrentIndex(0);
		show();
	}
	else{
		hide();
	}
}

QAction* AboutInfoWidget::getAction() {
	return mChangeVisibilityAction;
}

void AboutInfoWidget::closeEvent(QCloseEvent*) {
}

void AboutInfoWidget::createAboutTab() {
	QLabel *aboutText = new QLabel(mAboutWindowTabWidget);

	aboutText->setText("NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit<br><br>"
		"This program is part of the <a href=\"http://www.alear.eu/\">ALEAR</a>"
		"-project.<br> (Artificial Language Evolution on Autonomous Robots)<br>"
		"<br><br>Copyright (C) 2008-2010 Neurocybernetics Group<br>University of Osnabrueck, Germany"
		"<br><br>Contact:<br>Christian Rempis (christian.rempis@uni-osnabrueck.de)");
	aboutText->setTextFormat(Qt::RichText);
	aboutText->setAlignment(Qt::AlignCenter);
	aboutText->setOpenExternalLinks(true);
	mAboutWindowTabWidget->addTab(aboutText, "About");
}

void AboutInfoWidget::createContributorsTab() {
	QScrollArea *contributorsScrollArea = new QScrollArea(mAboutWindowTabWidget);
	QWidget *contributorsWidget = new QWidget(contributorsScrollArea);

	QString contribs;

	QString mDocumentFileName = ":contributors.txt";
	QFile docFile(mDocumentFileName);
	if(!docFile.open(QIODevice::ReadOnly | QIODevice::Text)){
		contribs.append("Could not contributors text file [").append(mDocumentFileName)
				.append("]!");
	}
	else {
		QTextStream in(&docFile);
		contribs.append(in.readAll());
	}

	QLabel *names = new QLabel(contributorsWidget);
	names->setText(contribs);
	names->setTextFormat(Qt::PlainText);
	names->setAlignment(Qt::AlignLeft);
	names->setOpenExternalLinks(true);

	QVBoxLayout *layout = new QVBoxLayout();
	contributorsWidget->setLayout(layout);
	layout->addWidget(names);
	contributorsScrollArea->setWidget(contributorsWidget);

	mAboutWindowTabWidget->addTab(contributorsScrollArea, "Contributors");
}

void AboutInfoWidget::createCreditsTab() {

	QScrollArea *creditsScrollArea = new QScrollArea(mAboutWindowTabWidget);
	QWidget *creditsWidget = new QWidget(creditsScrollArea);

	QLabel *namesCredits = new QLabel(creditsWidget);
	namesCredits->setText("University of Osnabrueck<div style=\"text-indent:20px;\">"
		"<a href=\"http://www.ikw.uni-osnabrueck.de/~neurokybernetik/\">"
		"http://www.ikw.uni-osnabrueck.de/~neurokybernetik/</a></div><br>"
		"Humboldt University of Berlin<div style=\"text-indent:20px;\">"
		"<a href=\"http://www.neurorobotik.de\">"
		"http://www.neurorobotik.de</a></div>"
		"<br>Seventh Framework "
		"Programme<div style=\"text-indent:20px;\"><a href=\"http://cordis."
		"europa.eu/fp7/ict/\">http://cordis.europa.eu/fp7/ict/</a></div>");
	namesCredits->setTextFormat(Qt::RichText);
	namesCredits->setAlignment(Qt::AlignLeft);
	namesCredits->setOpenExternalLinks(true);

	QVBoxLayout *creditslayout = new QVBoxLayout();
	creditsWidget->setLayout(creditslayout);
	creditslayout->addWidget(namesCredits);
	creditsScrollArea->setWidget(creditsWidget);
	mAboutWindowTabWidget->addTab(creditsScrollArea, "Credits");
}

void AboutInfoWidget::createLicenseTab() {

	QTextBrowser *licenseText = new QTextBrowser(mAboutWindowTabWidget);
	licenseText->setReadOnly(true);
	QString mDocumentFileName = ":/resources/doc/LicenseAgreement.txt";
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
	licenseText->setHtml(documentText);
	mAboutWindowTabWidget->addTab(licenseText, "License Agreement");
	docFile.close();
}

}
