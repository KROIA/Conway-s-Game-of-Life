# Conway's Game of Life

This is a small Example using the [Pixelengine](https://github.com/KROIA/Pixelengine)

## Setup
Clone this Git. It will contain the Pixelengine as submodule.
If you download the Repo using the download button on Github, you have to download the Pixelengine seperatly and put it in the Folder:<br>
```
Conway-s-Game-of-Life/Extern/PixelEngine
```
If you download PixelEngine also using the download button, you have to get the [SFML](https://github.com/SFML/SFML) Library and the [easy profiler](https://github.com/yse/easy_profiler)<br>
Put them in to the Pixelengine directory.
```
Conway-s-Game-of-Life
  +- Extern
  |    +- PixelEngine
  |    |    +- extern
  |    |    |    +- easy_profiler
  |    |    |    |    +- .git (Files from easy_profiler)
  |    |    |    |    +- CMakeLists.txt
  |    |    |    |       ...
  |    |    |    +- SFML
  |    |    |    |    +- .git (Files from SFML)
  |    |    |    |    +- CMakeLists.txt
  |    |    |    +- easy_profiler.pri
  |    |    |    +- SFML.pri 
  +- resources
  +- inc
  +- src
```

Then compile the SFML and easy_profiler project first using the CMakeLists.txt the specific repo folder.
For this, you can follow the instructions from the [PixelEngine](https://github.com/KROIA/Pixelengine/tree/d10c1e8fcba5029a7794cb862f963208fe033726#sfml-info)<br>

After that, you can compile this Project.
