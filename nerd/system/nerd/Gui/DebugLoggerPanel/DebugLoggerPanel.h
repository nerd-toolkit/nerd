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

#ifndef NERDDebugLoggerPanel_H_
#define NERDDebugLoggerPanel_H_

#include <QTextEdit>
#include "Core/SystemObject.h"
#include "Value/StringValue.h"
#include "Value/ValueChangedListener.h"
#include "Gui/Containers/PositionMemoryWidget.h"
#include "Value/IntValue.h"
#include <QLineEdit>


namespace nerd {

/**
 * DebugLoggerPanel.
 * The DebugLoggerPanel is a small window showing all messages which occure in a given   
 * StringValue. As default als Core:log messages are reported.
 */
class DebugLoggerPanel: public PositionMemoryWidget, public virtual SystemObject, 
					    public virtual ValueChangedListener 
{
	Q_OBJECT
	public:
		DebugLoggerPanel(QWidget *parent = 0, const QString &valueName = "/Logger/RecentMessage", 
						 const QString &loggerName = "Debug Logger Panel", 
						 bool showValueSelectionEdit = false, bool logWhileNotVisible = false);
		virtual ~DebugLoggerPanel();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;

		virtual void valueChanged(Value *value);

	private slots:
		void closeEvent(QCloseEvent *e);
		void showWindow();
		void valueNameWasChanged();

	signals:
		void messageOccured(const QString &message);

	private:
		QString mValueName;
		QTextEdit *mTextEdit;
		Value *mRecentMessageStr;

		int mXPos;
		int mYPos;

		bool mShowValueSelectionEdit;
		QLineEdit *mValueToMonitorEdit;

		bool mLogWhileNotVisible;

	
};
}
#endif
