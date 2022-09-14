# Planar Beamer Projection

This project can be used to setup a beamer to project images onto planar surfaces.

## Usage

`pBp widthOfScreen heightOfScreen image1 [image2 ...]`

After starting you can click on the surface to place the corner points of all images. These points are expected to be placed in a clockwise order starting with the upper left corner.
After placing all images you can adjust these corners by dragging the corner with your mouse.

Additionally there are some special keys:
| Key | Description |
| --- | --- |
| esc | stops the programm |
| space | shows / hides annotations |

## Howto build

This project uses cmake. Only opencv is needed as dependency.
```
cd planarBeamerProjection
mkdir build
cd build
cmake-gui ..
make
```
