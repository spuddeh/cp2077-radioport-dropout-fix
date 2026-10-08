# Radioport Fixes - Dropouts and Misc Fixes

Fixes for vanilla Radioport bugs in Cyberpunk 2077 2.31: a RED4ext plugin that stops the Radioport
going silent while a car or a world radio tuned to the same station is nearby, and a redscript fix
for the Radioport playing quieter and muffled after getting out of a vehicle.

## Dropouts

### The fault

The game plays at most four radio stations at once: the four whose nearest receiver is closest to
you. Each station is measured by one receiver, and the game refuses to use your Radioport for that
whenever any other receiver in the world is tuned to the same station, even one that is switched
off. A traffic car or a world radio tuned to your station, even switched off, becomes the station's
measuring point; wherever four other stations have a receiver closer than that car, your station
falls out of the four and goes silent until the car leaves.

### The fix

One instruction in the radio manager's list rebuild, so the Radioport counts like any other
receiver. It is on you, so it is always nearest, and the station you are on always keeps its place.
The function is resolved by RED4ext hash and every byte is verified before any is written; where a
game update has changed the function, the plugin logs a line and changes nothing.

## Muffled after a drive

### The fault

The Radioport's mix follows the `veh_radio_tier` audio parameter on the player. The game sets it to 0
on the Radioport's emitter when a save loads, and at 0 the Radioport plays clean. Getting out of a car
from the driver's seat leaves it at 1, where the Radioport is low- and high-passed and about 3.5 dB
quieter, until the next load. Other seats and the metro are not measured without the fix; the fix
resets the tier on every exit regardless.

### The fix

`r6/scripts/RadioportDropoutFix/RadioportTier.reds` wraps `PlayerPuppet.OnUnmountingEvent` and sets the
tier back to 0, with the same call the game makes on load, whenever the player leaves a vehicle from any
seat. A silent unmount is skipped: a seat switch inside the vehicle arrives as one, and the in-car tier
must hold there.

## Install

Download from [Nexus Mods](https://www.nexusmods.com/cyberpunk2077/mods/33838). Drop the archive into
your mod manager, or copy `red4ext\plugins\RadioportDropoutFix\` and `r6\scripts\RadioportDropoutFix\`
into the game folder. Requires [RED4ext](https://www.nexusmods.com/cyberpunk2077/mods/2380) and
[Redscript](https://www.nexusmods.com/cyberpunk2077/mods/1511).

## Build

See `plugin/CMakeLists.txt`. RED4ext.SDK is header-only; point the include path at a checkout.
Releases ship the committed DLL, so see `RELEASING.md` before tagging.

## License

Licensed under the [MIT License](LICENSE). Use, change and share this mod and its source,
including in your own mods. Keep the licence notice with any copy.

## Disclaimer

This mod was developed with the assistance of an LLM. All in-game testing and code validation was
performed by a human. No rogue AIs were permitted through the Blackwall.
