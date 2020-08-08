#!/bin/bash

dtoverlay -R atapi-direct
dtc -I dts -O dtb atapi-direct.dts > /boot/overlays/atapi-direct.dtbo
dtoverlay /boot/overlays/atapi-direct.dtbo
