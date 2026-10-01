<<<<<<< HEAD
# NeoCavEX
=======
# fCavEX
>>>>>>> other-fork/master

[Cave Explorer](https://github.com/xtreme8000/CavEX) by [xtreme8000](https://github.com/xtreme8000) is a Wii homebrew game with the goal to recreate most of the core survival aspects up until Beta 1.7.3. ~~Any features beyond *will not* be added.~~

*fCavEX* (forked CavEX) is an experimental fork of CavEX, made by [yyy257](https://github.com/yyy257), with all kinds of changes and additions. Some additions may not be from the original game and some mechanics may be different - this fork is not aiming for a 1:1 recreation. Unlike CavEX, fCavEX does not aim for complete save compatibility - for instance, chests and signs use an incompatible saving system with static limits and some blocks may use metadata values differently. Until stated otherwise, a freshly created save should be compatible, as fCavEX does not have its own world generator yet.

I took yyy257's fork and decided to continue the work he's done, and add more features. All is still highly experimental.

---
**Changes, compared to yyy257's fCavEX**

<<<<<<< HEAD
**Features**
* great performance on Wii (about 60fps)
* 5 chunk render distance currently
* load any beta world save
* nearly all blocks added, except redstone related
* many items from the original
* correct light propagation
* ambient occlusion on blocks
* configurable buttons
* GameCube support
=======
* Redstone functionality: redstone torches, wires, pressure plates can power doors and trapdoors. It's an early version and wire placement doesn't look good yet. Pistons and repeaters are planned to be added as well.
* Farming crops: dirt to farmland with a hoe, planting seeds to let crops grow
* Growing mushrooms: mushrooms spread when they're in the dark.
* Fire: You can set stuff on fire with your flint&steel, fire can spread to flammable objects.
* Bed: Using a bed progresses time. Still a work in progress but the basics are there
* Stars at night, to make it easier to orient during total darkness at night.
* Particle effects for torches. Just looks a bit better. (uses particles.png)
* TNT can now explode. 
>>>>>>> other-fork/master

---

**Planned features** *(in no particular order, not complete)*
<<<<<<< HEAD
* main menu
* UI for settings
* sound
* generation of new chunks
* biome colors
* real texture pack support
* Beta 1.7.3 multiplayer support
=======
* Sounds
* Block gravity: sand and gravel drop down when there's nothing underneath them to support
* Water/lava flow: once a brick has been removed next to, or underneath a liquid, it will flow there
* Additional controller support
* Mobs
* Minecarts
* Fixes to existing bugs:
	- Furnace has 4 identical sides
 	- leaves stay floatingin the air after log has been removed
 
**Known issues**
* Texture orientation for blocks that have a specific "direction"
	- Bed placement isn't correct yet
	- Redstone wire texture goes only in 1 orientation, no corners and intersections are displayed (although redstone does work in all directions)
	- Rails have the same texture behavior as redstone wire.
* Random crashes, once in a while.. Maybe I'll implement an optional auto-save, to prevent some headaches and tears
* Particles already spark fire before the torch is showing, after placing a torch
* Probably more, but don't be a hater please.
>>>>>>> other-fork/master

## Screenshot

![ingame0](docs/ingame0.png)
*(from the PC version)*

<<<<<<< HEAD
## Build instructions

You need to download these libraries yourself and place their source files to the following directories. They are required for any platform.

| library | files | destination |
| --- | --- | --- |
| [LodePNG](https://github.com/lvandeve/lodepng) | `lodepng.h` and `lodepng.c` | `source/lodepng/` |
| [cglm](https://github.com/recp/cglm) | `include/cglm/` | `source/cglm/` |
| [cNBT](https://github.com/chmod222/cNBT) | `buffer.c`, `buffer.h`, `list.h`, `nbt_loading.c`, `nbt_parsing.c`, `nbt_treeops.c`, `nbt_util.c` and `nbt.h` | `source/cNBT/` |
| [parson](https://github.com/kgabis/parson) | `parson.h` and `parson.c` | `source/parson/` |
| [M*LIB](https://github.com/P-p-H-d/mlib) | any root `*.h` | compiler include path |
| [ini](https://github.com/rxi/ini) | 'ini.h' and 'ini.c' | source/ini |

### Wii

For the Wii platform you need to install the [devkitPro](https://devkitpro.org/wiki/Getting_Started) Wii/Gamecube environment. Additionally install zlib using pacman of devkitPro.

```bash
dkp-pacman -S wii-dev ppc-zlib
```

To build, simply run make in the root directory. You might need to load the cross compiler env first (required e.g. if you use [fish](https://fishshell.com/) instead of bash).

```bash
source /etc/profile.d/devkit-env.sh
make
```

=======
>>>>>>> other-fork/master
There should then be a .dol file in the root directory that your Wii can run. To copy the game to your `apps/` folder, it needs to look like this:
```
cavex
├── assets
│   ├── terrain.png
│   ├── particles.png
│   ├── items.png
│   ├── anim.png
│   ├── default.png
│   ├── gui.png
│   └── gui2.png
├── saves
│   ├── world
│   └── ...
├── config
├── boot.dol
├── config.json
├── icon.png
└── meta.xml
```
