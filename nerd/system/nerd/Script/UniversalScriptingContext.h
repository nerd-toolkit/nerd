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



#ifndef NERDUniversalScriptingContext_H
#define NERDUniversalScriptingContext_H

#include <QString>
#include <QHash>
#include "Script/ScriptingContext.h"
#include "PlugIns/CommandLineArgument.h"
#include "Core/SystemObject.h"
#include "Event/Event.h"
#include "NerdConstants.h"
#include "Value/StringValue.h"

namespace nerd {

	/**
	 * UniversalScriptingContext.
	 *
	 */
	class UniversalScriptingContext : public ScriptingContext, public virtual SystemObject {

	public:
		UniversalScriptingContext(const QString &name, const QString &fileName = "",
				const QString &triggerEventName = "", const QString &resetEventName = "");
		virtual ~UniversalScriptingContext();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);

		void setTriggerEventName(const QString &triggerEventName);
		QString getTriggerEventName() const;
		void setResetEventName(const QString &resetEventName);
		QString getResetEventName() const;
		void setScriptFileName(const QString &fileName);

		virtual bool loadScriptCode(bool replaceExistingCode);

	private:
		UniversalScriptingContext(const UniversalScriptingContext &other);

	protected:
		bool mInitialized;
		StringValue *mTriggerEventName;
		StringValue *mResetEventName;
		Event *mTriggerEvent;
		Event *mResetEvent;
	};

}

#endif



