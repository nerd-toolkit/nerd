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



#ifndef MaterialProperties_H_
#define MaterialProperties_H_
#include <QString>
#include <QMap>
#include "Value/DoubleValue.h"

namespace nerd {

/**
* MaterialProperties stores the friction information between different materials. Currently
* only materials, that are used and needed for the simulation of the A-Series robot are
* supported. 
* Possible extension is the possibility to load a list/table of material properties from a 
* .txt file.
**/

class MaterialProperties {
	public:
		static int MATERIAL_NUMBER_LIGHT;
		static int MATERIAL_NUMBER_CUSTOM1;
		static int MATERIAL_NUMBER_CUSTOM2;

	public:
		MaterialProperties();
		~MaterialProperties();

		int getMaterialType(const QString &materialName);
		double getStaticFriction(int material1, int material2);
		double getDynamicFriction(int material1, int material2);
		double getRestitution(int material1, int material2);
	
		int getNumberOfMaterials() const;
		QString getMaterialName(int type) const;

		bool addTexture(const QString &textureName);
		int getTextureIndex(const QString &textureName);
		
		QList<QString> getMaterialNames() const;
		void addMaterialNamesAsValuePresets(StringValue *value);

	private:
		QMap<QString, int> mMaterialLookUp;
		QList<QString> mTextureLookUp;

		double** mStaticFrictionMatrix;
		double** mDynamicFrictionMatrix;
		double** mRestitutionMatrix;
		DoubleValue *mMaterialDynamicFrictionCustom1;
		DoubleValue *mMaterialStaticFrictionCustom1;
		DoubleValue *mMaterialRestitutionCustom1;
		DoubleValue *mMaterialDynamicFrictionCustom2;
		DoubleValue *mMaterialStaticFrictionCustom2;
		DoubleValue *mMaterialRestitutionCustom2;
	
};
}
#endif

