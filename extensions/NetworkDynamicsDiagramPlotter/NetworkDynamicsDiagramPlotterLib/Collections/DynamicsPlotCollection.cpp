/***************************************************************************
 *   NERD Kit - Neurodynamics and Evolutionary Robotics Development Kit    *
 *                                                                         *
 *   NetworkDynamicsPlotter project by Till Faber and Christian Rempis     *
 *   till.faber@uni-osnabrueck.de
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



#include "DynamicsPlotCollection.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"
#include "DynamicsPlot/Bifurcation_Calculator.h"
#include "DynamicsPlot/BifurcationPlotter.h"
#include "DynamicsPlot/Transients_Calculator.h"
#include "DynamicsPlot/Isoperiod_Calculator.h"
#include "DynamicsPlot/BasinOfAttraction_Calculator.h"
#include "DynamicsPlot/BasinPlotter.h"
#include "DynamicsPlot/IsoperiodPlotter.h"
#include "Event/EventManager.h"
#include "DynamicsPlot/PlotterTemplate_1.h"
#include "DynamicsPlot/PlotterTemplate_2.h"
#include "DynamicsPlot/PlotterTemplate_3.h"
#include "DynamicsPlot/PlotterTemplate_4.h"
#include "DynamicsPlot/PlotterTemplate_5.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new DynamicsPlotCollection.
 */
DynamicsPlotCollection::DynamicsPlotCollection()
{
	new Bifurcation_Calculator();
	new BifurcationPlotter();
	new Transients_Calculator();
	new Isoperiod_Calculator();
	new BasinOfAttraction_Calculator();
	new BasinPlotter();
	new IsoperiodPlotter();
	new PlotterTemplate_1();
	new PlotterTemplate_2();
	new PlotterTemplate_3();
	new PlotterTemplate_4();
	new PlotterTemplate_5();
}



/**
 * Destructor.
 */
DynamicsPlotCollection::~DynamicsPlotCollection() {
}




}




