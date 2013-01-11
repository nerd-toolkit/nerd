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


#include "StaticDataLoader.h"
#include "Gui/ValuePlotter/ValuePlotterWidget.h"
#include "Gui/ValuePlotter/PlotterWidget.h"
#include <QFile>
#include <QTextStream>
#include "Core/Core.h"
#include <iostream>

using namespace std;

namespace nerd {

StaticDataLoader::StaticDataLoader(ValuePlotterWidget *owner, 
				const QString &fileName,
				const QString &commentChar,
				const QString &separator)
	: QThread(0), mOwner(owner), mFileName(fileName), mCommentChar(commentChar), 
		mSeparator(separator), mRunLoader(false)
{
	if(mOwner != 0) {
		connect(this, SIGNAL(enableLoadStaticDataButton()),
				mOwner, SLOT(enableLoadStaticDataButton()));
	}

	Core::getInstance()->registerThread(this);
}

StaticDataLoader::~StaticDataLoader() {
	Core::getInstance()->deregisterThread(this);
}



void StaticDataLoader::stopLoader() {
	mRunLoader = false;
}


void StaticDataLoader::run() {
	
	mRunLoader = true;

	if(mOwner == 0 || mOwner->getValuePlotter() == 0) {
		mRunLoader = false;
		return;
	}

	//Try to open specified file
	QFile file(mFileName);

	if(!file.open(QIODevice::ReadOnly)) {
		file.close();
		mRunLoader = false;
		return;
	}

	//collect number of lines
	QTextStream data(&file);

	int dataLineCounter = 0;

	while(!data.atEnd() && mRunLoader) {
		QString line = data.readLine().trimmed();

		if(line == "") {
			continue;
		}
		if(!line.startsWith(mCommentChar)) {
			dataLineCounter++;
		}
	}

	//restart reading...
	data.seek(0);


	ValuePlotter *valuePlotter = mOwner->getValuePlotter();

	valuePlotter->removeAllStaticData();

	QList<QString> names;
	QList<PlotterItem*> items;
	QList<QColor> colors;

	while(!data.atEnd() && mRunLoader) {
		QString line = data.readLine().trimmed();

		if(line == "") {
			continue;
		}
		//collect names (usually the form <commentChar> <name>)
		if(line.startsWith(mCommentChar)) {
			names.append(QString("[")
				.append(line.mid(mCommentChar.size()).trimmed())
				.append("]"));
		}
		else {
			//parse the actual data series

			QStringList values = line.split(mSeparator);

			//always ensure that the number of values in a line match the number
			//of PlotterItems. If not, then add sufficiently many PlotterItems.
			if(values.size() > items.size()) {
				for(int i = items.size(); i < values.size(); ++i) {
					QString name = names.size() > i ? names.at(i) : "[data]";
					QColor color = valuePlotter->getSuitableColor(&colors);
					colors.append(color);
					PlotterItem *item = new PlotterItem(name, color);
					item->setStaticHistory(true);
					items.append(item);
				}
			}


			//Extend the history of all PlotterItems (parsing errors are ignored!)
			QListIterator<PlotterItem*> pi(items);
			QListIterator<QString> v(values);
			for(; pi.hasNext() && v.hasNext();) {
				PlotterItem *item = pi.next();
				item->setHistoryCapacity(dataLineCounter);
				item->addToHistory(v.next().toDouble());
			}
		}
	}
	file.close();

	if(!mRunLoader) {
		return;
	}

	PlotterWidget *pw = valuePlotter->getPlotterWidget();

	//All new PlotterItems are added to the PlotterWidget.
	for(int i = 0; i < items.size(); ++i) {
		pw->addPlotterItem(items.at(i));
	}

	//Make sure the static data is visible.
	valuePlotter->showStaticData(true);

	emit enableLoadStaticDataButton();
}

}


