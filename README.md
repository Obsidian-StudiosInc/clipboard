# Clipboard module

[<img align="left" src="http://i.imgur.com/ZzWlRgJ.png">](https://github.com/pasnox/oxygen-icons-png)
<br>
This is a clipboard module we created for the Bodhi Linux's e17 fork, Moksha. Despite being developed for Moksha, it should work on any e17 Desktop *meeting the requirements below*.

 Clipboard currently is a simple, basic features clipboard manager. It maintains a history of text copied to the clipboard from which you can choose with a bare minimum of configuration options.  Our main ambition, aside from learning EFL and enlightenment module programming, was to make it light on system resources with no additional dependencies, integrate in a natural way with the e17/Moksha desktops and be a usable alternative to other clipboard managers (Parcellite, CopyQ, glipper etc.).

# Main Features
<br>
[<img align="right" src="http://i.imgur.com/006sZxB.png">](http://i.imgur.com/006sZxB.png)

- Text only clipboard management. Sorry no image support.


- Keeps a clipboard history of customizable length.
 

- Clipboard contents are preserved even when you close applications


- Uses either or both Primary or Clipboard clipboards.


- Synchronize clipboards.


- Several View options to display items the way you like.


- Global hotkeys with configurable e17/Moksha key bindings


- Functions with or without a gadget added to the desktop or a shelf.


- Does not need or use a system tray


# Warnings

This module is currently in a pre-release beta state. *It is more a proof of concept than an actual application.* Hopefully in time that will change. We make no quarantines that it will function correctly and meet your needs. We fully admit to not really knowing what we doing: this is an educational project for us. It is mostly untested except briefly by the developers and has been and most likely will continue to be in rapid development. This may mean that on occasion this package may even fail to build. *This software is provided "as is", without warranty of any kind, expressed or implied.* 

# Screenshot

<br>

![alt text](http://i.imgur.com/NE8NeLp.png "Clipboard module")

# Requirements

<br>

We currently only support Linux and the X Window System clipboards so sorry users of other Operating systems and Display Servers. It is also worth noting we currently only build using the gcc compiler, other compilers remain untested due to a lack of time. *Patches are however accepted.*

It should build without any issues on any system meeting the above reguirements meeting [the reguirements for building e17 itself](https://www.enlightenment.org/download).

- [Enlightenment Foundation Libraries] (https://download.enlightenment.org/rel/libs/efl/)

- [Moksha](https://github.com/JeffHoogland/moksha) or 

- [Enlightenment 17](https://git.enlightenment.org/core/enlightenment.git/?h=enlightenment-0.17)

# Installation
<br>

```bash
./autogen.sh
make
sudo make install
```

See the INSTALL file for more complete instructions. Bodhi Users coming soon to the repos :)

# Usage

*to be completed latter*

# Future Release Ideas

*to be completed latter*

# Known Issues

*to be completed latter*

# Development

<br>

If you have found this module interesting and want to collaborate by making it better, you are more than welcome to do so and we are interested in adding more features. We would welcome knowledgeable e-developers or translators. 

# License

<br>

- Not Offically decided but certainly FOSS. For now see our [COPYING](https://github.com/thewaiter/Clipboard/blob/master/COPYING) file.

# Credits

<br>

- [Stefan Uram](https://github.com/thewaiter)
- [Robert Wiley](https://github.com/rbtylee)

# Thanks
- [Jeff Hoogland](https://github.com/JeffHoogland)
- [Marcel Hollerbach](https://github.com/marcelhollerbach)
- [Stephen Houston]



