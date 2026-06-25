Open-source firmware for SkyRC e430 LiFe/LiPo 4S battery charger.
=================================================================

This project supports only SkyRC e430. Other chargers are not supported.

Only 4S battery configuration is supported. Do not plug 1S/2S/3S battery packs!

The problem: SkyRC e430 ruins batteries, because it discharges cell #3 in 4S battery pack until it's unusable.

The solution: find pin numbers on the circuit board and write your own firmware.

SkyRC e430 uses STM8S903K3 8-bit CPU, and no other digital components.

The circuit board already has contacts for flashing firmware, so you only need to unscrew 4 screws
to open the casing, and connect 4 wires 5V, SWIM, GND, and NRST from your ST-LINK/V2 programmer
to the 4 pinholes on the circuit board, and hold them with your finger while the firmware is flashing -
no need to solder it.

![Firmware flashing contacts](stlink-port.jpg)

Hardware required: PZ1 screwdriver and ST-LINK/V2 programmer. It can program both STM32 and STM8 chips, I bet you did not know!
STM32 uses pins SWCLK and SWDIO, while STM8 uses pins SWIM and NRST.

Software required: SDCC compiler, optionally STM8CubeMX for editing .ioc8 project file, standard utilities like make and gcc.

Build instructions for Debian:
```
sudo apt install sdcc
make
```

Build instructions for MacOs and other Linux distributions:
```
Install SDCC using your favorite package manager.
Follow build instructions for Debian.
```

Build instructions for Windows:
```
₊˚ ✧ ‿︵‿୨ 𝔾 𝕆 𝕆 𝔻  𝕃 𝕌 ℂ 𝕂 ୧‿︵‿ ✧ ₊˚
```

Unlock STM8 flash before wirting firmware:
```
stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -u
```

Write firmware:
```
stm8flash/stm8flash -c stlinkv2 -p 'stm8s903?3' -w out/main.ihx

```

The firmware charges each cell individually, switching between 4 cells each 20 seconds.
The charging speed is slower than the original firmware.
The currently charging cell ID is shown using one LED.
The charger then shows voltage of this cell each 5 seconds using 4 LEDs.
Each decimal digit of the voltage is shown using the sum of 4 LED labels
1S / 2S / 3S / 4S, with zero = all 4 LEDs on.

For example cell #2 is charged to 3.096 volts, the LEDs will light up in this sequence:

|  1S |  2S |  3S |  4S | Meaning |
|-----|-----|-----|-----|---------|
|  -  |  +  |  -  |  -  | Cell #2 |
|  -  |  -  |  +  |  -  |    3    |
|  +  |  +  |  +  |  +  |    0    |
|  -  |  +  |  +  |  +  | 9=2+3+4 |
|  -  |  +  |  -  |  +  |  6=2+4  |

