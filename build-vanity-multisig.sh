#!/bin/bash
LIBS="-L$(pwd)/depends_build/lib -Wl,-Bstatic -lbitcoin-system -lsecp256k1 -Wl,-Bdynamic"
g++ -std=c++11 -g0 -O2 -march=native vanity-multisig.cpp -I$(pwd)/depends_build/include ${LIBS} -lboost_system -lboost_thread -lboost_program_options -lboost_regex -lgmp -pthread -o vanity-multisig