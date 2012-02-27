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



#ifndef NERDInterfaceValue_H
#define NERDInterfaceValue_H

#include "Value/NormalizedDoubleValue.h"
#include <QString>

namespace nerd {

	/**
	 * InterfaceValue
	 */
	class InterfaceValue : public NormalizedDoubleValue {
	public:
		InterfaceValue(const QString &prefix, const QString &name);
		InterfaceValue(const QString &prefix, const QString &name, double value, 
					   double min, double max);
		InterfaceValue(const QString &prefix, const QString &name, double value, 
					   double min, double max, double normalizedMin, double normalizedMax);
		InterfaceValue(const InterfaceValue &value);
		virtual ~InterfaceValue();

		virtual Value* createCopy();

		virtual QString getName() const;
		
		virtual QString getInterfaceName() const;
		virtual void setInterfaceName(const QString &name);
		
		virtual QString getPrefix() const;
		virtual void setPrefix(const QString &prefix);

		virtual bool setProperties(const QString &properties);
		virtual QString getProperties() const;

		virtual bool equals(const Value *value) const;

	private:
		QString mInterfaceName;
		QString mPrefix;
	};

}

#endif


