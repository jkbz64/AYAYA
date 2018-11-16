# AYAYA
***AYAYA is under heavy development, so be aware of bugs.*** 

Qt Desktop Twitch.tv client written in Qt5.

## Requirements

- **C++14**
- **Qt5** Core Widgets Network

**Dependencies:**
* [TwitchQt](https://github.com/jkbz64/TwitchQt) as Gitmodule
* [libcommuni](https://github.com/communi/libcommuni) as Gitmodule
* [QDarkStyleSheet](https://github.com/ColinDuquesnoy/QDarkStyleSheet) as Gitmodule

**Supported video backends:**
* Null - Allows you to use AYAYA only for chat
* [mpv](https://github.com/mpv-player/mpv)
* [vlc](https://www.videolan.org) - (Experimental)

**Supported stream extractors:**
* Null
* [youtube-dl](https://rg3.github.io/youtube-dl/)

## Getting dependencies
### Windows
Well... since the only available video backend(for now) is mpv, and it compiles on windows only under MSYS2, you will need to build it using MSYS2. You have to pretty much compile mpv by yourself(good luck with mpv's dependencies) and install MSYS2 version of QT5. Then do the clone-qmake-make routine. (More video backends are planned(tm) so we won't need to use MSYS2 in future for building on Windows).

### Linux
On linux it's so much easier, just install developer packages for `mpv` and `Qt5` using your distro package manager and just do the clone-qmake-make routine, it's that simple.

## Building
Once you have installed Qt5 and optionally one of the supported backends(multiple are supported too), you can just `git clone` **recursively** the repository and call `qmake AYAYA.pro` followed by `make` with optional backends.

```
git clone --recurse-submodules https://github.com/jkbz64/AYAYA
cd AYAYA && mkdir build && cd build
qmake "CONFIG+=mpv" ../AYAYA.pro // or qmake ../AYAYA.pro for null backend (no video)
make
```

## Screenshots
![Browse](/extra/browse.png?raw=true "Browse")
![Stream](/extra/stream.png?raw=true "Stream")
![Fullscreen](/extra/fullscreen.png?raw=true, "Fullscreen")
