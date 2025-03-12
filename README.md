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

## OR

### Use pre-built x64 clcminer

Clone this repository locally
```bash
git clone https://github.com/clc-crypto/clc-miner.git
```

Run clc-miner
```bash
./clcminer64
```
