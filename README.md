# Radioport Fixes - Dropouts and Misc Fixes

Fixes for vanilla Radioport bugs in Cyberpunk 2077 2.5 and 2.31: a RED4ext plugin that stops the Radioport
going silent while a car or a world radio tuned to the same station is nearby and keeps the radios audible
at low master volume, and redscript fixes for the Radioport playing quieter and muffled after getting out
of a vehicle, for the station selection UI keeping an old song name on foot, and for a station pick being
applied twice. An optional file, Radioport Off, keeps the Radioport off for good.

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

## Song name on foot

### The fault

The station selection UI (`VehicleRadioPopupGameController`) updates its song name while open only
from the native `VehicleRadioSongChanged` event. The engine sends it from a car's radio controller,
which compares the track name every vehicle audio tick. The Radioport has no such controller, so on
foot the name stays on the old song until a station switch.

### The fix

`r6/scripts/RadioportDropoutFix/RadioportTrackName.reds` wraps the popup's `OnPlayerAttach` and
`OnClose`. While it is open with no vehicle, a `DelaySystem` callback compares the Radioport's track
name every 0.5 s of real time and updates the shown name on a change. In a car nothing runs.

## Station applied twice

### The fault

`BaseModalListPopupGameController.OnAction` calls `Activate()` on the `proceed` button press and again on its
release, so a station picked in the station selection UI is sent twice.

### The fix

`r6/scripts/RadioportDropoutFix/RadioportSingleActivate.reds` wraps `OnAction`. A release that follows a press
which already activated is dropped. A release with no activating press before it (the press landed during the
popup's fade-in) still activates.

## Silent at low master volume

### The fault

The game passes Wwise connection thresholds of `{-70, -40, -40, -40}` dB (direct, game-defined send,
user-defined send, reflections), and Wwise drops a send below its threshold. Every player radio sound, the
Radioport and the car radio, is heard only through a user-defined aux send. At low master volume that send
sits near -40 dB, so a small cut (the radio's own slider, the police scanner, gunfire) silences the radio
completely. Game 2.31 sets no connection thresholds.

### The fix

The plugin changes the user-defined send threshold to -96 dB, the voice threshold, before the sound engine
starts. It patches the read-only array only where exactly one copy exists, and leaves the game alone otherwise.

## Radioport Off (optional)

`optional/RadioportOff/r6/scripts/RadioportOff/RadioportOff.reds` keeps the Radioport off: `PocketRadio.TurnOn`
does nothing, its input passes on only during a metro ride, and the hold that opens the station list is refused
outside a metro ride. It ships as a separate file and needs only Redscript.

## Install

Download from [Nexus Mods](https://www.nexusmods.com/cyberpunk2077/mods/33838). Drop the archive into
your mod manager, or copy `red4ext\plugins\RadioportDropoutFix\` and `r6\scripts\RadioportDropoutFix\`
into the game folder. Radioport Off is a separate optional file on the same page. Requires [RED4ext](https://www.nexusmods.com/cyberpunk2077/mods/2380) and
[Redscript](https://www.nexusmods.com/cyberpunk2077/mods/1511).

## Build

`pwsh -File plugin\build.ps1` builds the DLL against the RED4ext.SDK commit pinned in `plugin/builds.json`,
exported from a RED4ext.SDK clone (`-SdkRepo`). One DLL serves every game build: it uses no SDK engine types
and checks the bytes it patches.
Releases ship the committed DLL, so see `RELEASING.md` before tagging.

## License

Licensed under the [MIT License](LICENSE). Use, change and share this mod and its source,
including in your own mods. Keep the licence notice with any copy.

## Disclaimer

This mod was developed with the assistance of an LLM. All in-game testing and code validation was
performed by a human. No rogue AIs were permitted through the Blackwall.
