#!/bin/bash

c++ -fdiagnostics-color=always \
    -g \
    -shared \
    -fPIC \
    -I/opt/sundials/include \
    -I./lib/symengine/symengine \
    -I./lib/rapidjson/include \
    -I./lib/Eigen \
    $(python3.8 -m pybind11 --includes) \
    ./*.cpp \
    -o ./bin/environment$(python3.8-config --extension-suffix) \
    -L/opt/sundials/lib \
    -lsundials_cvode \
    -lsundials_nvecserial \
    -lsundials_sunmatrixdense \
    -lsundials_sunlinsoldense \
    -lsundials_core \
    -lsymengine \
    -lgmp \
    -lpython3.8

#    -std=c++14 \
#    -O3 \
#    -Wall \