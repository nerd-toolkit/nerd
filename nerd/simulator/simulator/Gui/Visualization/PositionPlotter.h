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



#ifndef PositionPlotter_H_
#define PositionPlotter_H_
#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QListWidget>

namespace nerd{

class OpenGLVisualization;

class PositionPlotter:public QWidget{
	Q_OBJECT

	public:
		PositionPlotter();
		~PositionPlotter();

	private:
		void fillComboBox();

    private slots:
		void addObjectToList();
		void removeSelectedObjects();
		void setLineColor();
		void setLineWidth(int);
		void plotterActivateAction();
		void plotterDeactivateAction();

	public slots:
		void showWindow();

	signals:
		void activatePlotter(QString, double, QColor, bool);
		void deactivatePlotter(bool, bool);
		void changeLineColor(QColor);
		void changeLineWidth(double);

	private:
		int mPosX;
		int mPosY;

		QColor mLineColor;
		double mLineWidth;
		bool mPlotterActive;

		QComboBox *mObjectSelectionBox;
		QGridLayout *mWindowLayout;

		QPushButton *mAddObject;
		QListWidget *mObjectList;
		QPushButton *mRemoveObject;
		QSlider *mWidthSlider;
		QPushButton *mColorButton;
		QPushButton *mActivateButton;
		QPushButton *mDeactivateButton;

		QStringList mObjectNameList;
		QCheckBox *mKeepData;
		QCheckBox *mControlSim;
};
}
#endif
