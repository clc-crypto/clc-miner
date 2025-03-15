# CLC Miner
A fast miner, written in C++ for the CLC crypto.

### Linux Installation Guide

Clone this repository locally
```bash
git clone https://github.com/clc-crypto/clc-miner.git
```

Install dependencies
for ubuntu / debian
```bash
sudo apt update && sudo apt install -y cmake g++ make pkg-config libssl-dev libcurl4-openssl-dev libsecp256k1-dev nlohmann-json3-dev
```
for Arch
```bash
sudo pacman -Syu --needed cmake gcc make pkgconf openssl curl secp256k1 nlohmann-json
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

### Use pre-built x64 clcminer for 64 bit systems (might fail!)

Clone this repository locally and install dependencies
```bash
sudo apt update && sudo apt install -y cmake g++ make pkg-config libssl-dev libcurl4-openssl-dev libsecp256k1-dev nlohmann-json3-dev

git clone https://github.com/clc-crypto/clc-miner.git
```

Run clc-miner
```bash
./clcminer64
```
