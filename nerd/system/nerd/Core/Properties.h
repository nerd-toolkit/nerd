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



#ifndef NERDProperties_H
#define NERDProperties_H

#include <QString>
#include <QHash>

namespace nerd {
	
	class PropertyChangedListener;

	/**
	 * Properties.
	 *
	 * Properties is a class similar to Properties at Java. Properties are
	 * QString pairs, one QString acting as a key, the other as value.
	 *
 	 * Properties can be stored to and loaded from files in a simple char based format.
	 */
	class Properties {
	public:
		Properties();
		Properties(const Properties &other);
		virtual ~Properties();

		virtual void setOptionalHiddenPrefixes(QList<QString> prefixes);
		virtual QList<QString> getOptionalHiddenPrefixes() const;

		virtual void setProperty(const QString &name, const QString &value = QString(""));
		virtual QString getProperty(const QString &name) const;
		virtual bool hasExactProperty(const QString &prefixedName) const;
		virtual bool hasProperty(const QString &name) const;
		virtual void removeProperty(const QString &name);

		virtual QList<QString> getPropertyNames() const;

		virtual QString getPropertyList() const;
		virtual bool saveToFile(const QString &fileName);
		virtual bool loadFromFile(const QString &fileName);

		bool addPropertyChangedListener(PropertyChangedListener *listener);
		bool removePropertyChangedListener(PropertyChangedListener *listener);

		virtual bool equals(Properties *properties) const;

	private:
		QHash<QString, QString> mProperties;
		QList<PropertyChangedListener*> mListeners;
		QList<QString> mOptionalHiddenPrefixes;
	};

}

#endif



