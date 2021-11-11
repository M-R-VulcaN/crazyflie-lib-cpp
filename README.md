# crazyflie-lib-cpp
Like crazyflie-lib-python but written fully in cpp

## How to install 

1. `git clone --recursive https://github.com/M-R-VulcaN/crazyflie-lib-cpp.git`
2. `cd crazyflie-lib-cpp/crazyflie-link-cpp`
3. `git submodule init -- libusb/`
4. `git submodule update -- libusb/`
5. `sudo apt install -y libusb-1.0-0-dev`
6. `cd ..`

## How to compile 
1. `mkdir build && cd build`
2. `cmake ..`
3. `cmake --build .` or just `make`

Run the file you wish with `./<executable_file_name>`
