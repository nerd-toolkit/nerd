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



#include "CommentWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QVBoxLayout>
#include <QSplitter>
#include "EvolutionConstants.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new CommentWidget.
 */
CommentWidget::CommentWidget(QWidget *owner)
	: QWidget(owner)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);

	QSplitter *splitter = new QSplitter();
	splitter->setOrientation(Qt::Vertical);

	mCommentHistoryArea = new QTextEdit();
	mCommentHistoryArea->setReadOnly(true);

	mCommentEdit = new QTextEdit();

	splitter->addWidget(mCommentHistoryArea);
	splitter->addWidget(mCommentEdit);
	splitter->setStretchFactor(0, 1000);
	splitter->setStretchFactor(1, 1);

	layout->addWidget(splitter);

	mSubmitCommentButton = new QPushButton("Submit Comment");
	layout->addWidget(mSubmitCommentButton);

	connect(mSubmitCommentButton, SIGNAL(pressed()),
			this, SLOT(submitButtonPressed()));
	connect(this, SIGNAL(commentHistoryUpdated(QString)),
			this, SLOT(updateCommentHistory(QString)));

	
	//bind to required objects 
	mLogger = dynamic_cast<SettingsLogger*>(Core::getInstance()->getGlobalObject(
				EvolutionConstants::OBJECT_SETTINGS_LOGGER));
	mLoggerValue = Core::getInstance()->getValueManager()->getStringValue(
				EvolutionConstants::VALUE_INCREMENTAL_LOGGER_COMMENT);
	
	if(mLogger != 0) {
		mLoggerCommentHistoryUpdatedEvent = mLogger->getCommentHistoryUpdatedEvent();
		mLoggerCommentHistoryUpdatedEvent->addEventListener(this);
	}
}


/**
 * Destructor.
 */
CommentWidget::~CommentWidget() {
}


QString CommentWidget::getName() const {
	return "CommentWidget";
}


void CommentWidget::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}
	else if(event == mLoggerCommentHistoryUpdatedEvent && mLogger != 0) {
		QList<QString> history = mLogger->getCommentHistory();
		QString comments;
		for(int i = history.size() - 1; i >= 0; --i) {
			comments = comments.append(history[i]);
		}
		emit commentHistoryUpdated(comments);
	}
}


void CommentWidget::submitButtonPressed() {
	QString comment = mCommentEdit->document()->toPlainText();
	mCommentEdit->clear();
	if(mLoggerValue != 0) {
		mLoggerValue->set(comment);
	}
}


void CommentWidget::updateCommentHistory(QString history) {
	mCommentHistoryArea->setText(history);
}



}



