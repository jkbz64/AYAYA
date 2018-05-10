# AYAYA
*** AYAYA is under heavy development, so be careful of bugs *** 

Qt Desktop Twitch.tv client written in Qt5.

## Requirements

- **C++14**
- **Qt5** Core Widgets Network

**Dependencies:**
* [TwitchQt](https://github.com/jkbz64/TwitchQt) as Gitmodule
* [libcommuni](https://github.com/communi/libcommuni) as Gitmodule
* [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet) as Gitmodule
* [mpv](https://github.com/mpv-player/mpv)


**Available video backends:**
* [mpv](https://github.com/mpv-player/mpv)

## Getting dependencies
### Windows
Well... since the only available video backend(for now) is mpv and it compiles on windows only under MSYS2, you will need to build it using MSYS2. You have to pretty much compile mpv by yourself(good luck with mpv's dependencies) and install MSYS2 version of QT5 then do the clone-qmake-make routine. (More video backends are planned(tm) so we won't need to use MSYS2 in future for building on Windows).

### Linux
On linux it's so much easier, just install developer packages for `mpv` and `Qt5` using your distro package manager and just do the clone-qmake-make routine, it's that simple.

## Building
Once you have installed mpv and Qt5, you can just `git clone` **recursively** the repository and call `qmake AYAYA.pro` followed by `make`.
```
git clone --recurse-submodules https://github.com/jkbz64/AYAYA
cd AYAYA && mkdir build && cd build
qmake ../AYAYA.pro
make
```
## Screenshots
![Browse](/extra/browse.png?raw=true "Browse")
![Stream](/extra/stream.png?raw=true "Stream")
