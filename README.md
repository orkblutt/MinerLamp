"# MinerLamp" 

> MinerLamp is a graphical interface to use with etherminer with advanced watchdog options

MinerLamp, written in Qt (C++), is a GUI with options to run ethminer under control. It can be built on Linux, Mac and Windows.

### Features
- nice GUI that can run on Windows, Linux and Mac (Qt)
- automate etherminer parameters
- watchdog to restart etherminer in case of errors
- enhanced display of ethminer output
- display usefull informations for nVidia cards

## Table of Contents

- [Install](#install)
- [Usage](#usage)
- [Build](#build)
- [Contribute](#contribute)
- [TODO](#todo)


## Install

MinerLamp uses Qt as main developpement framework. The binary (MinerLamp.exe) depends on several Qt libraries. You can find them in the archive availabe on the release page [![Releases](https://img.shields.io/github/downloads/atom/atom/total.svg)][Releases].
nVidia option need the nvml.dll (or .so). On Windows you can copy it and paste it in your MinerLamp directory or add path in the PATH environement variable. Traditionaly this path is: "C:\Program Files\NVIDIA Corporation\NVSMI"


## Usage
1 - Start MinerLamp.
2 - Fill the etherminer binary path in the first edit box.
3 - Fill the etherminer's parameters.
4 - Check the options you want to activate.
5 - Click on "Start miner".

All the parameters you set are automaticaly saved and MinerLamp will use them next run.

## Build

You need Qt installed (5.9 was used but all 5.x versions are ok). 
Open the .pro file with Qt Creator. Clic on "configure" with the good configuration. Then you can build.

## Contribute

Feel free to make pull request.

## TODO

- correct versioning
- CMake
- Support for AMD



