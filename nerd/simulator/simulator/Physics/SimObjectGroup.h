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



#ifndef NERDSimObjectGroup_H
#define NERDSimObjectGroup_H

#include "Physics/SimObject.h"
#include <QList>
#include <QString>
#include "Control/ControlInterface.h"

namespace nerd {

	/**
	 * SimObjectGroup
	 */
	class SimObjectGroup : public ControlInterface {	
	public:
		SimObjectGroup(const QString &name, const QString &type);
		SimObjectGroup(const SimObjectGroup &other);
		virtual ~SimObjectGroup();
		
		const QString& getName() const;
		void setName(const QString &name);
		const QString& getType() const;
		
		void clearGroup();
		bool addObject(SimObject *object);
		bool removeObject(SimObject *object);
		
		const QList<SimObject*>& getObjects() const;
		
		virtual QList<InterfaceValue*> getInputValues() const;
		virtual QList<InterfaceValue*> getOutputValues() const;
		virtual QList<InterfaceValue*> getInfoValues() const;

	private:		

		QList<SimObject*> mObjectGroup;
		QString mName;
		QString mType;

	};

}

#endif


