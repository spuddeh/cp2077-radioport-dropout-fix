# Radioport Dropout Fix

A RED4ext plugin for Cyberpunk 2077 2.31 that stops the Radioport going silent while a passing
car tuned to the same station is nearby.

## The fault

The game plays at most four radio stations at once: the four whose nearest receiver is closest to
you. Each station is measured by one receiver, and the game refuses to use your Radioport for that
whenever any other receiver in the world is tuned to the same station, even one that is switched
off. A traffic car tuned to your station, radio off, a hundred metres away, becomes the station's
measuring point; wherever four other stations have a receiver closer than that car, your station
falls out of the four and goes silent until the car leaves.

## The fix

One instruction in the radio manager's list rebuild, so the Radioport counts like any other
receiver. It is on you, so it is always nearest, and the station you are on always keeps its place.
The function is resolved by RED4ext hash and every byte is verified before any is written; where a
game update has changed the function, the plugin logs a line and changes nothing.

## Install

Drop the archive into your mod manager, or copy `red4ext\plugins\RadioportDropoutFix\` into the
game folder. Requires [RED4ext](https://www.nexusmods.com/cyberpunk2077/mods/2380).

## Build

See `plugin/CMakeLists.txt`. RED4ext.SDK is header-only; point the include path at a checkout.
Releases ship the committed DLL, so see `RELEASING.md` before tagging.

## License

MIT.
