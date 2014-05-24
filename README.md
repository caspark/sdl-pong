Messing around: Pong in SDL
===========================

This is a pong game written with [SDL 2.0](http://www.libsdl.org/) with a [fixed timestep](http://gafferongames.com/game-physics/fix-your-timestep/), which plays some sound and does some font rendering.

It might be useful for you if you're trying to figure out how to fix your timestep, play music, or do font rendering in SDL 2.0, but I wouldn't recommend using this project as your source for best practices ;)

(not) Building and Running
--------------------------

Sorry, this was just a toy project so I didn't get around to setting up a proper reproducible way to build and run it before I got distracted with another project.


Tips:

- Use Visual Studio. I think I used 2012.
- SDL2-2.0.0-VC should be extracted into `c:/tools/SDL2-2.0.0-VC`, so that (e.g.) you have `c:/tools/SDL2-2.0.0-VC/lib/x64/SDL2.dll`
- Put all the SDL extensions (ttf, net, mixer, image) into the same directory structure, so that (e.g.) you have `c:/tools/SDL2-2.0.0-VC/lib/x64/SDL2_ttf.dll`.

