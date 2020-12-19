#!/bin/bash

mkdir build
cd build
cmake ..
make Test
./Test
cd ..

lcov --directory ./build/CMakeFiles/Test.dir/ --capture --output-file ./code_coverage.info -rc lcov_branch_coverage=1
genhtml code_coverage.info --branch-coverage --output-directory ./code_coverage/

rm code_coverage.info
rm -r build
