#!/bin/bash


cd ./system/testNerd/;$1/system/testNerd/testNerd;cd ../..;
cd ./simulator/testSimulator;$1/simulator/testSimulator/testSimulator;cd ../..;
cd ./simulator/testOdePhysics;$1/simulator/testOdePhysics/testOdePhysics;cd ../..;
cd ./neuro/testNeuralNetwork;$1/neuro/testNeuralNetwork/testNeuralNetwork;cd ../..;
cd ./evolution/testEvolution;$1/evolution/testEvolution/testEvolution;cd ../..;
cd ./evolution/testNeuroEvolution;$1/evolution/testNeuroEvolution/testNeuroEvolution;cd ../..;
cd ../extensions/NetworkDynamicsDiagramPlotter/testNetworkDynamicsDiagramPlotterLib;$1/../extensions/NetworkDynamicsDiagramPlotter/testNetworkDynamicsDiagramPlotterLib/testNetworkDynamicsDiagramPlotterLib;cd ../../../nerd;
