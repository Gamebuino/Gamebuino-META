# Bootloader stuff
The bootloader itself starts at `0x0000` and goes all the way to `0x3FFF`, so the sketeches start at `0x4000`.  


## Vector table
All at the end of the bootloader are some useful things for you (all are four bytes long):  
Please note that functions are actually pointers to functions, so at that location is the address which you'll need to call.

|Offset|Address| Purpose                      |Arguments               |Description
|------|-------|------------------------------|------------------------|---
|4     |0x3FFC | Version number of bootloader | _not a function_       |Version number of bootloader, <16-bit major><8-bit minor><8-bit patch>
|8     |0x3FF8 | function `sd_load_game`      | `const char* filename` |Loads a .bin file from the SD card
|12    |0x3FF4 | function `sd_load_loader`    | _none_                 |Loads loader.bin from the SD card
|16    |0x3FF0 | function `board_init_full`   | _none_                 |Initializes the board, screen and SPI and sets input/output for the SPI peripherals screen, keys and sd-card
|20    |0x3FEC | function `enter_bootloader`  | _none_                     |Enters bootloader mode

## Boot process
* if reset was WDT try to flash loader
* if BOOT_DOUBLE_TAP_DATA is BOOTLOADER_MAGIC go to the bootloader
* check if start of program is 0xFFFFFFFF, if so, we had a SYST reset AND our BOOT_MAGIC2_DATA != NORMAL_RESET_MAGIC load bootloader, else load loader
* set BOOT_MAGIC2_DATA to 0
* check if a program is flashed, else flash the loader
* wait for double-tap only if we didn't software-reset (so actually hit the reset button)
* if we double-tap reset flash the loader
* reset magic ram variable (used for flashing loader and double-tap reset)
* enable the Watchdog timer
* Jump to the program

### Flashing the loader
* check if the loader hasn't already been flashed (magic ram variable)
* if loader has been flashed, fallback to reset_bootloader
* set magic RAM variable
* actually load the game via sd_load_game
* the magic RAM variable is preserved so that we actually know if the bootloader is flashed or not

### sd_load_game
* try mounting SD card, else fall back to reset_bootloader
* try opening the file, else fall back to loader
* delete the flash
* read from SD card and flash, if reading fails fall back to reset_bootloader
* if writing flash failed fall back to reset_bootloader
* reset the device

### reset
* set BOOT_MAGIC2_DATA to NORMAL_RESET_MAGIC
* issue a SYST reset

### reset_bootloader
* set BOOT_DOUBLE_TAP_DATA to BOOTLOADER_MAGIC
* reset the device

## Board Definitions

| Name                            | Purpose                               |
|---------------------------------|---------------------------------------|
| USB definitions                 |                                       |
|                                 |                                       |
|STRING_PRODUCT                   | Name of the product                   |
|USB_VID_HIGH                     | Higher byte of the vendor ID          |
|USB_VID_LOW                      | Lower byte of the vendor ID           |
|USB_PID_HIGH                     | Higher byte of the product ID         |
|USB_PID_LOW                      | Lower byte of the product ID          |
|                                 |                                       |
| Reserved ram                    |                                       |
|                                 |                                       |
| BOOT_DOUBLE_TAP_ADDRESS         | Address of four byte of reserved RAM  |
| BOOT_MAGIC2_ADDRESS             | Second address for 4 bytes in RAM     |
|                                 |                                       |
| USART config                    |                                       |
|                                 |                                       |
| BOOT_USART_MODULE               | SERCOM device for USART               |
| BOOT_USART_BUS_CLOCK_INDEX      |                                       |
| BOOT_USART_PER_BLOCK_INDEX      |                                       |
| BOOT_USART_PAD_SETTINGS         |                                       |
| BOOT_USART_PAD3                 |                                       |
| BOOT_USART_PAD2                 |                                       |
| BOOT_USART_PAD1                 |                                       |
| BOOT_USART_PAD0                 |                                       |
|                                 |                                       |
| Clock frequencies               |                                       |
|                                 |                                       |
| CPU_FREQUENCY                   | Frequency of the CPU clock            |
| VARIANT_MCK                     |                                       |
| VARIANT_MAINOSC                 |                                       |
| NVM_SW_CALIB_DFLL48M_COARSE_VAL |                                       |
| NVM_SW_CALIB_DFLL48M_FINE_VAL   |                                       |
|                                 |                                       |
| LED definitions                 |                                       |
|                                 |                                       |
| BOARD_LED_PORT                  | Port of the L LED                     |
| BOARD_LED_PIN                   | Pin of the L LED                      |
| BOARD_LEDRX_PORT                | Port of the RX LED If undefined then disable LED |
| BOARD_LEDRX_PIN                 | Pin of the RX LED                     |
| BOARD_LEDTX_PORT                | Port of the TX LED If undefined then disable LED |
| BOARD_LEDTX_PIN                 | Pin of the TX LED                     |
|                                 |                                       |
| SPI Definitions                 |                                       |
|                                 |                                       |
| SPI_BITBANG                     | (1/0) use bitbanging for SPI?         |
| SERCOM_FREQ_REF                 | Reference freq for BAUD (usually CPU) |
| BOARD_MOSI_PORT                 | MOSI port                             |
| BOARD_MOSI_PIN                  | MOSI pin                              |
| BOARD_MISO_PORT                 | MISO port                             |
| BOARD_MISO_PIN                  | MISO pin                              |
| BOARD_SCK_PORT                  | SCK port                              |
| BOARD_SCK_PIN                   | SCK pin                               |
| BOARD_SD_MMC_DESELECT           | (1/0) auto-deselect SD card           |
| BOOT_SPI_MODULE                 | SECOM SPI module                      |
| BOARD_GCLK_ID                   | GCLK ID for SPI SERCOM                |
| BOARD_PM_APB_ID                 | PM mask for SPI SERCOM                |
| BOARD_SERCOM_DIPO_PAD           | DIPO Pad                              |
| BOARD_SERCOM_DIPO_PMUX          | DIPO pmux                             |
| BOARD_SERCOM_DOPO_PAD           | DOPO pad (see subsection)             |
| BOARD_SERCOM_DOPO_PMUX          | DOPO pmux                             |
| BOARD_SERCOM_SCK_PMUX           | SCK pmux                              |
| BOARD_SD_MMC_CLOCK_INIT         | SD init frequency                     |
| BOARD_SD_MMC_CLOCK_FAST_INIT    | fast SD init frequency                |
|                                 |                                       |
| SD Definitions                  |                                       |
| BOARD_SD_LOADER                 | (string) name of the loader           |
| BOARD_SD_CS_PORT                | CS port (also known as SS)            |
| BOARD_SD_CS_PIN                 | CS pin (also known as SS)             |
|                                 |                                       |
| Misc.                           |                                       |
|                                 |                                       |
| BOARD_WDT_ENABLE                | (1/0) Enable watchdog timer?          |

### DOPO pad

Note that SS/DS is irrelevant as this bootloader does it manually

| DOPO | DO     | SCK    | SS/DS  |
|------|--------|--------|--------|
| 0x0  | PAD[0] | PAD[1] | PAD[2] |
| 0x1  | PAD[2] | PAD[3] | PAD[1] |
| 0x2  | PAD[3] | PAD[1] | PAD[2] |
| 0x3  | PAD[0] | PAD[3] | PAD[1] |

# Cloning stuff

```
git clone git@github.com:Rodot/Gamebuino-Meta-Bootloader.git
```

Then in that repo

```
git remote add arduino git@github.com:arduino/ArduinoCore-samd.git
```

push from your local repo thingy here:

```
# The first time yu do it you need to:
git push -u origin master
# After that you can just do
git push
```

Update from arduinos official repo thing:

```
git fetch arduino master
git commit -am "updated from official branch"
git push origin master
```


## License and credits

Adapt this?

This core has been developed by Arduino LLC in collaboration with Atmel.

```
  Copyright (c) 2015 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
```
