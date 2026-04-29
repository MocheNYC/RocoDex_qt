# Data Contract

## Source Bundle

The source remains the current RocoDex bundle:

```text
G:\rock_world\public\data\dex-bundle.json
```

The Qt app uses an exported subset:

```text
G:\RocoDex_qt\resources\data\dex-bundle.json
```

## Accepted Top-Level Shape

```ts
type QtDexDataBundle = {
  snapshot: DataSnapshot
  pets: Pet[]
  attributes: AttributeMeta[]
  skills: SkillInfo[]
}
```

The Qt edition does not use:

```text
supplement
eggGroups
eggMeasurements
canBreed
hasShinySeedRoute
rocomeggSkillCount
matchedSupplement counts
```

If a user imports an old full RocoDex bundle containing `supplement`, the loader should ignore that field rather than fail.

## Required Pet Fields

```text
key
id
nameZh
nameEn
image
attributes
formName
petType
hasShiny
introductionZh
traitName
traitDescription
height
weight
distributionZh
evolution
stage
stats
dexTasks
taskSkillStones
skills
bloodlineSkills
learnableSkillStones
updateVersion
pageUrl
sourceKey
```

## Required Attribute Fields

```text
key
nameZh
nameEn
color
textColor
descriptionZh
offense.0.5
offense.2.0
defense.0.5
defense.2.0
```

## Required Skill Fields

```text
name
attribute
category
energy
power
effect
description
version
pageUrl
```

## Image Path Rule

The exported bundle should keep existing image paths:

```text
/offline/pets/pet-0001-001.png
```

The Qt repository resolves them to:

```text
resources/offline/pets/pet-0001-001.png
```

In packaged builds, this can later be moved into Qt resources or deployed as ordinary app data. The UI must never hard-code filesystem paths.
