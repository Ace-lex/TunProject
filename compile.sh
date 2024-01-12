#!/bin/bash
cd libTun && make && cp libTun.so ../Test/libTun.so
cd ../Test && make
cd ..