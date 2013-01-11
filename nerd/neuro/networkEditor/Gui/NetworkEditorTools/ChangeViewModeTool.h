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



#ifndef NERDChangeViewModeTool_H
#define NERDChangeViewModeTool_H

#include <QString>
#include <QHash>
#include "Gui/NetworkEditor/NeuralNetworkEditor.h"
#include <QObject>
#include "Gui/KeyListener.h"
#include <QAction>

namespace nerd {

	/**
	 * ChangeViewModeTool.
	 *
	 */
	class ChangeViewModeTool : public QObject, public virtual KeyListener {
	Q_OBJECT
	public:
		static const int MENU_SEPARATOR = -1000;
		
	public:
		ChangeViewModeTool(NeuralNetworkEditor *owner, int mode, const QString &message = "",
						  Qt::Key pressedKey = Qt::Key_unknown, bool reverseActivation = false, 
						  bool initiallySelect = false);
		virtual ~ChangeViewModeTool();

		virtual void keyPressed(QKeyEvent *event);
		virtual void keyReleased(QKeyEvent *event);
		
		void enableViewMode();
		void disableViewMode();

	public slots:
		void updateKeyListenerRegistration(int tabIndex);
		void menuActionTriggered(bool checked = false);

	private:
		NeuralNetworkEditor *mOwner;
		bool mModeActive;
		int mMode;
		QString mStatusMessage;
		Qt::Key mPressedKey;
		bool mReverseActivation;
		QAction *mMenuAction;
		bool mKeyPressed;
	};

}

#endif



