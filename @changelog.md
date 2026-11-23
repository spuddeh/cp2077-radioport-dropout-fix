# Changelog - Radioport Dropout Fix

## [1.3.0] - 2026-11-23

### Fixed
- `plugin/src/Main.cpp` (T167): `PatchSendThreshold`. `audio::SoundEngine::Initialize` passes
  `AK::SoundEngine::SetConnectionVolumeThresholds` the read-only array `{-70, -40, -40, -40}` dB (2.5
  `0x1437bc6a0`, its only reader). Every player radio sound is heard only through a user-defined aux send, so at
  low master volume a small cut pushed that send under -40 dB and Wwise dropped it: total silence. Entry 2 (user-
  defined send) becomes -96 dB, the voice threshold, at plugin load, before the sound engine starts. Patched only when
  exactly one copy of the array is in `.rdata`; an already-patched copy is reported. Game 2.31 has no setter and no
  copy, so nothing changes there.
- `r6/scripts/RadioportDropoutFix/RadioportSingleActivate.reds` (T163).
  `BaseModalListPopupGameController.OnAction` calls `Activate()` on `proceed` `BUTTON_PRESSED` and again on
  `BUTTON_RELEASED`, so the station selection UI sent `SendRadioEvent` twice per pick. `@wrapMethod OnAction`
  records (`@addField m_radioportProceedActivated`) that a press activated, and drops the release that follows it
  without calling the wrapped method. A press that lands during the fade animation is not recorded, so its release
  still activates as before. On the base class, so `VehiclesManagerPopupGameController` (vehicle call-in) gets it
  too; its own `m_selectAnimProxy` guard already ignored the second call.

### Added
- `optional/RadioportOff/r6/scripts/RadioportOff/RadioportOff.reds` (T164): a separate optional file that does not
  need the main mod. `@wrapMethod PocketRadio.HandleInputAction` passes input on only while `m_isInMetro`;
  `@wrapMethod PocketRadio.TurnOn` does nothing (the only place `m_isOn` becomes true);
  `@wrapMethod PocketRadioWheelDecisions.EnterCondition` returns false unless V has `MetroRide`. Not yet in
  `release-manifest.json`.

### Verified in game
- 2026-11-23, Testing, send threshold: the log shows both patches; Radio Station Probe meter at master 5, the Radioport
  at 100/95/90 read -23.8/-24.3/-29.9 dB and the car radio (Archer, Dark Star) -22.8/-27.5/-29.8 dB. Without the patch
  both read -120 at 95 and 90. Sound engine cost of -96 against -40 on a city street: within window-to-window noise
  (mean per audio frame 694 against 721 microseconds).
- 2026-11-23, Testing, Radioport Off: pad D-pad Left and keyboard Z taps, a full pad hold (`BUTTON_HOLD_COMPLETE`)
  and a keyboard hold left the Radioport off and opened no station list; leaving a car with its radio on and Sync
  to Vehicle Radio on left it off. The same exit with the file unloaded turned the Radioport on. Metro ride not tested.
- 2026-11-23, Testing, observing `VehicleRadioPopupGameController.Activate` and `QuickSlotsManager.SendRadioEvent`.
  Without the file (hot reload): one keyboard press gave 2 `Activate` and 2 `SendRadioEvent`. With it: keyboard
  E and Enter, pad A and a mouse click each gave 1 and 1, on foot and in a car. Vehicle call-in list: one
  `Activate`, one `SummonVehicle`.

## [1.2.1] - 2026-11-02

### Changed
- Built with the portfolio's RED4ext plugin standard (`plugin/build.ps1`, `red4ext-plugin.cmake`, `builds.json`): one
  build, game `any`, RED4ext.SDK pinned to `5246e659`; PDB kept in `symbols/1.2.1/any/`, no incremental linking. Still
  runtime-independent: the plugin uses no SDK engine types.

### Verified
- Game 2.5 needs every fix: the rebuild loop still lets any untagged receiver replace the Radioport's with no distance
  test (patch site at the same `+0x14d`, `patched` on Testing); `vehicleRadioPopup.swift` is unchanged; with the mod
  off the Radioport's `veh_radio_tier` read 1 for the 24 s on foot after leaving a car. The 1.2.0 DLL patched on both
  2.31 (Gameplay) and 2.5 (Testing).

## [1.2.0] - 2026-10-17

### Fixed
- `r6/scripts/RadioportDropoutFix/RadioportTrackName.reds` (T115). `VehicleRadioPopupGameController`
  updates its track name while open only from the native `VehicleRadioSongChanged`, which the engine
  queues from `VehicleAudioRadioController::Update` (a per-tick track-name compare) and so only for a
  car. On foot it kept the old name until a station switch. `@wrapMethod OnPlayerAttach` starts a
  `DelaySystem.DelayCallback` loop (`RadioportTrackNameCallback`, 0.5 s, not affected by the popup's
  time dilation) when there is no `m_playerVehicle`; each call compares `GetRadioReceiverTrackName()`
  with the last name and calls `SetTrackName` on a change while `IsRadioReceiverActive()`.
  `@wrapMethod OnClose` cancels it. In a car nothing runs.

### Verified in game
- 2026-10-17, Testing, on foot, popup open: the callback ran every 0.51 s real time under the popup's
  dilation; the track went `00016DF6` -> `00016DF9` and the shown name followed. Before the fix, the
  same test showed 0 song events on foot and a stale name; in a car the event arrived with the change.

## [1.1.0] - 2026-10-08

### Added
- `r6/scripts/RadioportDropoutFix/RadioportTier.reds` (module `RadioportDropoutFix`, 1.1.0). A
  `@wrapMethod(PlayerPuppet) OnUnmountingEvent` that, when the unmounted child `IsPlayer()`, runs the
  game's own `GameObject.AudioParameter(player, n"veh_radio_tier", 0.00, n"pocket_radio_emitter")`
  (`pocketRadio.swift:74`). The engine's vehicle code leaves `veh_radio_tier` on the player at 1 after
  a driver's exit from a car (other seats and the metro not measured without the fix), and the Radioport mixer (801426841) plays tier 1 low- and high-passed and about 3.5 dB down
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
