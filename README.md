# Sample project

Simple example project that uses [the Framework](https://github.com/rbfx/rbfx).

It is automatically deployed to [itch.io](https://eugeneko.itch.io/sample-project) and it can be played in browser.

TODO: Expand this page and the sample.

## Building the Sample

Checkout and build this project like any other CMake project.

Provide `rbfx`:
- Build engine: provide `-DCMAKE_PREFIX_PATH=/path/to/rbfx/CMake`
- Use engine SDK: provide `-DCMAKE_PREFIX_PATH=/path/to/rbfx-SDK`

Check out `.github/workflows/build.yml` and `.github/workflows/deploy.yml` to see how this sample is built and how deployment reuses build artifacts without recompiling.

The build workflow demonstrates explicit framework and artifact I/O. Set the
`RBFX_FRAMEWORK_BUILD_TYPE` repository variable to `sdk` (the default) or `source`:

- `sdk` downloads and extracts the platform SDK in the workflow.
- `source` checks out the configured engine ref in the workflow.

`RBFX_REPOSITORY`, `RBFX_REF`, and `RBFX_RELEASE` repository variables select the
engine repository, source ref, and SDK release. Checkout and download steps have
mutually exclusive conditions, so only the configured framework variant is acquired.

Native and cross-target builds share one matrix and the same build steps. Host
acquisition is skipped entirely for native rows. Cross SDK builds download and
extract the native rbfx SDK. Cross source builds instead invoke
`ci-wait-for-build` and download the native project install—which also contains the
source-built engine binaries. These paths are
mutually exclusive, so SDK downloads happen only in SDK mode and project-artifact
waiting and downloading happen only in source mode. Each SDK is extracted once by
the workflow and may be reused by multiple action invocations in the job.

The preparation action receives one ordered `cmake_prefix_path` list containing all
possible target and native prefixes. It ignores paths not produced by the selected
workflow mode. The same list is exported as `CMAKE_PREFIX_PATH`, or as
`CMAKE_FIND_ROOT_PATH` for Web. The preparation action itself never waits for jobs,
downloads artifacts, or distinguishes source trees from SDKs.

The workflow uploads the CMake install directory directly, except Android, which
uploads the Gradle `build/outputs` directory exposed by the build action.

## Running the Sample

Run Editor:

1) Launch `Editor.exe`;

2) Open `sample-project/Project` folder;

3) Open `Scenes/Scene.xml` in Editor;

4) Press `Ctrl+P` to play the sample.

Run Player:

Launch `Player.exe`

## Portability of Resource Access

The engine needs to somehow locate project resources on launch.

By default, the engine attempts to find `Data`, `Cache` and `CoreData` folders next to the executable.
However, this behavior may be inconvenient during development:
* `Data` and `Cache` folders are located in `Project` folder,
* `CoreData` is located in `bin` folder of `rbfx` repository,
* The executable is located somewhere in CMake build folder.

This issue can be mitigated by having `ResourceRoot.ini` file.
See the comments in `ResourceRoot.ini` file in this repository for details.

![](/screenshot.png)
