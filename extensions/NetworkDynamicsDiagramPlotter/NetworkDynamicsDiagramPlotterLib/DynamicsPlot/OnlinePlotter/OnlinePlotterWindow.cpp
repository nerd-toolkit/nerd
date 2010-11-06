/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
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



#include "OnlinePlotterWindow.h"
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
// #include "vector"
using namespace std;

namespace nerd {


/**
 * Constructs a new OnlinePlotterWindow.
 */
	OnlinePlotterWindow::OnlinePlotterWindow(QWidget *parent)
	{
		setMouseTracking(true);
		mIsSetUp = false;
		mForceUpdate = false;
		moveToThread(QCoreApplication::instance()->thread());	
		setupGUI();		
	}



/**
 * Destructor.
 */
	OnlinePlotterWindow::~OnlinePlotterWindow() {
	}
	
	bool OnlinePlotterWindow::init() {
		bool ok = true;

		return ok;
	}

	bool OnlinePlotterWindow::bind() {
		bool ok = true;
		
		return ok;
	}

	bool OnlinePlotterWindow::cleanUp() {
		bool ok = true;

		return ok;
	}
	
	QString OnlinePlotterWindow::getName() const {
		return "OnlinePlotterWindow";
	}
	void OnlinePlotterWindow::paintEvent ( QPaintEvent * event ){
// 		mLabel->resize(mPixmap.width(), mPixmap.height());
	}
	
	void OnlinePlotterWindow::resizeEvent ( QResizeEvent * event ){
		if(mIsSetUp){
			int oldHeight = event->oldSize().height();
			int oldWidth = event->oldSize().width();
// 			cerr<<"margin " << mLabel->margin()<<endl;
			int currentWidth = event->size().width();
			int currentHeight = event->size().height();

			if((oldWidth * oldHeight) == 0){
				Core::log("OnlinePlotterWindow::resizeEvent(): Something went wrong. oldWidth * oldHeight must not be equal to zero. ", true); 
				return;
			}
			double xFactor = static_cast<double>(currentWidth)/static_cast<double>(oldWidth);
			double yFactor = static_cast<double>(currentHeight)/static_cast<double>(oldHeight);
// 			double xFactor = static_cast<double>(mLabel->width())/static_cast<double>(mMatrix->getMatrixWidth() - 1);
// 			double yFactor = static_cast<double>(mLabel->height())/static_cast<double>(mMatrix->getMatrixHeight() - 1);
			
			if(mPixmap.isNull()){
				Core::log("OnlinePlotterWindow::resizeEvent(): Pixmap is null.", true); 
				return;
			}

// 			mPixmap = mPixmap.scaled(static_cast<int>(static_cast<double>(mPixmap.width()) * xFactor + 0.5), static_cast<int>(static_cast<double>(mPixmap.height()) * yFactor + 0.5));
			mLabel->setFixedSize(static_cast<int>(static_cast<double>(mLabel->width()) * xFactor + 0.5), static_cast<int>(static_cast<double>(mLabel->height()) * yFactor + 0.5));
// 			QPixmap tempPixmap = mPixmap.scaled(static_cast<int>(static_cast<double>(mPixmap.width()) * xFactor + 0.5), static_cast<int>(static_cast<double>(mPixmap.height()) * yFactor + 0.5));
			QPixmap tempPixmap = mPixmap.scaled(mLabel->width(), mLabel->height());
			mLabel->clear();
// 			mLabel->resize(tempPixmap.width(), tempPixmap.height());
// 			mLabel->setFixedSize(tempPixmap.width(), tempPixmap.height());
			mLabel->setPixmap(tempPixmap); //print QImage on label
			mLabel->update();
			mLabel->show();
			show();
		}//if mIsSetUp

	}
	
	
	/**
	 *  Setup GUI, set attributes, create elements and layouts. 
	 * 
	 * 
	 */
	void OnlinePlotterWindow::setupGUI()
	{
		setAttribute(Qt::WA_QuitOnClose, false);
		setWindowTitle("Inbuilt Dynamics Plotter");
		
		
		mLabel = new MouseMoveLabel(this);
		mLabel->setFrameStyle(0);
// 		mLabel->setScaledContents(true);
		mLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		mTitleLabel = new QLabel( this);

		mYLabel = new QLabel("", this);
		mXLabel = new QLabel("", this);
		mBlankLabel = new QLabel(this);
		mBlankLabel2 = new QLabel(this);
		mMessageLabel = new QLabel("", this);
		mXMaxLabel = new QLabel(this);
		mXMinLabel = new QLabel(this);
		mYMaxLabel = new QLabel(this);
		mYMinLabel = new QLabel(this);
		
		
		mMainLayout = new QVBoxLayout();
		mOuterLayout = new QGridLayout();
		mInnerLayout = new QGridLayout();
		mMainLayout->setContentsMargins(0, 0, 0, 0);
		
		mTitleLabel->setAlignment(Qt::AlignHCenter);
		mLabel->setAlignment(Qt::AlignTop);
		mYLabel->setAlignment(Qt::AlignRight);
		mYLabel->setAlignment(Qt::AlignVCenter);
		mXLabel->setAlignment(Qt::AlignHCenter);
		
		mYMaxLabel->setAlignment(Qt::AlignRight);
		mYMaxLabel->setAlignment(Qt::AlignTop);
		mYMinLabel->setAlignment(Qt::AlignRight);
		mYMinLabel->setAlignment(Qt::AlignBottom);
		mXMinLabel->setAlignment(Qt::AlignLeft);
		mXMaxLabel->setAlignment(Qt::AlignRight);
				
		mInnerLayout->addWidget(mYMaxLabel, 0, 0, Qt::AlignRight);
		mInnerLayout->addWidget(mYMinLabel, 1, 0);
		mInnerLayout->addWidget(mBlankLabel2, 2, 0);
		mInnerLayout->addWidget(mLabel, 0, 1, 2, 1);
// 		mInnerLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Maximum, QSizePolicy::Maximum), 0, 3, 2, 1);
		mInnerLayout->addWidget(mXMinLabel, 2, 1);
		mInnerLayout->addWidget(mXMaxLabel, 2, 2);
		mInnerLayout->setColumnStretch(0, 0);
		mInnerLayout->setColumnStretch(1, 1);
		mInnerLayout->setRowStretch(1, 0);
		mInnerLayout->setRowStretch(0, 1);
// 		mInnerLayout->setColumnStretch(2, 0);
		
		mOuterLayout->addWidget(mYLabel, 0, 0);
		mOuterLayout->addLayout(mInnerLayout, 0, 1);
		mOuterLayout->addWidget(mBlankLabel, 1, 0);
		mOuterLayout->addWidget(mXLabel, 1, 1);
		mOuterLayout->setColumnStretch(0, 0);
		mOuterLayout->setColumnStretch(1, 1);
		mOuterLayout->setRowStretch(1, 0);
		mOuterLayout->setRowStretch(0, 1);
		
		mMainLayout->addWidget(mTitleLabel);
		mMainLayout->addLayout(mOuterLayout);
		mMainLayout->addWidget(mMessageLabel);
		mMainLayout->setStretch(0, 0);
		mMainLayout->setStretch(1, 1);
		mMainLayout->setStretch(2, 0);
		setLayout(mMainLayout);
		
		int mDots = 3;//number of dots in "updating..."
	}

	/**
	 * Prints the data matrix to a label. 
	 *
	 * @param name Name of active calculator
	 * @param dataMatrix Matrix with data from active calculator
	 * 
	*/
	void OnlinePlotterWindow::printData(QString name, MatrixValue *dataMatrix, QString xDescr, QString yDescr){
		if(dataMatrix == 0 || name == 0){
			Core::log("OnlinePlotterWindow: Couldn't find data Matrix or Name");
			return;
		}		
		mMatrix = dataMatrix;
// 		hide();
		mIsSetUp = false;
		mWidth = mMatrix->getMatrixWidth();
		mHeight = mMatrix->getMatrixHeight();
		mTitleLabel->setText(QString("<b><big>") + name + QString("<\big><\b>"));

		mYLabel->setText(yDescr);
		mXLabel->setText(xDescr);
		resize(1,1);
		emit timerStart();
	}
	
	/**
	 * Updates the diagram during calculation. 
	 *
	 */
	void OnlinePlotterWindow::updateData(){
// 		cerr<<"update received";
		mVM = Core::getInstance()->getValueManager();
		mPlotterOnlineValue = static_cast<BoolValue*>(mVM->getValue("/DynamicsPlotters/InbuiltPlotterOnline"));

		if(mPlotterOnlineValue->get() == false && mForceUpdate == false){
			if(mDots == 0){
				mMessageLabel->setText("<font color='red'>Please wait! Calculating</font>");			
				mDots = 1;
			}else if(mDots == 1){
				mMessageLabel->setText("<font color='red'>Please wait! Calculating.</font>");
				mDots = 2;
			}else if(mDots == 2){
				mMessageLabel->setText("<font color='red'>Please wait! Calculating..</font>");			
				mDots = 3;
			}else{
				mMessageLabel->setText("<font color='red'>Please wait! Calculating...</font>");
				mDots = 0;
			}
			int mWidth = mMatrix->getMatrixWidth();
			int mHeight = mMatrix->getMatrixHeight();
			return;
		}else{
			mForceUpdate = false;
			int mWidth = mMatrix->getMatrixWidth();
			int mHeight = mMatrix->getMatrixHeight();
	
			//create a QImage-object to be printed on the label

			if(mDots == 0){
				mMessageLabel->setText("<font color='red'>Updating</font>");
				mDots = 1;
			}else if(mDots == 1){
				mMessageLabel->setText("<font color='red'>Updating.</font>");
				mDots = 2;
			}else if(mDots == 2){
				mMessageLabel->setText("<font color='red'>Updating..</font>");
				mDots = 3;
			}else{
				mMessageLabel->setText("<font color='red'>Updating...</font>");	
				mDots = 0;
			}
			mYMaxLabel->setText("yMax: ...");
			mYMinLabel->setText("yMin: ...");
			mXMinLabel->setText("xMin: ...");
			mXMaxLabel->setText("xMax: ...");
			QImage image(mWidth - 1, mHeight - 1, QImage::Format_RGB32);
			image.fill(qRgb(255, 0, 0));
// 			resize(mWidth - 1, mHeight - 1);
			for(int j = 1; j < mWidth; j++){
				for (int k = 1; k < mHeight; k++){
					if(mMatrix->get(j, k, 0) == 0){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(255, 255, 255)); //no attractor
					}else if(mMatrix->get(j, k, 0) == 1){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(0, 0, 0));
					}else if(mMatrix->get(j, k, 0) == 2){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(255, 0, 0));
					}else if(mMatrix->get(j, k, 0) == 3){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(0, 255, 0));
					}else if(mMatrix->get(j, k, 0) == 4){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(0, 0, 255));
					}else if(mMatrix->get(j, k, 0) == 5){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(255, 255, 0));
					}else if(mMatrix->get(j, k, 0) == 6){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(0, 255, 255));
					}else if(mMatrix->get(j, k, 0) == 7){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(255, 0, 255));
					}else if(mMatrix->get(j, k, 0) == 8){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(150, 0, 255));
					}else if(mMatrix->get(j, k, 0) == 9){
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(255, 160, 0));
					}else{
						image.setPixel(j - 1,  mHeight - k - 1, qRgb(220, 220, 220));//period to high
					}
				} 
			}//for
			mLabel->clear();
			mLabel->setMatrix(mMatrix);
			mPixmap = QPixmap::fromImage(image);
// 			mLabel->resize(mPixmap.width(), mPixmap.height());
			mLabel->setFixedSize(mPixmap.width(), mPixmap.height());
// 			cerr << "pix width2 " << mPixmap.width()<<endl;
// 			cerr << "label wid2 " << mLabel->width()<<endl;
			mLabel->setPixmap(mPixmap); //print QImage on label
			mLabel->show();
			resize(1,1);
			show();

// 			mWidgetWidth = width();
// 			mWidgetHeight = height();
		}//if
	}//updateData()
	
	/**
	 * Sets message label to 'Done' when finished. 
	 * 
	 */
	void OnlinePlotterWindow::finishedProcessing(){
		mForceUpdate = true; // makes using the updateData() function once possible
		updateData();
		mMessageLabel->setText("<font color='red'>Done!</font>");
		mYMaxLabel->setText(QString("yMax: " + QString("%1").arg(mMatrix->get(0, mMatrix->getMatrixHeight() - 1, 0))));
		mYMinLabel->setText(QString("yMin: " + QString("%1").arg(mMatrix->get(0, 1, 0))));
		mXMinLabel->setText(QString("xMin: " + QString("%1").arg(mMatrix->get(1, 0, 0))));
		mXMaxLabel->setText(QString("xMax: " + QString("%1").arg(mMatrix->get(mMatrix->getMatrixWidth() - 1, 0, 0))));
		mIsSetUp = true;

	}
	
	/**
	 * Sets message label to 'Calculating...' while running the calculator not in online mode. 
	 *
	 */
	void OnlinePlotterWindow::processing(){
		mIsSetUp = false;
		mMessageLabel->setText("<font color='red'>Please wait! Calculating...</font>");
		mMessageLabel->show();
	
	}
}



