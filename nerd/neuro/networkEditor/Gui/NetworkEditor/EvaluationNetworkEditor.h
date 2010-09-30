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



#ifndef NERDEvaluationNetworkEditor_H
#define NERDEvaluationNetworkEditor_H

#include <QString>
#include <QHash>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include <QAction>
#include "Event/EventListener.h"
#include "Core/SystemObject.h"
#include "Value/ValueChangedListener.h"

namespace nerd {

	/**
	 * EvaluationNetworkEditor.
	 *
	 */
	class EvaluationNetworkEditor : public NeuralNetworkEditor, 
									public virtual EventListener,
									public virtual SystemObject,
									public virtual ValueChangedListener
	{
	Q_OBJECT
	public:
		EvaluationNetworkEditor(bool enableManualStasisControl = true);
		virtual ~EvaluationNetworkEditor();

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual QString getName() const;
		virtual void eventOccured(Event *event);
		virtual void valueChanged(Value *value);

		virtual int addNetworkVisualization(const QString &name);

		virtual void renameCurrentNetwork(const QString &name);


	public slots:
		void networksReplaced();
		void destroyAllTabs();
		void overwriteCurrentNetwork();

	signals:
		void networksReplacedEvent();
		void destroyAllTabsEvent();	

	protected:
		virtual void setupMenuBar();
		virtual QMenu* addFileMenu();
		virtual QMenu* addControlMenu();
		virtual void setupTools();
		virtual void extendEditMenu(QMenu *editMenu);

		virtual void handleNetworksReplaced();
		virtual void handleNetworksModified();

	protected:
		Event *mNetworksReplacedEvent;
		Event *mNetworksModifiedEvent;
		BoolValue *mStasisValue;
		QMenu *mFileMenu;
		bool mFirstNetwork;
		bool mEnableManualStasisControl;
		QAction *mSaveNetworkAction;

	};

}

#endif



