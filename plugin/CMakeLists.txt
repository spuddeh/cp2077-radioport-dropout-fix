cmake_minimum_required(VERSION 3.21)
project(RadioportDropoutFix LANGUAGES CXX)

include(red4ext-plugin.cmake)

add_library(RadioportDropoutFix SHARED src/Main.cpp)
red4ext_plugin(RadioportDropoutFix)
