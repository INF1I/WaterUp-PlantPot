<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->
**Table of Contents**
 - [Before we begin](#before-we-begin)
 - [CLion](#clion)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Setting up the project for development
This guide will show you how to setup your ide to work with this
project. I only added the documentation for Clion because that's the IDE
I'm using.

PlatformIO also has great [documentation](http://docs.platformio.org/en/latest/ide.html#standalone-ide) about configuring PlatformIO
Core to work with several IDE's. Like Clion, Visual Studio (code),
Codeblocks, Eclipse, Netbeans, QtCreator, Sublime Text, VIM and Emacs.

### Before we begin
First we have to make sure PlatformIO is installed. open an terminal or
command prompt and type:
```bash
platformio
```
If you get the PlatformIO menu you are ready to start configuring an
ide. If you get an message like command not found you have to install
PlatformIO Core they have an great [guide](http://docs.platformio.org/en/latest/installation.html) on doing this.

### CLion
To work on this project with clion, you open clion click on
`VCS > Checkout from version control > github` and choose this repository.
Then you will be asked if you want to open this project click yes and you
should be able to see the project now.

To install all the required libraries and configure cmake for
compilation and uploading to the arduino open an command prompt or
terminal from the toolbar buttons at the bottom of the window and type:
```bash
platformio init --ide clion
```
After PlatformIO has initiated the project you will probably be prompted
by clion that the project files have changed and asks if you want to
reload the project. Reload the project and if any errors still exist open
the CMake toolbar from the toolbar buttons at the bottom of the page,
and click on the reload button on the left.

If you encounter any problems please refer to the documentation on the
[PlatformIO website](http://docs.platformio.org/en/latest/ide/clion.html)