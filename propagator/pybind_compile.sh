#!/bin/bash

c++ -fdiagnostics-color=always \
    -g \
    -O3 \
    -Wall \
    -shared \
    -std=c++14 \
    -fPIC \
    -I/opt/sundials/include \
    -I/home/whitehole/PycharmProjects/master_thesis/propagator/lib/symengine/symengine \
    -I/home/whitehole/PycharmProjects/master_thesis/propagator/lib/rapidjson/include \
    $(python3.8 -m pybind11 --includes) \
    /home/whitehole/PycharmProjects/master_thesis/propagator/*.cpp \
    -o /home/whitehole/PycharmProjects/master_thesis/propagator/bin/environment$(python3.8-config --extension-suffix) \
    -L/opt/sundials/lib \
    -lsundials_cvode \
    -lsundials_nvecserial \
    -lsundials_sunmatrixdense \
    -lsundials_sunlinsoldense \
    -lsundials_core \
    -lsymengine \
    -lgmp \
    -lpython3.8
