# Chromium Ozone port to Raspberry Pi dispmanx

## Introduction

This is a [Chromium Ozone](https://chromium.googlesource.com/chromium/src/+/master/docs/ozone_overview.md) port to Raspberry Pi using dispmanx.  [Dispmanx](http://elinux.org/Raspberry_Pi_VideoCore_APIs) is a native display management API supporting OpenGL ES/EGL surfaces on VideoCore IV GPU equipped by Raspberry Pi.  It can achieve siginificant performance improvement compared to the traditional X11 platform.

[![YouTube video](https://img.youtube.com/vi/61sEBhJa7k8/0.jpg)](https://www.youtube.com/watch?v=61sEBhJa7k8)

Current status:

- Only fullscreen `content_shell` works (no navigation, no address bar, no bookmarks)
- No GPU process separation, run with `--in-process-gpu`.
- Keyboard/mouse input support using evdev driver, but no VT console handling.
- It's forced to support WebGL - renderings are severely broken / no output for most pages.

## Building

To build Chromium for Raspberry Pi, set up the build environment and check out [src repository](https://chromium.googlesource.com/chromium/src) by following [the official guide](https://chromium.googlesource.com/chromium/src/+/master/docs/linux_build_instructions.md).  Then follow the instruction below in `src` directory.

Set up ARM sysroot with dispmanx library:

```
./build/linux/sysroot_scripts/install-sysroot.py --arch=arm
git clone --depth 1 https://github.com/raspberrypi/firmware
cp -R firmware/hardfp/opt build/linux/debian_wheezy_arm-sysroot
cp firmware/hardfp/opt/vc/lib/pkgconfig/bcm_host.pc build/linux/debian_wheezy_arm-sysroot/usr/lib/pkgconfig
```

Clone this repository and apply patches:

```
git clone https://github.com/yaegashi/ozone-dispmanx ui/ozone/platform/dispmanx
cat ui/ozone/platform/dispmanx/patches/* | git am
```

Configure gn and build:

```
gn gen out/dispmanx
cp ui/ozone/platform/dispman/tools/args.gn out/dispmanx
# Review your build settings
gn args out/dispmanx
ninja -C out/dispmanx content_shell
```

After successful builds, you can run `pack.sh` to collect runtime files into archive `chromium-ozone-dispmanx.tar.gz`.  Bring it to your Raspberry Pi device and run.

```
./ui/ozone/platform/dispmanx/tools/pack.sh
scp out/dispmanx/chromium-ozone-dispmanx.tar.gz yourpi:
```

## Running

Check `/boot/config.txt` for GPU settings needed to run `content_shell`:

```
# dispmanx is exclusive with X acceleration, do not enable this
#dtoverlay=vc4-kms-v3d

# GPU needs memory at least 128M
gpu_mem=128

# Recommended to start with relatively lower resolutions
# HDMI 1360x768@60Hz
hdmi_group=2
hdmi_mode=39
```

`content_shell` needs some special options to run:

```
./content_shell --no-sandobx --in-process-gpu https://www.google.com
```

Type <kbd>Ctrl</kbd>+<kbd>C</kbd> to terminate `content_shell`.  Currently keyboard/mouse input events are duplicated and delivered to both the browser and your shell environment, so you'd better run it on the VT console (Press <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>F1</kbd> to switch to the VT console from X desktop).

Some recommended URLs:

- <https://www.google.com/logos/2010/pacman10-i.html> - Play PAC-MAN Doodle
- <https://threejs.org/examples> - Lots of WebGL pages to torture dispmanx
- <https://youtube.com> - See acceptable quality of movie playback

## Related Projects

- <https://github.com/fred-wang/ozone-caca>
