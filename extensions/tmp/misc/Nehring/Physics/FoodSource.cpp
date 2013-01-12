/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
 *                                                                         *
 *   Nehring library by Jan Nehring (Diploma Thesis)                       *
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



#include "FoodSource.h"

using namespace std;

#define PARAM(type, paraObj, paraName) \
		(dynamic_cast<type*>(paraObj->getParameter(paraName)))

namespace nerd {


/**
 * Constructs a new FoodSource.
 */
FoodSource::FoodSource(const QString &name)
	: SimBody(name)
{ 	
	mFoodAmount = new InterfaceValue("", "FoodAmount", 1, 0.0, 3.0);
	mFoodControl = new InterfaceValue("", "FoodControl", 1, 0.0, 3.0);
	mRange = new DoubleValue( 1 );
	
	addParameter("FoodAmount", mFoodAmount);
	addParameter("FoodControl", mFoodControl);
	addParameter("Range", mRange);
	
//	mOutputValues.append(mFoodControl);
//	mInputValues.append(mFoodAmount);
}

void FoodSource::setup(){
	SimBody::setup();
	if(mBodyCollisionObject != 0) {
		mBodyCollisionObject->setOwner(this);
		mBodyCollisionObject->setHostBody(this);
		SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
		if(geom != 0) {
			geom->setRadius(mFoodAmount->get());
		}
	}

	PhysicsManager *pm = Physics::getPhysicsManager();
	QString name = getName();
	SimObjectGroup *group = new SimObjectGroup(name, "FoodSource");
	
	//gruenen kreis erzeugen
	SimObject *cylinder
		= pm->getPrototype(SimulationConstants::PROTOTYPE_CYLINDER_BODY_NO_PHYSICS );
	SimObject* body = cylinder->createCopy();
	PARAM(StringValue, body, "Name")->set("/Body");
	PARAM(ColorValue, body, "Color")->set(0, 255, 0);
	PARAM(Vector3DValue, body, "Position")->set(0, 0.0005, 0);
	PARAM(DoubleValue, body, "Radius")->set( getRadius() );
	PARAM(DoubleValue, body, "Length")->set(0.0005);
	PARAM(Vector3DValue, body, "Orientation")->set(-90, 0, 0);
	PARAM(BoolValue, body, "Dynamic")->set(false);
	group->addObject( body );

 	//lichtquelle erzeugen
//  	SimObject *light
//  		= pm->getPrototype(SimulationConstants::PROTOTYPE_LIGHT_SOURCE)->createCopy();
//  	PARAM(StringValue, light, "Name")->set("/Light");
//  	PARAM(Vector3DValue, light, "Position")->set(0.0,0.0,0.0);
//  	PARAM(DoubleValue, light, "Range")->set(0.5);
//  	PARAM(ColorValue, light, "Color")->set( 0.0, 255, 0.0 );
// 	PARAM(BoolValue, light, "UseSphericLightCone")->set( false );
//  	group->addObject( light );

	pm->addSimObjectGroup(group);

	//createCollisionObject();
}


/**
 * Copy constructor. 
 * 
 * @param other the FoodSource object to copy.
 */
FoodSource::FoodSource(const FoodSource &other) 
	: Object(), ValueChangedListener(), SimBody(other)
{
	mFoodAmount =  dynamic_cast<InterfaceValue*>(getParameter("FoodAmount"));
	mFoodControl =  dynamic_cast<InterfaceValue*>(getParameter("FoodControl"));
	mRange = dynamic_cast<DoubleValue*>(getParameter( "Range" ));
	mOutputValues.clear();
	mInputValues.clear();

	mOutputValues.append(mFoodAmount);
	mInputValues.append(mFoodControl);

	createCollisionObject();
}

/**
 * Destructor.
 */
FoodSource::~FoodSource() {
}


SimBody* FoodSource::createCopy() const {
	return new FoodSource(*this);
}

double FoodSource::getRadius(){
	double amount = mFoodAmount->get();
	return amount * 0.1;
}

double FoodSource::exploit(){
	double amount = mFoodAmount->get();
 	amount -= 0.1;
 	mFoodControl->set( amount );
// 	//PARAM(DoubleValue, mCylinderBody, "Radius")->set( getRadius() );
	//amount = test->get();
	//test->set( amount-0.1);
	return 0.1;
}
	
void FoodSource::clear() {
	SimBody::clear();
}


		
void FoodSource::valueChanged(Value *value) {

 	SimBody::valueChanged(value);
 	if(value == 0) {
 		return;
 	}
 	if(value == mFoodAmount ) {
 		SphereGeom *geom = dynamic_cast<SphereGeom*>(mBodyCollisionObject->getGeometry());
 		if(geom != 0) {
 			geom->setRadius(getRadius());
 		}
 	}
 	else if(value == mHideLightCone) {
 		updateSensorValues();
 	}
}


void FoodSource::updateSensorValues() {
	mFoodAmount->set(mFoodControl->get());

	//Set transparency to a value between 0 and 80, depending on the current brightness.
	//80 is about 30 percent of full opacity (255) at max.
	Color color = mBodyCollisionObject->getGeometry()->getColor();
	color.setAlpha(0); 
	mBodyCollisionObject->getGeometry()->setColor(color);
}

void FoodSource::createCollisionObject() {
	mBodyCollisionObject = new CollisionObject(
		SphereGeom(this, mFoodAmount->get()));
	addGeometry(mBodyCollisionObject->getGeometry());
 	mBodyCollisionObject->setMaterialType("Light");
 	mBodyCollisionObject->setTextureType("None");
	mBodyCollisionObject->getGeometry()->setColor( 0, 0, 0, 0 );
	addCollisionObject(mBodyCollisionObject);
}


double FoodSource::getRange() const {
	return mRange->get();
}


void FoodSource::setRange(double range) {
	mRange->set(range);
}

}



