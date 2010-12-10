TEMPLATE = app

CONFIG += debug \
stl \
rtti \
qt \
opengl \
thread \
x11 \
warn_on \
qtestlib \
build_all \
 console
QT += core \
gui \
sql \
xml \
network \
opengl \
script
INCLUDEPATH += ../OrcsModelOptimizerLib \
  ../../evolution/neuroAndSimEvaluation \
  ../../evolution/neuroEvolution \
  ../../evolution/evolution \
  ../../simulator/odePhysics \
  ../../simulator/simulator \
  ../../neuro/neuralNetwork \
  ../../system/nerd


SOURCES += main.cpp \
 Math/TestParabolicBracketFinder.cpp \
 Math/TestMultiToOneDimFunction.cpp \
 Math/TestBrentOneDimMinimizer.cpp \
 Math/TestPowellMultiDimMinimizer.cpp \
 Evolution/TestESMarriage.cpp \
 Evolution/TestESDiscreteRecombination.cpp \
 Evolution/TestESIntermediateRecombination.cpp \
 Evolution/TestESPlusSelection.cpp \
 Evolution/TestESCommaSelection.cpp \
 Evolution/TestESSingleDensityFuncObjParaMutation.cpp \
 Evolution/TestESMultiDensityFuncObjParaMutation.cpp \
 Evolution/TestESSingleDensityFuncStratParaMutation.cpp \
 Evolution/TestESMultiDensityFuncStratParaMutation.cpp \
 Evolution/TestESOneFifthRuleStratParaMutation.cpp \
 Evolution/TestESRandomInitialize.cpp \
 Evolution/TestMuSlashRhoLambdaES.cpp \
 Evolution/ESTestFitnessEvaluation.cpp \
 Optimization/TestXMLConfigDocument.cpp


HEADERS += Math/TestParabolicBracketFinder.h \
Math/QuadraticFunction.h \
 Math/TwoDimQuadraticFunction.h \
 Math/TestMultiToOneDimFunction.h \
 Math/TestBrentOneDimMinimizer.h \
 Math/TestPowellMultiDimMinimizer.h \
 Evolution/TestESMarriage.h \
 Evolution/TestESDiscreteRecombination.h \
 Evolution/TestESIntermediateRecombination.h \
 Evolution/TestESPlusSelection.h \
 Evolution/TestESCommaSelection.h \
 Evolution/TestESSingleDensityFuncObjParaMutation.h \
 Evolution/TestESMultiDensityFuncObjParaMutation.h \
 Evolution/TestESSingleDensityFuncStratParaMutation.h \
 Evolution/TestESMultiDensityFuncStratParaMutation.h \
 Evolution/TestESOneFifthRuleStratParaMutation.h \
 Evolution/TestESRandomInitialize.h \
 Evolution/TestMuSlashRhoLambdaES.h \
 Evolution/ESTestFitnessEvaluation.h \
 Math/TwoDimQuadraticFunctionWithBounds.h \
 Optimization/TestXMLConfigDocument.h
CONFIG -= release

TARGETDEPS += ../../bin/libOrcsModelOptimizerLib.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libevolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libsimulator.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libnerd.a


LIBS += ../../bin/libOrcsModelOptimizerLib.a \
  ../../bin/libevolution.a \
  ../../bin/libneuralNetwork.a \
  ../../bin/libneuroAndSimEvaluation.a \
  ../../bin/libneuroEvolution.a \
  ../../bin/libodePhysics.a \
  ../../bin/libnerd.a \
  ../../bin/libsimulator.a \
  -L../../bin/ \
  -lOrcsModelOptimizerLib \
  -levolution \
  -lneuralNetwork \
  -lneuroAndSimEvaluation \
  -lneuroEvolution \
  -lsimulator \
  -lodePhysics \
  -lnerd

