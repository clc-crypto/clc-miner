# CLC Miner
A fast miner, written in C++ for the CLC crypto.

### Linux Installation Guide

Clone this repository locally
```bash
git clone https://github.com/clc-crypto/clc-miner.git
```

Build the miner
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
mv clcminer ../
cd ..
rm -fr build
```
#### If you encounter any errors, please install all the required libraries.

Run the miner
```./clcminer```

### Very Important!
#### For your coins to be saved during mining, create a "rewards" directory beside the clcminer executable.
