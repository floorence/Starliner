# Starliner

3D space exporation game written in C++ using OpenGL 

## Setup

Prerequisites: 
* computer on Linux, MacOS, or Windows
* C and C++ compiler. the project compiles in C++17 or above
* the following installed on your system. all other dependencies (glad, glfw, glm, json, khr, and stb) are header files in the project source (which you will get when you clone the repo)
    * cmake 
    * OpenGL
    * fmt
    * assimp

Running the game:

These steps work on linux, I actually have no idea if the procedure is different for MacOS or Windows. Maybe [@andrewli200504](https://github.com/AndrewLi200504) can help? :D
1. ensure you satisfy all the prereqs.
2. clone the repo
3. `cd 3D-Platformer`
4. `cmake -B build`
5. `cmake --build build`
6. `build/Platformer`

note: depending on your system, AddressSanitizer can be annoying and report a bunch of memory leaks from system libraries. to stop this, add the libraries you want to suppress in `asan_suppressions.txt` and run `export LSAN_OPTIONS=suppressions=./asan_suppressions.txt`

## Attributions

### Assets

* Spaceship model: Spaceship by Liz Reddington [[© CC-BY]](https://creativecommons.org/licenses/by/3.0/) via Poly Pizza: https://poly.pizza/m/5nWeu4IQXVX
* Planet textures (adapted): https://www.solarsystemscope.com/textures/
* Space skybox: https://jettelly.com/blog/some-space-skyboxes-why-not
* Font: https://www.dafont.com/pixel-operator.font
* Icons
  * Settings (adapted): https://www.vecteezy.com/vector-art/11786593-pixel-art-settings-button-with-gear-vector-icon-for-8bit-game-on-white-background
  * Gear: by me 

### Resources
Without people way smarter than me making tutorials and providing open source code examples, this project would not have been possible in a reasonable time frame, or be vibecoded :vomiting_face:

places in my code that have been adapted from the below resources are clearly marked as such.

* Victor Gordan's OpenGL tutorials
    * youtube playlist: https://www.youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
    * github repo: https://github.com/VictorGordan/opengl-tutorials/tree/main
* Joey de Vries' Learn OpenGL [©](https://creativecommons.org/licenses/by-nc/4.0/) [:bird:](https://twitter.com/JoeyDeVriez)
    * website: https://learnopengl.com/
    * github repo: https://github.com/JoeyDeVries/LearnOpenGL/tree/master
* "How to do Basic 3D Collision Detection in C++" by VertoStudio3D: https://youtu.be/wVhSQHKvBW4?si=aUEeWNe_OMxw2ShF
* Song's OpenGL Sphere: https://www.songho.ca/opengl/gl_sphere.html
* a lot more to come I'm sure!