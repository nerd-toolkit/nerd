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



#include "AlignNeuronsCommand.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "Gui/NetworkEditor/SimpleNetworkVisualizationHandler.h"
#include "Network/SynapseTarget.h"
#include "Network/Neuron.h"
#include "Network/Synapse.h"
#include "Math/Math.h"
#include "Network/Neuro.h"
#include "Util/NeuralNetworkUtil.h"
#include "NeuralNetworkConstants.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new AlignNeuronsCommand.
 * 
 * TODO: Temporariliy added modules and other network items here, but did not rename
 * 			the neuron related variable names!
 */
AlignNeuronsCommand::AlignNeuronsCommand(int type, int mode, NetworkVisualization *context, 
										 QList<NeuralNetworkElement*> neurons, QList<QRectF> boundingBoxes)
	: Command(""), mType(type), mMode(mode), mVisualizationContext(context), 
		mNeuronsToAlign(neurons), mBoundingBoxes(boundingBoxes)
{
	QString label = "Align ";
	if(type == TYPE_HORIZONTAL) {
		label = label + "Horizontal ";
	}
	else {
		label = label + "Vertical ";
	}
	if(mode == MODE_LEFT) {
		label = label + "Left";
	}
	else if(mode == MODE_RIGHT) {
		label = label + "Right";
	}
	else if(mode == MODE_RIGHT) {
		label = label + "Right";
	}
	else if(mode == MODE_TOP) {
		label = label + "Top";
	}
	else if(mode == MODE_BOTTOM) {
		label = label + "Bottom";
	}
	else if(mode == MODE_SPACING) {
		label = label + "Spacing";
	}
	else if(mode == MODE_DISTANCE) {
		label = label + "Distance";
	}
	setLabel(label);
}


/**
 * Destructor.
 */
AlignNeuronsCommand::~AlignNeuronsCommand() {
}

bool AlignNeuronsCommand::isUndoable() const {
	return true;
}


bool AlignNeuronsCommand::doCommand() {

	if(mNeuronsToAlign.size() < 2 || mVisualizationContext == 0 || mNeuronsToAlign.size() != mBoundingBoxes.size()) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHandler());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	ModularNeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	mPreviousPositions.clear();

	//calculate desired distance based on the positions of the upperleft elements.
	double distance = 1.0;
	double startingPos = 0.0;
	int secondStartingPos = 0.0;
	int indexOfFirst = 0;
	int indexOfSecond = 0;

	QList<NeuralNetworkElement*> mSortedNeurons = mNeuronsToAlign;

	if(mSortedNeurons.size() < 2) {
		return true;
	}
	
	QList<NeuralNetworkElement*> mOrderedNeurons;

	if(mType == TYPE_HORIZONTAL) {
		
		for(int i = 0; i < mSortedNeurons.size(); ++i) {
			NeuralNetworkElement *elem = mSortedNeurons.at(i);
			bool added = false;
			for(int j = 0; j < mOrderedNeurons.size(); ++j) {
				if(elem->getPosition().getX() < mOrderedNeurons.at(j)->getPosition().getX()) {
					mOrderedNeurons.insert(j, elem);
					added = true;
					break;
				}
			}
			if(!added) {
				mOrderedNeurons.append(elem);
			}
		}
		
		//TODO complete the spacing and distance handling!
		
		indexOfFirst = mOrderedNeurons.indexOf(mSortedNeurons.at(mSortedNeurons.size() - 1));
		indexOfSecond = mOrderedNeurons.indexOf(mSortedNeurons.at(mSortedNeurons.size() - 2));
		
		if(mMode == MODE_DISTANCE) {
			NeuralNetworkElement *lastElem1 = mSortedNeurons.at(mSortedNeurons.size() - 1);
			NeuralNetworkElement *lastElem2 = mSortedNeurons.at(mSortedNeurons.size() - 2);
			
			//flip bounding boxes if neccessary
			if(lastElem1->getPosition().getX() < lastElem2->getPosition().getX()) {
				QRectF box1 = mBoundingBoxes.at(mBoundingBoxes.size() - 1);
				QRectF box2 = mBoundingBoxes.at(mBoundingBoxes.size() - 2);
				
				startingPos = ((box1.right() - box1.left()) / 2.0) + box1.left();
				secondStartingPos = ((box2.right() - box2.left()) / 2.0) + box2.left();
				distance = (((box2.right() - box2.left()) / 2.0) + box2.left()) - startingPos;
				
				//cerr << "first less " << startingPos << " " << secondStartingPos << " d: " << distance << " ix1: " << indexOfFirst << " ix2: " << indexOfSecond << endl;
			}
			else {
				QRectF box1 = mBoundingBoxes.at(mBoundingBoxes.size() - 1);
				QRectF box2 = mBoundingBoxes.at(mBoundingBoxes.size() - 2);

				startingPos = ((box1.right() - box1.left()) / 2.0) + box1.left();
				secondStartingPos = ((box2.right() - box2.left()) / 2.0) + box2.left();
				distance = startingPos - (((box2.right() - box2.left()) / 2.0) + box2.left());
				
				//cerr << "second less " << startingPos << " " << secondStartingPos << " d: " << distance << " ix1: " << indexOfFirst << " ix2: " << indexOfSecond << endl;
			}
			
		}
		else if(mMode == MODE_SPACING) {
			//TODO
// 			QRectF box1 = mBoundingBoxes.at(mBoundingBoxes.size() - 1);
// 			QRectF box2 = mBoundingBoxes.at(mBoundingBoxes.size() - 2);
// 			startingPos = ((box1.right() - box1.left()) / 2.0) + box1.left();
// 			distance = box2.left() - box1.right();
		}
		else if(mMode == MODE_CENTER) {
			startingPos = mSortedNeurons.at(mSortedNeurons.size() - 1)->getPosition().getY();
		}
		else if(mMode == MODE_TOP) {
			startingPos = mBoundingBoxes.at(mSortedNeurons.size() - 1).top();
		}
		else if(mMode == MODE_BOTTOM) {
			startingPos = mBoundingBoxes.at(mSortedNeurons.size() - 1).bottom();
		}
	}
	else {
		if(mMode == MODE_DISTANCE) {
			QRectF box1 = mBoundingBoxes.at(mBoundingBoxes.size() - 1);
			QRectF box2 = mBoundingBoxes.at(mBoundingBoxes.size() - 2);
			startingPos = ((box1.bottom() - box1.top()) / 2.0) + box1.top();
			distance = ((box2.bottom() - box2.top()) / 2.0) + box2.top() - startingPos;
		}
		else if(mMode == MODE_SPACING) {
			//TODO
		}
		else if(mMode == MODE_CENTER) {
			startingPos = mSortedNeurons.at(mSortedNeurons.size() - 1)->getPosition().getX();
		}
		else if(mMode == MODE_LEFT) {
			startingPos = mBoundingBoxes.at(mSortedNeurons.size() - 1).left();
		}
		else if(mMode == MODE_RIGHT) {
			startingPos = mBoundingBoxes.at(mSortedNeurons.size() - 1).right();
		}
	}

	//mNeuronsToAlign = mSortedNeurons;

	for(int i = 0; i < mNeuronsToAlign.size(); ++i) {
		NeuralNetworkElement *neuron = mNeuronsToAlign.at(i);
		QRectF box = mBoundingBoxes.at(i);

		if(neuron == 0) {
			continue;
		}

		mPreviousPositions.append(neuron->getPosition());
		Vector3D pos = neuron->getPosition();
		if(mType == TYPE_HORIZONTAL) {
			if(mMode == MODE_DISTANCE) {
// 				pos.setX(startingPos + ((mNeuronsToAlign.size() - 1 - i) * distance) 
// 				+ (pos.getX() - (box.right() - box.left()) / 2.0) - box.left());


				int index = mOrderedNeurons.indexOf(neuron);
				
				//cerr << "getting pos " << index << " and " << indexOfFirst << " , " << indexOfSecond << " dist: " << distance << " p1 " << startingPos << " p2 " << secondStartingPos << endl;
				
				if(index < 0 || index > mOrderedNeurons.size()) {
					//cerr << "index was: " << index << endl;
					continue;
				}
				
				if(index < indexOfFirst && index < indexOfSecond) {
					pos.setX(startingPos + ((index - indexOfFirst) * distance) 
					+ (pos.getX() - (box.right() - box.left()) / 2.0) - box.left());
				}
				else if(index > indexOfFirst && index > indexOfSecond) {
					pos.setX(secondStartingPos - ((indexOfSecond - index) * distance) 
					+ (pos.getX() - (box.right() - box.left()) / 2.0) - box.left());
				}
			}
			else if(mMode == MODE_SPACING) {
// 				cerr << "Startingpos: " << startingPos << " and dist " << distance << " pos obj: " << pos.getX() << endl;
// 				pos.setX(startingPos 
// 						- (pos.getX() - (box.right() - box.left()) / 2.0) - box.left());
// 				startingPos -= (box.width() + distance);
				
			}
			else if(mMode == MODE_CENTER) {
				pos.setY(startingPos);
			}
			else if(mMode == MODE_TOP) {
				pos.setY(startingPos + (pos.getY() - box.top()));
			}
			else if(mMode == MODE_BOTTOM) {
				pos.setY(startingPos + (pos.getY() - box.bottom()));
			}
		}
		else {
			if(mMode == MODE_DISTANCE) {
				//pos.setY(startingPos + (i * distance));
				pos.setY(startingPos + ((mNeuronsToAlign.size() - 1 - i) * distance) 
						+ (pos.getY() - (box.bottom() - box.top()) / 2.0) - box.top());
			}
			else if(mMode == MODE_SPACING) {
				//TODO
			}
			else if(mMode == MODE_CENTER) {
				pos.setX(startingPos);
			}
			else if(mMode == MODE_LEFT) {
				pos.setX(startingPos + (pos.getX() - box.left()));
			}
			else if(mMode == MODE_RIGHT) {
				pos.setX(startingPos + (pos.getX() - box.right()));
			}
			
		}
		if(dynamic_cast<NeuroModule*>(neuron) != 0) {
			NeuralNetworkUtil::moveNeuroModuleTo(dynamic_cast<NeuroModule*>(neuron),
												 pos.getX(), pos.getY(), pos.getZ());
		}
		else {
			neuron->setPosition(pos);
		}
	}

	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}


bool AlignNeuronsCommand::undoCommand() {
	if(mNeuronsToAlign.empty() || mVisualizationContext == 0) {
		return false;
	}	
	SimpleNetworkVisualizationHandler *handler = dynamic_cast<SimpleNetworkVisualizationHandler*>(
			mVisualizationContext->getVisualizationHandler());

	if(handler == 0) {
		return false;
	}
	QMutexLocker guard(mVisualizationContext->getSelectionMutex());

	NeuralNetwork *network = handler->getNeuralNetwork();

	if(network == 0) {
		return false;
	}

	
	for(int i = 0; i < mNeuronsToAlign.size() && i < mPreviousPositions.size(); ++i) {
		NeuralNetworkElement *neuron = mNeuronsToAlign.at(i);
		Vector3D pos = mPreviousPositions.at(i);

		if(neuron == 0) {
			continue;
		}
		if(dynamic_cast<NeuroModule*>(neuron) != 0) {
			NeuralNetworkUtil::moveNeuroModuleTo(dynamic_cast<NeuroModule*>(neuron),
												 pos.getX(), pos.getY(), pos.getZ());
		}
		else {
			neuron->setPosition(pos);
		}
	}
	
	//handler->rebuildView();
	Neuro::getNeuralNetworkManager()->triggerNetworkStructureChangedEvent();
	//mVisualizationContext->notifyNeuralNetworkModified();

	return true;
}



}



