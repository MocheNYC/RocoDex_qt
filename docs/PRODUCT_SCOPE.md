# Product Scope

## Goal

Build a polished Qt edition of RocoDex that works as a local offline pet dex on Windows first, then Android. It should feel like a native, efficient, data-heavy tool rather than a marketing page or game UI.

## Removed From Qt Edition

These features are intentionally out of scope and should not appear in UI, data models, tests, or docs except as migration notes:

- Egg groups.
- Breeding state.
- Egg measurements.
- Shiny seed routes from rocomegg.
- rocomegg skill learner statistics.
- RocoFight.
- PVP teams.
- Battle simulator.
- MaskPPO training, action masks, replay, or engine bridge.

The Qt data export step strips the `supplement` object from `dex-bundle.json`. The app must ignore it if an old bundle still contains it.

## Preserved Features

- Pet list and detail view.
- Offline JSON and offline pet images.
- Search by pet id, Chinese name, initials, English name, distribution, trait, task, skill, and attribute name.
- Filter by attribute, attribute match mode, stage, and distribution availability.
- Sort by id, name, total base stats, speed, and health.
- Detail blocks for portrait, attributes, stage, role, stats, evolution chain, trait, body size, distribution, introduction, skills, and data source.
- Favorites and recent pets.
- Remote JSON update from GitHub raw.
- Reset to embedded data.

## Added Feature

Add first-class attribute matchup computation for the selected pet:

- Offensive coverage: which target attributes this pet's own attributes hit for 2x.
- Defensive weaknesses: incoming attack attributes with multiplier above 1.
- Defensive resistances: incoming attack attributes with multiplier below 1.
- Neutral matchups: incoming attack attributes with multiplier exactly 1.
- Dual-attribute stacking: a two-attribute pet can have 4x, 2x, 1x, 0.5x, or 0.25x incoming multipliers.
- UI explanation for each multiplier must be inspectable without exposing implementation wording in main layout.

## Platform Priority

1. Windows desktop.
2. Android portrait and tablet.
3. Linux desktop if needed.

macOS and iOS are not primary targets unless a signing and packaging plan is added later.
