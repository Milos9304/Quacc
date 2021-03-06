#!/bin/bash

CC=gcc
CXX=g++
FC=gfortran

BUILD_TYPE=Release
#INSTALL_DIR=$HOME/Quantum/XACC/installdir/xacc
BUILD_TESTS=TRUE
BUILD_EXAMPLES=TRUE
QUEST_DIR="/home/nx05/nx05/milos_pr/QuEST"

cmake \
  -DCMAKE_C_COMPILER=${CC} \
  -DCMAKE_CXX_COMPILER=${CXX} \
  -DCMAKE_Fortran_COMPILER=${FC} \
  -DQUEST_DIR=${QUEST_DIR} \
..
