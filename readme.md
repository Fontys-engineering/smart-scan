# SmartScan Software Repository
This repository contains the source code for the Semester 7 Fontys project "SmartScan".

**This project is no longer in active development.**

**A release CLI application and library version is available to download.**
**A Rhino plug-in implementation using the smart scan service can be found at https://github.com/eduardcazacu/smart-scan-rhino**

## Folder Structure:
1. Root
    1. SmartScanCLI - Command Line User Interface implementation of the library
    2. SmartScanService - The SmartScan library 

## Software functionalities
* To start working with the software files it is required to use Visual Studio 2019 or Visual Studio Code 2019.
* Before getting started it is needed to setup a few settings in the environment, see wiki page 'Development'.
* Once the application is running, the user can type commands, use 'help' to see a list of the commands.
* It is possible to use multiple scans. All scans need to be define the scan configuration one-by-one. 
* All scan configuations are: amount of reference points, filtering precision, scan duration in samples and outlier treshold.
* With the command 'list' it will show all scans on the CLI with differenet configurations.
* Command 'delete' will delete all scans or delete one-by-one when specifying the scan id.
* Command 'start' will start all scans or start one-by-one when specifying the scan id.

# System architecture

## Measurement pipeline
![Image of pipeline](readme_img/pipeline.png?raw=true)

## UML
![Image of architecture](readme_img/uml.png?raw=true)
