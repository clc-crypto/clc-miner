cd build
cmake ..
make -j$(nproc)
rm ../clcminer
mv clcminer ../
cd ..
./clcminer
