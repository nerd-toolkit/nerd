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


#ifndef NERDSynapseItem_H
#define NERDSynapseItem_H

#include <QString>
#include <QHash>
#include "Network/Synapse.h"
#include "Gui/NetworkEditor/PaintItem.h"
#include <QSize>
#include "Core/PropertyChangedListener.h"
#include "Math/Vector3D.h"

namespace nerd {

	class NeuronItem;
	class NetworkVisualizationHandler;

	/**
	 * SynapseItem.
	 *
	 */
	class SynapseItem : public PaintItem, public PropertyChangedListener {
	public:
		SynapseItem(NetworkVisualization *owner);
		SynapseItem(const SynapseItem &other);
		virtual ~SynapseItem();

		virtual SynapseItem* createCopy() const = 0;

		virtual bool setSynapse(Synapse *synapse);
		Synapse* getSynapse() const;

		virtual bool isHit(const QPointF &point, Qt::MouseButtons mouseButton, double scaling) = 0;
		virtual void mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) = 0;
		virtual void paintSelf(QPainter *painter);

		virtual Properties* getEncapsulatedProperties() const;
		virtual void propertyChanged(Properties *owner, const QString &property);
		
		virtual void setViewMode(int mode, bool enabled);
		virtual bool isViewModeEnabled(int mode);
		
	protected:
		NetworkVisualization *mOwner;
		Synapse *mSynapse;
		NeuronItem *mSourceNeuron;
		PaintItem *mTarget;
		Vector3D mPositionOffset;
		bool mHideWeight;
		bool mUseSynapseTypeSymbols;
		bool mLocalHideWeight;
	};

}

#endif




