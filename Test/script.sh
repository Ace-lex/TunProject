#!/bin/bash
ifconfig tun0 0.0.0.0 up
route add 10.10.10.1 dev tun0