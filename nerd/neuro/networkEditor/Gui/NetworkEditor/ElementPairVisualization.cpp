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



#include "ElementPairVisualization.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/NetworkVisualizationHandler.h"
#include "Gui/NetworkEditor/NetworkVisualization.h"
#include "ModularNeuralNetwork/ModularNeuralNetwork.h"
#include "Gui/NetworkEditor/NeuronItem.h"
#include <math.h>
#include "Math/Math.h"
#include <limits>
#include "Gui/NetworkEditor/ModuleItem.h"
#include "Gui/NetworkEditor/GroupItem.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new ElementPairVisualization.
 */
ElementPairVisualization::ElementPairVisualization(int paintLevel)
	: PaintItem(paintLevel)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the ElementPairVisualization object to copy.
 */
ElementPairVisualization::ElementPairVisualization(const ElementPairVisualization &other) 
	: PaintItem(other)
{
}

/**
 * Destructor.
 */
ElementPairVisualization::~ElementPairVisualization() {
}


bool ElementPairVisualization::isHit(const QPointF&, Qt::MouseButtons, double) {
	return false;
}


void ElementPairVisualization::mouseMoved(const QPointF&, Qt::MouseButtons) {
	
}


void ElementPairVisualization::setElementPairString(const QString &elementPairs) {

	mElementPairs.clear();

	if(mOwner != 0 && mOwner->getVisualizationHander() != 0) {
		NetworkVisualizationHandler *handler = mOwner->getVisualizationHander();
		ModularNeuralNetwork *net = mOwner->getNeuralNetwork();
// 		mSourceNeuron = handler->getNeuronItem(synapse->getSource());
// 		if(dynamic_cast<Neuron*>(synapse->getTarget()) != 0) {
// 			mTarget = handler->getNeuronItem(dynamic_cast<Neuron*>(synapse->getTarget()));
// 		}
// 		else {
// 			mTarget = handler->getSynapseItem(dynamic_cast<Synapse*>(synapse->getTarget()));
// 		}

		if(handler == 0 || net == 0) {
			return;
		}
		QList<Neuron*> neurons = net->getNeurons();
		QList<NeuroModule*> modules = net->getNeuroModules();
		QList<NeuronGroup*> groups = net->getNeuronGroups();

		QStringList pairs = elementPairs.split("|");
		for(QListIterator<QString> i(pairs); i.hasNext();) {
			QStringList pair = i.next().split(",");
			if(pair.size() != 2) {
				continue;
			}
			bool ok = false;
			qulonglong obj1Id = pair.at(0).toULongLong(&ok);
			if(!ok) {
				continue;
			}
			qulonglong obj2Id = pair.at(1).toULongLong(&ok);
			if(!ok) {
				continue;
			}

			
			PaintItem *item1 = 0;
			PaintItem *item2 = 0;

			{
				//Check if pair is a neuron pair
				Neuron *n1 = NeuralNetwork::selectNeuronById(obj1Id, neurons);
				Neuron *n2 = NeuralNetwork::selectNeuronById(obj2Id, neurons);

				if(n1 != 0 && n2 != 0) {
					item1 = handler->getNeuronItem(n1);
					item2 = handler->getNeuronItem(n2);
				}
			}
			if(item1 == 0 && item2 == 0) {
				//Check if pair is a module pair
				NeuroModule *m1 = ModularNeuralNetwork::selectNeuroModuleById(obj1Id, modules);
				NeuroModule *m2 = ModularNeuralNetwork::selectNeuroModuleById(obj2Id, modules);

				if(m1 != 0 && m2 != 0) {
					item1 = handler->getModuleItem(m1);
					item2 = handler->getModuleItem(m2);
				}
			}
			if(item1 == 0 && item2 == 0) {
				NeuronGroup *g1 = ModularNeuralNetwork::selectNeuronGroupById(obj1Id, groups);
				NeuronGroup *g2 = ModularNeuralNetwork::selectNeuronGroupById(obj2Id, groups);
				
				if(g1 != 0 && g2 != 0) {
					item1 = handler->getGroupItem(g1);
					item2 = handler->getGroupItem(g2);
				}
			}

			if(item1 == 0 || item2 == 0) {
				continue;
			}

			QList<PaintItem*> items;
			items.append(item1);
			items.append(item2);
			mElementPairs.append(items);

		}
		
	}
}


QRectF ElementPairVisualization::getBoundingBox() {
	return QRectF();
}

QRectF ElementPairVisualization::getPaintingBox() {
	return QRectF(numeric_limits<float>::max() / -2.0, numeric_limits<float>::max() / -2.0,
				  numeric_limits<float>::max(), numeric_limits<float>::max());
}

void ElementPairVisualization::paintSelf(QPainter *painter) {
	if(!mElementPairs.empty() && !mOwner == 0) {
		QPen pen = painter->pen();
		QColor fillColorBlack = QColor(0,0,0,255);
		QPen newPenBlackSmall = QPen(fillColorBlack);
		QPen newPenBlackLarge = QPen(fillColorBlack);
		newPenBlackSmall.setWidth(6);
		newPenBlackLarge.setWidth(10);
		newPenBlackLarge.setStyle(Qt::DashLine);
		newPenBlackSmall.setStyle(Qt::DashLine);

		QPen largePen = newPenBlackLarge;
		QColor fillColor = fillColorBlack;
		painter->setPen(newPenBlackSmall);
		

		const QList<PaintItem*> &availableItems = mOwner->getPaintItems();

		QList<QList<PaintItem*> > elems = mElementPairs;
		for(QListIterator<QList<PaintItem*> > i(elems); i.hasNext();) {
			
			QList<PaintItem*> items = i.next();

			if(!availableItems.contains(items.at(0))
				|| !availableItems.contains(items.at(1)))
			{
				continue;
			}
			QPointF p1 = items.at(0)->getGlobalPosition();
			QPointF p2 = items.at(1)->getGlobalPosition();


			if(dynamic_cast<NeuronItem*>(items.at(0)) != 0
				|| dynamic_cast<GroupItem*>(items.at(0)) != 0) 
			{
				painter->drawLine(p1, p2);

				//draw arrow
				QPainterPath arrow(QPointF(0, 0));
				arrow.lineTo(-20, -10);
				arrow.lineTo(-20, 10);
				arrow.lineTo(0, 0);
		
				QMatrix transformation = painter->worldMatrix();
				
				double angle = atan2(p2.x() - p1.x(), p2.y() - p1.y());
				double distance = Math::distance(p1, p2);
		
				painter->translate(p2);
				painter->rotate(90.0 + (-angle / Math::PI * 180.0));
				painter->translate(-1.0 * Math::min(15.0, distance - 20.0), 0.0);
				painter->fillPath(arrow, fillColor);
		
				painter->setWorldMatrix(transformation);
			}
			else if(dynamic_cast<ModuleItem*>(items.at(0)) != 0) {
				ModuleItem *item1 = dynamic_cast<ModuleItem*>(items.at(0));
				ModuleItem *item2 = dynamic_cast<ModuleItem*>(items.at(1));

				if(item1 == 0 || item2 == 0) {
					continue;
				}
				p1 = p1 + QPointF(item1->getSize().width() / 2.0, item1->getSize().height() / 2.0);
				p2 = p2 + QPointF(item2->getSize().width() / 2.0, item2->getSize().height() / 2.0);

				QPen pen_normal = painter->pen();
				painter->setPen(largePen);
				painter->drawLine(p1, p2);
				painter->setPen(pen_normal);

				//draw arrow
				QPainterPath arrow(QPointF(0, 0));
				arrow.lineTo(-40, -20);
				arrow.lineTo(-40, 20);
				arrow.lineTo(0, 0);
		
				QMatrix transformation = painter->worldMatrix();
				
				double angle = atan2(p2.x() - p1.x(), p2.y() - p1.y());
				double distance = Math::distance(p1, p2);
		
				painter->translate(p2);
				painter->rotate(90.0 + (-angle / Math::PI * 180.0));
				painter->translate(-1.0 * Math::min(12.0, distance - 20.0), 0.0);
				painter->fillPath(arrow, fillColor);
		
				painter->setWorldMatrix(transformation);
			}
		}
		painter->setPen(pen);
	}
}


void ElementPairVisualization::setOwner(NetworkVisualization *owner) {
	mOwner = owner;
}


NetworkVisualization* ElementPairVisualization::getOwner() const {
	return mOwner;
}



}



