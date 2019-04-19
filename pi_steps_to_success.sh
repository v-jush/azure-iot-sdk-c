    1  sudo raspi-config
    2  sudo systemctl status ssh
    3  ifconfig
    4  ssh
    5  ssh 127.0.0.1
    6  sudo vim /etc/rc.local
    7  sudo apt install vim
    8  ls
    9  sudo vim /etc/rc.local
   10  sudo systemctl start ssh
   11  sudo systemctl status ssh
   12  logout
   13  vim /etc/rc.local
   14  wget https://curl.haxx.se/download/curl-7.64.1.tar.gz
   15  export CURL_ROOT=/home/curl_install
   16  apt-get update
   17  apt-get install -y vim git build-essential pkg-config git cmake libssl-dev uuid-dev valgrind
   18  sudo su
   19  sudo apt-get update -y
   20  sudo apt-get install -y vim git build-essential pkg-config git cmake libssl-dev uuid-dev valgrind
   21  mkdir curl_source
   22  mkdir $CURL_ROOT
   23  export CURL_ROOT=/home/pi/curl_install
   24  mkdir $CURL_ROOT
   25  tar -C curl_source -xzvf curl-7.64.1.tar.gz
   26  cd curl_source/curl-7.64.1/
   27  ./configure --prefix=$CURL_ROOT --with-ssl
   28  make -j
   29  make install
   30  vim /etc/rc.local
   31  sudo sed -i '19i\systemctl start ssh' /etc/rc.local
   32  vim /etc/rc.local
   33  sudo vim /etc/rc.local
   34  export LD_LIBRARY_PATH=$CURL_ROOT/lib/
   35  git clone --recursive http://github.com/Azure/azure-iot-sdk-c
   36  ls
   37  cd ..
   38  cd ~
   39  git clone --recursive http://github.com/Azure/azure-iot-sdk-c
   40  cd azure-iot-sdk-c
   41  mkdir cmake && cd cmake
   42  ls
   43  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug ..
   44  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
   45  cd iothub_client/tests/iothubclient_mqtt_e2e
   46  ls
   47  cmake --buidl .
   48  cmake --build .
   49  ls
   50  cmake --build .
   51  sudo apt install zlib
   52  sudo apt-cache find zlib
   53  apt-cache search zlib
   54  sudo apt install z
   55  sudo apt install zlib1g-dev
   56  sudo apt install zlib1g
   57  cmake --build .
   58  cd ..
   59  cd samples/
   60  ll
   61  ls -l
   62  cd iothub_ll_telemetry_sample
   63  ll
   64  ls -l
   65  vi ~/azure-iot-sdk-c/iothub_client/samples/iothub_ll_telemetry_sample/iothub_ll_telemetry_sample.c
   66  ls -l
   67  make -j
   68  d ..
   69  cd ..
   70  cd curl_source/
   71  cd curl-7.64.1/
   72  ./configure --help | grep -i z
   73  make uninstall
   74  ./configure --prefix=$CURL_ROOT --with-ssl --with-zlib
   75  cd ..
   76  ls
   77  cd azure-iot-sdk-c/cmake
   78  rm -rf *
   79  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
   80  sudo rm -rf *
   81  cd ~/curl_source/curl-7.64.1/
   82  make -j\
   83  make -j
   84  make install
   85  ./bin/curl --version
   86  ~/curl_install/bin/curl --version
   87  pushd ~/azure-iot-sdk-c/cmake
   88  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
   89  cd iothub_client/tests/iothubclient_mqtt_e2e
   90  ls
   91  cmake --build .
   92  cd ..
   93  rm -rf *
   94  vi ../iothub_client/tests/iothubclient_mqtt_e2e/CMakeLists.txt
   95  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
   96  vi ../iothub_client/tests/iothubclient_mqtt_e2e/CMakeLists.txt
   97  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
   98  cd iothub_client/tests/
   99  cd iothubclient_mqtt_e2e/
  100  ll
  101  ls -l
  102  make -j
  103  ls -l
  104  tops
  105  top
  106  sudo shutdown -r -t 0
  107  ls
  108  sudo raspi-config
  109  ls
  110  cd azure-iot-sdk-c/
  111  curl
  112  curl --version
  113  cd /home/pi/curl_source
  114  ls
  115  cd curl-7.64.1/
  116  ls
  117  vim Makefile
  118  whereis zlib
  119  cd ../..
  120  ls
  121  cd curl_isntall
  122  cd curl_install
  123  find . -iname zlib
  124  clear
  125  cd ..
  126  cd azure-iot-sdk-c/
  127  ls
  128  cd cmake
  129  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
  130  cd iothub_client/samples/iothub_ll_telemetry_sample/
  131  cmake --build .
  132  ~/curl_install/bin/curl
  133  ~/curl_install/include
  134  ls ~/curl_install/include
  135  alias ll='ls -al'
  136  ls
  137  ls ~/curl_install/include/curl
  138  ls
  139  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include/curl  ..
  140  cd ../../../
  141  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include/curl  ..
  142  cd iothub_client/samples/iothub_ll_telemetry_sample/
  143  cmake --build .
  144  cd ..
  145  ls
  146  cd ..
  147  git status
  148  git checkout -- iothub_client/tests/iothubclient_mqtt_e2e/CMakeLists.txt
  149  git status
  150  cd cmake
  151  rm -rf *
  152  cmake -Drun_unittests=ON -Drun_e2e_tests=ON -DCMAKE_BUILD_TYPE=Debug -DCURL_LIBRARY=$CURL_ROOT/lib/libcurl.a -DCURL_INCLUDE_DIR=$CURL_ROOT/include  ..
  153  cd iothub_client/samples/iothub_ll_telemetry_sample/
  154  cmake --build . -j
  155  cmake --build .
  156  cd ~
  157  ls
  158  cd curl_install
  159  ls
  160  cd ..
  161  cd curl_source
  162  ls
  163  cd curl
  164  cd curl-7.64.1/
  165  ls
  166  history
