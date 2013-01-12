/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#include "OnlinePlotterTabbedWindow.h"
#include <iostream>
#include <QList>
#include <QCoreApplication>
#include <QApplication>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <DynamicsPlot/OnlinePlotter/MouseMoveLabel.h>
#include <QLabel>
#include <QLineEdit>
#include "Core/Core.h"
#include "Value/MatrixValue.h"
#include "Value/BoolValue.h"
#include "QHBoxLayout"
#include "QVBoxLayout"
#include "QGridLayout"
#include <QTabWidget>
#include "OnlinePlotterWindow.h"
// #include "vector"
using namespace std;

namespace nerd {


/**
 * Constructs a new OnlinePlotterTabbedWindow.
 */
	OnlinePlotterTabbedWindow::OnlinePlotterTabbedWindow(QWidget *parent)
		: QWidget(parent)
	{
		moveToThread(QCoreApplication::instance()->thread());	
		
		QTabWidget *tabWidget = new QTabWidget(this);
		QHBoxLayout *layout = new QHBoxLayout(this);
		setLayout(layout);
		
		resize(300, 200);
		
		for(int i = 0; i < 5; ++i) {
			OnlinePlotterWindow *opw = new OnlinePlotterWindow(i);
			
			mPlotterWindows.append(opw);
			
			connect(opw, SIGNAL(timerStart()),
					this, SLOT(timerStarted()));
			connect(opw, SIGNAL(timerStop()),
					this, SLOT(timerStopped()));
			
			//tabWidget->addTab(opw, "Plot " + QString::number(i));
		}
		
		setAttribute(Qt::WA_QuitOnClose, false);
		setAttribute(Qt::WA_DeleteOnClose, false);
	}



/**
 * Destructor.
 */
	OnlinePlotterTabbedWindow::~OnlinePlotterTabbedWindow() {
	}
	
	bool OnlinePlotterTabbedWindow::init() {
		bool ok = true;

		return ok;
	}

	bool OnlinePlotterTabbedWindow::bind() {
		bool ok = true;
		
		return ok;
	}

	bool OnlinePlotterTabbedWindow::cleanUp() {
		bool ok = true;

		return ok;
	}
	
	QString OnlinePlotterTabbedWindow::getName() const {
		return "OnlinePlotterTabbedWindow";
	}
	


	/**
	 * Prints the data matrix to a label. 
	 *
	 * @param name Name of active calculator
	 * @param dataMatrix Matrix with data from active calculator
	 * 
	*/
	void OnlinePlotterTabbedWindow::printData(QString name, MatrixValue *dataMatrix, QString xDescr, QString yDescr){
		//notify tabs
		for(int i = 0; i < mPlotterWindows.size(); ++i) {
			mPlotterWindows.at(i)->printData(name, dataMatrix, xDescr, yDescr);
		}
	}
	
	/**
	 * Updates the diagram during calculation. 
	 *
	 */
	void OnlinePlotterTabbedWindow::updateData(){
		cerr << "update data" << endl;
		//notify tabs
		for(int i = 0; i < mPlotterWindows.size(); ++i) {
			mPlotterWindows.at(i)->updateData();
		}
	}
	
	/**
	 * Sets message label to 'Done' when finished at set min and max labels 
	 * 
	 */
	void OnlinePlotterTabbedWindow::finishedProcessing(){
		//notify tabs
		for(int i = 0; i < mPlotterWindows.size(); ++i) {
			mPlotterWindows.at(i)->finishedProcessing();
		}
	}
	

	
	void OnlinePlotterTabbedWindow::processing(){
		//Notify tabs
		for(int i = 0; i < mPlotterWindows.size(); ++i) {
			mPlotterWindows.at(i)->processing();
		}
	}
	
	
	void OnlinePlotterTabbedWindow::showWindow() {
		show();
	}
	
	
	void OnlinePlotterTabbedWindow::timerStopped() {
		cerr << "stopped timer"  << endl;
		emit timerStop();
	}
	
	
	void OnlinePlotterTabbedWindow::timerStarted() {
		cerr << "start timer" << endl;
		emit timerStart();
	}
}



