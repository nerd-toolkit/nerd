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


#ifndef CollisionHandlerAdapter_H_
#define CollisionHandlerAdapter_H_

#include "Collision/CollisionHandler.h"

namespace nerd{

class CollisionHandlerAdapter:public CollisionHandler{
	
	public:
		CollisionHandlerAdapter(CollisionManager *manager, bool *destroyFlag = 0)
				: mDestroyFlag(destroyFlag), mManager(manager), mCountUpdate(0),
				mLastUpdateCollisionManager(0)
		{
		}


	virtual ~CollisionHandlerAdapter() {
		if(mDestroyFlag != 0) {
			(*mDestroyFlag) = true;
		}
	}

	virtual void disableCollisions(CollisionObject *firstCollisionPartner, 
						CollisionObject *secondCollisionPartner, 
						bool disable) 
	{
			if(disable) {
				Contact newC(firstCollisionPartner, secondCollisionPartner);
				mContacts.push_back(newC);
			} 
			else {
				for(int i = 0; i < mContacts.size(); i++) {
					if((mContacts.at(i).getFirstCollisionObject() == firstCollisionPartner && 
						mContacts.at(i).getSecondCollisionObject() == secondCollisionPartner) ||
						(mContacts.at(i).getFirstCollisionObject() == secondCollisionPartner  &&
						mContacts.at(i).getSecondCollisionObject() == firstCollisionPartner )) {
							mContacts.removeAt(i);
							return;
					}
				}	
			}
	}

	virtual QList<Contact> getContacts() const {
		return mContacts;
	}
	
	virtual void updateCollisionHandler(CollisionManager *cManager) {
		mLastUpdateCollisionManager = cManager;
		mCountUpdate++;
	}
	
	void clearHandler() {
		mContacts.clear();
	}


private:
	bool *mDestroyFlag;
	QList<Contact> mContacts;
	CollisionManager *mManager;
	int mCountUpdate;
	CollisionManager *mLastUpdateCollisionManager;

};
}
#endif

