TEMPLATE = lib

CONFIG += staticlib \
debug \
rtti \
opengl \
x11 \
qtestlib \
build_all \
console
CONFIG -= release

QT += xml \
network \
opengl \
script
INCLUDEPATH += ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/networkEditor \
  ../../neuro/neuralNetwork \
  ../../system/nerd

DESTDIR = ../../bin

SOURCES += OrcsModelOptimizerConstants.cpp \
Optimization/OptimizationManager.cpp \
 Optimization/TrainSet.cpp \
 Optimization/XMLConfigDocument.cpp \
 Optimization/XMLConfigLoader.cpp \
 Optimization/SimulationData.cpp \
 Optimization/Tools.cpp \
 Optimization/Trigger.cpp \
 Optimization/Normalization.cpp \
 Optimization/PhysicalData.cpp \
 Optimization/PhysicalDataSource.cpp \
 Optimization/BinFilePhysicalDataSource.cpp \
 Optimization/StatisticTools.cpp \
 Optimization/MotionData.cpp \
 Optimizer/ModelOptimizer.cpp \
 Optimizer/TestModelOptimizer.cpp \
 Optimizer/PrintModelOptimizer.cpp \
 Optimizer/PowellModelOptimizer.cpp \
 Math/MultiToOneDimFunction.cpp \
 Math/ParabolicBracketFinder.cpp \
 Math/OneDimMinimizer.cpp \
 Math/BrentOneDimMinimizer.cpp \
 Math/PowellMultiDimMinimizer.cpp \
 Optimizer/QuadraticErrorFunction.cpp \
 Optimizer/ReducedChiSquareErrorFunction.cpp \
 Math/MultiDimMinimizer.cpp \
 Optimizer/ErrorFunction.cpp \
 Optimization/ReportLogger.cpp \
 Optimizer/CheckDataSourceIntegrity.cpp \
 Evolution/ESCommaSelection.cpp \
 Evolution/ESIndividual.cpp \
 Evolution/ESMarriage.cpp \
 Evolution/ESPlusSelection.cpp \
 Evolution/MuSlashRhoLambdaES.cpp \
 Evolution/ESRecombination.cpp \
 Evolution/ESDiscreteRecombination.cpp \
 Evolution/ESIntermediateRecombination.cpp \
 Evolution/ESMultiDensityFuncObjParaMutation.cpp \
 Evolution/ESSingleDensityFuncObjParaMutation.cpp \
 Evolution/ESSingleDensityFuncStratParaMutation.cpp \
 Evolution/ESMultiDensityFuncStratParaMutation.cpp \
 Evolution/ESOneFifthRuleStratParaMutation.cpp \
 Evolution/ESRandomInitialize.cpp \
 Optimizer/MuSlashRhoLambdaESOptimizer.cpp \
 Optimizer/ESSingleClientFitnessEvaluation.cpp \
 Player/KeyFramePlayerWrapper.cpp \
 Player/PhysicalDataPlayer.cpp \
 Player/Player.cpp \
 Optimization/TextFilePhysicalDataSource.cpp \
 Optimizer/CalcErrorFunction.cpp
HEADERS += OrcsModelOptimizerConstants.h \
Optimization/OptimizationManager.h \
 Optimization/TrainSet.h \
 Optimization/XMLConfigDocument.h \
 Optimization/XMLConfigLoader.h \
 Optimization/SimulationData.h \
 Optimization/Tools.h \
 Optimization/Trigger.h \
 Optimization/Normalization.h \
 Optimization/PhysicalData.h \
 Optimization/PhysicalDataSource.h \
 Optimization/BinFilePhysicalDataSource.h \
 Optimization/StatisticTools.h \
 Optimization/MotionData.h \
 Optimizer/ModelOptimizer.h \
 Optimizer/TestModelOptimizer.h \
 Optimizer/PrintModelOptimizer.h \
 Optimizer/PowellModelOptimizer.h \
 Math/MultiToOneDimFunction.h \
 Math/OneDimFunction.h \
 Math/BracketFinder.h \
 Math/OneDimFunctionPoint.h \
 Math/ParabolicBracketFinder.h \
 Math/MultiDimFunction.h \
 Math/MultiDimFunctionPoint.h \
 Math/OneDimMinimizer.h \
 Math/BrentOneDimMinimizer.h \
 Math/MultiDimMinimizer.h \
 Math/PowellMultiDimMinimizer.h \
 Optimizer/QuadraticErrorFunction.h \
 Optimizer/ReducedChiSquareErrorFunction.h \
 Optimizer/ErrorFunction.h \
 Optimization/ReportLogger.h \
 Optimizer/CheckDataSourceIntegrity.h \
 Evolution/ESCommaSelection.h \
 Evolution/ESFitnessEvaluation.h \
 Evolution/ESIndividual.h \
 Evolution/ESMarriage.h \
 Evolution/ESObjectParametersMutation.h \
 Evolution/ESPlusSelection.h \
 Evolution/ESSelection.h \
 Evolution/ESStrategyParametersMutation.h \
 Evolution/MuSlashRhoLambdaES.h \
 Evolution/ESRecombination.h \
 Evolution/ESDiscreteRecombination.h \
 Evolution/ESIntermediateRecombination.h \
 Evolution/ESMultiDensityFuncObjParaMutation.h \
 Evolution/ESSingleDensityFuncObjParaMutation.h \
 Evolution/ESSingleDensityFuncStratParaMutation.h \
 Evolution/ESInformation.h \
 Evolution/ESMultiDensityFuncStratParaMutation.h \
 Evolution/ESOneFifthRuleStratParaMutation.h \
 Evolution/ESInitialize.h \
 Evolution/ESRandomInitialize.h \
 Value/ValueRange.h \
 Value/OptimizationDouble.h \
 Optimizer/MuSlashRhoLambdaESOptimizer.h \
 Optimizer/ESSingleClientFitnessEvaluation.h \
 Evolution/ESInitializeValue.h \
 Player/KeyFramePlayerWrapper.h \
 Player/PhysicalDataPlayer.h \
 Player/Player.h \
 Optimization/TextFilePhysicalDataSource.h \
 Optimizer/CalcErrorFunction.h
TARGETDEPS += ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libnetworkEditor.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a

