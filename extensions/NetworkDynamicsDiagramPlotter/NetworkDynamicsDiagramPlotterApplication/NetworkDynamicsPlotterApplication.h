

/***************************************************************************
 *   NERD - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Verena Thomas                 *
 *   christian.rempis@uni-osnabrueck.de +                                  *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   Acknowledgments:                                                      *
 *   The NERD library is part of the EU project ALEAR                      *
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
 ***************************************************************************/



#ifndef NERDNeuralNetworkDynamicsPlotterApplication_H
#define NERDNeuralNetworkDynamicsPlotterApplication_H

#include <QString>
#include <QHash>
#include "Application/BaseApplication.h"
#include "Gui/DynamicsPlotterMainWindow.h"
#include "Execution/PlotterExecutionLoop.h"
#include "DynamicsPlot/OnlinePlotter/OnlinePlotter.h"
#include "DynamicsPlot/OnlinePlotter/OnlinePlotterWindow.h"
#include "DynamicsPlot/Exporters/Exporter.h"
namespace nerd {

	/**
	 * NeuralNetworkEditorApplication.
	 *
	 */
	class NetworkDynamicsPlotterApplication : public BaseApplication {
	public:
		NetworkDynamicsPlotterApplication();
		virtual ~NetworkDynamicsPlotterApplication();

	protected:
		virtual bool setupGui();
		virtual bool setupApplication();
		virtual bool runApplication();
		
	private:
		DynamicsPlotterMainWindow *mMainWindow;
		PlotterExecutionLoop *mExecutionLoop;
		OnlinePlotter *op;
		OnlinePlotterWindow *opw;
		Exporter *exporter;
	};

}

#endif




