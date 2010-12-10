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


#include "KeyFramePlayerWrapper.h"

#include "Core/Core.h"
#include "Optimization/Tools.h"
#include "OrcsModelOptimizerConstants.h"
#include "PlugIns/KeyFramePlayer.h"
#include <QFile>

namespace nerd {

KeyFramePlayerWrapper::KeyFramePlayerWrapper(const QString &configPath)
	: Player(configPath),
		mKeyFramePlayer(0),
		mIsInitOK(true)
{
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	StringValue *mAgent = Tools::getStringValue(vm,
																							configPath + 
																								OrcsModelOptimizerConstants::VM_AGENT_VALUE,
																							Tools::EM_ERROR,
											 												getName(),
											 												mIsInitOK);
	
	
	mKeyFramePlayer = new KeyFramePlayer(mAgent->get());
}

bool KeyFramePlayerWrapper::init()
{
	bool ok = Player::init();
	
	ok &= mIsInitOK;
	
	return ok;
}

QString KeyFramePlayerWrapper::getName() const{
	return "KeyFramePlayerWrapper";
}

bool KeyFramePlayerWrapper::setSource(const QString &source) 
{
	if(QFile::exists(source) == false){
		Core::log(QString("%1: ERROR - Can not find file [%2]!").arg(
												getName(),
												source));
		return false;	
	}
	
	return Player::setSource(source);
}


void KeyFramePlayerWrapper::sourceChanged() 
{	
	mKeyFramePlayer->setKeyFrameFile(mSource->get());
}


} // namespace nerd
