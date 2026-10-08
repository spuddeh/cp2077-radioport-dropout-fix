# Features - Radioport Dropout Fix

## Implemented
- The radio manager measures a station by its nearest receiver, the Radioport included. A car
  tuned to the player's station no longer displaces the Radioport as the measuring point, so the
  station keeps its place among the four that play.
- The patch is verified byte for byte before it is written. RED4ext loads the plugin on any game
  build; where the function or the bytes differ, the plugin logs and does nothing.
- (1.1.0, unreleased) After any vehicle exit - driver, passenger or metro - the Radioport plays at the
  radio tier the game gives it on loading, clean and at full level, instead of the filtered tier the
  vehicle leaves behind. A seat switch inside the vehicle is left alone. Redscript.

## Verified in game
- 2026-09-12, Testing, at the spot with four world radios within 80 m: a car tuned to Body Heat,
  radio off, joined the station's listener list for 28 s and Body Heat stayed active with its voice
  handle, `radio_station_05_pop` staying in the manager's list at the Radioport's distance
  throughout. Heard: the station played the whole time. Before the fix the same event silenced it
  every time, five runs out of five.
- 2026-10-08, Testing, the radio tier fix: driver exit, sync-to-car off, the game's slide-across to a
  passenger exit, and a metro ride. The Radioport read tier 0 and its pre-drive level after each.

## Released
- 1.0.0 on Nexus (mod 33838) and GitHub, 2026-09-13. The runtime-independent build logged `patched:` on Testing and held against world radios and traffic.
