#!/bin/bash
sudo chmod +x ./script.sh
cd libTun && make && cp libTun.so ../Test/libTun.so
cd ../Test && make
cd ..