# SanderOS64
A 64bit OS

## Target
* Boot via UEFI :white_check_mark:
* Boot via GRUB :white_check_mark:
* Load kernel from AHCI :white_check_mark:
* Read filesystem from AHCI :white_check_mark:
* Write filesystem from AHCI :white_check_mark:
* Load drivers :white_check_mark:
* Edit text files
* Use FASM to assemble programs :white_check_mark:
* Send messages to IRC client

## Installation
To build the system, you need to have the following for the first time:
* Download the GNU-EFI folder and extract it in a location
* CD into this GNU-EFI folder
* Put OVMF.fd in this folder
* clone this project in this folder
* Define the variable ```${SANDEROS_MOUNT_POINT}``` with the place where to install everything
* Define the variable ```${SANDEROS_CROSS_LOCATION}``` with the place of the cross compiler

After this , in the root folder of the project:
* Use ```./build``` to build the base of the project
* Use ```./build --install``` to install the project at the system
* Use ```./build --emulate ``` to create a diskimage and use qemu to emulate the project

## Suported Devices
Currently the following devices are supported:
* :question: unknown
* :x: not supported
* :white_check_mark: works
* :bug: partly working

Devicename | VirtualBox | Qemu | Bochs | Real hardware
----------|---------- | ---- | ----- | -------------
ahci - ATA | :white_check_mark: | :white_check_mark: | :question: | :white_check_mark: 
comport | :white_check_mark: | :white_check_mark: | :question: | :question:
e1000 | :bug: | :question: | :question: | :question: 
ehci (usb2) | :x: | :x: | :question: | :white_check_mark:
ps2 keyboard | :white_check_mark: | :white_check_mark: | :question: | :white_check_mark:
rtl | :question: | :bug: | :question: | :white_check_mark:
usb MSD | :question: | :question: | :question: | :white_check_mark:
xhci (usb3) | :x: | :x: | :question: | :bug: 
fs: fat32 | :white_check_mark: | :white_check_mark: | :question: | :white_check_mark:
