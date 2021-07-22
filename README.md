# Conway's Game of Life
<img src="https://github.com/KROIA/Conway-s-Game-of-Life/blob/main/resources/output/GliderGun.png" width="400"><img src="https://github.com/KROIA/Conway-s-Game-of-Life/blob/main/resources/output/NetFill.png" width="396"><br>
This is a small Example using the [Pixelengine](https://github.com/KROIA/Pixelengine)

## Overview
Play with the interactive Game of Life implementation, create your own creations and watch them evolve<br>
### Features
* Pause and Play the game.<br>
* Add or kill living cells per mouseclick.<br>
* Edit the map, insert stored images to the map.<br>
* Safe the map to an image.<br>
* Just play around.<br>

### Keybinding:<br>

| Key | Description |
| --- | --- |
| SPACE | Will pause the game |
| P | Will show some stats about the running engine |
| 1 | Will load the resources\Glider_gun.png |
| 2 | Will load the resources\Simkin_glider_gun.png |
| 3 | Will load the resources\Netmaker.png |
| 4 | Will load the resources\frothing_spaceship.png |
| 5 | Will load the resources\Sun.png |
| C | Clears the map |
| S | Saves the current map to an image in the Folder: resources\output |
| A | Will load the last saved map |
| O | Will open the filebrowser, so you can load images.<br>Move the mouse for positioning the image, rotate by right click and placing by left click. |
| --> | Will step one step forward |
| ENTER | Will remove this infotext |

| Mouse | Description |
| --- | --- |
| WheelScroll | Zoom +/- |
| WheelKlick MouseMove | Moves the Camera |
| LeftKlick | Sets the cell on the mouse cursor to alive |
| RightKlick | Sets the cell on the mouse cursor to dead |

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

Then compile the SFML and easy_profiler project first, using the CMakeLists.txt the specific repo folder.
For this, you can follow the instructions from the [PixelEngine](https://github.com/KROIA/Pixelengine/tree/d10c1e8fcba5029a7794cb862f963208fe033726#sfml-info)<br>

After that, you can compile this Project.
