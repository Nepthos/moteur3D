cmake_minimum_required(VERSION 3.17)
project(Moteur3D)

set(CMAKE_CXX_STANDARD 14)

add_executable(Moteur3D main.cpp tgaimage.h tgaimage.cpp VectInt.h VectInt.cpp VectFloat.cpp VectFloat.h Matrix.cpp Matrix.h)