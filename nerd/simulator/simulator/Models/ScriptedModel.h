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


#ifndef ORCSScriptedModel_H
#define ORCSScriptedModel_H

#include <QString>
#include <QHash>
#include "Script/ScriptingContext.h"
#include "Models/ModelInterface.h"
#include "Physics/SimObject.h"
#include "Physics/SimObjectGroup.h"
#include "Collision/CollisionRule.h"
#include "Physics/SimBody.h"

namespace nerd {

	/**
	 * ScriptedModel.
	 *
	 */
	class ScriptedModel : public ScriptingContext, public ModelInterface {
	Q_OBJECT
	Q_PROPERTY(QString prototypeName WRITE setPrototypeName READ getPrototypeName);
	Q_PROPERTY(QString modelName WRITE setModelName READ getModelName);
	public:
		ScriptedModel(const QString &name, const QString &script = "");
		ScriptedModel(const ScriptedModel &other);
		virtual ~ScriptedModel();

		virtual QString getName() const;
		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);

		virtual void loadScriptFromFile(const QString &fileName);

		virtual SimObject* createCopy() const;
		virtual void createModel();
		virtual void layoutObjects();
		virtual void randomizeObjects();
		virtual void createEnvironment();

	public slots:
		void setPrototypeName(const QString &name);
		QString getPrototypeName() const;
		void setModelName(const QString &name);
		QString getModelName() const;

		void definePrototypeParameter(const QString &name, const QString &initialValue);

		int createObject(const QString &prototypeName, const QString &name);
		int copyObject(int objectId, const QString &name);
		bool setProperty(int objectId, const QString &propertyName, const QString &value);
		bool setProperty(const QString &fullPropertyName, const QString &value);
		bool hasProperty(int objectId, const QString &propertyName);
		QString getProperty(int objectId, const QString &propertyName);
		
		bool makeCurrent(int objectId);
		int getCurrent();
		bool setP(const QString &propertyName, const QString &value);

		int createCollisionRule(const QString &name, const QString &prototypeName = "EventCollisionRule");
		bool crAddSource(int collisionRule, int bodyId);
		bool crAddSource(int collisionRule, const QString &bodyRegExp);
		bool crAddTarget(int collisionRule, int bodyId);
		bool crAddTarget(int collisionRule, const QString &bodyRegExp);
		void crNegateRule(int collisionRule, bool negate);
		bool crIsNegated(int collisionRule);

		bool allowCollisions(int objectId1, int objectId2, bool allow);

		QString toVector3DString(double x, double y, double z);
		QString toColorString(double r, double g, double b, double t);

		bool hasEnvironmentSection();
		bool hasModelSection();

	protected:
		virtual void reportError(const QString &message);
		virtual void importVariables();
		virtual void addCustomScriptContextStructures();

	protected:
		QString mPrototypeName;
		int mIdCounter;
		QHash<int, SimObject*> mSimObjectsLookup;
		QHash<int, SimObject*> mEnvironmentObjectLookup;
		QHash<int, CollisionRule*> mCollisionRulesLookup;
		SimObjectGroup *mAgent;
		SimObject *mCurrentSimObject;
		QHash<StringValue*, QString> mPrototypeParameters;
		bool mEnvironmentMode;
		bool mSetupEnvironmentMode;
		bool mRandomizationMode;
		
	};

}

#endif



