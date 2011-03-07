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



#ifndef NERDPaintItem_H
#define NERDPaintItem_H

#include <QString>
#include <QHash>
#include <QPointF>
#include <QPainter>
#include <Qt>
#include "Core/Properties.h"

namespace nerd {

	class NetworkVisualization;

	/**
	 * PaintItem.
	 *
	 */
	class PaintItem {
	public:
		enum {SHOW_NEURON_INPUT_OUTPUT, SHOW_NEURON_BIAS, SHOW_NEURON_BIAS_AS_NUMBER,
			  SHOW_NEURON_ACTIVATION, SHOW_NEURON_OUTPUT_ACTIVATION,
			  SHOW_NEURON_NAME, SHOW_ALL_NAMES, SHOW_NEURON_ACTIVATION_FLIPPED,
			  SHOW_NEURON_MODULE_INPUT_OUTPUT, SHOW_ELEMENT_SLAVE_STATUS,
			  HIDE_UNSELECTED, USE_COSMETIC_LINES, FORCED_HIDDEN};
	public:
		PaintItem(int paintLevel);
		PaintItem(const PaintItem *other);
		virtual ~PaintItem();

		void setParent(PaintItem *parent);
		PaintItem* getParent();

		virtual void invalidate();

		void setActive(bool active);
		bool isActive() const;
		void setHidden(bool hidden);
		bool isHidden() const;
		void setSelected(bool selected);
		bool isSelected() const;
		void setToHiddenLayer(bool hidden);
		bool isInHiddenLayer() const;

		virtual void setLocalPosition(const QPointF &position, bool force = false);
		virtual QPointF getLocalPosition();
		virtual QPointF getGlobalPosition();

		virtual bool isHit(const QPointF &point, Qt::MouseButtons mouseButton, double scaling) = 0;
		virtual void mouseMoved(const QPointF &distance, Qt::MouseButtons mouseButton) = 0;

		virtual void setPaintLevel(int level);
		virtual int getPaintLevel() const;
		virtual QRectF getBoundingBox() = 0;
		virtual QRectF getPaintingBox() = 0;
		virtual void updateLayout();

		virtual void paintSelf(QPainter *painter) = 0;

		virtual void setViewMode(int mode, bool enabled);
		virtual bool isViewModeEnabled(int mode);
		
		virtual Properties* getEncapsulatedProperties() const;

	protected:
		PaintItem *mParent;
		QPointF mLocalPosition;
		QPointF mGlobalPosition;
		bool mInvalidated;
		int mPaintLevel;
		bool mActive;	
		bool mHidden;
		bool mSelected;
		bool mShowSlaveState;
		bool mHideUnselectedElements;
		bool mUseCosmeticLines;
		bool mForcedHidden;
		bool mIsInHiddenLayer;
	};

}

#endif




