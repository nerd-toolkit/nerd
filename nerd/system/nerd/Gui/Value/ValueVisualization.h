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


/***************************************************************************
 * Deprecated!                                                             *
 *                                                                         *
 * This class will be removed in a later release!                          *
 * *************************************************************************/



#ifndef VALUEVISUALIZATION_H_
#define VALUEVISUALIZATION_H_

#include <QObject>
#include "Value/Value.h"
#include "Value/ValueChangedListener.h"
#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QCheckBox>

namespace nerd{

class ValueVisualizationWindow;

/**
 * ValueVisualization.
 */
class ValueVisualization:public QFrame, public virtual ValueChangedListener {

	Q_OBJECT

	public:
		ValueVisualization (ValueVisualizationWindow *list, Value *value, QString name );
		~ValueVisualization();
		void valueChanged ( Value *value );
		QString getName() const;
		Value* getValue();
		void reset();
		void setCurrentValue(const QString &currentValue);

	private slots:
		void deleteLater();
		void valueEdited();
		void destroy();

	public slots:
		void changeValue();
		void changeUpdateProperty(int newState);
		void setDoUpdateValue(int doUpdate);

	signals:
		void lineEditTextChanged(QString newText);
		void destroyThis(QString valueName);
	private:
		QFrame *mValueManipulationFrame;
		QLineEdit *mValueField;
		Value *mValue;
		QString mValueName;
		ValueVisualizationWindow *mValueList;
		QLabel *mNameLabel;
		QPushButton *mCloseButton;
		QCheckBox *mUpdateValueCheckBox;
		bool mUpdateValue;
		bool mValueChanged;
};
}

#endif
