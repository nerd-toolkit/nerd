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



#ifndef NERDMotorAdapter_H
#define NERDMotorAdapter_H

#include <QString>
#include <QHash>
#include "Physics/SimSensor.h"
#include "Physics/SimActuator.h"
#include "Value/StringValue.h"
#include "Value/BoolValue.h"
#include "MotorModels/MotorModel.h"
#include <QList>
#include "Value/InterfaceValue.h"

namespace nerd {


	/**
	 * MotorAdapter.
	 *
	 * All non-virtual subclasses have to call initializeConstruction in their constructor, initializeCopyConstruction in
	 * their copy-constructor and returnObject->collectCompatibleModels() at the end of their createCopy method!
	 */
	class MotorAdapter
			: public virtual SimSensor, public virtual SimActuator 
	{
	public:
		MotorAdapter(int jointType);
		MotorAdapter(const MotorAdapter &other);
		virtual ~MotorAdapter();

		virtual SimObject* createCopy() const = 0;

		virtual void setup();
		virtual void clear();

		virtual QString getAbsoluteName() const = 0;

		virtual void updateSensorValues();
		virtual void updateActuators();

		MotorModel* getActiveMotorModel() const;

		virtual bool addParameter(const QString &name, Value *value) = 0;
		virtual Value* getParameter(const QString &name) const = 0;
		

		virtual QList<InterfaceValue*> getInputValues() const = 0;
		virtual QList<InterfaceValue*> getOutputValues() const = 0;

		virtual void valueChanged(Value *value);

	protected:
		virtual bool isValidMotorModel(MotorModel *model) const = 0;
		void initializeConstruction();
		void initializeCopyConstruction();
		virtual void collectCompatibleMotorModels();

	protected:
		MotorModel *mActiveMotorModel;
		StringValue *mActiveMotorModelName;

		QHash<QString, MotorModel*> mMotorModels;
		int mJointType;

	};

}

#endif



