# Features - Radioport Dropout Fix

## Implemented
- The radio manager measures a station by its nearest receiver, the Radioport included. A car
  tuned to the player's station no longer displaces the Radioport as the measuring point, so the
  station keeps its place among the four that play.
- The patch is verified byte for byte before it is written; on any other game build the plugin logs
  and does nothing.

## Verified in game
- 2026-09-12, Testing, at the spot with four world radios within 80 m: a car tuned to Body Heat,
  radio off, joined the station's listener list for 28 s and Body Heat stayed active with its voice
  handle, `radio_station_05_pop` staying in the manager's list at the Radioport's distance
  throughout. Heard: the station played the whole time. Before the fix the same event silenced it
  every time, five runs out of five.

## Planned
- Nexus release once the run passes.
