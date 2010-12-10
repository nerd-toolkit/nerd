
cmake_minimum_required(VERSION 2.6)

add_subdirectory(neuralNetwork)
add_subdirectory(testNeuralNetwork)
add_subdirectory(networkEditor)

add_dependencies(neuralNetwork nerd)
add_dependencies(testNeuralNetwork neuralNetwork nerd)
add_dependencies(networkEditor neuralNetwork nerd)
