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



#include "IndividualPreviewButton.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include <QProcess>

using namespace std;

namespace nerd {


/**
 * Constructs a new IndividualPreviewButton.
 */
IndividualPreviewButton::IndividualPreviewButton(const QString &startScriptFullFileName, int generation, int id, bool toggle)
	: QPushButton("G: " + QString::number(generation) + " I:" + QString::number(id))
{
	mArguments << startScriptFullFileName << QString::number(id) << "-test" << "-gui";
	if(toggle) {
		mArguments << "-toggle";
	}
	QString titleString = QString("Preview_Gen:_").append(QString::number(generation))
							.append("_Ind:_").append(QString::number(id));
	//Core::log("String: " + titleString, true);
	mArguments <<  "-setTitle" << titleString << "-gui";

	setToolTip("Preview Ind. " + QString::number(id));
}


/**
 * Destructor.
 */
IndividualPreviewButton::~IndividualPreviewButton() {
}

void IndividualPreviewButton::previewIndividual() {
	QProcess p;
	p.startDetached("/usr/bin/zsh", mArguments);
	p.waitForStarted(10000);
	

// 	QString command;
// 	for(QListIterator<QString> i(mArguments); i.hasNext();) { command.append(" ").append(i.next()); }
// 	Core::log("CMD: " + command, true);

	p.waitForFinished();
	p.close();
}


}



