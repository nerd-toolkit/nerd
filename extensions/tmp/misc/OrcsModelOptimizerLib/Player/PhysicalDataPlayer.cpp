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


#include "PhysicalDataPlayer.h"

#include "OrcsModelOptimizerConstants.h"
#include "SimulationConstants.h"
#include "Optimization/PhysicalDataSource.h"
#include "Optimization/Normalization.h"
#include "Optimization/Trigger.h"
#include "Optimization/Tools.h"
#include "Optimization/XMLConfigLoader.h"
#include "Value/Value.h"
#include "Value/StringValue.h"
#include "Value/DoubleValue.h"
#include "Value/IntValue.h"
#include "Event/Event.h"
#include "Event/EventManager.h"
#include "Core/Core.h"


#include <QHash>

namespace nerd {

PhysicalDataPlayer::PhysicalDataPlayer(const QString &configPath) 
	: Player(configPath),
		mConfigFilePath(0),
		mPhysicalDataSourceConfigFilePath(0),
		mPhysicalDataSource(0),
		mCurrentSimStep(0),
		mMotionLength(0)
{}

PhysicalDataPlayer::~PhysicalDataPlayer() 
{
	
	for(int i = 0; i < mPlayChannels.size(); i++)
	{
		if(mPlayChannels.at(i).normalization != 0){
			delete mPlayChannels.at(i).normalization;
		}
	}
	mPlayChannels.clear();
	mPlayValues.clear();
	mPlayValueNormalizations.clear();
	
	
	QMapIterator<QString, QPair<PhysicalDataSource*, int> > iter(mPhysicalDataSourceCache);
	while (iter.hasNext()) {
		iter.next();
		
		if(iter.value().first != 0)
		{
			delete 	iter.value().first;
		}
	}
	
}

bool PhysicalDataPlayer::init() 
{
	bool ok = Player::init();
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	//////////////////////////////////////
	// Read name of config file 
	//////////////////////////////////////
	
	mConfigFilePath = Tools::getStringValue(vm,
																					mConfigPath 
																							+ OrcsModelOptimizerConstants::VM_CONFIGFILE_VALUE,
																				 	Tools::EM_ERROR,
																				 	getName(),
																					ok);
		
	if(ok == false){
		return false;
	}
	
	//////////////////////////////////////
	// Load Configfile
	//////////////////////////////////////
	
	QHash<QString, QString> templates;
	templates.insert(OrcsModelOptimizerConstants::TEMPLATE_PLAYER_PATH,
									 mConfigPath);
	
	
	XMLConfigLoader *configLoader = new XMLConfigLoader(
											OrcsModelOptimizerConstants::CONFIG_FOLDER +	
												mConfigFilePath->get(),
	 										OrcsModelOptimizerConstants::CONFIG_FOLDER + 
													OrcsModelOptimizerConstants::FILE_PHYSICALDATAPLAYER_CONFIG_DESCRIPTION,
											templates);

	if(!configLoader->init()) {
		return false;
	}

	if(!configLoader->loadFromXML()) {
		return false;
	}

	delete configLoader;
	
	//////////////////////////////////////////
	// Read configuration
	//////////////////////////////////////////
	
	mPhysicalDataSourceConfigFilePath = Tools::getStringValue(vm,
																				mConfigPath 
																					+ OrcsModelOptimizerConstants::VM_FORMAT_VALUE, 
																				Tools::EM_ERROR, 
																				getName(), 
																				ok);
	
	return ok;
}

bool PhysicalDataPlayer::bind() 
{
	bool ok = true;
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	///////////////////////////////////////////////////////////
	// Get Event which is used as trigger to play the channels
	///////////////////////////////////////////////////////////
	
	StringValue *eventName = Tools::getStringValue(vm,
																			mConfigPath 
																				+ OrcsModelOptimizerConstants::VM_PLAYEVENT_VALUE, 
																		 Tools::EM_ERROR, 
																		 getName(), 
																		 ok);
	
	Event *playEvent = 0;
	if(ok == true)
	{	
		EventManager *em = Core::getInstance()->getEventManager();
		
		playEvent = em->registerForEvent(eventName->get(), this, true);
		if(playEvent == 0) {
			Core::log(QString("%1: ERROR - Could not find configured Event [%2].").arg(
							getName(),
							eventName->get()));
			ok = false;
		}
	}
	
	///////////////////////////////////////////////////////////
	// Go through configured channels to create mPlayChannels
	///////////////////////////////////////////////////////////
	
	// first: go through general channels
	
	QString channelListTemplate = mConfigPath 
																+ OrcsModelOptimizerConstants::VM_CHANNELS_PATH
																+ OrcsModelOptimizerConstants::VM_CHANNEL_LIST;

	QString standartNormalizationPath = mConfigPath
																			+ OrcsModelOptimizerConstants::VM_NORMALIZATION_PATH;

	QStringList normalizationPathList;

	normalizationPathList += standartNormalizationPath;
	
	ok &= readChannelConfiguration(channelListTemplate,
																 normalizationPathList,
								 								 playEvent);
	
	// second: go through normalization groups and their channels

	QString normalizationListTemplate = mConfigPath 
																			+ OrcsModelOptimizerConstants::VM_CHANNELS_PATH
																			+ OrcsModelOptimizerConstants::VM_NORMALIZATIONGROUP_LIST;
	
	int i = 1;
	
	QString bla = normalizationListTemplate.arg(i) 
			+ OrcsModelOptimizerConstants::VM_CHANNEL_LIST.arg(1)
			+ OrcsModelOptimizerConstants::VM_VALUE_VALUE;
	
	StringValue *firstChannelValueNameVal =	vm->getStringValue(
																					normalizationListTemplate.arg(i) 
																					+ OrcsModelOptimizerConstants::VM_CHANNEL_LIST.arg(1)
																					+ OrcsModelOptimizerConstants::VM_VALUE_VALUE);
	while(firstChannelValueNameVal != 0)
	{
		QStringList specialNormalizationPathList;
		
		specialNormalizationPathList += normalizationListTemplate.arg(i);
		specialNormalizationPathList += standartNormalizationPath;

		ok &= readChannelConfiguration(	normalizationListTemplate.arg(i) 
																			+ OrcsModelOptimizerConstants::VM_CHANNEL_LIST,
									 									specialNormalizationPathList,
																		playEvent);

    // try to get next value
		i++;
		firstChannelValueNameVal = vm->getStringValue(
																		normalizationListTemplate.arg(i) 
																		+ OrcsModelOptimizerConstants::VM_CHANNEL_LIST.arg(1)
																		+ OrcsModelOptimizerConstants::VM_VALUE_VALUE);
	}
	
	if(mPlayChannels.size() <= 0 && ok == true){
		Core::log(QString("%1: ERROR - Found no configuration for channels which should be played! Please check the configuration file [%2].").arg(
									getName(),
									mConfigFilePath->get()));
		ok = false;
	}
	
	
	//////////////////////////////////////////////////////////////////////
	// Create mPlayValues and mPlayValueNormalizations which are used to
	// create the PhysicalDataSource
	////////////////////////////////////////////////////////////////////// 
	
	
	for(int i = 0; i <  mPlayChannels.size(); i++)
	{
		mPlayValues.append(mPlayChannels.at(i).fromValue);
		mPlayValueNormalizations.append(mPlayChannels.at(i).normalization);
	}
	
	
	mCurrentSimStep = Tools::getIntValue(	vm,
																				SimulationConstants::VALUE_EXECUTION_CURRENT_STEP,
																				Tools::EM_ERROR,
																				getName(),
																				ok);
	
	return ok;
}

bool PhysicalDataPlayer::readChannelConfiguration(const QString &channelListTemplate,
																									const QStringList &normalizationPathList,
																				 					Event *defaultPlayEvent)
{
	bool ok = true;
	
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	int i = 1;
	StringValue *channelValueNameVal =	vm->getStringValue(
																				channelListTemplate.arg(i) 
																				+ OrcsModelOptimizerConstants::VM_VALUE_VALUE);
	while(channelValueNameVal != 0)
	{
		Value *toValue = Tools::getValue(vm,
																		 channelValueNameVal->get(), 
																		 Tools::EM_ERROR, 
																		 getName(), 
																		 ok,
																		 "Value is configured as channel which should be played.");
		
		Normalization *normalization = new Normalization(normalizationPathList);
		ok &= normalization->init();

		PlayChannel playChannel;
		playChannel.fromValue = channelValueNameVal;
		playChannel.toValue = toValue;
		playChannel.normalization = normalization;
		playChannel.playEvent = defaultPlayEvent;
		
		mPlayChannels.append(playChannel);

    // try to get next value
		i++;
		channelValueNameVal = vm->getStringValue(channelListTemplate.arg(i) 
													+	OrcsModelOptimizerConstants::VM_VALUE_VALUE);
	}
	
	return ok;
}


QString PhysicalDataPlayer::getName() const{
	return "PhysicalDataPlayer";
}

void PhysicalDataPlayer::sourceChanged() 
{
	QPair<PhysicalDataSource*, int> dataSource;
	
	if(mPhysicalDataSourceCache.contains(mSource->get()) == false)
	{
		int motionLength = 0;
		PhysicalDataSource * newSource = PhysicalDataSource::createPhysicalDataSource(
																					0,
																					0,
																					mPlayValues,
																					mPlayValueNormalizations,
																					OrcsModelOptimizerConstants::CONFIG_FOLDER
																						+ mPhysicalDataSourceConfigFilePath->get(),
																					mSource->get());	
		
		if(newSource != 0)
		{
			if(newSource->init() == true)
			{
				motionLength = newSource->readData();
				dataSource.first = newSource;
				dataSource.second = motionLength;
				
				mPhysicalDataSourceCache.insert(mSource->get(),
																				dataSource);
			}
		}
	}
	else
	{
		dataSource = mPhysicalDataSourceCache.value(mSource->get());
	}
		
	mPhysicalDataSource = dataSource.first;
	mMotionLength = dataSource.second;
}

void PhysicalDataPlayer::eventOccured(Event *event)
{
	if(event == 0){
		return;
	}
	
	if(mCurrentSimStep->get() <= mMotionLength)
	{	
		for(int i = 0; i < mPlayChannels.size(); i++)
		{
			if(mPlayChannels.at(i).playEvent == event)
			{
				((DoubleValue*)mPlayChannels.at(i).toValue)->set(mPhysicalDataSource->getData(i,mCurrentSimStep->get()));
			}
		}
	}
}



} // namespace nerd
