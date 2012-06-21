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




#include "TestMatlabExporter.h"
#include <iostream>
#include "Core/Core.h"
#include "Network/NeuralNetwork.h"
#include "ActivationFunction/AdditiveTimeDiscreteActivationFunction.h"
#include "SynapseFunction/SimpleSynapseFunction.h"
#include "TransferFunction/TransferFunctionRamp.h"
#include "Adapters/TransferFunctionAdapter.h"
#include <Util/NeuralNetworkUtil.h>
#include <Util/DynamicsPlotterUtil.h>
#include "Exporters/MatlabExporter.h"
#include "Value/MatrixValue.h"

using namespace std;
using namespace nerd;

void TestMatlabExporter::initTestCase() {
}

void TestMatlabExporter::cleanUpTestCase() {
}


//Chris
void TestMatlabExporter::testConstructors() {
	Core::resetCore();
	
	int numberOfSystemObjects = Core::getInstance()->getSystemObjects().size();
	
	MatlabExporter *exporter = new MatlabExporter();
	
	QCOMPARE(Core::getInstance()->getSystemObjects().size(), numberOfSystemObjects + 1);
	QVERIFY(Core::getInstance()->getSystemObjects().contains(exporter));
	
	//TODO test value connections when implemented.
	
}


//Chris
void TestMatlabExporter::testMatlabExport() { 

	MatlabExporter *exporter = new MatlabExporter();
	
	QVERIFY(Core::getInstance()->init());
	ValueManager *vm = Core::getInstance()->getValueManager();
	
	QString diagramName = "MyDiagram";
	QString fileName = "testMatlabOutput.txt";
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == false);
	
	MatrixValue *matrix = new MatrixValue(20, 30, 3);
	StringValue *axisDescriptions = new StringValue();
	StringValue *titleNames = new StringValue();
	
	
	
	//************************************************************************************************************
	//Test detection of required values in ValueManager.
	
	//provide values in the wrong order
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Internal/Data", axisDescriptions));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/AxisNames", axisDescriptions));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/TitleNames", titleNames));
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == false);
	
	vm->removeValue(matrix);
	vm->removeValue(axisDescriptions);
	vm->removeValue(titleNames);
	
	
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Internal/Data", matrix));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/AxisNames", axisDescriptions));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/TitleNames", matrix));
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == false);
	
	vm->removeValue(matrix);
	vm->removeValue(axisDescriptions);
	vm->removeValue(titleNames);
	
	
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Internal/Data", matrix));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/AxisNames", matrix));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/TitleNames", titleNames));
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == false);
	
	vm->removeValue(matrix);
	vm->removeValue(axisDescriptions);
	vm->removeValue(titleNames);
	
	
	//provide the correct order
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Internal/Data", matrix));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/AxisNames", axisDescriptions));
	QVERIFY(vm->addValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/TitleNames", titleNames));
	
	QVERIFY(vm->getValue("/DynamicsPlotters/" + diagramName + "/Internal/Data") == matrix);
	QVERIFY(vm->getValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/AxisNames") == axisDescriptions);
	QVERIFY(vm->getValue("/DynamicsPlotters/" + diagramName + "/Config/Diagram/TitleNames") == titleNames);
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	
	QFile file(fileName);
	QVERIFY(file.exists());
	QVERIFY(findTextInFile(fileName, "names = {'x' 'y' 'x' 'y' 'x' 'y' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'Title 1' 'Title 2' 'Title 3' };", true));
	QVERIFY(findTextInFile(fileName, "matrices = {", false));
	//TODO test content
	
	
	//************************************************************************************************************
	//test correct title and axis descriptions...
	axisDescriptions->set("\\theta,output|bias,activation|a(t-1),a(t)");
	titleNames->set("OutputPlot|ActivationPlot|First Return Map");
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'\\theta' 'output' 'bias' 'activation' 'a(t-1)' 'a(t)' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'OutputPlot' 'ActivationPlot' 'First Return Map' };", true));
	
	
	//************************************************************************************************************
	//test incorrect titles and axis descriptions
	
	//too many number of entries (superfluous entries should be removed)
	axisDescriptions->set("\\theta,output|bias,activation|a(t-1),a(t)|extra1,extra2");
	titleNames->set("OutputPlot|ActivationPlot|First Return Map|Extra1");
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'\\theta' 'output' 'bias' 'activation' 'a(t-1)' 'a(t)' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'OutputPlot' 'ActivationPlot' 'First Return Map' };", true));
	
	matrix->resize(10, 20, 2);
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'\\theta' 'output' 'bias' 'activation' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'OutputPlot' 'ActivationPlot' };", true));
	matrix->resize(20, 30, 3);
	
	
	//too few entires
	axisDescriptions->set("\\theta,output|bias,activation");
	titleNames->set("OutputPlot|ActivationPlot");
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'x' 'y' 'x' 'y' 'x' 'y' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'Title 1' 'Title 2' 'Title 3' };", true));
	
	matrix->resize(10, 20, 2);
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'\\theta' 'output' 'bias' 'activation' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'OutputPlot' 'ActivationPlot' };", true));
	matrix->resize(20, 30, 3);
	
	
	//************************************************************************************************************
	//test empty axis descriptions and title
	axisDescriptions->set("");
	titleNames->set("");
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "names = {'x' 'y' 'x' 'y' 'x' 'y' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'Title 1' 'Title 2' 'Title 3' };", true));
	
	
	
	//************************************************************************************************************
	//************************************************************************************************************
	//test matrix entries
	matrix->resize(10, 5, 2);
	for(int d = 0; d < matrix->getMatrixDepth(); ++d) {
		for(int h = 0; h < matrix->getMatrixHeight(); ++h) {
			for(int w = 0; w < matrix->getMatrixWidth(); ++w) {
				matrix->set(w + (100 * h) + (10000 * d), w, h, d);
			}
		}
	}
	
	QVERIFY(exporter->exportMatrix(diagramName, fileName) == true);
	QVERIFY(findTextInFile(fileName, "matrices = {[0; 1; 2; 3; 4; 5; 6; 7; 8; 9; "
									 "400; 401; 402; 403; 404; 405; 406; 407; 408; 409; "
									 "300; 301; 302; 303; 304; 305; 306; 307; 308; 309; "
									 "200; 201; 202; 203; 204; 205; 206; 207; 208; 209; "
									 "100; 101; 102; 103; 104; 105; 106; 107; 108; 109], "
									 "[10000; 10001; 10002; 10003; 10004; 10005; 10006; 10007; 10008; 10009; "
									 "10400; 10401; 10402; 10403; 10404; 10405; 10406; 10407; 10408; 10409; "
									 "10300; 10301; 10302; 10303; 10304; 10305; 10306; 10307; 10308; 10309; "
									 "10200; 10201; 10202; 10203; 10204; 10205; 10206; 10207; 10208; 10209; "
									 "10100; 10101; 10102; 10103; 10104; 10105; 10106; 10107; 10108; 10109]};", true));
	QVERIFY(findTextInFile(fileName, "names = {'x' 'y' 'x' 'y' };", true));
	QVERIFY(findTextInFile(fileName, "titles = {'Title 1' 'Title 2' };", true));
	
	
}


bool TestMatlabExporter::findTextInFile(const QString &fileName, const QString &text, bool entireLinesOnly) {
	
	QFile file(fileName);
	if(!file.exists()) {
		return false;
	}
	
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		file.close();
		return false;
	}
	
	QTextStream input(&file);
	
	while(!input.atEnd()) {
		QString line = input.readLine();
		if(entireLinesOnly && line == text) {
			return true;
		}
		if(!entireLinesOnly && line.contains(text)) {
			return true;
		}
	}
	
	return false;
}




