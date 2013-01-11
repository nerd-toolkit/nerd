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



#include "PositionMemoryWidget.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

using namespace std;

namespace nerd {



class ShowWindowTask : public virtual Task {
	
	public:
 		ShowWindowTask(PositionMemoryWidget *owner) : mOwner(owner) {};
		virtual bool runTask() {
			if(mOwner != 0) {
				mOwner->showWindow();
			}
			return true;
		}

	private:
		PositionMemoryWidget *mOwner;
	
};



/**
 * Constructs a new PositionMemoryWidget.
 */
PositionMemoryWidget::PositionMemoryWidget(const QString &name, QWidget *parent) 
	: QMainWindow(parent), mName(name), mInitCompletedEvent(0), mShutDownEvent(0)
{

	QWidget *mainWidget = new QWidget(this);
	setCentralWidget(mainWidget);

	connect(this, SIGNAL(showWindowSignal()), this, SLOT(show()));

	mInitCompletedEvent = Core::getInstance()->getInitCompletedEvent();
	mShutDownEvent = Core::getInstance()->getShutDownEvent();
	mInitCompletedEvent->addEventListener(this);	
	mShutDownEvent->addEventListener(this);

	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, false);
}


/**
 * Destructor.
 */
PositionMemoryWidget::~PositionMemoryWidget()
{
}

QString PositionMemoryWidget::getName() const  {
	return mName;
}

void PositionMemoryWidget::eventOccured(Event *event) {
	if(event == 0) {
		return;
	}

// 	Core *core = Core::getInstance();
// 	QString path = core->getConfigDirectoryPath() + "/properties";
// 
// 	if(event == mInitCompletedEvent) {
// 		Properties p;
// 		if(p.loadFromFile(path + "/" + mName + ".props")) {
// 			int width = p.getProperty("width").toInt();
// 			int height = p.getProperty("height").toInt();
// 			int posx = p.getProperty("posx").toInt();
// 			int posy = p.getProperty("posy").toInt();
// 
// 			setGeometry(posx, posy, width, height);
// 			move(posx, posy);
// 
// 			if(p.getProperty("visible") == "T" || p.getProperty("visible") == "t") {
// 				Core::getInstance()->scheduleTask(new ShowWindowTask(this));
// 			}
// 			
// 		}
// 	}
// 	else if(event == mShutDownEvent) {
// 		
// 		core->enforceDirectoryPath(path);
// 
// 		Properties p;
// 		p.setProperty("width", QString::number(frameGeometry().width()));
// 		p.setProperty("height", QString::number(frameGeometry().height()));
// 		p.setProperty("posx", QString::number(frameGeometry().x()));
// 		p.setProperty("posy", QString::number(frameGeometry().y()));
// 		
// 		if(isVisible()) {
// 			p.setProperty("visible", "T");
// 		}
// 		else {
// 			p.setProperty("visible", "F");
// 		}
// 
// 		p.saveToFile(path + "/" + mName + ".props");
// 	}
}


void PositionMemoryWidget::showWindow() {
	emit showWindowSignal();
}


}



