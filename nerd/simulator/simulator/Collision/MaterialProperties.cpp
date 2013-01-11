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
 *   Copyright (C) 2008 - 2013 by the Neurocybernetics Group Osnabrück     *
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



#include "MaterialProperties.h"
#include "Core/Core.h"
#include "Value/ValueManager.h"
#include <Value/StringValue.h>

namespace nerd {


int MaterialProperties::MATERIAL_NUMBER_LIGHT = 8;
int MaterialProperties::MATERIAL_NUMBER_CUSTOM1 = 9;
int MaterialProperties::MATERIAL_NUMBER_CUSTOM2 = 10;

// TODO: read settings from a Material-file
/**
 * Constructor. Fills the Lookup-table with default material names and friction coefficients.
 */
MaterialProperties::MaterialProperties() {

	mMaterialDynamicFrictionCustom1 = new DoubleValue(0.8);
	mMaterialStaticFrictionCustom1 = new DoubleValue(1.2);
	mMaterialRestitutionCustom1 = new DoubleValue(0.1);
	mMaterialDynamicFrictionCustom2 = new DoubleValue(0.8);
	mMaterialStaticFrictionCustom2 = new DoubleValue(1.2);
	mMaterialRestitutionCustom2 = new DoubleValue(0.1);

	ValueManager *vm = Core::getInstance()->getValueManager();
	vm->addValue("/MaterialProperties/Custom1/DynamicFriction", mMaterialDynamicFrictionCustom1);
	vm->addValue("/MaterialProperties/Custom1/StateFriction", mMaterialStaticFrictionCustom1);
	vm->addValue("/MaterialProperties/Custom1/Restitution", mMaterialRestitutionCustom1);
	vm->addValue("/MaterialProperties/Custom2/DynamicFriction", mMaterialDynamicFrictionCustom2);
	vm->addValue("/MaterialProperties/Custom2/StateFriction", mMaterialStaticFrictionCustom2);
	vm->addValue("/MaterialProperties/Custom2/Restitution", mMaterialRestitutionCustom2);

	mMaterialLookUp["PlasticsWhite"] = 0;
	mMaterialLookUp["PlasticsGray"] = 1;
	mMaterialLookUp["ABS"] = 2;
	mMaterialLookUp["Latex"] = 3;
	mMaterialLookUp["PVC"] = 4;
	mMaterialLookUp["Wood"] = 5;
	mMaterialLookUp["Carpet"] = 6;
	mMaterialLookUp["Default"] = 7;
	mMaterialLookUp["Light"] = MATERIAL_NUMBER_LIGHT; //8
	mMaterialLookUp["Custom1"] = MATERIAL_NUMBER_CUSTOM1;
	mMaterialLookUp["Custom2"] = MATERIAL_NUMBER_CUSTOM2;

	addTexture("PlasticsWhite");
	addTexture("PlasticsGray");
	addTexture("ABS");
	addTexture("Latex");
	addTexture("PVC");
	addTexture("Wood");
	addTexture("Carpet");
	addTexture("Default");
	addTexture("Custom1");
	addTexture("Custom2");
	
	mStaticFrictionMatrix = new double*[mMaterialLookUp.size()];
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		mStaticFrictionMatrix[i] = new double[mMaterialLookUp.size()];
	}

	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		for(int j = 0; j < mMaterialLookUp.size(); j++) {
			mStaticFrictionMatrix[i][j] = 0.0;
		}
	}
	mStaticFrictionMatrix[0][0] = 0.11935;

	mStaticFrictionMatrix[0][1] = 0.08835;
	mStaticFrictionMatrix[1][0] = 0.08835;

	mStaticFrictionMatrix[0][4] = 0.14024;
	mStaticFrictionMatrix[4][0] = 0.14024;

	mStaticFrictionMatrix[0][5] = 0.0955;
	mStaticFrictionMatrix[5][0] = 0.0955;

	mStaticFrictionMatrix[0][6] = 0.3119;
	mStaticFrictionMatrix[6][0] = 0.3119;

	mStaticFrictionMatrix[1][6] = 0.36235;
	mStaticFrictionMatrix[6][1] = 0.36235;

	mStaticFrictionMatrix[1][4] = 0.24739;
	mStaticFrictionMatrix[4][1] = 0.24739;
	
	mStaticFrictionMatrix[1][5] = 0.2297;
	mStaticFrictionMatrix[5][1] = 0.2297;

	mStaticFrictionMatrix[2][4] = 0.20874;
	mStaticFrictionMatrix[4][2] = 0.20874;

	mStaticFrictionMatrix[2][5] = 0.2237;
	mStaticFrictionMatrix[5][2] = 0.2237;

	mStaticFrictionMatrix[2][6] = 0.55167;
	mStaticFrictionMatrix[6][2] = 0.55167;

	mStaticFrictionMatrix[3][4] = 0.83462;
	mStaticFrictionMatrix[4][3] = 0.83462;

	mStaticFrictionMatrix[3][5] = 1.0135;
	mStaticFrictionMatrix[5][3] = 1.0135;

	mStaticFrictionMatrix[3][6] = 1.25193;
	mStaticFrictionMatrix[6][3] = 1.25193;

	mStaticFrictionMatrix[2][2] = 0.8097;

	mStaticFrictionMatrix[0][2] = 0.2659;
	mStaticFrictionMatrix[2][0] = 0.2659;

	mStaticFrictionMatrix[2][1] = 0.6782;
	mStaticFrictionMatrix[1][2] = 0.6782;

	mStaticFrictionMatrix[0][7] = 0.83462;
	mStaticFrictionMatrix[1][7] = 0.83462;
	mStaticFrictionMatrix[2][7] = 0.83462;
	mStaticFrictionMatrix[3][7] = 0.83462;
	mStaticFrictionMatrix[4][7] = 0.83462;
	mStaticFrictionMatrix[5][7] = 0.83462;
	mStaticFrictionMatrix[6][7] = 0.83462;

	mStaticFrictionMatrix[7][0] = 0.83462;
	mStaticFrictionMatrix[7][1] = 0.83462;
	mStaticFrictionMatrix[7][2] = 0.83462;
	mStaticFrictionMatrix[7][3] = 0.83462;
	mStaticFrictionMatrix[7][4] = 0.83462;
	mStaticFrictionMatrix[7][5] = 0.83462;
	mStaticFrictionMatrix[7][6] = 0.83462;
	mStaticFrictionMatrix[7][7] = 0.83462;

	mDynamicFrictionMatrix = new double*[mMaterialLookUp.size()];
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		mDynamicFrictionMatrix[i] = new double[mMaterialLookUp.size()];
	}

	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		for(int j = 0; j < mMaterialLookUp.size(); j++) {
			mDynamicFrictionMatrix[i][j] = 0.0;
		}
	}

	mDynamicFrictionMatrix[0][0] = 0.09548;

	mDynamicFrictionMatrix[0][1] = 0.07363;
	mDynamicFrictionMatrix[1][0] = 0.07363;

	mDynamicFrictionMatrix[0][6] = 0.297;
	mDynamicFrictionMatrix[6][0] = 0.297;

	mDynamicFrictionMatrix[0][4] = 0.15516;
	mDynamicFrictionMatrix[4][0] = 0.15516;

	mDynamicFrictionMatrix[0][5] = 0.1343;
	mDynamicFrictionMatrix[5][0] = 0.1343;

	mDynamicFrictionMatrix[4][1] = 0.25034;
	mDynamicFrictionMatrix[1][4] = 0.25034;

	mDynamicFrictionMatrix[5][1] = 0.215;
	mDynamicFrictionMatrix[1][5] = 0.215;

	mDynamicFrictionMatrix[1][6] = 0.3004;
	mDynamicFrictionMatrix[6][1] = 0.3004;

	mDynamicFrictionMatrix[2][4] = 0.20874;
	mDynamicFrictionMatrix[4][2] = 0.20874;

	mDynamicFrictionMatrix[2][5] = 0.2058;
	mDynamicFrictionMatrix[5][2] = 0.2058;

	mDynamicFrictionMatrix[2][6] = 0.47116;
	mDynamicFrictionMatrix[6][2] = 0.47116;

	mDynamicFrictionMatrix[3][4] = 0.83462;
	mDynamicFrictionMatrix[4][3] = 0.83462;

	mDynamicFrictionMatrix[3][5] = 1.1923;
	mDynamicFrictionMatrix[5][3] = 1.1923;

	mDynamicFrictionMatrix[3][6] = 1.237;
	mDynamicFrictionMatrix[6][3] = 1.237;
	
	mDynamicFrictionMatrix[2][2] = 0.5677;

	mDynamicFrictionMatrix[0][2] = 0.239;
	mDynamicFrictionMatrix[2][0] = 0.239;

	mDynamicFrictionMatrix[2][1] = 0.4183;
	mDynamicFrictionMatrix[1][2] = 0.4183;

	mDynamicFrictionMatrix[0][7] =1.237;
	mDynamicFrictionMatrix[1][7] =1.237;
	mDynamicFrictionMatrix[2][7] =1.237;
	mDynamicFrictionMatrix[3][7] =1.237;
	mDynamicFrictionMatrix[4][7] =1.237;
	mDynamicFrictionMatrix[5][7] =1.237;
	mDynamicFrictionMatrix[6][7] =1.237;


	mDynamicFrictionMatrix[7][1] =1.237;
	mDynamicFrictionMatrix[7][2] =1.237;
	mDynamicFrictionMatrix[7][3] =1.237;
	mDynamicFrictionMatrix[7][4] =1.237;
	mDynamicFrictionMatrix[7][5] =1.237;
	mDynamicFrictionMatrix[7][6] =1.237;

	mDynamicFrictionMatrix[7][7] =1.237;

	mRestitutionMatrix = new double*[mMaterialLookUp.size()];
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		mRestitutionMatrix[i] = new double[mMaterialLookUp.size()];
	}
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		for(int j = 0; j < mMaterialLookUp.size(); j++) {
			mRestitutionMatrix[i][j] = 0.1;
		}
	}
	mRestitutionMatrix[7][4] = 0.5;
	mRestitutionMatrix[7][5] = 0.5;
	mRestitutionMatrix[4][7] = 0.5;
	mRestitutionMatrix[5][7] = 0.5;
}


MaterialProperties::~MaterialProperties() {

	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		delete[] mRestitutionMatrix[i];
	}	
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		delete[] mDynamicFrictionMatrix[i];
	}
	for(int i = 0; i < mMaterialLookUp.size(); i++) {
		delete[] mStaticFrictionMatrix[i];
	}
	delete[] mDynamicFrictionMatrix;
	delete[] mStaticFrictionMatrix;
	delete[] mRestitutionMatrix;

	mMaterialLookUp.clear();

}

/**
 * Maps the string-definition of the material to the index, saved in the MaterialProperties.
 * @param materialName The name of the material.
 * @return The index of the material.
 */
int MaterialProperties::getMaterialType(const QString &materialName) {
	if(mMaterialLookUp.contains(materialName)) {
		return mMaterialLookUp.value(materialName);
	} 
	else {
		return -1;
	}
}

/**
 * 
 * @param material1 First material
 * @param material2 Second material
 * @return The static friction coefficient for the two materials.
 */
double MaterialProperties::getStaticFriction(int material1, int material2) {
	if(material1 == MATERIAL_NUMBER_CUSTOM1 || material2 == MATERIAL_NUMBER_CUSTOM1) {
		return mMaterialStaticFrictionCustom1->get();
	}
	if(material1 == MATERIAL_NUMBER_CUSTOM2 || material2 == MATERIAL_NUMBER_CUSTOM2) {
		return mMaterialStaticFrictionCustom2->get();
	}
	if(mMaterialLookUp.size() > material1 && mMaterialLookUp.size() > material2) {
		return mStaticFrictionMatrix[material1][material2];
	}
	return 0.0;
}

/**
 * 
 * @param material1 First material
 * @param material2 Second material
 * @return The dynamic friction coefficient for the two materials.
 */
double MaterialProperties::getDynamicFriction(int material1, int material2) {
	if(material1 == MATERIAL_NUMBER_CUSTOM1 || material2 == MATERIAL_NUMBER_CUSTOM1) {
		return mMaterialDynamicFrictionCustom1->get();
	}
	if(material1 == MATERIAL_NUMBER_CUSTOM2 || material2 == MATERIAL_NUMBER_CUSTOM2) {
		return mMaterialDynamicFrictionCustom2->get();
	}
	if(mMaterialLookUp.size() > material1 && mMaterialLookUp.size() > material2) {
		return mDynamicFrictionMatrix[material1][material2];
	}
	return 0.0;
}

/**
 * 
 * @param material1 First material
 * @param material2 Second material
 * @return The restitution value for the two materials.
 */
double MaterialProperties::getRestitution(int material1, int material2) {
	if(material1 == MATERIAL_NUMBER_CUSTOM1 || material2 == MATERIAL_NUMBER_CUSTOM1) {
		return mMaterialRestitutionCustom1->get();
	}
	if(material1 == MATERIAL_NUMBER_CUSTOM2 || material2 == MATERIAL_NUMBER_CUSTOM2) {
		return mMaterialRestitutionCustom2->get();
	}
	if(mMaterialLookUp.size() > material1 && mMaterialLookUp.size() > material2) {
		return mRestitutionMatrix[material1][material2];
	}
	return 0.0;	
}

/**
 * 
 * @return The number of known material-definitions.
 */
int MaterialProperties::getNumberOfMaterials() const {
	return mMaterialLookUp.size();
}


/**
 * Maps the index of the material to the string definition.
 * @param type The index of the searched material
 * @return The name of the material with index type.
 */
QString MaterialProperties::getMaterialName(int type) const {
	return mMaterialLookUp.key(type);
}



bool MaterialProperties::addTexture(const QString &textureName) {
	if(mTextureLookUp.contains(textureName)){
		return false;
	}
	else {
		mTextureLookUp.push_back(textureName);
		return true;
	}
}

int MaterialProperties::getTextureIndex(const QString &textureName) {
	
	return mTextureLookUp.indexOf(textureName);
}


QList<QString> MaterialProperties::getMaterialNames() const {
	return mTextureLookUp;
}

void MaterialProperties::addMaterialNamesAsValuePresets(StringValue *value) {
	if(value == 0) {
		return;
	}
	value->getOptionList().append("None");
	for(QListIterator<QString> i(mTextureLookUp); i.hasNext();) {
		value->getOptionList().append(i.next());
	}
}

}
