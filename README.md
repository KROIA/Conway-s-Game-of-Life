# Conway's Game of Life
<img src="https://github.com/KROIA/Conway-s-Game-of-Life/blob/main/resources/output/GliderGun.png" width="400"><img src="https://github.com/KROIA/Conway-s-Game-of-Life/blob/main/resources/output/NetFill.png" width="396"><br>
This is a small Example using the [Pixelengine](https://github.com/KROIA/Pixelengine)

## Overview
Play with the interactive Game of Life implementation, create your own creations and watch them evolve<br>
### Features
* Pause and Play the game using the space key.<br>
* Leftklick for adding a living cell and rightklick to kill a cell.<br>
* Press key: 1,2 or 3  to load one of 3 example images.<br>
* Press key: S  to store the current frame.<br>
* Press key: A  to load the last stored frame.<br>
* Zoom in and out using the mousewheel. <br>
* Drag the map around by klick and drag on the mousewheel.<br>

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
