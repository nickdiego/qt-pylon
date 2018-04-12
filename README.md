## Qt Pylon

This project provides a Qt library and demo applicaction providing easy integration with
[Basler Vision Machines](https://www.baslerweb.com), through their [Pylon Software Suite](
https://www.baslerweb.com/en/products/software/pylon-linux-x86/).

![Alt text](/screenshots/demo.jpg?raw=true "Demo App")

*PS: Still a Work in Progress :)*

### Dependencies

* Some modern Linux distro (for now tested only on ArchLinux)
* Qbs >= 1.6
* Qt5 >= 5.7
  * Modules: Core, Gui, Multimedia
  * Additional modules: Quick and QuickControls2 for running the demo app
* With linux toolchain installed (only Linux x86/x86_64 target supported for now)
* Pylon Software Suite 5

### Environment

The `PYLON_ROOT` env variable is used by the build system
to configure Pylon module. So you must set it properly, for example:

```shell
$ export PYLON_ROOT=/opt/pylon5
```

If you haven't already, start by setting up a qt5 profile for qbs:

PS: Make sure that qmake is in PATH
```shell
$ qbs setup-toolchains --type gcc /usr/bin/g++ gcc
$ qbs setup-qt $(which qmake) qt5
$ qbs config profiles.qt5.baseProfile gcc
```

### Building & Running

At the root dir of the project run:
```shell
$ qbs build profile:qt5
$ qbs run --products pylondemo
```

### Integrating as dependency on your app
Add the following snippet to you higest-level Qbs file:

```qml
SubProject {
    filePath: "path/to/qt-pylon/qt-pylon.qbs"
    Properties {
        withDemo: false
    }
}
```
