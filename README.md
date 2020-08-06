<p align="center">
<img src="https://github.com/SignalHound/gr-vsg60/blob/master/docs/SH-GR.jpg" width="75%" />
</p>

## A [GNU Radio](https://www.gnuradio.org) module for the [Signal Hound VSG60 6 GHz Vector Signal Generator](https://signalhound.com/products/vsg60a-6-ghz-vector-signal-generator/)

### Requirements

- 64-bit Linux operating system
    - Tested on Ubuntu 18.04
- Native USB 3.0 support

### Prerequisites

1. [Install GNU Radio](https://wiki.gnuradio.org/index.php/InstallingGR).
    - GNURadio 3.8, currently a release candidate, updates to Python 3, which is not currently supported by this module. Look [here](https://wiki.gnuradio.org/index.php/GNU_Radio_3.8_OOT_Module_Porting_Guide) if you are interested in porting the code.
2. [Install the Signal Hound SDK](https://signalhound.com/software/signal-hound-software-development-kit-sdk/).
    - Follow directions in _device_apis/vsg60_series/linux/README.txt_.

### Installation

1. Clone this repository.
2. Run the following commands from the root directory of the repository:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ sudo make install
$ sudo ldconfig
```

### Usage

- Add the __VSG60: IQ Sink__ block to flowgraphs in the GNU Radio Companion. It is located under the __Signal Hound VSG60__ category.
    - See _examples_ folder for demos.
- Use the block in Python with `import vsg60`.
