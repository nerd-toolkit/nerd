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


#ifndef NERDNeuroModulator_H
#define NERDNeuroModulator_H


#include "Core/ParameterizedObject.h"
#include "Network/ObservableNetworkElement.h"
#include "Math/Vector3D.h"
#include <QRectF>

namespace nerd {

class NeuralNetworkManager;

	class NeuroModule;
	class NeuralNetworkElement;
	class Neuron;
	class NeuralNetwork;
	class BoolValue;

	/**
	 * NeuroModulator
	 */
	class NeuroModulator {
	public:
		NeuroModulator();
		NeuroModulator(const NeuroModulator &other);
		virtual ~NeuroModulator();

		virtual NeuroModulator* createCopy();
		virtual void reset(NeuralNetworkElement *owner);
		virtual void update(NeuralNetworkElement *owner);
		
		virtual void updateType(int type, NeuralNetworkElement *owner, bool reset = false);

		virtual void setConcentration(int type, double concentration, NeuralNetworkElement *owner);
		virtual double getConcentration(int type, NeuralNetworkElement *owner);
		virtual double getConcentrationAt(int type, Vector3D position, NeuralNetworkElement *owner);
		virtual QList<int> getModulatorTypes() const;
		
		virtual void setMaxConcentration(int type, double concentration);
		virtual double getMaxConcentration(int type);
		
		virtual void setLocalAreaRect(int type, double width, double height, const Vector3D &offset, bool isCircle);
		virtual void setAreaReferenceModule(int type, NeuroModule *module);
		virtual QRectF getLocalRect(int type);
		virtual bool isCircularArea(int type);
		
		virtual void setDistributionModus(int type, int modus);
		virtual int getDistributionModus(int type = -1) const;
		virtual void setUpdateModus(int type, int modus);
		virtual int getUpdateModus(int type = -1) const;
		virtual bool setUpdateModusParameters(int modus, const QList<double> &parameters);
		virtual QList<double> getUpdateModusParameters(int type) const;
		virtual QList<QString> getUpdateModusParameterNames(int type) const;
		virtual QList<double> getUpdateModusVariables(int type) const;
		virtual QList<QString> getUpdateModusVariableNames(int type) const;
		virtual QHash<int, QString> getUpdateModusDocumentations() const;
		
		virtual bool equals(NeuroModulator *modulator) const;
		
		static double getConcentrationInNetworkAt(int type, const Vector3D &position, NeuralNetwork *network);
		
	protected:
		virtual QString getModulatorDefaultDoc();
		virtual void updateModulatorDefault(int type, NeuralNetworkElement *owner, bool reset = false);
		
	protected:
		QHash<int, double> mConcentrations;
		QHash<int, QRectF> mAreas;
		QHash<int, NeuroModule*> mReferenceModules;
		QHash<int, bool> mIsCircle;
		QHash<int, double> mMaxConcentrations;
		QHash<int, int> mDistributionModi;
		QHash<int, int> mUpdateModi;
		QHash<int, QList<double> > mUpdateModiParameters;
		QHash<int, QList<double> > mUpdateModiVariables;
		QHash<int, QList<QString> > mUpdateModiParameterNames;
		QHash<int, QList<QString> > mUpdateModiVariableNames;
		QHash<int, QString> mUpdateModiDocumentations;
		int mDefaultDistributionModus;
		int mDefaultUpdateModus;
		bool mResetPending;
		
		BoolValue *mEnableUpdate;
		BoolValue *mEnableConcentrationCalculation;
		
		NeuralNetworkManager *mNetworkManager;

	};

}

#endif


