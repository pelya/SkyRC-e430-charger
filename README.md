Open-source firmware for SkyRC e430 LiFe/LiPo 4S battery charger.
=================================================================

This project supports only SkyRC e430. Other chargers are not supported.

The problem: SkyRC e430 ruins batteries, because it discharges cell #3 in 4S battery pack until it's unusable.

The solution: Write your own firmware. Easy!

Only 4S battery configuration is supported. Do not plug 1S/2S/3S battery packs, or they will burn!

SkyRC e430 has only one charging port - the red/black banana socket.
Ports 2S/3S/4S have no power, they are only used to discharge individual cells to balance them.
Once any cells are charged above 3.0 volts LiFe / 3.5 volts LiPo, the charger will balance the battery
by discharging high-voltage cells until they are lower than the lowest cell voltage plus 0.1 volts.
If any cell reaches 3.65 volts LiFe / 4.2 volts LiPo, the charger will stop charging and will only
discharge cells to balance them.
The charging and balancing/discharging individual cells occurs at the same time.
There are no error modes - the charger will try to recover batteries discharged even to zero volts.

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

SkyRC e430 uses STM8S903K3 8-bit CPU, and no other digital components.

Pin numbers and functions are described in [gpio-test.txt](gpio-test.txt).

The voltage of each charging cell is shown using four 'Cells Equalizer' LEDs.
The charger shows cell number first.
Then the charger shows voltage of this cell using 4 LEDs.
Each decimal digit of the voltage is shown using the sum of 4 LED labels
1S / 2S / 3S / 4S, with zero = all 4 LEDs off.
Between digits all 4 LEDs are on.

For example cell #2 is charged to 3.96 volts, the LEDs will light up in this sequence:

|  1S |  2S |  3S |  4S |   Meaning  |
|-----|-----|-----|-----|------------|
|  -  |  +  |  -  |  -  |   Cell #2  |
|  +  |  +  |  +  |  +  | Next digit |
|  -  |  -  |  +  |  -  |      3     |
|  +  |  +  |  +  |  +  | Next digit |
|  -  |  +  |  +  |  +  |   9=2+3+4  |
|  +  |  +  |  +  |  +  | Next digit |
|  -  |  +  |  -  |  +  |    6=2+4   |

