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

#include "MatlabExporter.h"
#include <iostream>
#include "Core/Core.h"
#include <QFile>
#include <QTextStream>
#include "Event/EventManager.h"
#include <QVector>
#include "DynamicsPlotConstants.h"
#include <QStringList>
#include <QDate>
#include <qdir.h>
#include <Util/DynamicsPlotterUtil.h>

using namespace std;

namespace nerd {
	
	
	/**
	 * Constructs a new MatlabExporter.
	 */
	MatlabExporter::MatlabExporter()
	: SystemObject(), EventListener(), mExportFormatValue(0), mActiveCalculatorValue(0)
	{
		Core::getInstance()->addSystemObject(this);
	}
	
	/**
	 * Destructor.
	 */
	MatlabExporter::~MatlabExporter() {
	}
	
	
	QString MatlabExporter::getName() const {
		return "MatlabExporter";
	}	
	
	
	bool MatlabExporter::init() {
		bool ok = true;
		
		return ok;
	}
	
	
	bool MatlabExporter::bind() {
		bool ok = true;
		
		EventManager *em = Core::getInstance()->getEventManager();
		if(em != 0){
			mStartEvent = em->registerForEvent(
									DynamicsPlotConstants::EVENT_CALCULATION_STARTED, this);
			mFinishEvent = em->registerForEvent(
									DynamicsPlotConstants::EVENT_CALCULATION_COMPLETED, this);
		}
		
		if(mStartEvent == 0 || mFinishEvent == 0) {
			Core::log("MatlabExporter: Could not get start or finish event. Disabling MatlabExporter", true);
		}
		
		mValueManager = Core::getInstance()->getValueManager();
		
		mActiveCalculatorValue = mValueManager->getStringValue(
									DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER);
		mExportFormatValue = mValueManager->getStringValue(
									DynamicsPlotConstants::VALUE_PLOTTER_OUTPUT_FORMAT);
		
		if(mActiveCalculatorValue == 0) {
			Core::log("MatlabExporter: Could not find requried value [" 
						+ DynamicsPlotConstants::VALUE_PLOTTER_ACTIVE_PLOTTER
						+ "]. MatlabExporter disabled.");
		}
		if(mExportFormatValue == 0) {
			Core::log("MatlabExporter: Could not find requried value [" 
					  + DynamicsPlotConstants::VALUE_PLOTTER_OUTPUT_FORMAT
					  + "]. MatlabExporter disabled.");
		}
		
		return ok;
	}
	
	
	bool MatlabExporter::cleanUp() {
		bool ok = true;
		
		return ok;
	}
	
	
	void MatlabExporter::eventOccured(Event *event) {
		if(event == 0) {
			return;
		}
		else if(event == mStartEvent && mActiveCalculatorValue != 0) {
			mActiveCalculatorName =mActiveCalculatorValue->get();
// 			mPlotters = dynamic_cast<StringValue*>(mPlotterProgramValue)->get();
		}
		else if(event == mFinishEvent && mActiveCalculatorValue != 0){
			exportMatrix(mActiveCalculatorName, "");
		}
	}
	
	
	bool MatlabExporter::exportMatrix(const QString diagramPlotterName, const QString &fileName) {
		
		ValueManager *vm = Core::getInstance()->getValueManager();
		
		Core::getInstance()->enforceDirectoryPath(QDir::currentPath() + "/matlab");
		
		MatrixValue *matrix = dynamic_cast<MatrixValue*>(vm->getValue(
			"/DynamicsPlotters/" + diagramPlotterName + "/Internal/Data"));
		
		
		
		StringValue *axisNames = vm->getStringValue("/DynamicsPlotters/" 
													 + diagramPlotterName 
													 + "/Config/Diagram/AxisNames");
		
		StringValue *titleNames = vm->getStringValue("/DynamicsPlotters/" 
													 + diagramPlotterName 
													 + "/Config/Diagram/TitleNames");
		
		StringValue *outputPath = vm->getStringValue("/DynamicsPlotters/" 
													 + diagramPlotterName 
													 + "/Config/Diagram/FilePrefix");
		
		if(matrix == 0  || axisNames == 0 || titleNames == 0 || outputPath == 0) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: Could not find required values for diagram ["
						+ diagramPlotterName + "]. Skipping matlab export...");
			return false;
		}
		
		//prepare filename
		QString outputFileName = fileName;
		if(outputFileName == "") {
			if(outputPath->get() != "") {
				outputFileName = "./matlab/" + outputPath->get();
			}
			else {
				outputFileName = "./matlab/export";
			}
			outputFileName += QDate::currentDate().toString("_dd_MM_yyyy_")
								.append(QTime::currentTime().toString("hh_mm_ss")).append(".m");
		}
		
		
		//prepare axis names
		QStringList axisDescriptions;
		
		QStringList axisDescParts = axisNames->get().split("|");
		if(axisDescParts.size() == 0) {
			for(int i = 0; i < matrix->getMatrixDepth(); ++i) {
				axisDescParts.append("x,y");
			}
		}
		if(axisDescParts.size() < matrix->getMatrixDepth()) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: The number of axis descriptions does not fit the number of diagrams. "
					  "Using default.");
			axisDescParts.clear();
			for(int i = 0; i < matrix->getMatrixDepth(); ++i) {
				axisDescParts.append("x,y");
			}
		}
		//shrink number of descriptions down to number of diagrams.
		while(axisDescParts.size() > matrix->getMatrixDepth()) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: Too many axis descriptions. [" + axisDescParts.last() + "]. "
					  "Removing superfluous entry.");
			axisDescParts.removeLast();
		}
		for(int i = 0; i < axisDescParts.size(); ++i) {
			QStringList axes = axisDescParts.at(i).split(",");
			if(axes.size() != 2) { 
				DynamicsPlotterUtil::reportProblem("MatlabExporter: One of the axis descriptions does not have the correct format 'x,y'. "
						  "Using default.");
				axes.clear();
				axes << "x" << "y";
			}
			axisDescriptions << axes;
		}
		
		
		//prepare diagram titles
		QStringList diagramTitles = titleNames->get().split("|");
		if(diagramTitles.size() < matrix->getMatrixDepth()) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: The number of titles does not match the number of diagrams. "
					  "Skipping titles.");
			diagramTitles.clear();
		}
		//shrink number of descriptions down to number of diagrams.
		while(diagramTitles.size() > matrix->getMatrixDepth()) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: Too many diagram titles. [" + diagramTitles.last() + "]. "
					  "Removing superfluous entry.");
			diagramTitles.removeLast();
		}
		
		
		//prepare file
		QFile file(outputFileName);
		
		if(!file.open(QIODevice::WriteOnly)) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter: Cannot write to file [" + outputFileName + "]. "
						"Please check the file path and make sure "
						"you have writing permission for the specified directory.");
			return false;
		}
		
		QTextStream out(&file);
		
		
		//write data
		out << "matrices = {";
		
		for(int i = 0; i < matrix->getMatrixDepth(); ++i) {
			if(i != 0) {
				out << ", ";
			}
			out << "[";
			
			//add matric content in reverse order
			for(int j = matrix->getMatrixHeight() - 1; j >= 0; --j) {
				if(j != matrix->getMatrixHeight() - 1) {
					out << "; ";
				}
				addMatrixRow(out, matrix, i, j);
			}
			out << "]";
		}
		out << "};" << endl;
		
		
		//write axis names
		out << "names = {";
		for(int i = 0; i * 2  < axisDescriptions.size() - 1; ++i) {
			out << "'" << axisDescriptions.at(i * 2) << "' '" << axisDescriptions.at(i * 2 + 1) << "' ";
		}
		out << "};\n";
		
		
		//write diagram titles
		out << "titles = {";
		if(diagramTitles.empty()) {
			for(int i = 0; i < matrix->getMatrixDepth(); ++i) {
				out << "'Title " << (i + 1) << "' ";
			}
		}
		else {
			for(int i = 0; i < diagramTitles.size(); ++i) {
				out << "'" << diagramTitles.at(i) << "' ";
			}
		}
		out << "};\n";
		
		DynamicsPlotterUtil::reportProblem("MatlabExporter: Stored diagram data in file [" + outputFileName + "]");
		
		return true;
	}
	
	void MatlabExporter::addMatrixRow(QTextStream &out, MatrixValue *matrix, int layer, int row) {
		if(layer >= matrix->getMatrixDepth() || row >= matrix->getMatrixHeight()) {
			DynamicsPlotterUtil::reportProblem("MatlabExporter::addMatrixRow: Layer or row out of bound!");
			return;
		}
		
		for(int i = 0; i < matrix->getMatrixWidth(); ++i) {
			out << matrix->get(i, row, layer);
			if(i < matrix->getMatrixWidth() - 1) {
				out << " ";
			}
		}
	}
	
	
// 	/**
// 	 * Transfers the data from the 3dim matrix to a 2dim QString vector. The format is adjusted to the respective plotting program.
// 	 * @param plotters String containing the names of the plotter programs that will be used.
// 	 */
// 	void MatlabExporter::prepareExport(QString plotters){
// 		MatrixValue *matrix = dynamic_cast<MatrixValue*>(
// 			vM->getValue(QString("/DynamicsPlotters/" + mRunningCalculator + "/" + QString("Internal/Data"))));
// 			
// 			if(matrix == 0) {
// 				Core::log("MatlabExporter: Could not find matrix value.", true);
// 			return;
// 			}
// 			int width = matrix->getMatrixWidth();
// 			int height = matrix->getMatrixHeight();
// 			int depth = matrix->getMatrixDepth();
// 			
// 			if(plotters.contains("matlab", Qt::CaseInsensitive)){
// 			QString delimiter = " ";//seperates the entries in the file
// 			
// 			QVector<QVector<QString> > outV(height * depth);// matrix is mirrored along the diagonal and all layers of third dimension are appended
// 			
// 			for(int j = 0; j < height; j++){
// 				
// 				outV[j].resize(width);
// 				
// 				for(int l = 0; l < depth; l++){
// 					
// 					outV[l * height + j].resize(width);
// 					
// 					for(int k = 0; k < width; k++) {
// 						outV[l * height + j][k] = QString::number(matrix->get(k, j, l)) + delimiter;
// 					}
// 				}
// 			}
// 			
// 			// for every layer in third dimension add { and }:
// 			for(int l = 0; l < depth; l++){
// 				outV[l * height + 1][1] = QString("{") + outV[l * height + 1][1];
// 				outV[l * height + height - 1][width - 1] = outV[l * height + height -1][width - 1].simplified() + QString("}");
// 			}
// 			
// 			QString xDescr = vM->getValue(QString("/DynamicsPlotters/" 
// 												  + mRunningCalculator + "/" 
// 																		 + QString("Config/XAxisDescription")))
// 																		 ->getValueAsString(); //Description for x-axis
// 																		 
// 																		 QString yDescr = vM->getValue(QString("/DynamicsPlotters/" 
// 																											   + mRunningCalculator + "/" 
// 																																	  + QString("Config/YAxisDescription")))
// 																																	  ->getValueAsString();
// 																																	  
// 																																	  outV[0][0] = "\"" + mRunningCalculator + " ;; " 
// 																																											   + xDescr + " ;; " 
// 																																														  + yDescr + "\" "; // add name of calculator; add quotation marks to keep text together
// 																																														  
// 																																														  
// 																																														  Value *mOutputPath = vM->getValue(QString("/DynamicsPlotters/" + mRunningCalculator + "/" + QString("OutputPath")));
// 																																														  
// 																																														  if(mOutputPath == 0 || mOutputPath->getValueAsString() == "")
// 																																														  {
// 																																															  writeToFile(outV, "matlabExport");
// 																																														  }
// 																																														  else {
// 																																															  writeToFile(outV, mOutputPath->getValueAsString());
// 																																														  }
// 			}
// 			
// 	}
// 	/**
// 	 * Prints the 2-dimensional vector to a file. 
// 	 * @param outV 2-dim QString vector containing the calculated data.
// 	 * @param fileName QString with the name of the output file.
// 	 */
// 	void MatlabExporter::writeToFile(QVector<QVector<QString> > outV, QString fileName ) {
// 		
// 		QFile file(fileName);
// 		
// 		if(!file.open(QIODevice::WriteOnly)) {
// 			Core::log("MatlabExporter: Cannot write to file. Please check the file path and make sure "
// 					  "you have writing permission for the specified directory.", true);
// 			return;
// 		}
// 		
// 		QTextStream out(&file);
// 		
// 		for(int j = 0; j < outV.size(); j++) {
// 			
// 			QString strColumn;
// 			
// 			for(int k = 0; k < outV[j].size(); k++){
// 				strColumn.append(outV[j][k]); //append all data from column
// 			}
// 			
// 			if (!strColumn.isEmpty()){
// 				out << strColumn << endl; // print next line
// 			}
// 		}
// 		file.close();
// 	}
}




