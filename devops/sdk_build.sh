# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

mkdir cmake
cd cmake 
[ $? -eq 0 ] || { echo "cd cmake failed "; exit 1; }

cmake -D C_SDK_ROOT=/sdk -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
    -Duse_prov_client:BOOL=OFF \
    -DCMAKE_INSTALL_PREFIX=${TOOLCHAIN_PREFIX} \ 
    -DCMAKE_BUILD_TYPE=Debug \
    -Drun_e2e_tests:BOOL=ON \
    -Drun_unittests=ON \
    -D use_amqp=ON -D use_mqtt=ON -D use_http=ON \
    ..

[ $? -eq 0 ] || { echo "cmake failed"; exit 1; }

cmake --build .
[ $? -eq 0 ] || { echo "make failed"; exit 1; }
