# SensoGrip

Repository consists of software and brief descpription of each individual step to successfuly set up Sensogrip pencil.

**Table of Contents**

- [SensoGrip](#sensogrip)
  * [About The Project](#about-the-project)
    + [Built With](#built-with)
  * [Getting Started](#getting-started)
    + [Prerequisites](#prerequisites)
  * [Installation](#installation)
    + [Uploading bootloader](#uploading-bootloader)
    + [Uploading software](#uploading-software)
  * [Usage](#usage)
  * [License](#license)
  * [Contact](#contact)
  * [Acknowledgements](#acknowledgements)

<!-- ABOUT THE PROJECT -->
## About The Project

Sensogrip pencil was developed as a therapeutical help tool for children with graphomotoric difficulties. It consists of two sensors: one for measuring tip pressure, and the second one for measuring finger pressure. User is able to get feedback via built-in RGB led or via mobile app.

It features:
* Piezoelectric sensor for measuring tip pressure
* FSR sensor for measuring finger pressure
* Optical RGB led feedback (for example: green color lights up when the both sensors are in range)
* Automatic measured pressure correction with angle from IMU
* Sleep funtion with wake-up by shaking
* Bluetooth BLE connectivity
* Rechargable battery, which provides 10 hour of operating time
* Mobile App companion

### Built With

* [Arduino](https://www.arduino.cc)
* [Visual Studio Code](https://code.visualstudio.com)
* [Segger](https://www.segger.com/)



<!-- GETTING STARTED -->
## Getting Started

Follow this steps to upload firmware and software to your Sensogrip pencil.

### Prerequisites

* Arduino IDE (with Mbed OS boards support installed)
* Sensogrip custom board definition and bootloader
* [J-Link debugger](https://www.segger.com/products/debug-probes/j-link/)
* [JFlashSPI Software](https://www.segger.com/downloads/jlink)


## Installation

### Uploading bootloader

1. Install [JFlashSPI Software](https://www.segger.com/downloads/jlink)
2. [Connect Segger debugger to Sensogrip board via SWD pins](http://djynet.net/?p=969)
3. Start `JFlashLite.exe` and upload bootloader - or to skip uploading the software step, upload .hex file of program

![jflashlite](jflashlite.png)

### Uploading software

1. Install [Arduino IDE](https://www.arduino.cc/en/guide/windows)
2. Install Arduino Mbed OS boards support
3. Copy `SENSOGRIP_NINAB306` custom board definition to directory
> _C:\Users\...\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.0\variants_
4. Add the folowing lines to boards.txt file located in directory
> _C:\Users\...\AppData\Local\Arduino15\packages\arduino\hardware\mbed\1.3.0_

   ```sh
    ninaDevBoard.name=Dev NINA B306
    ninaDevBoard.build.core=arduino
    ninaDevBoard.build.crossprefix=arm-none-eabi-
    ninaDevBoard.build.compiler_path={runtime.tools.arm-none-eabi-gcc.path}/bin/
    ninaDevBoard.build.variant=DEV_NINAB306
    ninaDevBoard.build.mcu=cortex-m4
    ninaDevBoard.build.extra_flags=
    ninaDevBoard.build.architecture=cortex-m4
    ninaDevBoard.build.fpu=fpv4-sp-d16
    ninaDevBoard.build.float-abi=softfp
    ninaDevBoard.build.board=DEV_NINAB306
    ninaDevBoard.build.ldscript=linker_script.ld
    ninaDevBoard.compiler.mbed.arch.define=-DARDUINO_ARCH_NRF52840
    ninaDevBoard.compiler.mbed.defines={build.variant.path}/defines.txt
    ninaDevBoard.compiler.mbed.ldflags={build.variant.path}/ldflags.txt
    ninaDevBoard.compiler.mbed.cflags={build.variant.path}/cflags.txt
    ninaDevBoard.compiler.mbed.cxxflags={build.variant.path}/cxxflags.txt
    ninaDevBoard.compiler.mbed.includes={build.variant.path}/includes.txt
    ninaDevBoard.compiler.mbed.extra_ldflags=-lstdc++ -lsupc++ -lm -lc -lgcc -lnosys
    ninaDevBoard.compiler.mbed="{build.variant.path}/libs/libmbed.a" "{build.variant.path}/libs/libcc_310_core.a" "{build.variant.path}/libs/libcc_310_ext.a" "{build.variant.path}/libs/libcc_310_trng.a"
    ninaDevBoard.vid.0=0x9999
    ninaDevBoard.pid.0=0x9999
    ninaDevBoard.build.usb_product="Nina B306 dev"
    ninaDevBoard.vid.1=0x2341
    ninaDevBoard.pid.1=0x805a
    ninaDevBoard.upload.tool=bossac
    ninaDevBoard.upload.protocol=
    ninaDevBoard.upload.use_1200bps_touch=true
    ninaDevBoard.upload.wait_for_upload_port=true
    ninaDevBoard.upload.native_usb=true
    ninaDevBoard.upload.maximum_size=983040
    ninaDevBoard.upload.maximum_data_size=262144
    ninaDevBoard.bootloader.tool=openocd
    ninaDevBoard.bootloader.extra_action.preflash=echo INFO:removed_mass-erase
    ninaDevBoard.bootloader.config=-f target/nrf52.cfg
    ninaDevBoard.bootloader.programmer=-f interface/cmsis-dap.cfg
    ninaDevBoard.bootloader.file=nano33ble/bootloader.hex
   ```
   
5. Open Arduino IDE or VSCode IDE, select `SensoGrip NINA B306` board and upload the program `sensoGripBLE.ino`



<!-- USAGE EXAMPLES -->
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](/documentation)_



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Primoz Flander: primoz.flander@fh-campuswien.ac.at

Project Link: [SensoGrip](https://github.com/primozflander/senso-grip)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [Gernot Korak](https://www.fh-campuswien.ac.at/forschung/forschende-von-a-z/personendetails/gernot-korak.html)
