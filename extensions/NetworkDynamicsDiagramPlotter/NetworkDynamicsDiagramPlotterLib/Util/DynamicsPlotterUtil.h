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


#ifndef DYNAMICSPLOTTERUTIL_H
#define DYNAMICSPLOTTERUTIL_H

#include "Core/SystemObject.h"
#include "Core/ParameterizedObject.h"
#include "Value/DoubleValue.h"
#include <QList>
#include "Network/NeuralNetwork.h"
#include <QStringList>
#include "Core/Core.h"

namespace nerd {

	class EditorMessageWidget;
	class DynamicsPlotterUtil {

	public:

		static DoubleValue* getElementValue(const QString &parameter,
											NeuralNetworkElement* networkElement,
											QList<Neuron*> *neuronsWithActivationChange = 0);

		static DoubleValue* getElementValue(const QString &specifier,
											const QList<NeuralNetworkElement*> &networkElements,
											QList<Neuron*> *neuronsWithActivationChange = 0);

		static QList< QList<DoubleValue*> > getElementValues(const QList<QStringList> &specifiers,
															const QList<NeuralNetworkElement*> &networkElements,
															QList<Neuron*> *neuronsWithActivationChange = 0);

		static double getMeanValue(const QList<DoubleValue*> &valuesList);

		static QList<double> getMeanValues(const QList< QList<DoubleValue*> > &valuesListList);

		static QList<QStringList> parseElementString(QString const &string);

		static QList<double> getDoublesFromString(const QString &list, const QString &separator = ",",
												const QString &replace = "|");

		static QList<DoubleValue*> getNetworkValues(const QList<NeuralNetworkElement*> networkElements);

		static QList<double> getNetworkState(const QList<DoubleValue*> networkValues);

		static QList<double> getNeuronActivations(NeuralNetwork* network);

		static int findAttractorPeriod(QList< QList<double> > history);

		static bool compareNetworkStates(const QList<double> &state1,
										const QList<double> &state2, double accuracy = 0.001);

		static double getDistance(const QList<double> &state1, const QList<double> &state2);

		static void transferNeuronActivationToOutput(NeuralNetwork *network);
		static void transferNeuronActivationToOutput(const QList<Neuron*> neurons);

		static void reportProblem(const QString &errorMessage);
		static void clearProblemMessageArea();

	private:
		static EditorMessageWidget *sMessageWidget;
		static Core *sCore;
	};

}

#endif // DYNAMICSPLOTTERUTIL_H
