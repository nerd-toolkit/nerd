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



#ifndef NERDParameterPanel_H
#define NERDParameterPanel_H

#include <QString>
#include <QHash>
#include <QWidget>
#include "Value/ValueChangedListener.h"
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>

namespace nerd {

	/**
	 * ParameterPanel.
	 *
	 */
	class ParameterPanel : public QObject, public virtual ValueChangedListener {
	Q_OBJECT
	public:
		ParameterPanel(QObject *parent, QGridLayout *targetLayout, QString name, Value *parameter);
		virtual ~ParameterPanel();

		virtual void valueChanged(Value *value);
		virtual void forceListenerDeregistration(Value *value);

		virtual QString getName() const;

		QString getParameterName();
		Value* getParameterValue();

		void invalidateListeners();

	public slots:
		void parameterContentChanged();

    private slots:
        void markAsValueEdited();
		void markAsValueUpdated();

	signals:
		void parameterChanged(Value *parameter, const QString &name, const QString &content);

	private:
		bool mRegisteredAsListener;
		QGridLayout *mTargetLayout;
		QString mParameterName;
		QLineEdit *mParameterContent;
		QLabel *mNameLabel;
		Value *mParameter;
		bool mInvalid;

	};

}

#endif




