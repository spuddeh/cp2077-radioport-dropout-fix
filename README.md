# Radioport Dropout Fix

A RED4ext plugin for Cyberpunk 2077 2.31 that stops the Radioport going silent while a car or a world
radio tuned to the same station is nearby.

## The fault

The game plays at most four radio stations at once: the four whose nearest receiver is closest to
you. Each station is measured by one receiver, and the game refuses to use your Radioport for that
whenever any other receiver in the world is tuned to the same station, even one that is switched
off. A traffic car or a world radio tuned to your station, even switched off, becomes the station's
measuring point; wherever four other stations have a receiver closer than that car, your station
falls out of the four and goes silent until the car leaves.

## The fix

One instruction in the radio manager's list rebuild, so the Radioport counts like any other
receiver. It is on you, so it is always nearest, and the station you are on always keeps its place.
The function is resolved by RED4ext hash and every byte is verified before any is written; where a
game update has changed the function, the plugin logs a line and changes nothing.

## Install

Download from [Nexus Mods](https://www.nexusmods.com/cyberpunk2077/mods/33838). Drop the archive into your mod manager, or copy `red4ext\plugins\RadioportDropoutFix\` into the
game folder. Requires [RED4ext](https://www.nexusmods.com/cyberpunk2077/mods/2380).

## Build

See `plugin/CMakeLists.txt`. RED4ext.SDK is header-only; point the include path at a checkout.
Releases ship the committed DLL, so see `RELEASING.md` before tagging.

## License

Licensed under the [MIT License](LICENSE). Use, change and share this mod and its source,
including in your own mods. Keep the licence notice with any copy.

## Disclaimer

This mod was developed with the assistance of an LLM. All in-game testing and code validation was
performed by a human. No rogue AIs were permitted through the Blackwall.
