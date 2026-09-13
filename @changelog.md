# Changelog - Radioport Dropout Fix

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
