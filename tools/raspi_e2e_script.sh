sudo apt install git cmake -y 

mkdir Source
cd Source
git clone --recursive https://github.com/Azure/azure-iot-sdk-c c-sdk
cd c-sdk
mkdir cmake
cd cmake
cmake .. -DCMAKE_BUILD_TYPE=Debug -Drun_e2e_tests=ON -Drun_unit_tests=ON