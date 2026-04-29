# Architecture

## Top-Level Choice

Use Qt 6 + C++ + QML.

```text
C++: data loading, validation, filtering, sorting, matchup computation, persistence, network update
QML: visual layout, navigation, interactions, responsive states, animation
```

This keeps the business logic testable and avoids burying rules in UI components.

## Runtime Modules

```text
DexRepository
  Loads embedded or user-updated dex-bundle.json.
  Parses JSON into typed C++ structures.
  Ignores unsupported supplement data.
  Provides fallback to embedded data when user data is invalid.

DexRuntime
  Builds maps:
    petMap
    petNameMap
    skillMap
    attributeMap
  Calculates:
    maxStats
    evolution chains
    role label
    offensive coverage
    defensive profile
    full attribute matchup table

PetListModel
  QAbstractListModel for QML ListView.
  Owns current filter/sort state.
  Emits stable role data to QML.

FavoritesStore
  Stores favorite and recent pet keys through QSettings.

DataUpdateService
  Fetches remote dex-bundle.json through QNetworkAccessManager.
  Compares updatePackageVersion, importedAt, and data-size score.
  Saves user-updated data under QStandardPaths::AppDataLocation.
```

## Data Flow

```text
resources/data/dex-bundle.json
        |
        v
DexRepository ---- user data override in AppDataLocation
        |
        v
DexRuntime
        |
        +---- PetListModel ---- QML ListView
        |
        +---- selected pet detail ---- QML detail page
        |
        +---- matchup profile ---- QML matchup panel
```

## QML Structure

```text
Main.qml
  ApplicationWindow
  adaptive shell
  global navigation state

DexPage.qml
  desktop: data manager + filter rail + pet list + detail
  quick access: favorites + recent pets

PetDetailPage.qml
  hero block
  stat panels
  evolution chain
  matchup panel
  skill groups

components/
  AttributeBadge.qml
  DataManager.qml
  FilterRail.qml
  QuickPetStrip.qml
  PetListItem.qml
  StatBars.qml
  SkillCard.qml
  MatchupPanel.qml
```

## What Must Stay Out Of QML

- JSON parsing.
- Filter matching.
- Sorting.
- Evolution chain traversal.
- Attribute matchup multiplication.
- Data update comparison.
- Persistent key migration.

QML should receive ready-to-render values and invoke small commands.

## Generated Data Policy

The Qt project consumes an exported bundle generated from the current React project. It does not own the BWIKI import pipeline.

```text
G:\rock_world\public\data\dex-bundle.json
G:\rock_world\public\offline\pets\
        |
        v
scripts/export_qt_data.mjs
        |
        v
G:\RocoDex_qt\resources\data\dex-bundle.json
G:\RocoDex_qt\resources\offline\pets\
```

The exporter removes unsupported supplement fields so the Qt app stays clean.
