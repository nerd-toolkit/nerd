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



#ifndef GuiMainWindow_H_
#define GuiMainWindow_H_
#include "Gui/Visualization/OpenGLVisualization.h"
#include "Gui/Value/ValueVisualizationWindow.h"
#include "Gui/Event/EventVisualizationWindow.h"
#include "Gui/Visualization/VisualizationChooser.h"
#include "Gui/ControlWindow.h"
#include <QGLWidget>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMenuBar>
#include "Gui/Value/ValueVisualizationWindow.h"
#include "Gui/Event/EventVisualizationWindow.h"
#include "Gui/Event/EventDetailPanel.h"
#include "Gui/Value/ValueDetailPanel.h"
#include "Gui/Parameter/MultipleParameterWindowWidget.h"
#include "Gui/Parameter/ParameterVisualization.h"
#include "Gui/DebugLoggerPanel/DebugLoggerPanel.h"
#include "Util/Color.h"
#include "Gui/Containers/MainWindowContainer.h"
#include <QTimer>
#include "PlugIns/CommandLineArgument.h"
#include "Value/ValueChangedListener.h"

namespace nerd{

class RobotView;
class MotorControlGui;
class MotorControlManager;


class GuiMainWindow : public QWidget, ValueChangedListener {

	Q_OBJECT
	
	public:
		GuiMainWindow(bool simulationIsControllable, bool enableDebugging = false);
		virtual ~GuiMainWindow();
		
		virtual void valueChanged(Value *value);
		virtual QString getName() const;

		QMenuBar* getMenuBar();
		QMenu* getMenu(const QString &name);
		QPushButton* addButton(const QString &buttonName, QWidget *newPluginWindow);
		QMenu* addMenu(const QString &buttonName, QWidget *newPluginWindow);
		void addWidget(QWidget *newView);

		virtual void createBasicControlMenu();
		
		static QString getIconName();

	private:
		void setup(bool openGLControllable, bool enableDebugging);
		
	protected slots:
		void closeEvent(QCloseEvent *e);
		

	public slots:
		void toggleTimerExpired();
		void showWindow();
		void openWebHelp();
		void openWebHome();

	private:
		OpenGLVisualization *mVisualization;
		ControlWindow *mControlArea;
		QGridLayout *mWidgetLayout;
		QMenuBar *mMainMenuBar;

		EventVisualizationWindow *mEventList;
		VisualizationChooser *mVisualizationChooser;
		EventDetailPanel *mEventDetailPanel;
		ValueDetailPanel *mValueDetailPanel;
		DebugLoggerPanel *mDebugLoggerPanel;
		DebugLoggerPanel *mValueLoggerPanel;
// 		MotorControlGui *mMotorControlGui;
		MotorControlManager *mMotorControlGui;
		MainWindowContainer *mParameterManipulationWindow;
		MultipleParameterWindowWidget *mParameterLists;
		QVector<QWidget*> mPluginWindows;
		QMap<QString, QMenu*> mMenuLookup;	
		QList<QWidget*> mViewList;
		bool mShutDownTriggered;
		bool mWindowToggleState;
		QTimer mWindowToggleTimer;
		CommandLineArgument *mToggleWindowArgument;
		StringValue *mToggleWindowValue;

	

};
}
#endif

