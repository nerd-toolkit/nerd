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



#ifndef NERDEvolutionParameterPanel_H
#define NERDEvolutionParameterPanel_H

#include <QString>
#include <QHash>
#include "Core/SystemObject.h"
#include <QWidget>
#include <QTabWidget>
#include <QPushButton>
#include "Value/ValueChangedListener.h"
#include "Value/BoolValue.h"
#include <QMainWindow>
#include <QLineEdit>


namespace nerd {

	/**
	 * EvolutionParameterPanel.
	 *
	 */
	class EvolutionParameterPanel : public QMainWindow, public virtual SystemObject, 
									public virtual ValueChangedListener
	{
	Q_OBJECT
	public:
		EvolutionParameterPanel(bool includeStarterButtonToMainControlPanel = false);
		virtual ~EvolutionParameterPanel();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void valueChanged(Value *value);

	signals:
		void initialize();

	public slots:
		void initializePanel();
		void runEvolutionButtonToggled(bool);
		void restartEvolutionButtonToggled(bool);
		void pauseButtonToggled(bool);
		void loadSettings();
		void saveSettings();
		void previewIndividual();

	protected:
		
		QString loadParametersFromFile(const QString &fileName);
		void saveCurrentParameters(const QString &fileName);
		QList<QString> getParameterValueNames();

	private:
		QList<QString> mEvolutionAlgorithmPrefixes;
		QTabWidget *mTabWidget;
		QPushButton *mRunEvolutionButton;
		QPushButton *mRestartGenerationButton;
		QPushButton *mPauseEvaluationButton;
		BoolValue *mRunEvolutionValue;
		BoolValue *mRestartGenerationValue;
		BoolValue *mPauseEvaluationValue;
		StringValue *mCurrentEvoWorkingDirectory;
		IntValue *mCurrentGenerationNumber;
		bool mIncludeStarterButtonToMainControlPanel;
		QPushButton *mStartIndividualScriptButton;
		QLineEdit *mIndividualSelectionEdit;
	};

}

#endif



