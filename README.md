# Clipboard module
[![License](http://img.shields.io/badge/license-GPLv3-blue.svg?colorB=9977bb&style=plastic)](https://github.com/Obsidian-StudiosInc/clipboard/blob/master/LICENSE)
[![Build Status](https://img.shields.io/travis/Obsidian-StudiosInc/clipboard/master.svg?colorA=9977bb&style=plastic)](https://travis-ci.org/Obsidian-StudiosInc/clipboard)
[![Build Status](https://img.shields.io/shippable/59f38a4f6ad2b30700c00b6c/master.svg?colorA=9977bb&style=plastic)](https://app.shippable.com/projects/59f38a4f6ad2b30700c00b6c/)
[![Code Quality](https://img.shields.io/coverity/scan/14160.svg?colorA=9977bb&style=plastic)](https://scan.coverity.com/projects/obsidian-studiosinc-clipboard)

[<img align="left" src="http://i.imgur.com/ZzWlRgJ.png">](https://github.com/pasnox/oxygen-icons-png)

This is a clipboard module we created for the Bodhi Linux's e17 fork, 
Moksha. Despite being developed for Moksha, it should work on any e17 
Desktop *meeting the requirements below*.

Clipboard currently is a simple, basic features clipboard manager. It 
maintains a history of text copied to the clipboard from which you can 
choose with a bare minimum of configuration options.  Our main 
ambition, aside from learning EFL and enlightenment module programming, 
was to make it light on system resources with no additional 
dependencies, integrate in a natural way with the e17/Moksha desktops 
and be a usable alternative to other clipboard managers (Parcellite, 
CopyQ, glipper etc.).

# Main Features
[<img align="right" src="http://i.imgur.com/006sZxB.png">](http://i.imgur.com/006sZxB.png)
- Text only clipboard management. Sorry no image support.
- Stores a clipboard history of customizable length.
- Clipboard contents are preserved even when you close applications
- Uses either or both Primary or Clipboard clipboards.
- Synchronize clipboards.
- Several View options to display items the way you like.
- Global hotkeys with configurable e17/Moksha key bindings.
- Functions with or without a gadget added to the desktop or a shelf.
- Does not need or use a system tray.

# Warnings
This module is currently in a pre-release beta state. *It is more a 
proof of concept than an actual application.* Hopefully in time that 
will change. We make no guarantees that it will function correctly and 
meet your needs. We fully admit to not really knowing what we doing: 
this is an educational project for us. It is mostly untested except 
briefly by the developers and has been and most likely will continue to 
be in rapid development. This may mean that on occasion this package 
may even fail to build. *This software is provided "as is", without 
warranty of any kind, expressed or implied.* 

# Screenshot

[![alt text](http://i.imgur.com/4GreCI0.png "Clipboard module")](http://i.imgur.com/4GreCI0.png)

# Requirements
We currently only support Linux and the X Window System clipboards so 
sorry users of other Operating systems and Display Servers. It is also 
worth noting we currently only build using the gcc compiler, other 
compilers remain untested due to a lack of time. *Patches are however 
accepted.*

It should build without any issues on any system meeting the above 
reguirements as well as meeting [the reguirements for building e17 
itself](https://www.enlightenment.org/download).

- [Enlightenment Foundation Libraries](https://download.enlightenment.org/rel/libs/efl/)
- [Moksha](https://github.com/JeffHoogland/moksha) or 
- [Enlightenment 17](https://git.enlightenment.org/core/enlightenment.git/?h=enlightenment-0.17)

# Installation
System installation
```bash
mkdir build
meson . build
ninja -C build
ninja -C build install
```

To install in home directory
```bash
mkdir build
meson . -Dhomedir-install=${HOME} build
ninja -C build
ninja -C build install
```

# Development
If you have found this module interesting and want to collaborate by 
making it better, you are more than welcome to do so and we are 
interested in adding more features. We would welcome knowledgeable 
e-developers or translators.

# Credits
- [Stefan Uram](https://github.com/thewaiter)
- [Robert Wiley](https://github.com/rbtylee)

# Thanks
- [Jeff Hoogland](https://github.com/JeffHoogland)
- [Leif Middelschulte](https://github.com/leiflm)
- [Marcel Hollerbach](https://github.com/marcelhollerbach)
- [Stephen Houston](https://github.com/okratitan)

<p align="center"><i>In memory of John B. Minick IV (1952-2016)<br>
Brick mason, truck driver, father, Linux user, intellectual and revolutionary.</i></p>
