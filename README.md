# AYAYA

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

## Building
Once you have installed mpv, you can just `git clone` **recursively** the repository and call `qmake AYAYA.pro` followed by `make`.
```
git clone --recurse-submodules https://github.com/jkbz64/AYAYA
cd AYAYA && mkdir build && cd build
qmake ../AYAYA.pro
make
```
## Screenshots
![Browse](/extra/browse.png?raw=true "Browse")
![Stream](/extra/stream.png?raw=true "Stream")
