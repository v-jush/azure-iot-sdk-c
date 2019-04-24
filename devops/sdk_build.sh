# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.

mkdir cmake
cd cmake 
[ $? -eq 0 ] || { echo "cd cmake failed "; exit 1; }

cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake \
    -Duse_prov_client:BOOL=OFF 
    -DCMAKE_INSTALL_PREFIX=${TOOLCHAIN_PREFIX} \
    -Drun_e2e_tests:BOOL=ON \
    -Drun_unittests=ON \
    ..


[ $? -eq 0 ] || { echo "cmake failed"; exit 1; }

cmake --build .
[ $? -eq 0 ] || { echo "make failed"; exit 1; }
