# OpenHCL
A fanmade, cross-platform port of [E. (Buster) Hashimoto's](https://hp.vector.co.jp/authors/VA025956/) Hydra Castle Labyrinth.

## Supported Platforms and Features
#### 3DS
- 3D display
- Inventory displayed on the bottom screen.

#### PSP
- Also playable on Vita.

#### Wii
- 240p mode for CRT televisions.
- Controllable with wiimote, nunchuk, gamecube, and classic controllers.

## Compiling
Note: to run the game after compiling you may need the assets from the release page.

#### 3DS
- Install [DevkitPro](https://devkitpro.org/wiki/Getting_Started) with devkitARM and libctru.
- Install [bannertool](https://github.com/Steveice10/bannertool/releases).
- Install vorbisidec: `pacman -S 3ds-libvorbisidec`
- To just build a 3dsx file, run: `make 3ds`
- To build a cia file, install [MakeROM](https://github.com/3DSGuy/Project_CTR/releases) and run `make cia`

#### PSP
- Install [PSPSDK](https://github.com/pspdev/pspsdk). ([Minimalist PSPSDK](https://sourceforge.net/projects/minpspw/))
- Install the [libvorbis and libogg devpaks](https://sourceforge.net/projects/minpspw/files/devpak%20%28posix%29/devpaks%20%28cygwin_nix%29/). (In Minimalist PSPSDK, keep the 'Basic Devpaks' box checked during setup.)
- Run `make psp`

#### Wii
- Install [DevkitPro](https://devkitpro.org/wiki/Getting_Started) with devkitPPC and libogc.
- Install vorbisidec: `pacman -S ppc-libvorbisidec`
- Run `make wii`

## Acknowledgments
The game’s graphics, design, and data files are owned by [Buster](https://hp.vector.co.jp/authors/VA025956/).

Music: [Matajuurou](http://www5c.biglobe.ne.jp/~mataz/)

Sound effects: [Osabishyuuki](http://osabisi.sakura.ne.jp/m2/)

Project Quadruple D: [Sandman](http://karen.saiin.net/~hayase/)

English translation: [Gary the Krampus](https://hkcomplex.tumblr.com/)

Korean translation: [DDinghoya](https://github.com/DDinghoya)
