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



#ifndef MotorControlPanel_H_
#define MotorControlPanel_H_
#include <QFrame>
#include <QLabel>
#include <QSlider>
#include "MotorControlGui.h"
#include "Value/ValueChangedListener.h"
#include "Event/EventListener.h"

namespace nerd{

class InterfaceValue;

class MotorControlPanel : public QFrame, public virtual ValueChangedListener, 
					public virtual EventListener
{
	Q_OBJECT
	public:
		MotorControlPanel(InterfaceValue *valueToControl, QWidget *parent = 0);
		virtual ~MotorControlPanel();	
	
		virtual void valueChanged(Value *value);
		virtual void eventOccured(Event *event);
		virtual QString getName() const;

		virtual void unbind();

		virtual double getSliderValue() const;
		virtual void setSliderValue(double value);

	public slots:
		void setValue(int value);
	
	signals:
		void actuatorNameChanged(const QString &name);
		
	
	private:
		double mMin;
		double mMax;
		QLabel *mNameLabel;
		InterfaceValue *mControlledValue;
		QSlider *mControlSlider;
		Event *mResetEvent;
};

}
#endif

