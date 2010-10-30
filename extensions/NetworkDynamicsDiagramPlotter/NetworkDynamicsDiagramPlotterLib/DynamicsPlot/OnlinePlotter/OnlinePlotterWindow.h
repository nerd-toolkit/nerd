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



#ifndef NERDOnlinePlotterWindow_H
#define NERDOnlinePlotterWindow_H

#include <QString>
#include <QWidget>
#include <QLabel>
#include <DynamicsPlot/OnlinePlotter/MouseMoveLabel.h>
#include "Core/SystemObject.h"
#include "Value/MatrixValue.h"
#include <QVBoxLayout>
#include <QGridLayout>

namespace nerd {

	/**
	 * OnlinePlotterWindow.
	 *
	 */
	class OnlinePlotterWindow : public QWidget, public virtual SystemObject  {
		Q_OBJECT
		public:
			OnlinePlotterWindow(QWidget *parent = 0);
			virtual ~OnlinePlotterWindow();
			virtual bool init();
			virtual bool bind();
			virtual bool cleanUp();
			virtual QString getName() const;
			
		public slots:
			void printData(QString name, MatrixValue *dataMatrix, QString xDescr, QString yDescr);
			void updateData();
			void finishedProcessing();
			void processing();
			
		signals:
			void timerStart();
			
		protected:
			void resizeEvent(QResizeEvent * event);
		private:
			MouseMoveLabel *mLabel;
			QPixmap mPixmap;
			QLabel *mYLabel;
			QLabel *mXLabel;
			QLabel *mTitleLabel;
			QLabel *mBlankLabel;
			QLabel *mInInUppTopLabel;
			QLabel *mInInUppMidLabel;
			QLabel *mInInUppLowLabel;
			QLabel *mInInLowLeftLabel;
			QLabel *mInInLowMiddleLabel;
			QLabel *mInInLowRightLabel;
			QLabel *mMessageLabel;
			QLabel *mBlankLabel2;
			QLabel *mXMaxLabel;
			QLabel *mXMinLabel;
			QLabel *mYMaxLabel;
			QLabel *mYMinLabel;
			
			QVBoxLayout *mMainLayout;
			QGridLayout *mOuterLayout;
			QGridLayout *mInnerLayout;
			int mDots;
			int mHeight;
			int mWidth;
			int mWidgetWidth;
			int mWidgetHeight;
			BoolValue *mPlotterOnlineValue;			
			bool mIsSetUp;
			MatrixValue *mMatrix;
			
			void setupGUI();
			ValueManager *mVM;
			
			
	};

}

#endif




