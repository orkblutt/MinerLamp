# MinerLamp

[![standard-readme compliant](https://img.shields.io/badge/readme%20style-standard-brightgreen.svg)](https://github.com/RichardLitt/standard-readme)
[![Releases](https://img.shields.io/github/downloads/atom/atom/total.svg)][Releases]

> MinerLamp is a graphical interface to use with ethminer with advanced watchdog options and monitoring.

MinerLamp, written in Qt (C++), is a GUI with options to run ethminer under control. It can be built on Linux, Mac and Windows.

### Features
- simple GUI that can run on Windows, Linux and Mac (Qt).
- automated ethminer parameters.
- watchdog for restarting ethminer in case of errors.
- enhanced displaying of ethminer output - highlighted successful solutions, shares & zero hashrate
- display useful information for nVidia cards - core temperatures, fan speed, memory and gpu clock, power drawing - a minimum and a maximum from all cards - easy for spotting OC issues.
- funny option like blinking cards LEDs on share accepted.
- integrated overclocking of nVidia cards - available soon!
- integrated pools monitoring - available soon!

## Table of Contents
- [Download]("#download")
- [Install](#install)
- [Usage](#usage)
- [Build](#build)
- [Contribute](#contribute)
- [TODO](#todo)

## Download
You can find the ready to use package in [[!Releases]][Releases]

## Install

MinerLamp uses Qt as main developpement framework. The binary (MinerLamp.exe) depends on several Qt libraries. You can find them in the archive availabe on the release page [https://github.com/orkblutt/MinerLamp/releases].
nVidia option needs the nvml.dll (or .so). On Windows you can copy and paste it in your MinerLamp directory or add path in the PATH environment variable. Traditionaly, the path is: "C:\Program Files\NVIDIA Corporation\NVSMI"


## Usage
1 - Start MinerLamp.

2 - Fill the ethminer binary path in the first edit box, e.g.:
> C:\Users\USERNAME\Desktop\ethminer\ethminer.exe

3 - Fill the ethminer's startup parameters, as you would normally use on your ethminer startup script, but excluding "ethminer.exe" before, e.g.:
> --farm-recheck 2000 -U -S POOL_ADDRESS:PORT -O WALLET_ID.MINER_NAME/E-MAIL_FOR_MONITORING

4 - Check the options you want to activate.

5 - Click on "Start mining".

All the parameters you set are automaticaly saved and MinerLamp will use them on next run.

Please note, "Auto mining" isn't auto startup of the program!
We suggest to autorun the minerlamp.exe on startup > head to Users/USERNAME/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup and paste a linked file for autostarting.

## Build

You need Qt installed (5.9 was used but all 5.x versions are compatible). 
Open the .pro file with Qt Creator. Clic on "configure" with the good configuration. Then you can build (execute qmake and compile).
If you don't want to build for nVIDIA GPU, delete the NVIDIA flag in the DEFINE option in the .pro file.

## Contribute

Feel free to make pull request.

## TODO

- correct versioning
- CMake
- Support for AMD
- build Linux and Mac releases
- monitoring for various pools


[Releases]: https://github.com/orkblutt/MinerLamp/releases

