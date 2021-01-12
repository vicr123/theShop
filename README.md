<img src="readme/splash.svg" width="100%" />

---
<p align="center">
<img src="https://img.shields.io/github/v/release/vicr123/theshop?label=LATEST&style=for-the-badge">
<img src="https://img.shields.io/github/license/vicr123/theshop?style=for-the-badge" />
</p>

theShop is a software store, using PackageKit and AppStream to be distro agnostic.

---

# Dependencies
- Qt 5
  - Qt Core
  - Qt GUI
  - Qt Widgets
  - Qt SVG
- [the-libs](https://github.com/vicr123/the-libs)
- PackageKit
- AppStream
- theDesk (optional, build)
  - To compile theDesk integration, theDesk must be installed.


## Build
Run the following commands in your terminal. 
```
mkdir build
qmake ../theShop.pro
make
```

## Install
On Linux, run the following command in your terminal (with superuser permissions)
```
make install
```

---

> © Victor Tran, 2021. This project is licensed under the GNU General Public License, version 3, or at your option, any later version.
> 
> Check the [LICENSE](LICENSE) file for more information.