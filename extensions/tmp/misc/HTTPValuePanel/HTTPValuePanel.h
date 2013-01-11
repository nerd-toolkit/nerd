/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   HTTPValuePanel by Ferry Bachmann (bachmann@informatik.hu-berlin.de)   *
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

#ifndef NERDHTTPValuePanel_H
#define NERDHTTPValuePanel_H

#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QScrollArea>
#include <QBoxLayout>
#include "HTTPValueAccess.h"
#include "HTTPValueVisualization.h"
#include "Value/ValueManager.h"

namespace nerd {

	/**
	 * HTTPValuePanel
	 */
	class HTTPValuePanel : public QWidget {

	Q_OBJECT

	public:

		HTTPValuePanel();
		virtual ~HTTPValuePanel();

		static bool setValueFromStringThreadSafe(Value *value, const QString &newValue);

	public:
		
		static const QString mValuePrefix;

	private:
		enum State {
			None,
			WaitingForAllValuesAvailable,
			WaitingForSelectedValues,
		};

	private:

		bool eventFilter(QObject *object, QEvent *event);

	private:
		State mState;
		QVBoxLayout *mMainLayout;
		QGroupBox *mServerSettingsBox;
		QGridLayout *mServerSettingsLayout;
		QLabel *mPostUrlLabel;
		QLineEdit *mPostUrlEdit;
		QLabel *mGetUrlLabel;
		QLineEdit *mGetUrlEdit;
		QLabel *mGetValueFileLabel;
		QLineEdit *mGetValueFileEdit;
		QLabel *mGetValueTypeFileLabel;
		QLineEdit *mGetValueTypeFileEdit;
		QLabel *mPostValueFileLabel;
		QLineEdit *mPostValueFileEdit;

		QGroupBox *mValuesBox;
		QGridLayout *mValuesLayout;
		QPushButton *mGetAllValuesAvailable;

		QLineEdit *mValueEdit;
		QLabel *mValueCountLabel;
		QComboBox *mValueComboBox;
		QPushButton *mAddValue;

		QScrollArea *mValueListArea;
		QWidget *mValueList;
		QBoxLayout *mListLayout;

		QGridLayout *mValueButtonLayout;
		QPushButton *mApplyAllChanges;
		QPushButton *mRemoveAll;
		QPushButton *mGetSelectedValues;
		QPushButton *mPostSelectedValues;

		HTTPValueAccess *mHTTPValueAccess;

		ValueManager *mValueManager;

		QMap<Value*, HTTPValueVisualization*> mValueVisualizations;

	private slots:
		void requestFinished(bool error);
		void getAllValuesAvailable();
		void getSelectedValues();
		void postSelectedValues();
		void fillValueList(const QString &text);
		void showMatchingValues();
		void removeAllValuesFromList();
		void applyAllChanges();

	};

}

#endif
