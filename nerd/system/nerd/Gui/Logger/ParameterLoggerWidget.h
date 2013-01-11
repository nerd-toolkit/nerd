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



#ifndef NERDParameterLoggerWidget_H
#define NERDParameterLoggerWidget_H


#include <QGroupBox>
#include <QPoint>
#include <QList>
#include <QPushButton>
#include <QLineEdit>
#include <QTextStream>
#include <QFile>
#include "Value/Value.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Gui/Logger/ValueProvider.h"

namespace nerd {

	class ParameterVisualizationWindow;

	/**
	 * ParameterLoggerWidget.
	 * This widget allows the logging of variables after each simulation step.
	 * This is useful to log sensor or motor data or the positions of objects
	 * during simulation.
	 *
	 * The ParameterLoggerWidget provides GUI elements to select a file name
	 * to log to and a start button. When pressed all Values currently selected
	 * in the corresponding ParameterVisualizationWindow (present in the list
	 * AND selected for update) are logged in a file.
	 * Changes in the selection of the ParameterVisualizationWindow while the
	 * logging is active are not applied to the logger to keep the log files
	 * consistent.
	 */
	class ParameterLoggerWidget : public QGroupBox, public virtual EventListener
	{
	Q_OBJECT

	public:
		ParameterLoggerWidget(const QString &name, ValueProvider *owner);
		virtual ~ParameterLoggerWidget();

		virtual QString getName() const;

		virtual void eventOccured(Event *event);

	public slots:
		void showDialog();
		void logButtonPressed();

	signals:
		void loggerStatusChanged(bool loggingActive);

	private:
		bool startLogging();
		void stopLogging();
		void logVariables();

		QString formatControlChars(const QString &string);

	private:
		QString mName;
		ValueProvider *mValueProvider;
		QPoint mDialogPosition;
		QPushButton *mLogButton;
		QLineEdit *mFileNameEdit;
		QLineEdit *mSeparatorCharEdit;
		QLineEdit *mCommentCharEdit;
		QList<Value*> mLoggedValues;
		bool mLoggingInProgress;
		QTextStream *mOutputStream;
		QFile *mOutputFile;
		Event *mStepCompletedEvent;
		QString mSeparatorString;
		QString mCommentString;
	};

}


#endif



