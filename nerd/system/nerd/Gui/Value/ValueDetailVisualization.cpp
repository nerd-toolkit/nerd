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

/***************************************************************************
 * Deprecated!                                                             *
 *                                                                         *
 * This class will be removed in a later release!                          *
 * *************************************************************************/




#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include "ValueDetailVisualization.h"
#include "Value/ValueChangedListener.h"
#include "Value/ValueManager.h"

#include <iostream>

using namespace std;

namespace nerd{

/**
 * Constructor.
 *
 * @param value the value whos properties will be visualized.
 */
ValueDetailVisualization::ValueDetailVisualization(Value *value, ValueManager *valueManager) {
	mValue = value;
	mValueManager = valueManager;

	setAttribute(Qt::WA_DeleteOnClose, true);

	mNames = mValueManager->getNamesOfValue(mValue);
	QString name("Value [");
	for(int i = 0; i < mNames.size(); ++i) {
		name.append(mNames.at(i));
		if(i < mNames.size() - 1) {
			name.append(", ");
		}
	}
	name.append("]");
	mNameLabel = new QLabel(name);
	mNameLabel->setFocusPolicy(Qt::NoFocus);

	mCloseEvent = new QPushButton("x");
	mCloseEvent->setFixedWidth(20);
	mCloseEvent->setFocusPolicy(Qt::NoFocus);

	QVBoxLayout *layout = new QVBoxLayout;
	QHBoxLayout *subLayout = new QHBoxLayout;

//	layout->setMargin(1);	
	setLayout(layout);
	layout->addLayout(subLayout);

	subLayout->addWidget(mNameLabel);
	subLayout->addWidget(mCloseEvent);

	mTextEdit = new QTextEdit();
	mTextEdit->setReadOnly(true);
	mTextEdit->setLineWrapMode(QTextEdit::NoWrap);
	mTextEdit->setFixedHeight(120);
	layout->addWidget(mTextEdit);

	fillContents();

	setFrameStyle(QFrame::Panel | QFrame::Plain);
	setLineWidth(2);

	connect(mCloseEvent, SIGNAL(clicked()), this, SLOT(close()));
}

/**
 * This slot fills the TextEdit of the Visualization with the properties of the value.
 */
void ValueDetailVisualization::fillContents() {
	QString textEditContent;

	textEditContent.append("<b>Names:</b><br>");
	QListIterator<QString> itNames(mNames);
	for(; itNames.hasNext(); ) {
		textEditContent.append(itNames.next()).append("<br>");
	}

	textEditContent.append("<b>Type:</b><br>");
	QString typeName = mValue->getTypeName();
	if(typeName.isEmpty()) {
		textEditContent.append("--<br>");
	}
	else {
		textEditContent.append(typeName).append("<br>");
	}
	
	textEditContent.append("<b>Description:</b><br>");
	QString description = mValue->getDescription();
	if(description.isEmpty()) {
		textEditContent.append("--<br>");
	}
	else {
		textEditContent.append(description).append("<br>");
	}

	textEditContent.append("<b>CurrentValue:</b><br>");
	textEditContent.append(mValue->getValueAsString()).append("<br>");


	textEditContent.append("<b>ValueChangedListeners:</b><br>");
	QList<ValueChangedListener*> valueChangedListeners = mValue->getValueChangedListeners();
	QListIterator<ValueChangedListener*> itListeners(valueChangedListeners);
	if(!itListeners.hasNext()) {
		textEditContent.append("--<br>");
	}
	else {
		for(; itListeners.hasNext(); ) {
			textEditContent.append(itListeners.next()->getName()).append("<br>");
		}
	}

	textEditContent.append("<b>IsNotifyingAllSetAttempts:</b><br>");
	if(mValue->isNotifyingAllSetAttempts()) {
		textEditContent.append("True<br>");
	} 
	else {
		textEditContent.append("False<br>");
	}

	mTextEdit->setText(textEditContent);
}

Value* ValueDetailVisualization::getValue() {
	return mValue;
}

QTextEdit* ValueDetailVisualization::getTextEdit() const {
	return mTextEdit;
}

}
