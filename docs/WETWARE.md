# Wetware — growing and wiring the Physarum polycephalum culture

This is the core of the "bio" in Smart Bio-Bathroom: a living slime mold
(*Physarum polycephalum*) plasmodium acts as the adaptive element that
reinforcement-learns from the "No" button and correct-recognition pulses.
It's a real organism with real care requirements — budget a few days before
the electronics side is useful.

## What you need

| Item | Where to get it | Notes |
|---|---|---|
| *Physarum polycephalum* sclerotium (dried plasmodium) | Biological supply houses (e.g. Carolina Biological Supply), or a hobbyist slime-mold community | This is the "seed" — it rehydrates into an active plasmodium |
| Agar | Pharmacy / chemistry supply / some grocery stores | 2-3g per 100ml water |
| Rolled oats (NOT instant/quick-cook) | Any grocery store | Its food source |
| Petri dish or shallow plastic container | Any hardware/lab supply store | With a lid, to control humidity |
| Silver wire or Ag/AgCl electrodes | Electronics/lab supply (AliExpress etc.) | Contact points for stimulation |
| Distilled water | Grocery/pharmacy | Tap water's chlorine and minerals can harm the culture |

## Growing the culture

1. Make the agar substrate: dissolve 2-3g agar per 100ml distilled water,
   bring to a near-boil (fully dissolved), pour a ~3-5mm layer into the
   Petri dish/container, let it cool and set (~30 min).
2. Place a few rolled oats on the agar surface as food.
3. Rehydrate the sclerotium on a damp paper towel for a few hours, then
   transfer it onto the agar near (not on) the oats.
4. Cover the container (lid on, slightly ajar for airflow) and keep it in
   indirect light, 20-25°C (68-77°F). *Physarum* actively avoids bright
   light — this matters later, since your RGB strip and bathroom lighting
   are near it.
5. Over 1-3 days, the plasmodium should spread across the agar, visibly
   fanning out toward the oats as bright yellow veins.
6. Feed it a few fresh oat flakes every 1-2 days; keep the agar moist (mist
   with distilled water if it starts drying out).

**This organism is genuinely alive and needs upkeep** — unlike the rest of
this repo, you can't `git reset` a dead culture. Expect to re-grow it
periodically (weeks to a couple months lifespan depending on conditions).

## Wiring the electrodes

The plasmodium's electrical response is millivolt-scale and needs the same
instrumentation amplifier discussed in `docs/HARDWARE.md` if you intend to
*read* signals from it. For the *stimulation* side (the punish/reward
pulses this firmware sends), the ESP32 GPIO is driving the tissue directly,
so current-limiting matters:

```
GPIO32 --[ 10kΩ series resistor ]-- Electrode 1 (in/near the plasmodium)
GND    ------------------------------ Electrode 2 (in/near the plasmodium)
```

- The series resistor caps the current the 3.3V GPIO can push into the
  tissue (roughly ≤0.33mA), which is the standard precaution for
  electrical bio-stimulation at this scale — GPIO pins are not
  current-limited on their own.
- Use silver wire or Ag/AgCl electrodes pressed lightly into the agar near
  (not through) the plasmodium body; don't let bare copper or steel touch
  the culture — electrochemical corrosion products are toxic to it.
- `BioAdapter::punish()` fires a 200ms pulse, `BioAdapter::reward()` a
  50ms pulse (see `src/BioAdapter.cpp`) — these durations came from the
  original design spec and are a starting point, not a calibrated value;
  watch the culture's response (see Testing below) and adjust if it seems
  over- or under-stimulated.

## Testing the wetware connection

You're checking two things: that the electrode circuit is intact, and that
the plasmodium visibly responds to a pulse.

1. **Continuity check (before touching the culture):** with the ESP32
   powered off, use a multimeter across the two electrodes (through the
   10kΩ resistor) — you should read a consistent, dish-dependent resistance
   (typically tens of kΩ to a few MΩ through a healthy plasmodium; open
   circuit means an electrode has lost contact or the culture doesn't
   bridge them yet).
2. **Pulse test:** flash the firmware, open the serial monitor
   (`./scripts/monitor.sh`), and press the "No" button. You should see:
   ```
   BioAdapter: punish pulse ("No" button pressed)
   ```
   and, watching the culture under indirect light, a visible local
   contraction or a slight color/texture change near the electrode within
   a few seconds to a minute — plasmodial streaming visibly slows or
   reverses direction near an aversive stimulus. This is the "the wiring
   reaches the organism" check, not a claim about learning yet.
3. **Behavioral check (over days, not minutes):** repeatedly pair a
   specific gait pattern (have the same person step on the sensor) with a
   `reward()` pulse (a correct recognition) versus punishing an incorrect
   one, and watch whether the plasmodium's growth/electrical response
   pattern shifts to favor the rewarded stimulus over several days. This
   is the actual "adaptive filter" behavior the spec describes — it's a
   slow biological process, not something that changes on one button
   press.
4. If you have the instrumentation amplifier wired for *reading* the
   plasmodium (not just stimulating it), log its raw output to Serial for
   a baseline before wiring the punish/reward electrodes — you want a
   reference of what "normal" looks like before you start perturbing it.

## Honest caveat

This is a real, unconventional bio-electronics setup, not a validated
sensor. *Physarum*'s electrical responses to stimuli are well documented in
research literature, but "learns to associate a specific gait with a
reward" over the timescales and specificity implied by the original spec is
an experimental claim, not an established result — treat this module as
the hobbyist/research part of the build, separate from the piezo-based
gait-recognition (which is documented, deterministic and the part in
`src/core` that's actually unit-tested).
