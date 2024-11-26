#!/bin/bash

pushd src
flex -o semic_token.cpp semic.l
bison -d -o semic_grammar.cpp semic.y
popd
mv ./src/semic_grammar.hpp ./include/
mkdir -p build/ && cd build
cmake ..
cmake --build . 