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

#ifndef NERDBaseApplication_H
#define NERDBaseApplication_H

#include <QThread>
#include "Core/SystemObject.h"
#include <QString>
#include <QList>
#include "PlugIns/CommandLineArgument.h"

namespace nerd {

	/**
	 * BaseApplication.
	 *
	 * BaseApplication can be used as base class for many differnt applictions
	 * that make use of the NERD system. BaseApplications handles the interpretation
	 * of command line arguments, the initialization of the core, loading of values
	 * from files during startup (pre-init and post-init) and provides a thread
	 * with an own event loop, independent of the QApplication event loop used for the
	 * GUI.
	 *
	 * To use this base class overwrite the protected methods setupApplication() and
	 * runApplication(). Both methods will be called while the system is started in
	 * the run() method of the thread.
	 *
	 * SetupApplications() can be used to create all required objects that will
	 * make up your application. This method will be called before any initialization
	 * of the Core took place.
	 *
	 * RunApplication() will be called after a successful initialization of the Core
	 * and should be used to run the Application. Usually this method should not
	 * block the thread, since the event loop of the BaseApplication thread will be
	 * started only after runApplication returned. If you require blocking algorithms,
	 * you should start a separate thread within runApplication and return afterwards.
	 *
	 * Although BaseApplication handles the interpretation of the command line
	 * parameters it is sometimes useful to process these parameters in your own
	 * subclass to filter your own command line parameters now known by the
	 * BaseApplication.
	 */
	class BaseApplication  : public QThread, public virtual SystemObject {
	Q_OBJECT
	public:
		BaseApplication();
		virtual ~BaseApplication();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();
		virtual QString getName() const;

	signals:
		void quitMainApplication();
		void showGui();

	public slots:
		void startApplication();

	protected:
		virtual void run();

		virtual bool setupGui();
		virtual bool setupApplication() = 0;
		virtual bool runApplication() = 0;

		virtual void loadValues(QStringList fileNames);

		virtual bool setUp();
		virtual bool startSystem();

	protected:
		QList<QString> mPreInitValueFileName;
		QList<QString> mPostInitValueFileName;
		bool mSetUp;
		bool mShutDown;
		QString mName;
		QList<QString> mCommandLineArguments;
		CommandLineArgument *mArgument_ival;
		CommandLineArgument *mArgument_val;
	};

}

#endif


