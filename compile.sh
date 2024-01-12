#!/bin/bash
make
cd libTun && make
cd ../Test && make
cd ../TunRoce && make
cd ..