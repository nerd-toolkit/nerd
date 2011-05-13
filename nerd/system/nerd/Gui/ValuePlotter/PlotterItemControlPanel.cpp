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



#include "PlotterItemControlPanel.h"
#include <QColorDialog>
#include <QHBoxLayout>

namespace nerd {


/**
 * Constructor.
 *
 * @param item the PlotterItem adjusted by this panel.
 * @param parent the parent QWidget
 */
PlotterItemControlPanel::PlotterItemControlPanel(PlotterItem *item, QWidget *parent)
	: QFrame(parent), mPlotterItem(item)
{
	setFrameStyle(QFrame::Box);
	setLineWidth(2);

	QHBoxLayout *layout = new QHBoxLayout();
	setLayout(layout);
	layout->setSpacing(2);
	layout->setMargin(2);

	mShowCheckBox = new QCheckBox(this);
	mShowCheckBox->setChecked(true);
	if(item != 0) {
		mShowCheckBox->setChecked(item->isVisible());
	}

	mColorButton = new QPushButton("", this);
	mColorButton->setFixedSize(25, 25);
	if(item != 0) {
		QColor color = item->getColor();
		mColorButton->setStyleSheet("* { background-color: rgb(" 
					+ QString::number(color.red())
					+ "," + QString::number(color.green()) + 
					+ "," + QString::number(color.blue()) + ") }");
	}
	
	mDashPatternEdit = new QLineEdit("", this);
	mDashPatternEdit->setFixedWidth(25);
	mDashPatternEdit->setToolTip("Dash pattern for the line.\nExamples: 10,10 or 3,10.");

	mNameEdit = new QLineEdit("", this);
	mNameEdit->setMinimumWidth(250);
	if(item != 0) {
		mNameEdit->setText(item->getName());
	}

	layout->addWidget(mShowCheckBox);
	layout->addWidget(mColorButton);
	layout->addWidget(mDashPatternEdit);
	layout->addWidget(mNameEdit, 100);

	connect(mShowCheckBox, SIGNAL(stateChanged(int)),
			this, SLOT(visibilityCheckBoxChanged(int)));
	connect(mColorButton, SIGNAL(pressed()),
			this, SLOT(changeColorButtonPressed()));
	connect(mNameEdit, SIGNAL(returnPressed()),
			this, SLOT(plotterNameFieldChanged()));
	connect(mDashPatternEdit, SIGNAL(returnPressed()),
			this, SLOT(dashPatternChanged()));
}

/**
 * Destructor.
 */
PlotterItemControlPanel::~PlotterItemControlPanel() {
}


/**
 * Returns the PlotterItem this panel works on.
 * @return the PlotterItem.
 */
PlotterItem* PlotterItemControlPanel::getPlotterItem() const {
	return mPlotterItem;
}


/**
 * Called when the color button is pressed. This allows the user to change
 * the color in which this PlotterItem is drawn in the plotter.
 */
void PlotterItemControlPanel::changeColorButtonPressed() {
	if(mPlotterItem != 0) {
		QColor color = QColorDialog::getColor(mPlotterItem->getColor(), this);
		if(color != mPlotterItem->getColor()) {
			mPlotterItem->setColor(color);

			mColorButton->setStyleSheet("* { background-color: rgb(" 
					+ QString::number(color.red())
					+ "," + QString::number(color.green()) + 
					+ "," + QString::number(color.blue()) + ") }");
		}
	}
}


/**
 * Called when the visbility checkbox in the dialog is changed.
 * This applies the change to the PlotterItem.
 *
 * @param state the current state of the checkbox.
 */
void PlotterItemControlPanel::visibilityCheckBoxChanged(int state) {
	mShowCheckBox->setChecked(state);
	if(mPlotterItem != 0) {
		mPlotterItem->setVisible(mShowCheckBox->isChecked());
	}
}


/**
 * Called when the name text field of the PlotterItem was changed.
 * This applies the new name to the PlotterItem, thus its name is
 * also changed in the plotter window.
 */
void PlotterItemControlPanel::plotterNameFieldChanged() {
	if(mPlotterItem != 0) {
		mPlotterItem->setName(mNameEdit->text());
	}
}


/**
 * Called when the PlotterItem changed its visibilty.
 * This synchronizes this control panel with the visibility state of the PlotterItem.
 *
 * @param visible if true, then the PlotterItem is visible and the checkbox will be checked.
 */
void PlotterItemControlPanel::visibilityChanged(bool visible) {
	if(mShowCheckBox->isChecked() != visible) {
		mShowCheckBox->setChecked(visible);
	}
}

void PlotterItemControlPanel::dashPatternChanged() {
	QStringList patternItems = mDashPatternEdit->text().split(",");
	QVector<qreal> pattern;
	for(QListIterator<QString> i(patternItems); i.hasNext();) {
		bool ok = true;
		qreal item = i.next().toDouble(&ok);
		if(ok) {
			pattern.append(item);
		}
	}
	mPlotterItem->setDashPattern(pattern);
}

}


