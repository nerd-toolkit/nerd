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



#ifndef CollisionRule_H_
#define CollisionRule_H_

#include <QList>
#include "Value/StringValue.h"
#include "Contact.h"
#include "Core/ParameterizedObject.h"
#include "Value/StringValue.h"

namespace nerd {

/**
* A CollisionRule holds two sets of CollisionObjects and can be a negated rule or not. After each collision handling the CollisionHandler calls every registered CollisionRule and informs the rules about the newly occured contacts (sequentially). Classes which inherite the abstract CollisionRule interface need to implement the method: handleContact that is called for every contact and defines, in which way the rule should react.
**/

class Event;
class EventManager;
class CollisionObject;
class SimBody;

class CollisionRule : public ParameterizedObject {

	public:
		CollisionRule(const QString &ruleName, bool negateRule = false);
		CollisionRule(const CollisionRule &rule);
		virtual ~CollisionRule();
	
		virtual CollisionRule* createCopy() const = 0;

		virtual void valueChanged(Value *value);
		
		virtual void set(QList<CollisionObject*> sourceGroup, 
						QList<CollisionObject*> targetGroup);
	
		virtual bool addToSourceGroup(CollisionObject *collisionObject);
		virtual bool addToTargetGroup(CollisionObject *collisionObject);
		virtual bool removeFromSourceGroup(CollisionObject *collisionObject);
		virtual bool removeFromTargetGroup(CollisionObject *collisionObject);
	
		virtual bool handleContact(const Contact &contact) = 0;
		
		void negateRule(bool negate);
		bool isNegated() const;
		
		QList<CollisionObject*> getSourceGroup() const;
		void setSourceGroup(QList<CollisionObject*> sourceGroup);
		QList<CollisionObject*> getTargetGroup() const;
		void setTargetGroup(QList<CollisionObject*> targetGroup);

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		StringValue* getSourceList() const;
		StringValue* getTargetList() const;

		void updateObjectList(bool source);
	
		
	protected:
		QString mRuleName;
		QList<CollisionObject*> mSourceGroup;
		QList<CollisionObject*> mTargetGroup;
		StringValue *mSourceList;
		StringValue *mTargetList;
		bool mNegateRule;
};

}
#endif

