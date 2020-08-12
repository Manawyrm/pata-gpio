pata-gpio
=============================

![Adapter PCB](https://screenshot.tbspace.de/nceyjhftwua.jpg)
![dmesg Output](https://screenshot.tbspace.de/iralxfbzwke.png)

### Overview

**DISCLAIMER: This driver was developed for fun and learning ATA internals. If you're looking for a practical way to use PATA devices, look for commercial USB adapters.**  

pata-gpio is a Linux kernel driver for GPIO bitbanged PATA (also known as ATA/ATAPI or IDE).  
Pin configuration can be set via device tree, the regular ATA framework is used.  
  
IDE Primary/Secondary device support is implemented.  
  
### Performance

On a Raspberry Pi 4 (with direct GPIO) about **800 KiB/s reading, 500 KiB/s writing speed** is possible.
Most of the performance bottleneck is down to the usage of libgpiod (kernel gpio abstraction), 
which is optimized for tasks like switching LEDs. Data transmission is well outside the design scope of libgpiod. 
  
DMA and interrupts are not implemented at the moment.  
  
### Device tree configuration

Pin configuration can be specified in the device tree.  
An example device tree is provided as [atapi-direct.dts](https://github.com/Manawyrm/pata-gpio/blob/master/atapi-direct.dts).  
This includes the pin configuration for the schematic linked at the bottom of this readme.  
  
The following device tree properties are present:  

| Property      | Usage         | Required  |
| ------------- |-------------| -----|
| `databus-gpios`      | list of 16 GPIO pins, connected to ATA D0 - D15 in this order | yes |
| `cs-gpios`      | list of 2 GPIO pins, connected to ATA CS0 and CS1 in this order      | yes |
| `address-gpios` | list of 3 GPIO pins, connected to ATA DA0, DA1 and DA2 in this order  | yes |
| `strobe-write-gpio`      | GPIO pin, connected to ATA IOW line | yes |
| `strobe-read-gpio`      | GPIO pin, connected to ATA IOR line | yes |
| `reset-gpio` | GPIO pin, connected to ATA RESET line | no |

#### Interrupts

The driver has experimental interrupt support. An example configuration with GPIO interrupts is outlined in
`atapi-direct-irq.dts`.  
Interrupts are configured via the generic `interrupts` property. If the `interrupts` property is not specified
the driver falls back to polling.

### Compiling

Before loading (and running) the kernel module, make sure to load the device tree overlay.  
This can be done via the config.txt (on a Raspberry Pi): `dtoverlay=atapi-direct`, or by running `./loaddt.sh`.  
The latter script will also compile and save the device tree to `/boot/overlays/atapi-direct.dtbo`.  
  
This module needs kernel headers, regular build dependencies and a kernel with the config option `CONFIG_ATA_SFF=y` set.  
The `ATA_SFF` option is not the default on Raspberry Pis, so you will need to build a custom kernel.  

Compile the module using:   
`make`  

Load the module using:  
`insmod pata-gpio.ko`  
 
A helper script called `./run.sh` is provided, which will remove any loaded pata-gpio module, compile it and load the resulting module.  
Further status information can be shown by running `dmesg`. 

### Additional info

[Adapter PCB](https://github.com/Manawyrm/ATAPIHat-SMI)  
[Adapter schematic](https://github.com/Manawyrm/ATAPIHat-SMI/blob/master/gerbers/RevA/ATAPIHat-SMI/ATAPIHat-SMI.pdf)  

[Older schematic (not SMI-ready)](https://screenshot.tbspace.de/cmzhjfglbya.png)  
[Video: Audio CD streaming](https://www.youtube.com/watch?v=cHQhuzSn2oE)  
[Twitter: Project presentation](https://twitter.com/Manawyrm/status/1292084926980984833)  

### Credits
Thanks to [@Toble_Miner](https://github.com/TobleMiner) for a lot of help in the development of this driver!
