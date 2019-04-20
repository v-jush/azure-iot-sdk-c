
if ! grep -Fxq "systemctl start ssh" /etc/rc.local
then
    echo 'Enabling automatic start of ssh on bootup...'
    sudo sed -i '19i\systemctl start ssh' /etc/rc.local
    # code if not found
fi

sudo apt-get update -y
sudo apt-get install -y vim git build-essential pkg-config git cmake libssl-dev uuid-dev valgrind

if ! grep -Fxq "CURL_ROOT" ~/.bashrc
then
    echo 'Enabling automatic start of ssh on bootup...'
    echo 'export CURL_ROOT=/home/pi/curl_install' >> ~/.bashrc 
    # code if not found
fi

if ! grep -Fxq "LD_LIBRARY_PATH" ~/.bashrc
then 
    echo 'export LD_LIBRARY_PATH'
    echo 'export LD_LIBRARY_PATH=$CURL_ROOT/lib' >> ~/.bashrc
fi

echo 'Setting variables by calling bashrc'
source ~/.bashrc

# BUILD AND INSTALL NEW CURL
wget https://curl.haxx.se/download/curl-7.64.1.tar.gz
mkdir $CURL_ROOT
mkdir curl_source
tar -C curl_source -xzvf curl-7.64.1.tar.gz
cd curl_source/curl-7.64.1/
./configure --prefix=$CURL_ROOT --with-ssl --with-zlib --disable-shared --enable-static 
make -j
make install


# BUILD THE SDK
cd ~
cd azure-iot-sdk-c
mkdir cmake && cd cmake
cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..