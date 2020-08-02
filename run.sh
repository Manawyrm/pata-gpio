#!/bin/bash

set -e

rmmod pata-gpio || true
make
insmod pata-gpio.ko
