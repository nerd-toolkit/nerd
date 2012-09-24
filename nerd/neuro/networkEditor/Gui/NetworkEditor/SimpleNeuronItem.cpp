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


#include "SimpleNeuronItem.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Math/Math.h"
#include "Gui/NetworkEditor/ModuleItem.h"
#include "ModularNeuralNetwork/NeuroModule.h"
#include "NeuralNetworkConstants.h"
#include <QFont>
#include "NeuroModulation/NeuroModulatorElement.h"


using namespace std;

namespace nerd {


/**
 * Constructs a new SimpleNeuronItem.
 */
SimpleNeuronItem::SimpleNeuronItem(NetworkVisualization *owner)
	: NeuronItem(owner), mRadius(15.0), mShowOutputActivityByColor(true), 
	  mShowActivityByColor(false)
{
}


/**
 * Copy constructor. 
 * 
 * @param other the SimpleNeuronItem object to copy.
 */
SimpleNeuronItem::SimpleNeuronItem(const SimpleNeuronItem &other) 
	: NeuronItem(other), mRadius(other.mRadius), 
	  mShowOutputActivityByColor(other.mShowOutputActivityByColor),
	  mShowActivityByColor(other.mShowActivityByColor)
{
}

/**
 * Destructor.
 */
SimpleNeuronItem::~SimpleNeuronItem() {
}


NeuronItem* SimpleNeuronItem::createCopy() const {
	return new SimpleNeuronItem(*this);
}


bool SimpleNeuronItem::setNeuron(Neuron *neuron) {
	bool ok = NeuronItem::setNeuron(neuron);
	return ok;
}


void SimpleNeuronItem::setLocalPosition(const QPointF &position, bool force) {
	ModuleItem *parent = dynamic_cast<ModuleItem*>(getParent());

	if(parent == 0 || force) {
		NeuronItem::setLocalPosition(position, force);
	}
	else {
		QPointF newPosition = position;
		QSizeF parentSize = parent->getSize();
		if(newPosition.x() + mRadius > parentSize.width()) {
			newPosition += QPointF(parentSize.width() - (newPosition.x() + mRadius), 0.0);
		}
		if(newPosition.x() - mRadius < 0.0) {
			newPosition += QPointF(-(newPosition.x() - mRadius), 0.0);
		}
		if(newPosition.y() + mRadius > parentSize.height()) {
			newPosition += QPointF(0.0, parentSize.height() - (newPosition.y() + mRadius));
		}
		if(newPosition.y() - mRadius < 0.0) {
			newPosition += QPointF(0.0, -(newPosition.y() - mRadius));
		}
		
		NeuronItem::setLocalPosition(newPosition);
	}
}


QRectF SimpleNeuronItem::getBoundingBox() {
	return QRectF(getGlobalPosition() - QPoint(mRadius, mRadius), QSize(mRadius * 2.0, mRadius * 2.0));
}

QRectF SimpleNeuronItem::getPaintingBox() {
	return QRectF(getGlobalPosition() - QPoint(220, mRadius + 30), QSize(440, 200));
}


bool SimpleNeuronItem::isHit(const QPointF &point, Qt::MouseButtons, double) {
	if(mIsInHiddenLayer) {
		return false;
	}
	
	if(Math::distance(point, getGlobalPosition()) <= mRadius) {
		return true;
	}
	return false;
}


void SimpleNeuronItem::mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) {
	if(mouseButton & Qt::LeftButton) {
		setLocalPosition(getLocalPosition() + distance);
	}
}

void SimpleNeuronItem::paintSelf(QPainter *painter) {
	if(mHidden || !mActive || painter == 0 || getNeuron() == 0 
		|| (mHideUnselectedElements && !mSelected) || mIsInHiddenLayer) 
	{
		return;
	}
	QPen oldPen = painter->pen();
	QPen newPen;
	if(mUseCosmeticLines) {
		newPen.setCosmetic(true);
	}

	QPointF pos = getGlobalPosition();

	if(mSelected) {
		QPainterPath p;
		p.addEllipse(pos, mRadius + 4.0, mRadius + 4.0);
		painter->fillPath(p, QColor(255, 0, 0, 100));
	}

	//Move this to an own PaintItem type (NeuroModulatorItem) to choose the painting level.
	if(mShowNeuroModulators) {
		NeuroModulatorElement *nme = dynamic_cast<NeuroModulatorElement*>(mNeuron->getActivationFunction());

		if(nme != 0) {
			NeuroModulator *modulator = nme->getNeuroModulator();
			
			if(modulator != 0) {
				QList<int> types = modulator->getModulatorTypes();
				
				for(int i = 0; i < types.size(); ++i) {
					int type = types.at(i);
					QRectF rect = modulator->getLocalRect(type);
					bool isCircle = modulator->isCircularArea(type);
					double concentration = modulator->getConcentration(type, mNeuron);
					
					Color color = Color::getColor((type * 2) % 7);
					color.setAlpha(150 * Math::min(1.0, Math::max(0.0, concentration)));
					
					if(concentration > 0.0 && rect.width() > 0.0 && rect.height() > 0.0) {
						if(isCircle) {
							QPainterPath p;
							p.addEllipse(getGlobalPosition() + QPointF(rect.x(), rect.y()), rect.width(), rect.height());
							painter->fillPath(p, QColor(color.red(), color.green(), color.blue(), color.alpha()));
						}
					}
				}
			}
		}
		
// 		QStringList propNames = mNeuron->getPropertyNames();
// 		for(QListIterator<QString> i(propNames); i.hasNext();) {
// 			QString modName = i.next();
// 			if(modName.startsWith("_NMT")) {
// 				QStringList modParams = mNeuron->getProperty(modName).split(",");
// 				if(modParams.size() == 2) {
// 					int type = modName.mid(4).toInt();
// 					if(type > 0) {
// 						double radius = modParams[1].toDouble();
// 						double concentration = modParams[0].toDouble();
// 						
// 						if(radius > 0.0 && concentration > 0.0) {
// 							Color color = Color::getColor((type * 2) % 7);
// 							color.setAlpha(150 * Math::min(1.0, Math::max(0.0, concentration)));
// 							QPainterPath p;
// 							p.addEllipse(getGlobalPosition(), radius, radius);
// 							painter->fillPath(p, QColor(color.red(), color.green(), color.blue(), color.alpha()));
// 						}
// 					}
// 				}
// 			}
// 		}
	}
	

	QColor outlineColor(0, 0, 0, 255);

// 	if(mShowSlaveState && mNeuron->hasProperty(NeuralNetworkConstants::TAG_CONSTRAINT_SLAVE)) {
// 		outlineColor = QColor(255, 0, 0, 255);
// 	}
// 	else if(mShowSlaveState && mNeuron->hasProperty(NeuralNetworkConstants::TAG_ELEMENT_PROTECTED)) {
// 		outlineColor = QColor(0, 255, 0, 255);
// 	}
	if(mShowSlaveState) {
		QString rdof = mNeuron->getProperty(
				NeuralNetworkConstants::TAG_ELEMENT_REDUCED_DEGREES_OF_FREEDOM);
		if(rdof.contains("B")) {
			if(mDOFHighlightColor != 0) {
				Color c = mDOFHighlightColor->get();
				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
			}
			else {
				outlineColor = QColor(255, 0, 0, 255);
			}
		}
		else {
			if(mDOFNormalColor != 0 && mDOFHighlightWidth != 0) {
				Color c = mDOFNormalColor->get();
				outlineColor = QColor(c.red(), c.green(), c.blue(), c.alpha());
				newPen.setWidth(mDOFHighlightWidth->get());
			}
		}
	}
	
	newPen.setColor(outlineColor);

	if(mShowBias) {
		QColor neuronColor(255, 255, 255, 255);
		double bias = 0.0;
		
		bias = getNeuron()->getBiasValue().get();
		if(bias > 0.0) {
			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 - bias)));
			neuronColor = QColor(255, colorVal, colorVal);
		}
		else if(bias < 0.0) {
			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 + bias)));
			neuronColor = QColor(colorVal, colorVal, 255.0);
		}
	
		newPen.setColor(outlineColor);
		painter->setPen(newPen);
		{
			QPainterPath p;
			p.addEllipse(getGlobalPosition(), mRadius, mRadius);
			painter->fillPath(p, neuronColor);
			painter->drawEllipse(pos, mRadius, mRadius);
		}
		if(bias != 0.0) {
			QPainterPath p;
			p.addEllipse(getGlobalPosition(), 4, 4);
			painter->fillPath(p, QColor(0, 0, 0));
		}
	}
	else {
		QColor neuronColor(255, 255, 255, 255);
		double activity = 0.0;
		
		if(mShowActivityByColor) {
			activity = getNeuron()->getActivationValue().get();
		}
		else if(mShowOutputActivityByColor) {
			activity = getNeuron()->getOutputActivationValue().get();
		}
		if(activity > 0.0) {
			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 - activity)));
			neuronColor = QColor(255, colorVal, colorVal);
		}
		else if(activity < 0.0) {
			double colorVal = 255.0 * Math::max(0.0, Math::min(1.0, (1.0 + activity)));
			neuronColor = QColor(colorVal, colorVal, 255.0);
		}
	
		newPen.setColor(outlineColor);
		painter->setPen(newPen);
		{
			QPainterPath p;
			p.addEllipse(getGlobalPosition(), mRadius, mRadius);
			painter->fillPath(p, neuronColor);
			painter->drawEllipse(pos, mRadius, mRadius);
		}
	}

	if(mShowActivationFlipped) {
		if(getNeuron()->isActivationFlipped()) {
			QPainterPath p;
			p.addEllipse(getGlobalPosition(), 6, 6);
			painter->fillPath(p, QColor(0, 0, 0));
		}
	}

	if(mShowName) {
		QString name = mNeuron->getNameValue().get();
		name = name.replace("\\\\", "\n");
		
		QRectF textRect(pos.x() - 80.0, pos.y() + mRadius + 2, 160.0, 100.0);
		
		if(mIncreaseReadability) {
			QRectF textBoundingRect = painter->boundingRect(textRect, 
							Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere, 
							name);
			painter->fillRect(QRectF(textBoundingRect.x(), textBoundingRect.y() + 2,
									 textBoundingRect.width(), textBoundingRect.height() - 4), 
							  QColor(255,255,255,180));
		}
		
		if(mDrawNamesSolid) {
			painter->setPen(QColor(0, 0, 0, 255));
		}
		
		painter->drawText(textRect, 
				Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap | Qt::TextWrapAnywhere,
				name);
		
		painter->setPen(newPen);
	}

	if(mShowBiasAsString) {
		
		if(mNeuron->getBiasValue().get() != 0.0) {
			
			QRectF textRect(pos.x() + 20.0, pos.y() - 6.0, 150.0, 20.0);
			
			QString biasText = mNeuron->getBiasValue().getValueAsString();
		
			if(mIncreaseReadability) {
				QRectF textBoundingRect = painter->boundingRect(textRect, 
								Qt::AlignLeft, 
								biasText);
				painter->fillRect(QRectF(textBoundingRect.x(), textBoundingRect.y() + 2,
										textBoundingRect.width(), textBoundingRect.height() - 4), 
								QColor(255,255,255,180));
			}
			
			painter->drawText(textRect, Qt::AlignLeft, biasText);
		}
	}


	if(mShowInputOutputMarker) {
		if(getNeuron()->hasProperty(Neuron::NEURON_TYPE_OUTPUT)) {
// 			QPainterPath blackCircle;
// 			blackCircle.addEllipse(pos.x() - 10.0, pos.y() - 10.0, 20.0, 20.0);
// 			painter->fillPath(blackCircle, outlineColor);
// 			QPainterPath whiteCircle;
// 			whiteCircle.addEllipse(pos.x() - 6.0, pos.y() - 6.0, 12.0, 12.0);
// 			painter->fillPath(whiteCircle, QColor(255, 255, 255, 255));
			QFont oldFont = painter->font();
			QFont newFont = oldFont;
			newFont.setPixelSize(25.0);
			painter->setFont(newFont);
			painter->drawText(pos.x() - 9.0, pos.y() - 11.0, 20.0, 20.0,
					Qt::AlignCenter, "M");
			painter->setFont(oldFont);
		}
		if(getNeuron()->hasProperty(Neuron::NEURON_TYPE_INPUT)) {
// 			painter->fillRect(QRectF(pos.x() - 2.0, pos.y() - 8.5, 5.0, 17.0), 
// 								outlineColor);
			QFont oldFont = painter->font();
			QFont newFont = oldFont;
			newFont.setPixelSize(25.0);
			painter->setFont(newFont);
			painter->drawText(pos.x() - 9.0, pos.y() - 11.0, 20.0, 20.0,
					Qt::AlignCenter, "S");
			painter->setFont(oldFont);
		}
		
	}
	if(mShowModuleInputOutputMarker) {
		QString outputLevelString;
		QString inputLevelString;
		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT)) {
			QPainterPath blackCircle;
			blackCircle.addEllipse(pos.x() - 10.0, pos.y() - 10.0, 20.0, 20.0);
			painter->fillPath(blackCircle, outlineColor);
			QPainterPath whiteCircle;
			whiteCircle.addEllipse(pos.x() - 6.0, pos.y() - 6.0, 12.0, 12.0);
			painter->fillPath(whiteCircle, QColor(255, 255, 255, 255));

			outputLevelString = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_OUTPUT);
		}
		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_INPUT)) {
			painter->fillRect(QRectF(pos.x() - 2.0, pos.y() - 8.5, 5.0, 17.0), 
								outlineColor);
		
			inputLevelString = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_INPUT);
		}
		if(getNeuron()->hasProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)) {
			int level = getNeuron()->getProperty(NeuralNetworkConstants::TAG_MODULE_EXTENDED_INTERFACE)
							.toInt();
			painter->drawText(pos.x() - 49.0, pos.y() - (mRadius / 2.0) - 22.0, 100.0, 20.0, 
				Qt::AlignCenter, QString::number(level));
		}
		else {
			QString outputString;
			if(inputLevelString != "") {
				outputString += "I:" + inputLevelString;
			}
			if(outputLevelString != "") {
				if(outputString != "") {
					outputString += " ";
				}
				outputString += "O:" + outputLevelString;
			}
			painter->drawText(pos.x() - 99.0, pos.y() - (mRadius / 2.0) - 22.0, 200.0, 20.0, 
				Qt::AlignCenter, outputString);
		}
	}
	

	painter->setPen(oldPen);
}

}




