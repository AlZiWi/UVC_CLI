# UVC_CLI

macOS CLI for libuvc to control, stream, and record USB cameras (frame-based).
Supports FPS, resolution, exposure, and trigger settings.

Note: Requires root on macOS.

## Requirements

- macOS
- Xcode Command Line Tools
- CMake 3.16 or newer
- Homebrew

Install the required libuvc dependency with:

```sh
brew install libuvc
```

The preview functionality is optional. To build it, also install OpenCV:

```sh
brew install opencv
```

## Build

The default build does not use OpenCV:

```sh
cmake -S . -B build
cmake --build build
```

To include the OpenCV preview functionality:

```sh
cmake -S . -B build-preview -DUVC_ENABLE_PREVIEW=ON
cmake --build build-preview
```

The resulting executable is `build/libuvc_util` or
`build-preview/libuvc_util`. If a dependency is installed outside Homebrew's
standard prefix, pass its CMake or pkg-config location when configuring, for
example:

```sh
PKG_CONFIG_PATH=/path/to/libuvc/lib/pkgconfig:$PKG_CONFIG_PATH \
  cmake -S . -B build
```

## Usage

Note: Requires root on macOS due to an inherent limitation in libusb.

To view all options, run without arguments:

```sh
./build/libuvc_util
```

List connected cameras:

```sh
./build/libuvc_util -list
```

Capture a camera by its product or serial name:

```sh
./build/libuvc_util "Camera Name" -out=frames
```

When built with `UVC_ENABLE_PREVIEW=ON`, display the stream with metadata:

```sh
./build-preview/libuvc_util "Camera Name" -preview
```

Press Q to quit the preview.
