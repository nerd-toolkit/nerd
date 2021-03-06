/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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

#include "EvaluationMethod.h"
#include "Evolution/World.h"
#include <Core/Core.h>
#include "Value/ValueManager.h"

namespace nerd {

EvaluationMethod::EvaluationMethod(const QString &name)
	: ParameterizedObject(name), mOwnerWorld(0), mEvaluationGroupsBuilder(0)
{
	mShellBinary = new FileNameValue("/bin/bash");
	mShellBinary->setDescription("The shell used to start the preview of individuals in.");
	mShellBinary->getOptionList().append("/bin/bash");
	mShellBinary->getOptionList().append("/bin/zsh");
	mShellBinary->getOptionList().append("/bin/sh");
	
	Core::getInstance()->getValueManager()->addValue("/Evolution/Preview/Shell", mShellBinary);
}

EvaluationMethod::EvaluationMethod(const EvaluationMethod &other)
	: Object(), ValueChangedListener(), ParameterizedObject(other), mOwnerWorld(0),
	  mEvaluationGroupsBuilder(0)
{
}

EvaluationMethod::~EvaluationMethod() {
	delete mEvaluationGroupsBuilder;
}

void EvaluationMethod::setOwnerWorld(World *world) {
	if(mOwnerWorld != 0 && mOwnerWorld != world) {
		mOwnerWorld->setEvaluationMethod(0);
	}
	mOwnerWorld = world;
	if(mOwnerWorld != 0 && mOwnerWorld->getEvaluationMethod() != this) {
		mOwnerWorld->setEvaluationMethod(this);
	}
}


World* EvaluationMethod::getOwnerWorld() const {
	return mOwnerWorld;
}


void EvaluationMethod::setEvaluationGroupsBuilder(EvaluationGroupsBuilder *evalBuilder) {
	if(mEvaluationGroupsBuilder != 0) {
		delete mEvaluationGroupsBuilder;
		mEvaluationGroupsBuilder = 0;
	}
	mEvaluationGroupsBuilder = evalBuilder;
	mEvaluationGroupsBuilder->setEvaluationMethod(this);
}

EvaluationGroupsBuilder* EvaluationMethod::getEvaluationGroupsBuilder() const {
	return mEvaluationGroupsBuilder;
}


}


