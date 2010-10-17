/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   tfaber@uni-osnabrueck.de
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



#ifndef NERDDynamicsPlotter_H
#define NERDDynamicsPlotter_H

#include <QString>
#include <QHash>
#include "Core/SystemObject.h"
#include "Core/ParameterizedObject.h"
#include "Value/BoolValue.h"
#include "Value/MatrixValue.h"
#include "Value/ULongLongValue.h"
#include "Network/NeuralNetwork.h"

namespace nerd {

	/**
	 * DynamicsPlotter.
	 *
	 */
	class DynamicsPlotter : public ParameterizedObject, public virtual SystemObject,
							public EventListener
	{
	public:
		DynamicsPlotter(const QString &name);
		virtual ~DynamicsPlotter();

		virtual QString getName() const;

		virtual bool init();
		virtual bool bind();
		virtual bool cleanUp();

		virtual void eventOccured(Event *event);

		virtual void calculateData() = 0;

		void execute();
		BoolValue* getActiveValue() const;

		NeuralNetwork* getCurrentNetwork() const;

		
		
	protected:
		void storeCurrentNetworkActivities();
		void restoreCurrentNetworkActivites();
		
		
		NeuralNetworkElement* getVariedNetworkElement(ULongLongValue *idOfVariedNetworkElement);
		void setVariedNetworkElementValue(NeuralNetworkElement *variedElem, double value);
		double getVariedNetworkElementValue(NeuralNetworkElement *variedElem);
				
		bool checkStringlistsItemCount(StringValue *idsString, StringValue *minsString, StringValue *maxsString);
		QList<ULongLongValue*> createListOfIds(StringValue *idsString);
		QList<double> createListOfDoubles(StringValue *minsString);
		
	protected:
		Event *mNextStepEvent;
		Event *mResetEvent;
		Event *mEvaluateNetworkEvent;
		BoolValue *mStasisValue;
		BoolValue *mActiveValue;
		IntValue *mExecutionTime;
		QHash<qulonglong, double> mNetworkActivities;
		//****Till****//
		MatrixValue *mData;
// 		IntValue *mPlotPixelsX;
// 		IntValue *mPlotPixelsY;	
		StringValue *mOutputPath;
		
		Event *mStartEvent;
		Event *mFinishEvent;
		//***/Till****//

	};

}

#endif




