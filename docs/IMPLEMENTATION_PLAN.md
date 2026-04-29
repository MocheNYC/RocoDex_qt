# Implementation Plan

## Milestone 0: Skeleton And Contract

Deliverables:

- CMake Qt project.
- QML shell.
- C++ type definitions.
- Data exporter that strips unsupported fields.
- Documentation for architecture, UI, data, and workflow.

Acceptance:

- `G:\RocoDex_qt` opens in Qt Creator.
- Project configures even before final app logic is complete.
- No egg-group or RocoFight module exists.

## Milestone 1: Desktop MVP

Deliverables:

- Load exported `dex-bundle.json`.
- Parse pets, attributes, skills, snapshot.
- Show pet list.
- Select pet and show detail hero.
- Load local pet image with fallback.
- Basic search by id/name/initials.

Acceptance:

- Windows desktop app can browse all pets offline.
- Search result count updates immediately.

## Milestone 2: Full Dex Features

Deliverables:

- Attribute filter and any/all mode.
- Stage filter.
- Distribution-only toggle.
- Sort controls.
- Stats bars.
- Evolution chain.
- Trait/body/distribution/introduction sections.
- Skill groups.
- Favorites and recents.

Acceptance:

- Functional parity with current React app except removed egg/supplement features.

## Milestone 3: Attribute Matchup

Deliverables:

- `DexRuntime::offensiveCoverage`.
- `DexRuntime::defensiveProfile`.
- `DexRuntime::matchupTable`.
- Matchup panel UI.
- Unit tests for 4x, 2x, 1x, 0.5x, 0.25x.

Acceptance:

- Every selected pet shows weaknesses, resistances, and offensive coverage.
- Dual-attribute stacking is correct.

## Milestone 4: Data Update

Deliverables:

- Remote JSON fetch.
- Version/time/size comparison.
- Save updated JSON to app data.
- Reset to embedded data.
- Invalid-user-data fallback.

Acceptance:

- Bad remote JSON never overwrites working local data.
- User can recover to embedded data from UI.

## Milestone 5: Android

Deliverables:

- Responsive mobile list/detail states.
- Android back behavior.
- Touch target pass.
- APK packaging with JSON and images.

Acceptance:

- App is usable on 360 x 800 portrait.
- No visible overlap in list rows, filters, or detail hero.

## Milestone 6: Release

Deliverables:

- Windows packaging script.
- Android release signing notes.
- License/source attribution page.
- Versioned data export.

Acceptance:

- Release package can be reproduced from a clean checkout plus exported data.
