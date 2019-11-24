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
1. Restart Arduino IDE

## Programming Board
1. Open `badge-2019` code in Arduino IDE (double click on `badge-2019.ino` or use file->open)
1. Select board `ESP32 Dev Module`
1. Connect badge to computer with micro USB cable
1. Select COM port for the badge (there should only be one - ignore any bluetooth COM ports)
   1. `Tools` -> `Port`
1. Select "Upload" in Arduino IDE to compile and send code to badge
   1. It will take awhile (minutes) to compile the first time
   1. Pay attention to when you see in orange `Connecting........________`
   1. Press and hold SW2 (up arrow) on badge, until the upload process move onto next line.
1. You should see yellow lights on "eyes" of badge when it finishes downloading
   1. If you don't get yellow eyes, try unplugging and replugging board (power cycle)
   1. This means code has been loaded onto board but but not the SPIFFS data
1. Download the SPIFFS data (JPEG images) by clicking `Tools` -> `ESP32 Sketch Data Upload`
   1. Pay attention to when you see in orange `Connecting........________`
   1. Press and hold SW2 (up arrow) on badge, until the upload process move onto next line.

## Testing Badge
1. A freshly programmed badge will show all red LEDs - in the self test
1. press each switch SW2 to SW7 to see the LEDs for the eyes and crest toggle from red to green (when pressed) to blue (when released).
1. SW8 toggles IR LED (can see IR led in dark room with phone camera) - not essential testing
1. After approx 5s idle, the self test will exit and badge will return to menu
   1. In menu the LEDs will do a colour rainbow.

## Bulk Programming Board
Suggested workflow for programming many boards, one after another.
1. Put code on one board as per above steps in "Programming Board"
1. Save the commandline to upload from Arduino IDE log window which should look like this:
   1. `C:\Users\user\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\2.6.1/esptool.exe --chip esp32 --port COM117 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0xe000 C:\Users\user\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.2/tools/partitions/boot_app0.bin 0x1000 C:\Users\user\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.2/tools/sdk/bin/bootloader_qio_80m.bin 0x10000 C:\Users\user\AppData\Local\Temp\arduino_build_12566/badge-2019.ino.bin 0x8000 C:\Users\user\AppData\Local\Temp\arduino_build_12566/badge-2019.ino.partitions.bin`
   1. Notice the arduino IDE has built to a temporary location. When Arduino IDE is closed this will likely be lost.
   1. You could take a copy and change the paths to work.
1. Open a windows command prompt and paste in the commandline (or make a `.bat` file with `%1` for COM number e.g. `--port COM%1`)
1. For each board
   1. Plug in board
   1. In Arduino IDE, choose port for this board (will vary each time) - Remember port number
   1. Go back to command window and edit com port before running upload
   1. Hold SW2 until connects
   1. Ensure yellow eyes appear - if not power cycle
   1. When upload complete, go back to Arduino IDE and do `ESP32 Sketch Data Upload`
   1. Board should now start with all red LEDs
   1. press each switch SW2 to SW7 and see that each one changes LED to blue. SW8 toggles IR LED.
   1. Done!



