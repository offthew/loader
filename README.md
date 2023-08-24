<hr>

<div align="center"> 
    <img src="assets/logo.svg" height=312/>
</div>

<div align="center"> 

Modding Framework for Coromon  

</div>

---

> [!IMPORTANT]  
> Please do not report any issues that may've been caused through this project to the official game developers as they will not assist you with broken games / saves when you are using mods.  
> Please [open an issue here](https://github.com/CoroModLoader/loader/issues/new) instead!

> [!NOTE]  
> This Project is a continuation of [chalchiu](https://github.com/Curve/chalchiu) with greater emphasis on Coromon

# Introduction

This project allows you to load third-party mods into the game and helps you by supplying useful utilities provided by the [core](https://github.com/CoroModLoader/core).

# Installation

* Locate your game directory
  > e.g. `C:/GOG Games/Coromon/`

* Download the zip file from the latest [Release](https://github.com/CoroModLoader/loader/releases) and unpack all files into the game directory
  * Your Game Folder should now roughly look like this
    > [!NOTE]  
    > Some files have been omitted
    ```
    📂 C:/GOG Games/Coromon
    ├── coromon.exe
    ├── CoronaLabs.Corona.Native.dll
    ├── corona-plugins
    ├── iphlpapi.dll                  <-- This file should be new
    ├── libeay32.dll
    ├── lua.dll
    ├── ...
    ```

* When starting the game a `coromodloader.log` should appear in the game directory
  * > [!WARNING]  
    > If this does not happen you probably did something wrong.

* Done!

# Un-Installation

* Locate your game directory
  > e.g. `C:/GOG Games/Coromon/`

* Delete `iphlpapi.dll`

* Done!

# For Developers

Checkout the [wiki](https://github.com/CoroModLoader/loader/wiki) for more information.