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

#ifndef ModelInterface_H_
#define ModelInterface_H_

#include "Physics/SimObject.h"
#include "Physics/SimObjectGroup.h"
#include "Value/Vector3DValue.h"
#include "Event/EventListener.h"
#include "Event/Event.h"
#include "Physics/SimObjectGroup.h"


namespace nerd{


/**
 * ModelInterface. Interface class to create new simulation models that can be loaded using the SimpleEnvironmentParser. Two methods need to be implemented: createCopy: this method should simply return an empty instance of the class. createModel: in this method all objects, objectGroups and collisionObjects should be created and added to the physics.
**/

class ModelInterface : public SimObject, public virtual EventListener {

	public:
		ModelInterface(const QString &name);	
		ModelInterface(const ModelInterface &model);
		virtual ~ModelInterface();

		virtual void eventOccured(Event *event);
		virtual QString getName() const;
		virtual void setName(const QString &name); 

		virtual SimObject* createCopy() const = 0;
		virtual void createModel() = 0;
		virtual void setup();
		void addTransformation(const QString &transformationType, 
							   const Vector3D &transformation);

		virtual void reset();

		void switchInput(const QString &simObjectName, const QString &valueName, bool makeInfo);
		void switchInputs();

		virtual void performTransformations();
		virtual void layoutObjects();
		virtual void randomizeObjects();

		SimObjectGroup* getAgentInterface() const;

		virtual void valueChanged(Value *value);

	protected:
		Vector3DValue *mPosition;
		Vector3DValue *mOrientation;

		QList<SimObject*> mSimObjects;
		QList<QString> mTransformationNames;
		QList<Vector3D> mTransformationData;

		Event *mResetSettingsCompleted;
		Event *mRandomizeEnvironmentEvent;

		bool mIsInitialized;

		QString mMorphologyParametersPrefix;
	
		QList<QString> mSwitchInputObjectNames;
		QList<QString> mSwitchInputValueNames;
		QList<bool> mSwitchInputMakeInfoFlags;

		SimObjectGroup *mAgent;
};
}
#endif

