# UI / UX Spec

## Design Direction

RocoDex Qt should feel like a polished, information-dense native dex. It is not a landing page and not a game simulator.

Visual principles:

- Quiet tool surface.
- Dense but readable layout.
- Strong scanning hierarchy.
- Small-radius cards, no nested cards.
- Attribute color chips are the main color accents.
- Pet artwork should be visible and useful, not decorative background.

## Desktop Layout

```text
App header
  brand
  data counters
  update/reset controls

Main workspace
  left filter rail
  middle pet list
  right pet detail
```

Recommended desktop widths:

```text
filter rail: 280
list: 360-420
detail: flexible, min 520
```

## Mobile Layout

Use a two-state layout:

```text
List state:
  compact header
  search
  filter drawer
  pet list

Detail state:
  top toolbar
  pet detail flickable
  previous / next / favorite / home actions
```

Android back behavior:

```text
detail -> list
filtered/list scrolled -> home state
home state -> app exit
```

## Core Controls

- Search: `TextField` with search icon.
- Attribute filter: chip/toggle grid.
- Attribute mode: segmented control, `任一` / `全部`.
- Stage filter: segmented or compact combo.
- Distribution filter: checkbox/toggle.
- Sort: menu button or combo box.
- Detail navigation: icon buttons.
- Skill groups: expandable panels.
- Matchup panel: chip groups with multiplier badges.

## Detail Page Sections

Order:

1. Hero: portrait, id, name, English name, attributes, stage, role.
2. Stats.
3. Attribute matchup.
4. Evolution chain.
5. Trait.
6. Body size.
7. Distribution.
8. Introduction.
9. Skills.
10. Data source.

The matchup section is intentionally promoted above evolution because it is a new Qt-edition value-add feature.

## Frontend Quality Bar

Before considering the UI complete:

- No text overlaps at 360 px width.
- Pet list row height is stable regardless of name length.
- Large detail pages scroll smoothly.
- Image fallback is clean when a pet image is missing.
- Keyboard navigation works on desktop.
- Touch targets are at least 40 px on mobile.
- The current selected pet is visually obvious.
- Loading, empty, failed-update, and invalid-data states are implemented.

## Theme Tokens

```text
surface: #f7f8fb
panel: #ffffff
panelMuted: #f1f4f8
textStrong: #18202a
text: #334155
textMuted: #697386
border: #d8dee8
accent: #2563eb
danger: #c2410c
success: #15803d
radius: 8
```

Attribute chips use colors from `AttributeMeta`.
