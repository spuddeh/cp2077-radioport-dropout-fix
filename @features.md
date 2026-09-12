# Features - Radioport Dropout Fix

## Implemented
- The radio manager measures a station by its nearest receiver, the Radioport included. A car
  tuned to the player's station no longer displaces the Radioport as the measuring point, so the
  station keeps its place among the four that play.
- The patch is verified byte for byte before it is written; on any other game build the plugin logs
  and does nothing.

## Awaiting in-game verification
- At a spot with four world radios within 80 m, Body Heat on the Radioport keeps playing while a
  car tuned to Body Heat passes at 100 m. The station probe's list dump keeps
  `radio_station_05_pop` in the manager's list throughout.

## Planned
- Nexus release once the run passes.
