# Bsides Badge 2019
BSidesPerth Badge 2019


## Setup Environment
1. Install Arduino IDE: https://www.arduino.cc/en/guide/windows
1. Add ESP32 to Arduino IDE:
   1. For beginners, follow this tutorial up to the point of installing the ESP32 boards:
   1. https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/
   1. For experienced people, here is link for Preference menu: https://dl.espressif.com/dl/package_esp32_index.json
1. Install SPIFFS plugin for ESP32 to Arduino IDE:
   1. https://randomnerdtutorials.com/install-esp32-filesystem-uploader-arduino-ide/
   1. Direct link: https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/
   1. Goes in: `<home_dir>/Arduino-<version>/tools/ESP32FS/tool/esp32fs.jar`
   1. Or maybe: `<documents>/Arduino/tools/ESP32FS/tool/esp32fs.jar`
   1. Restart IDE and will add menu item: `ESP32 Sketch Data Upload`
1. In Arduino IDE preferences, tick `upload` next to `show verbose output during`.
1. Download the code
   1. Click `clone or download` green button on this github page
   1. Click `download zip`
   1. Extract zip to your arduino sketch folder (often in `documents\Arduino`)
   1. Should look like: `arduino\badge-2019\badge-2019.ino` (folder name is important)
 1. Install libraries by copying them from `badge-2019\libraries\` into the `libraries\` folder in your arduino sketch directory
 1. Restart Arduino IDE and then open `badge-2019` code in Arduino IDE (double click on `badge-2019.ino` or use file->open)
 1. Select board `ESP32 Dev Module`
 1. Connect badge to computer with micro USB cable
 1. Select COM port for the badge (there should only be one - ignore any bluetooth COM ports)
    1. `Tools` -> `Port`
 1. Select "Upload" in Arduino IDE to compile and send code to badge
 
