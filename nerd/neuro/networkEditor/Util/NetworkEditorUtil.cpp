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



#include "NetworkEditorUtil.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Core/Core.h"
#include "ModularNeuralNetwork/NeuroModule.h"

using namespace std;

namespace nerd {

void NetworkEditorUtil::setPaintItemLocation(PaintItem *item, const QString &location) {
	if(item == 0 || location.size() < 5) {
		return;
	}
	QStringList coordinates = location.split(",");
	if(coordinates.size() != 3) {
		return;
	}
	bool okx = true;
	bool oky = true;
	double x = coordinates.at(0).toDouble(&okx);
	double y = coordinates.at(1).toDouble(&oky);

	PaintItem *parent = item->getParent();
	if(parent != 0) {
		x -= parent->getGlobalPosition().x();
		y -= parent->getGlobalPosition().y();
	}
	
	if(okx && oky) {
		item->setLocalPosition(QPointF(x, y));
	}
}

void NetworkEditorUtil::setModuleItemSize(ModuleItem *item, const QString &size) {
	if(item == 0 || size.size() < 3) {
		return;
	}
	QStringList box_size = size.split(",");
	if(box_size.size() != 2) {
		return;
	}
	bool okw = true;
	bool okh = true;
	double w = box_size.at(0).toDouble(&okw);
	double h = box_size.at(1).toDouble(&okh);
	
	if(okw && okh) {
		item->setSize(QSizeF(w, h));
	}
}



}



