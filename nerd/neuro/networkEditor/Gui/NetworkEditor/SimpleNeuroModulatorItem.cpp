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


#include "SimpleNeuroModulatorItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "NetworkVisualization.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "NeuralNetworkConstants.h"
#include "TransferFunction/TransferFunction.h"
#include "SynapseFunction/SynapseFunction.h"
#include "ActivationFunction/ActivationFunction.h"
#include <QFont>
#include "NeuroModulation/NeuroModulatorElement.h"
#include <limits>


using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleNeuroModulatorItem.
 */
SimpleNeuroModulatorItem::SimpleNeuroModulatorItem(NetworkVisualization *owner)
	: NeuroModulatorItem(owner)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleNeuroModulatorItem object to copy.
 */
SimpleNeuroModulatorItem::SimpleNeuroModulatorItem(const SimpleNeuroModulatorItem &other) 
	: NeuroModulatorItem(other)
{
}

/**
 * Destructor.
 */
SimpleNeuroModulatorItem::~SimpleNeuroModulatorItem() {
}


NeuroModulatorItem* SimpleNeuroModulatorItem::createCopy() const {
	return new SimpleNeuroModulatorItem(*this);
}



QRectF SimpleNeuroModulatorItem::getBoundingBox() {
	return QRectF(0.0, 0.0, 0.0, 0.0);
}

QRectF SimpleNeuroModulatorItem::getPaintingBox() {
	return QRectF(-0.5 * std::numeric_limits<qreal>::max(), -0.5 * std::numeric_limits<qreal>::max(), 
				  std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max());
}


bool SimpleNeuroModulatorItem::isHit(const QPointF&, Qt::MouseButtons, double) {
	return false;
}


void SimpleNeuroModulatorItem::mouseMoved(const QPointF&, Qt::MouseButtons) {
}

void SimpleNeuroModulatorItem::paintSelf(QPainter *painter) {
	if(!mShowNeuroModulators || mOwner == 0) 
	{
		return;
	}
	
	ModularNeuralNetwork *net = mOwner->getNeuralNetwork();
	if(net == 0) {
		return;
	}
	
	//TODO currently, NM is only supported for neurons...
	QList<Neuron*> neurons = net->getNeurons();
	
	for(QListIterator<Neuron*> i(neurons); i.hasNext();) {
		Neuron *neuron = i.next();
		
		NeuroModulatorElement *modulatorElem = 0;
		if(dynamic_cast<NeuroModulatorElement*>(neuron->getActivationFunction()) != 0) {
			modulatorElem = dynamic_cast<NeuroModulatorElement*>(neuron->getActivationFunction());
		}
		else if(dynamic_cast<NeuroModulatorElement*>(neuron->getTransferFunction()) != 0) {
			modulatorElem = dynamic_cast<NeuroModulatorElement*>(neuron->getTransferFunction());
		}
		
		
		if(modulatorElem != 0 && modulatorElem->getNeuroModulator() != 0) {
			
			NeuroModulator *modulator = modulatorElem->getNeuroModulator();
			
			QList<int> types = modulator->getModulatorTypes();
			
			for(int i = 0; i < types.size(); ++i) {
				int type = types.at(i);
				QRectF rect = modulator->getLocalRect(type);
				bool isCircle = modulator->isCircularArea(type);
				double concentration = modulator->getConcentration(type, neuron);
				
				Color color = Color::getColor((type * 2) % 7);
				color.setAlpha(150 * Math::min(1.0, Math::max(0.0, concentration)));
				
				if(concentration > 0.0 && rect.width() > 0.0 && rect.height() > 0.0) {
					Vector3D position = neuron->getPosition();
					
					if(isCircle) {
						QPainterPath p;
						
						p.addEllipse(QPointF(position.getX(), position.getY()) + QPointF(rect.x(), rect.y()), 
									 rect.width() / 2.0, rect.height() / 2.0);
						painter->fillPath(p, QColor(color.red(), color.green(), color.blue(), color.alpha()));
					}
					else {
						QPointF pos = QPointF(position.getX(), position.getY());
						painter->fillRect(pos.x() + rect.x() - (rect.width() / 2.0), 
										pos.y() + rect.y() - (rect.height() / 2.0),
										rect.width(), 
										rect.height(),
										QColor(color.red(), color.green(), color.blue(), color.alpha()));
					}
				}
			}
		}
	}
	
// 	QPen oldPen = painter->pen();
// 	QPen newPen;

// 	QPointF pos = getGlobalPosition();
// 
// 	if(mSelected) {
// 		QPainterPath p;
// 		p.addEllipse(pos, mRadius + 4.0, mRadius + 4.0);
// 		painter->fillPath(p, QColor(255, 0, 0, 100));
// 	}
// 
// 	//Move this to an own PaintItem type (NeuroModulatorItem) to choose the painting level.
// 	if(mShowNeuroModulators) {
// 		NeuroModulatorElement *nme = dynamic_cast<NeuroModulatorElement*>(mNeuron->getActivationFunction());
// 
// 		if(nme != 0) {
// 			NeuroModulator *modulator = nme->getNeuroModulator();
// 			
// 			if(modulator != 0) {
// 				QList<int> types = modulator->getModulatorTypes();
// 				
// 				for(int i = 0; i < types.size(); ++i) {
// 					int type = types.at(i);
// 					QRectF rect = modulator->getLocalRect(type);
// 					bool isCircle = modulator->isCircularArea(type);
// 					double concentration = modulator->getConcentration(type, mNeuron);
// 					
// 					Color color = Color::getColor((type * 2) % 7);
// 					color.setAlpha(150 * Math::min(1.0, Math::max(0.0, concentration)));
// 					
// 					if(concentration > 0.0 && rect.width() > 0.0 && rect.height() > 0.0) {
// 						if(isCircle) {
// 							QPainterPath p;
// 							p.addEllipse(getGlobalPosition() + QPointF(rect.x(), rect.y()), rect.width() / 2.0, rect.height() / 2.0);
// 							painter->fillPath(p, QColor(color.red(), color.green(), color.blue(), color.alpha()));
// 						}
// 						else {
// 							QPointF pos = getGlobalPosition();
// 							painter->fillRect(pos.x() + rect.x() - (rect.width() / 2.0), 
// 											  pos.y() + rect.y() - (rect.height() / 2.0),
// 											  rect.width(), 
// 											  rect.height(),
// 											  QColor(color.red(), color.green(), color.blue(), color.alpha()));
// 						}
// 					}
// 				}
// 			}
// 		}
// 		
// // 		QStringList propNames = mNeuron->getPropertyNames();
// // 		for(QListIterator<QString> i(propNames); i.hasNext();) {
// // 			QString modName = i.next();
// // 			if(modName.startsWith("_NMT")) {
// // 				QStringList modParams = mNeuron->getProperty(modName).split(",");
// // 				if(modParams.size() == 2) {
// // 					int type = modName.mid(4).toInt();
// // 					if(type > 0) {
// // 						double radius = modParams[1].toDouble();
// // 						double concentration = modParams[0].toDouble();
// // 						
// // 						if(radius > 0.0 && concentration > 0.0) {
// // 							Color color = Color::getColor((type * 2) % 7);
// // 							color.setAlpha(150 * Math::min(1.0, Math::max(0.0, concentration)));
// // 							QPainterPath p;
// // 							p.addEllipse(getGlobalPosition(), radius, radius);
// // 							painter->fillPath(p, QColor(color.red(), color.green(), color.blue(), color.alpha()));
// // 						}
// // 					}
// // 				}
// // 			}
// // 		}
// 	}
// 	
// 
// 	QColor outlineColor(0, 0, 0, 255);
// 
// // 	if(mShowSlaveState && mNeuron->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
// // 		outlineColor = QColor(255, 0, 0, 255);
// // 	}
// // 	else if(mShowSlaveState && mNeuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
// // 		outlineColor = QColor(0, 255, 0, 255);
// // 	}
// 	if(mShowSlaveState) {
// 		QString rdof = mNeuron->getProperty(
// 				NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
// 		if(rdof.contains("B")) {
// 			if(mDOFHighlightColor != 0) {
// 				Color c = mDOFHighlightColor->get();
// 				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
// 			}
// 			else {
// 				outlineColor = QColor(255, 0, 0, 255);
// 			}
// 		}
// 		else {
// 			if(mDOFNormalColor != 0 && mDOFHighlightWidth != 0) {
// 				Color c = mDOFNormalColor->get();
// 				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
// 				newPen.setWidth(mDOFHighlightWidth->get());
// 			}
// 		}
// 	}
// 	
// 	newPen.setColor(outlineColor);
// 
// 	if(mShowBias) {
// 		QColor neuronColor(255, 255, 255, 255);
// 		double bias = 0.0;
// 		
// 		bias = getNeuron()->getBiasValue().get();
// 		if(bias > 0.0) {
// 			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 - bias)));
// 			neuronColor = QColor(255, colorVal, colorVal);
// 		}
// 		else if(bias < 0.0) {
// 			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 + bias)));
// 			neuronColor = QColor(colorVal, colorVal, 255.0);
// 		}
// 	
// 		newPen.setColor(outlineColor);
// 		painter->setPen(newPen);
// 		{
// 			QPainterPath p;
// 			p.addEllipse(getGlobalPosition(), mRadius, mRadius);
// 			painter->fillPath(p, neuronColor);
// 			painter->drawEllipse(pos, mRadius, mRadius);
// 		}
// 		if(bias != 0.0) {
// 			QPainterPath p;
// 			p.addEllipse(getGlobalPosition(), 4, 4);
// 			painter->fillPath(p, QColor(0, 0, 0));
// 		}
// 	}
// 	else {
// 		QColor neuronColor(255, 255, 255, 255);
// 		double activity = 0.0;
// 		
// 		if(mShowActivityByColor) {
// 			activity = getNeuron()->getActivationValue().get();
// 		}
// 		else if(mShowOutputActivityByColor) {
// 			activity = getNeuron()->getOutputActivationValue().get();
// 		}
// 		if(activity > 0.0) {
// 			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 - activity)));
// 			neuronColor = QColor(255, colorVal, colorVal);
// 		}
// 		else if(activity < 0.0) {
// 			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 + activity)));
// 			neuronColor = QColor(colorVal, colorVal, 255.0);
// 		}
// 	
// 		newPen.setColor(outlineColor);
// 		painter->setPen(newPen);
// 		{
// 			QPainterPath p;
// 			p.addEllipse(getGlobalPosition(), mRadius, mRadius);
// 			painter->fillPath(p, neuronColor);
// 			painter->drawEllipse(pos, mRadius, mRadius);
// 		}
// 	}
// 
// 	if(mShowActivationFlipped) {
// 		if(getNeuron()->isActivationFlipped()) {
// 			QPainterPath p;
// 			p.addEllipse(getGlobalPosition(), 6, 6);
// 			painter->fillPath(p, QColor(0, 0, 0));
// 		}
// 	}
// 
// 	if(mShowName) {
// 		QString name = mNeuron->getNameValue().get();
// 		name = name.replace("\\\\", "\n");
// 		
// 		QRectF textRect(pos.x() - 80.0, pos.y() + mRadius + 2, 160.0, 100.0);
// 		
// 		if(mIncreaseReadability) {
// 			QRectF textBoundingRect = painter->boundingRect(textRect, 
// 							Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere, 
// 							name);
// 			painter->fillRect(QRectF(textBoundingRect.x(), textBoundingRect.y() + 2,
// 									 textBoundingRect.width(), textBoundingRect.height() - 4), 
// 							  QColor(255,255,255,180));
// 		}
// 		
// 		if(mDrawNamesSolid) {
// 			painter->setPen(QColor(0, 0, 0, 255));
// 		}
// 		
// 		painter->drawText(textRect, 
// 				Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere,
// 				name);
// 		
// 		painter->setPen(newPen);
// 	}
// 
// 	if(mShowBiasAsString) {
// 		
// 		if(mNeuron->getBiasValue().get() != 0.0) {
// 			
// 			QRectF textRect(pos.x() + 20.0, pos.y() - 6.0, 150.0, 20.0);
// 			
// 			QString biasText = mNeuron->getBiasValue().getValueAsString();
// 		
// 			if(mIncreaseReadability) {
// 				QRectF textBoundingRect = painter->boundingRect(textRect, 
// 								Qt::AlignLeft, 
// 								biasText);
// 				painter->fillRect(QRectF(textBoundingRect.x(), textBoundingRect.y() + 2,
// 										textBoundingRect.width(), textBoundingRect.height() - 4), 
// 								QColor(255,255,255,180));
// 			}
// 			
// 			painter->drawText(textRect, Qt::AlignLeft, biasText);
// 		}
// 	}
// 
// 
// 	if(mShowInputOutputMarker) {
// 		if(getNeuron()->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
// // 			QPainterPath blackCircle;
// // 			blackCircle.addEllipse(pos.x() - 10.0, pos.y() - 10.0, 20.0, 20.0);
// // 			painter->fillPath(blackCircle, outlineColor);
// // 			QPainterPath whiteCircle;
// // 			whiteCircle.addEllipse(pos.x() - 6.0, pos.y() - 6.0, 12.0, 12.0);
// // 			painter->fillPath(whiteCircle, QColor(255, 255, 255, 255));
// 			QFont oldFont = painter->font();
// 			QFont newFont = oldFont;
// 			newFont.setPixelSize(25.0);
// 			painter->setFont(newFont);
// 			painter->drawText(pos.x() - 9.0, pos.y() - 11.0, 20.0, 20.0,
// 					Qt::AlignCenter, "M");
// 			painter->setFont(oldFont);
// 		}
// 		if(getNeuron()->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
// // 			painter->fillRect(QRectF(pos.x() - 2.0, pos.y() - 8.5, 5.0, 17.0), 
// // 								outlineColor);
// 			QFont oldFont = painter->font();
// 			QFont newFont = oldFont;
// 			newFont.setPixelSize(25.0);
// 			painter->setFont(newFont);
// 			painter->drawText(pos.x() - 9.0, pos.y() - 11.0, 20.0, 20.0,
// 					Qt::AlignCenter, "S");
// 			painter->setFont(oldFont);
// 		}
// 		
// 	}
// 	if(mShowModuleInputOutputMarker) {
// 		QString outputLevelString;
// 		QString inputLevelString;
// 		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT)) {
// 			QPainterPath blackCircle;
// 			blackCircle.addEllipse(pos.x() - 10.0, pos.y() - 10.0, 20.0, 20.0);
// 			painter->fillPath(blackCircle, outlineColor);
// 			QPainterPath whiteCircle;
// 			whiteCircle.addEllipse(pos.x() - 6.0, pos.y() - 6.0, 12.0, 12.0);
// 			painter->fillPath(whiteCircle, QColor(255, 255, 255, 255));
// 
// 			outputLevelString = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
// 		}
// 		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT)) {
// 			painter->fillRect(QRectF(pos.x() - 2.0, pos.y() - 8.5, 5.0, 17.0), 
// 								outlineColor);
// 		
// 			inputLevelString = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
// 		}
// 		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
// 			int level = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)
// 							.toInt();
// 			painter->drawText(pos.x() - 49.0, pos.y() - (mRadius / 2.0) - 22.0, 100.0, 20.0, 
// 				Qt::AlignCenter, QString::number(level));
// 		}
// 		else {
// 			QString outputString;
// 			if(inputLevelString != "") {
// 				outputString += "I:" + inputLevelString;
// 			}
// 			if(outputLevelString != "") {
// 				if(outputString != "") {
// 					outputString += " ";
// 				}
// 				outputString += "O:" + outputLevelString;
// 			}
// 			painter->drawText(pos.x() - 99.0, pos.y() - (mRadius / 2.0) - 22.0, 200.0, 20.0, 
// 				Qt::AlignCenter, outputString);
// 		}
// 	}
// 	
// 
// 	painter->setPen(oldPen);
}

}




