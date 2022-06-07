# RTL8762C-gcc-examples
Firmware examples for the RTL8762C BLE SoC

[![hackaday.io](https://img.shields.io/badge/hackaday-io-gold.svg)](https://hackaday.io/project/182205-py-ft10)

## Download
To download all necessary sources and tools, clone this repository recursively.
```shell
git clone --recurse-submodules https://github.com/cyber-murmel/rtl8762c-gcc-examples.git
```

## Toolchain
To build the firmware you need
- GNU Make 4.3
- arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)

### rtltool
For flashing you need to install the dependencies for  [rtltool](https://github.com/cyber-murmel/rtltool/blob/main/README.md#python-modules).

### Nix
Users of Nix or NixOS can simply run `nix-shell` to enter an environment with all necessary dependencies.

## Building
To build an example, set `TARGET` to one of the directories in [src/targets](src/targets) and run make.

```shell
make TARGET=00-blink
```

The default `TARGET` is `00-blink`

Currently available targets are
- `00-blink`
- `01-uart_echo`
- `02-uart_printf`

### Cleanup
It's advised to clean up build results before switching to a different target.

```shell
make clean
```

## Flashing
To flash the firmware onto the board via UART, run

```shell
make TARGET=00-blink flash
```

### Port
The UART port can be be set via the `PORT` variable.

```shell
make TARGET=00-blink PORT=/dev/ttyUSB0 flash
```

The default `PORT` is `/dev/ttyUSB1`

## Terminal
To open a miniterm session, run

```shell
make term
```

The port can be changed in the same way as for [flashing](#port).
