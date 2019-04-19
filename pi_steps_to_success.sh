sudo sed -i '19i\systemctl start ssh' /etc/rc.local
sudo apt-get update -y
sudo apt-get install -y vim git build-essential pkg-config git cmake libssl-dev uuid-dev valgrind

export CURL_ROOT=/home/pi/curl_install
wget https://curl.haxx.se/download/curl-7.64.1.tar.gz
mkdir $CURL_ROOT
mkdir curl_source
tar -C curl_source -xzvf curl-7.64.1.tar.gz
cd curl_source/curl-7.64.1/
./configure --prefix=$CURL_ROOT --with-ssl --with-zlib
make -j
make install
vim /etc/rc.local


export LD_LIBRARY_PATH=$CURL_ROOT/lib/

cd ~
cd azure-iot-sdk-c
mkdir cmake && cd cmake
ls
cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug ..
cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..