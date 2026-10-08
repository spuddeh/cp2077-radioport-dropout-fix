# Changelog - Radioport Dropout Fix

## [Unreleased - 1.1.0]

### Added
- `r6/scripts/RadioportDropoutFix/RadioportTier.reds` (module `RadioportDropoutFix`, 1.1.0). A
  `@wrapMethod(PlayerPuppet) OnUnmountingEvent` that, when the unmounted child `IsPlayer()`, runs the
  game's own `GameObject.AudioParameter(player, n"veh_radio_tier", 0.00, n"pocket_radio_emitter")`
  (`pocketRadio.swift:74`). The engine's vehicle code leaves `veh_radio_tier` on the player at 1 after
  any exit, and the Radioport mixer (801426841) plays tier 1 low- and high-passed and about 3.5 dB down
  until the next load. The event arrives after the engine's write, so the 0 is set inside it with no
  delay. Covers every seat, where vanilla's `PocketRadio.HandleVehicleUnmounted` hears only the driver's.
- An unmount carrying `mountData.mountEventOptions.silentUnmount` is skipped: a seat switch (the
  game's slide-across, `SwitchSeatsDecisions`) arrives as one while the player stays mounted, and a 0
  there would make the Radioport audible over the car at tier 2. A real exit carries no `mountData`.
- `release-manifest.json` ships `["red4ext","r6"]`. Redscript is a new requirement.

### Verified in game
Measurements: the wiki page `entities/radioport-dropout-fix`, section "The radio tier after a drive".
- Driver exit: tier 2 in the car, 1 as the exit starts, 0 as the unmount lands; the Radioport back to
  its pre-drive level on the same voice.
- Sync-to-car off; the game's slide-across from the driver's door against a wall (two silent unmounts
  skipped, tier held at 2, then 0 on the real passenger exit); a metro ride (boarding `Base` unmount and
  leaving `Passengers` unmount, neither silent).

## [1.0.0] - 2026-09-13

### Changed
- `Main.cpp` declares `RED4EXT_V1_RUNTIME_VERSION_INDEPENDENT` instead of
  `RED4EXT_V1_RUNTIME_VERSION_LATEST`. RED4ext refuses a plugin pinned to another runtime before its
  `Main` runs, so the hash resolve and byte check never got the chance to decide. The plugin now
  loads on any build, and the check is the only gate: resolved, prologue matches, site matches, or
  nothing is written.
- Version 1.0.0 in the file header and `RED4EXT_V1_SEMVER`. First Nexus release.

### Added
- `nexus_description.bbc`, `release-manifest.json`, `RELEASING.md`, `.github/workflows/release.yml`
  (byte-identical to `MyMods/_shared/release/release.yml`), `.gitattributes`.

## [0.1.0] - 2026-09-12

### Added
- The plugin. On load it resolves the radio manager's list-rebuild function by RED4ext hash
  (`905582677`, `0x9da83c` on 2.31), verifies the eight-byte prologue and the twelve bytes around
  the patch site, and replaces `mov r15b, al` at `+0x14d` with `xor r15b, r15b`. Every receiver is
  then an ordinary candidate to represent its station, so the Radioport, at the listener, always
  wins for the station it is on. Any mismatch logs and patches nothing. Verified in game the same day: a car tuned to the
  player's station passed and the station stayed active, list entry and voice handle kept.
