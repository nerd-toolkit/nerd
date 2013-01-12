/***************************************************************************
 *   NERD - Neurodynamics and Evolutionary Robotics Development Toolkit    *
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

#include "Exporter.h"
#include <iostream>
#include "Core/Core.h"
#include <QFile>
#include <QTextStream>
#include "Event/EventManager.h"
#include <QVector>
#include "DynamicsPlotConstants.h"

using namespace std;

namespace nerd {

	
	/**
	 * Constructs a new Exporter.
	 */
	Exporter::Exporter()
	{
		Core::getInstance()->addSystemObject(this);
	}

/**
	 * Destructor.
 */
	Exporter::~Exporter() {
	}

	QString Exporter::getName() const {
		return "Exporter";
	}	
	
	
	bool Exporter::init() {
		bool ok = true;

		return ok;
	}

	bool Exporter::bind() {
		bool ok = true;

		EventManager *em = Core::getInstance()->getEventManager();
		if(em != 0){
			mStartEvent = em->registerForEvent(DynamicsPlotConstants::EVENT_CALCULATION_STARTED, this);
			mFinishEvent = em->registerForEvent(DynamicsPlotConstants::EVENT_CALCULATION_COMPLETED, this);
		}
	
		if(mStartEvent == 0 || mFinishEvent == 0) {
			Core::log("Exporter: Could not get start or finish event.", true);
			ok = false;
		}

		vM = Core::getInstance()->getValueManager();
		mActiveCalculatorValue = vM->getValue(DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER);
		mPlotterProgramValue = vM->getValue(DynamicsPlotConstants::VALUE_PLOTTER_OUTPUT_FORMAT);		
		
		return ok;
	}

	bool Exporter::cleanUp() {
		bool ok = true;

		return ok;
	}
	void Exporter::eventOccured(Event *event) {
		if(event == 0) {
			return;
		}else if(event == mStartEvent) {
 			mRunningCalculator = dynamic_cast<StringValue*>(mActiveCalculatorValue)->get();
			mPlotters = dynamic_cast<StringValue*>(mPlotterProgramValue)->get();
		}else if(event == mFinishEvent){
 			prepareExport(mPlotters);
			
		}
	}
	/**
	 * Transfers the data from the 3dim matrix to a 2dim QString vector. The format is adjusted to the respective plotting program.
	 * @param plotters String containing the names of the plotter programs that will be used.
	 */
	void Exporter::prepareExport(QString plotters){
		MatrixValue *matrix = dynamic_cast<MatrixValue*>(
					vM->getValue(QString("/DynamicsPlotters/" + mRunningCalculator + "/" + QString("Internal/Data"))));
		
		if(matrix == 0) {
			Core::log("Exporter: Could not find matrix value.", true);
			return;
		}
		int width = matrix->getMatrixWidth();
		int height = matrix->getMatrixHeight();
		int depth = matrix->getMatrixDepth();
	
		if(plotters.contains("matlab", Qt::CaseInsensitive)){
			QString delimiter = " ";//seperates the entries in the file
			
			QVector<QVector<QString> > outV(height * depth);// matrix is mirrored along the diagonal and all layers of third dimension are appended
			
			for(int j = 0; j < height; j++){
				
				outV[j].resize(width);
				
				for(int l = 0; l < depth; l++){
					
					outV[l * height + j].resize(width);
					
					for(int k = 0; k < width; k++) {
						outV[l * height + j][k] = QString::number(matrix->get(k, j, l)) + delimiter;
					}
				}
			}
			
			// for every layer in third dimension add { and }:
			for(int l = 0; l < depth; l++){
				outV[l * height + 1][1] = QString("{") + outV[l * height + 1][1];
				outV[l * height + height - 1][width - 1] = outV[l * height + height -1][width - 1].simplified() + QString("}");
			}
			
			QString xDescr = vM->getValue(QString("/DynamicsPlotters/" 
												  + mRunningCalculator + "/" 
												  + QString("Config/XAxisDescription")))
									->getValueAsString(); //Description for x-axis
									
			QString yDescr = vM->getValue(QString("/DynamicsPlotters/" 
												  + mRunningCalculator + "/" 
												  + QString("Config/YAxisDescription")))
									->getValueAsString();
									
			outV[0][0] = "\"" + mRunningCalculator + " ;; " 
							  + xDescr + " ;; " 
							  + yDescr + "\" "; // add name of calculator; add quotation marks to keep text together
			
			
			Value *mOutputPath = vM->getValue(QString("/DynamicsPlotters/" + mRunningCalculator + "/" + QString("OutputPath")));
			
			if(mOutputPath == 0 || mOutputPath->getValueAsString() == "")
			{
				writeToFile(outV, "matlabExport");
			}
			else {
				writeToFile(outV, mOutputPath->getValueAsString());
			}
		}
		
	}
	/**
	 * Prints the 2-dimensional vector to a file. 
	 * @param outV 2-dim QString vector containing the calculated data.
	 * @param fileName QString with the name of the output file.
	 */
	void Exporter::writeToFile(QVector<QVector<QString> > outV, QString fileName ) {
		
		QFile file(fileName);
		
		if(!file.open(QIODevice::WriteOnly)) {
			Core::log("Exporter: Cannot write to file. Please check the file path and make sure "
					  "you have writing permission for the specified directory.", true);
			return;
		}
		
		QTextStream out(&file);
		
		for(int j = 0; j < outV.size(); j++) {
			
			QString strColumn;
			
			for(int k = 0; k < outV[j].size(); k++){
				strColumn.append(outV[j][k]); //append all data from column
			}
			
			if (!strColumn.isEmpty()){
				out << strColumn << endl; // print next line
			}
		}
		file.close();
	}
}



			
