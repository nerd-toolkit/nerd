/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   OrcsModelOptimizer by Chris Reinke (creinke@uni-osnabrueck.de         *
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


#include "Player.h"


#include "OrcsModelOptimizerConstants.h"
#include "Core/Core.h"
#include "Value/StringValue.h"
#include "Optimization/Tools.h"

// Player
#include "KeyFramePlayerWrapper.h"
#include "PhysicalDataPlayer.h"


namespace nerd {

Player* Player::createPlayer(const QString &configPath)
{
	bool ok = true;
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	StringValue *type = Tools::getStringValue(vm,
																						configPath +
																								OrcsModelOptimizerConstants::VM_TYPE_VALUE,
																						Tools::EM_ERROR,
																						"Player",
																						ok);	
		
	if(ok == false){
		return 0;
	}
			
	
	if(type->get() == OrcsModelOptimizerConstants::PLAYER_KEYFRAME) {
		return new KeyFramePlayerWrapper(	configPath );
  }

	if(type->get() == OrcsModelOptimizerConstants::PLAYER_PHYSICALDATA) {
		return new PhysicalDataPlayer(	configPath );
	}

  // error if the type is unknown
	Core::log(QString("Player: ERROR - Do not know player type \"%1\"]. Possible types are: \"%2\", \"%3\"").arg(
								type->get(),
								OrcsModelOptimizerConstants::PLAYER_KEYFRAME,
								OrcsModelOptimizerConstants::PLAYER_PHYSICALDATA));
  return 0;
}
	
Player::Player(const QString &configPath) 
	: mSource(0)
{
	if(configPath.right(1) == "/"){
		mConfigPath = configPath;
	}
	else {
		mConfigPath = configPath + "/";
	}
	
	Core::getInstance()->addSystemObject(this);
}

QString Player::getName() const{
	return "Player";
}

bool Player::init()
{
	bool ok = true;
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	QString sourceValuePath = mConfigPath + "Source";
	
	mSource = vm->getStringValue(sourceValuePath);
	if(mSource == 0) {
		mSource = new StringValue("");
		ok &= vm->addValue(sourceValuePath, mSource);
	}
	
	mSource->addValueChangedListener(this);
	
	return ok;
}

bool Player::bind()
{
	return true;
}

bool Player::cleanUp()
{
	return true;
}

bool Player::setSource(const QString &source)
{
	mSource->set(source);
	
	return true;
}

void Player::valueChanged(Value *value)
{
	if(value == 0){
		return;
	}
	
	if(value == mSource){
		sourceChanged();
	}
}

	

} // namespace nerd
