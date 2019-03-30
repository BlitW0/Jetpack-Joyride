# Jetpack Joyride

A basic 2D implementation of the famous mobile device game jetpack joyride using OpenGL

## How to run
* Run the following commands in the cloned directory
```console
mkdir build
cd build
cmake ..
make
```
* To play the game, run the following commands in the build directory
```console
make
./graphics_asgn1
```

## Controls
* <kbd>LEFT</kbd> - To move left
* <kbd>RIGHT</kbd> - To move right
* <kbd>UP</kbd> - To use jet
* <kbd>Z</kbd> - To throw balloons

## Details
* Golden coins increase the score by 50
* Red coins appear randomly and increase score by 500
* Magnets attract player upwards
* Boomerangs appear randomly and cause damage to health
* Player can throw balloons to extinguish fire
* Some fire lines move up and down
* If health becomes less than or equal to zero, game is over
* Fire lines disappear after doing damage
