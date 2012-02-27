
cmake_minimum_required(VERSION 2.6)

add_subdirectory(simulator)
add_subdirectory(testSimulator)
add_subdirectory(odePhysics)
add_subdirectory(testOdePhysics)
add_subdirectory(simple2DPhysics)

add_dependencies(testSimulator simulator nerd)
add_dependencies(odePhysics simulator nerd)
add_dependencies(testOdePhysics odePhysics simulator nerd)
add_dependencies(simple2DPhysics simulator nerd)
