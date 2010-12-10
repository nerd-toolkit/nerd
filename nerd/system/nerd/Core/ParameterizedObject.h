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



#ifndef NERDPARAMETERIZEDOBJECT_H
#define NERDPARAMETERIZEDOBJECT_H

#include <QHash>
#include <QList>
#include <QString>
#include "Value/Value.h"
#include "Value/ValueChangedListener.h"



namespace nerd {

/**
 * ParameterizedObject
 *
 * A ParameterizedObject provides methods to add Value objects as named
 * parameters. Other objects can access all parameters through the
 * get parameter methods.
 *
 * ParameterizedObjects can choose to publish their parameters while adding.
 * In this case the parameter is published at the ValueManager on a
 * value path that is created from a prefix and the parameter name.
 * Each ParameterizedObject can set its own prefix and therefore  get its
 * own parameter space at the ValueManager.
 *
 * Note: It is recommended to add parameters only before the bind() phase
 * of the Core. If a ParameterizedObject is created later it still can publish
 * its parameters at the Core, but dependent objects might not be able to
 * bind() to these parameters if they do not explicitely after the paramters
 * have been added.
 * It is also recommendet NOT to remove parameters from the ValueManager.
 * This happens if a ParameterizedObjects is destroyed. Objects, that are
 * planned to be destroyed at runtime should not publish their values until
 * a rebinding method is implemented to ensure, that binding objects are notified
 * when parameters are removed to avoid working with destroyed parameters.
 *
 * TODO Take care that the removeParameters() method is called for
 * all ParameterizedObjects BEFORE the ValueManager is destroyed.
 * Otherwise the parameter Values will not be destroyed savely.
 */
class ParameterizedObject : public virtual ValueChangedListener {

	public:
		ParameterizedObject(const QString &name);
		ParameterizedObject(const QString &name, const QString &valuePrefix);
		ParameterizedObject(const ParameterizedObject &object);
		virtual ~ParameterizedObject();
	
		virtual bool addParameter(const QString &name, Value *value);
		virtual bool addParameter(const QString &name, Value *value, bool publish);
		virtual Value* getParameter(const QString &name) const;

		virtual bool publishParameter(Value *value, bool publish);

		virtual void removeParameters();
		virtual QList<Value*> getParameters() const;
		virtual QList<QString> getParameterNames() const;
		virtual void publishAllParameters();
		virtual void unpublishAllParameters();
	
		virtual void valueChanged(Value *value);
	
		virtual QString getName() const;
		virtual void setName(const QString &name);
		virtual QString getPrefix() const;
		void setPrefix(const QString &prefix);

		virtual bool equals(const ParameterizedObject *obj) const;

	private:
		QHash<QString, Value*> mParameters;
		QString mName;

	protected:
		QString mValuePrefix;
		ValueManager *mValueManager;
};

}

#endif
