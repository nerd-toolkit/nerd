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



#ifndef NERDSimObject_H
#define NERDSimObject_H

#include "Core/ParameterizedObject.h"
#include "Value/StringValue.h"
#include "SimGeom.h"
#include "Math/Vector3D.h"
#include <QString>
#include <QList>
#include "PhysicalSimulationAlgorithm.h"
#include "Value/InterfaceValue.h"
#include <QList>
#include "Value/ColorValue.h"

namespace nerd {

class SimObjectGroup;

/**
 * SimObject is the base class of all objects used in the simulation.
 * Specialized SimObjects encapsulate the physics engine and implementation
 * details behind a uniform object facade. 
 * 
 */
class SimObject : public ParameterizedObject {
	public:
		SimObject(const QString &name);
		SimObject(const QString &name, const QString &prefix);
		SimObject(const SimObject &object);
		virtual ~SimObject();
		
		virtual SimObject* createCopy() const;
		
		virtual void synchronizeWithPhysicalModel(PhysicalSimulationAlgorithm *psa);
		virtual void setup();
		virtual void clear();
		
		virtual QList<InterfaceValue*> getInputValues() const;
		virtual QList<InterfaceValue*> getOutputValues() const;
		virtual QList<InterfaceValue*> getInfoValues() const;
		
		virtual bool useOutputAsInfoValue(InterfaceValue *outputValue, bool useAsInfoValue);
	
		virtual QString getName() const;
		virtual QString getAbsoluteName() const;
		virtual void setName(const QString &name);
		virtual void valueChanged(Value *value);
	
		QList<SimGeom*> getGeometries();
		void setGeometries(QVector<SimGeom*> geometries);
		bool addGeometry(SimGeom *simGeometry);
		bool removeGeometry(SimGeom *simGeometry);

		virtual void setObjectGroup(SimObjectGroup *group);
		SimObjectGroup* getObjectGroup() const;

		virtual bool addChildObject(SimObject *child);
		virtual bool removeChildObject(SimObject *child);
		QList<SimObject*> getChildObjects() const;

	protected:
		void updateInterfaceNames();
	
	protected:
		StringValue *mNameValue;
		QList<SimGeom*> mSimGeometries;
		
		QList<InterfaceValue*> mInputValues;
		QList<InterfaceValue*> mOutputValues;
		QList<InterfaceValue*> mInfoValues;
		
		SimObjectGroup *mOwnerGroup;
		bool mSynchronizingWithPhysicalModel;

		QList<SimObject*> mChildObjects;
};

}

#endif

