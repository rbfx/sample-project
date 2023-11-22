# Sample project

Simple example project that uses [the Framework](https://github.com/rbfx/rbfx) as submodule.

It is automatically deployed to [itch.io](https://eugeneko.itch.io/sample-project) and it can be played in browser.

TODO: Expand this page and the sample.

## Building the Sample

Checkout and build this project like any other CMake project.

Make sure that checked out `rbfx` folder is next to the folder of this repository (`sample-project` or however you call it).

If you want to keep `rbfx` folder somewhere else, you will have to tweak `../rbfx` paths in `./CMakeLists.txt`.

Check out `Scripts/` folder and `.github/workflows/deploy.yml` to see how this sample can be built.

## Running the Sample

Run Editor:

1) Launch `Editor.exe`;

2) Open `sample-project/Project` folder;

3) Open `Scenes/Scene.xml` in Editor;

4) Press `Ctrl+P` to play the sample.

Run Player:

Launch `Player.exe --pp path/to/sample-project/Project`

![](/screenshot.png)
