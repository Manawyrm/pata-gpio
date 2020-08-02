#!/bin/bash

dtoverlay -R atapihat
dtc -I dts -O dtb /boot/overlays/atapihat.dts > /boot/overlays/atapihat.dtbo
dtoverlay /boot/overlays/atapihat.dtbo
