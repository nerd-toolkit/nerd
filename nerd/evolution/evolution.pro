QT -= gui

INCLUDEPATH += ../system/orcs

LIBS += ../bin/liborcs.a

TARGETDEPS += ../bin/liborcs.a

TEMPLATE = subdirs

CONFIG += ordered





SUBDIRS += evolution \
  neuroEvolution \
  neuroAndSimEvaluation \
  testEvolution \
  testNeuroEvolution

