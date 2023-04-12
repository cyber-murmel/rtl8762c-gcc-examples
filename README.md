# RTL8762C-gcc-examples
Firmware examples for the RTL8762C BLE SoC

[![hackaday.io](https://img.shields.io/badge/hackaday-io-gold.svg)](https://hackaday.io/project/182205-py-ft10)

## Download
To obtain this code, clone this repository recursively.
```shell
git clone --recurse-submodules https://github.com/cyber-murmel/rtl8762c-gcc-examples.git
```

### SDK
Register an account at [RealMCU](https://www.realmcu.com) and download the
*RTL8762C SDK GCC(ZIP)* from the [product page](https://www.realmcu.com/en/Home/Product/93cc0582-3a3f-4ea8-82ea-76c6504e478a).
This procedure requires an email account.

Extract the zip archive and move the `sdk` directory into the root of this repository.

```shell
unzip RTL8762C-sdk-gcc-v0.0.5.zip
mv bee2-sdk-gcc-v0.0.5/sdk ./sdk
rm -r bee2-sdk-gcc-v0.0.5
```

## Toolchain
To build the firmware you need
- GNU Make 4.3
- arm-none-eabi-gcc (GNU Arm Embedded Toolchain 9-2020-q2-update) 9.3.1 20200408 (release)

### rtltool
For flashing you need to download and install the dependencies for the rtltool submodule.
Please follow the [installtion section](https://github.com/cyber-murmel/rtltool/blob/main/README.md#installation).

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
