#!/bin/bash

# sudo apt-get install libgmp-dev libevent-dev libboost-dev libboost-system-dev libboost-filesystem-dev libboost-test-dev libboost-chrono-dev libboost-date-time-dev libboost-iostreams-dev libboost-locale-dev libboost-log-dev libboost-program-options-dev libboost-thread-dev libboost-regex-dev
curdir=$(pwd)
read -p "Download and build depends? [Y/n] " -n 1 -r
if [[ $REPLY =~ ^[Yy]$ ]]
then

mkdir $curdir/depends
mkdir $curdir/depends_build
cd $curdir/depends
git clone -b version5 https://github.com/libbitcoin/secp256k1
cd $curdir/depends/secp256k1
./autogen.sh
./configure --enable-module-recovery --prefix=$curdir/depends_build # --enable-static 
make -j$(nproc)
make install
cd $curdir/depends
git clone https://github.com/libbitcoin/libbitcoin
cd $curdir/depends/libbitcoin
sed -i 's/1.62.0/1.58.0/g' configure.ac 
./autogen.sh
# --enable-static --disable-shared 
secp256k1_LIBS="-L$curdir/depends_build/lib -lsecp256k1 -lgmp" secp256k1_CFLAGS=-I$curdir/depends_build/include ./configure --prefix=$curdir/depends_build
make -j$(nproc)
make install
fi

cd $curdir

# force linker to use static -lbitcoin -lsecp256k1
# g++ -std=c++11 cpp/bip44-quick-gen.cpp cpp/coin_data.cpp -I$curdir/depends_build/include -L$curdir/depends_build/lib -Wl,-Bstatic -lbitcoin -lsecp256k1 -Wl,-Bdynamic -lboost_system -lboost_thread -lboost_program_options -lboost_regex -lgmp -o bip44-quick-gen



