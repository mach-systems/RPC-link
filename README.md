## Project information
Example RPC server to be used with [Vanetza](https://github.com/riebl/vanetza) vehicular-networking stack. Skeleton is to be used as Autotalks device link layer wrapper.

This repository contains Eclipse project that contains build configurations:
- **Secton** that can be built.
- **Craton 2** that is for cross-compilation for ARMv7 and it is work in progress right now (there are some expectations about the cross-compilation toolchain that are not documented here yet).
- **Secton-ARM**  which is a build for 64-bit ARMv8 and it is also work in progress.

For the build, it is expected that you have installed Cap'n Proto C++ runtime. See [here](https://capnproto.org/install.html) for installation guide. Library version must be 1.0.2.
