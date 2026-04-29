# Attribute Matchup

## Input

Each pet has one or more attributes:

```json
"attributes": ["fire", "wing"]
```

Each attribute meta has offense and defense tables:

```json
"offense": {
  "0.5": ["water"],
  "2.0": ["grass"]
},
"defense": {
  "0.5": ["fire"],
  "2.0": ["water"]
}
```

## Offensive Coverage

For a selected pet, offensive coverage is the union of all target attributes hit for 2x by any of the pet's own attributes.

Example:

```text
fire offense 2x: grass, bug
wing offense 2x: bug, fighting
coverage: grass, bug, fighting
```

Duplicates are removed and labels are sorted by Chinese locale.

## Defensive Profile

For every possible incoming attack attribute, multiply its effect against each defensive attribute the pet owns.

```text
current multiplier = 1
for each defensive attribute on pet:
  if defensive meta says incoming attribute is 2x:
    current *= 2
  else if defensive meta says incoming attribute is 0.5x:
    current *= 0.5
```

Classification:

```text
multiplier > 1: weak
multiplier = 1: neutral
multiplier < 1: resistant
```

Dual-attribute results can naturally become:

```text
4
2
1
0.5
0.25
```

## UI Requirements

The detail page must show:

- Weaknesses first, sorted from highest multiplier to lowest.
- Resistances second, sorted from lowest multiplier to highest.
- Offensive coverage third, sorted by label.
- Optional neutral list behind an expand control.

The main panel should use compact chips:

```text
水 x4
石 x2
火 x0.25
草 x0.5
```

Do not show a long prose explanation in the primary UI. Use a tooltip, popover, or help affordance for the formula.

## Test Cases

Minimum tests:

- Single-attribute pet with one 2x weakness.
- Single-attribute pet with one 0.5x resistance.
- Dual-attribute pet where two 2x entries stack to 4x.
- Dual-attribute pet where 2x and 0.5x cancel to 1x.
- Dual-attribute pet where two 0.5x entries stack to 0.25x.
- Offensive coverage removes duplicates.
