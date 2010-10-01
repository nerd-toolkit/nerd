/***************************************************************************
 *   NERD - Open Robot Control and Simulation Library                      *
 *                                                                         *
 *   University of Osnabrueck, Germany                                     *
 *   Institute of Cognitive Science                                        *
 *   Neurocybernetics Group                                                *
 *   http://www.ikw.uni-osnabrueck.de/~neurokybernetik/                    *
 *                                                                         *
 *   Copyright (C) 2008 by Christian Rempis, Ferry Bachmann                *
 *   christian.rempis@uni-osnabrueck.de +                                  *
 *   ferry.bachmann@uni-osnabrueck.de                                      *
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



#include "DynamicsPlotCollection.h"
#include <iostream>
#include <QList>
#include "Core/Core.h"

#include "DynamicsPlot/ExampleDynamicsPlotter.h"
#include "DynamicsPlot/SimpleBifurcationPlotter.h"
#include "DynamicsPlot/StandardBifurcation_Calculator.h"
#include "DynamicsPlot/Transients_Calculator.h"
#include "DynamicsPlot/Isoperiod_Calculator.h"
#include "Event/EventManager.h"

using namespace std;

namespace nerd {


/**
 * Constructs a new DynamicsPlotCollection.
 */
DynamicsPlotCollection::DynamicsPlotCollection()
{
	new ExampleDynamicsPlotter();
	new SimpleBifurcationPlotter();
	new StandardBifurcation_Calculator();
	new Transients_Calculator();
	new Isoperiod_Calculator();
	
}



/**
 * Destructor.
 */
DynamicsPlotCollection::~DynamicsPlotCollection() {
}




}




