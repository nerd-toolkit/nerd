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
 *   Copyright (C) 2008 - 2012 by the Neurocybernetics Group Osnabrück    *
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


#ifndef NERDValueFileInterfaceManager_H
#define NERDValueFileInterfaceManager_H


#include "Event/Event.h"
#include "Value/StringValue.h"
#include "Core/SystemObject.h"
#include "Core/ParameterizedObject.h"
#include "Event/EventListener.h"
#include "Value/BoolValue.h"

namespace nerd {

	/**
	 * ValueFileInterfaceManager.
	 */
	class ValueFileInterfaceManager : public virtual SystemObject, 
					public ParameterizedObject, public virtual EventListener {
	public:
		ValueFileInterfaceManager(const QString &name, const QString &ownParamPath,
				const QString &loadTriggerEventName, const QString &loadFileName,
				const QString &storeTriggerEventName, const QString &storeFileName,
				const QString &storePattern);
		virtual ~ValueFileInterfaceManager();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void eventOccured(Event *event);

		StringValue& getTypeFileNameValue() const;

	private:
		void storeValueTypeInformation();
		
	private:
		Event *mLoadFileTriggerEvent;
		Event *mStoreToFileTriggerEvent;
		Event *mRepositoryChangedEvent;
		StringValue *mAutoLoadValueFile;
		StringValue *mAutoStoreValueFile;
		StringValue *mAutoStoreValuePattern;
		BoolValue *mEnableManager;
		StringValue *mValueTypeFileName;
		QString mLoadTriggerEventName;
		QString mStoreTriggerEventName;
		
	};

}

#endif


