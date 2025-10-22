# LEGO NXT PID Line Follower

A compact NXC PID line follower for the LEGO NXT brick. Reads two light sensors (left/right), computes a normalized lateral error, runs a PID controller (with derivative filtering and integral clamp), and differentially drives two motors.

---

## Hardware / Wiring

* **Left light sensor** → **S1**
* **Right light sensor** → **S2**
* **Left motor** → **Port B**
* **Right motor** → **Port C**

Sensors should face the ground, ~5–15 mm above it. Place them 2–4 cm apart (center-to-center).

---

## Features

* Two-sensor PID steering (proportional, integral, derivative)
* Derivative low-pass filtering
* Integral clamping (anti-windup)
* Output clamping and optional slew-rate limiting
* Safe defaults and tuning recommendations

---

## Quick start

1. Flash the NXC program to the NXT (BricxCC or your usual tool).
2. Place robot on the track, start on a straight section.
3. Run the program and tune gains while observing behavior.

---

## Key constants (where to change)

* `ERROR_SCALE` — normalizes raw sensor difference (default `100.0`)
* `base_speed` — base forward power (start `30–45`)
* `kp`, `ki`, `kd` — PID gains (tune as described below)
* `DERIV_TAU` — derivative smoothing time constant (e.g. `0.02` s)
* `INTEGRAL_LIMIT` — clamp for the integral term
* `LOOP_MS` / `update_time` — control loop timing (keep them matched)

---

## Tuning recipe

1. Set `ki = 0`, `kd = 0`.
2. Increase `kp` until the robot follows the line but slightly oscillates; back off 20–30%.
3. Add `kd` to damp oscillation.
4. Add a small `ki` only if persistent drift remains.
5. Tune at low `base_speed`, then increase speed once stable.

Suggested starting values:

```
base_speed = 40
kp = 1.2
kd = 0.04
ki = 0.00 (add later if needed)
ERROR_SCALE = 100.0
DERIV_TAU = 0.02
LOOP_MS = 10
INTEGRAL_LIMIT = 30
MAX_DELTA_PER_LOOP = 8
```

---

## Troubleshooting

* **Oscillation** → reduce `kp` or increase `kd`.
* **Drift to one side** → check sensor alignment or add a small `ki` after verifying sensors.
* **Jittery output** → increase `DERIV_TAU` or decrease `kd`.
* **Frequent saturation** → lower gains or reduce `base_speed`.
* **Lighting issues** → implement per-sensor white/black calibration and normalize readings.

---

## To‑Do / Improvements by Priority

### High Priority

* **Sensor calibration (white/black normalization)** → ensures stable values across lighting conditions.
* **Sensor smoothing / moving average** → reduces noise and jitter in PID input.
* **Speed adaptation in curves** → slow down slightly when error is large for smoother turning.

### Medium Priority

* **Clean anti-windup logic** → stop integrating when output is saturated.
* **Adaptive derivative scaling** → reduce `kd` effect when error is large (less jitter in sharp curves).
* **Dynamic error scaling** → increase steering response only when far from the line.

###  Optional / Advanced

* **Add 3rd (middle) sensor** → improves robustness and helps recover lost line.
* **Lost-line detection and recovery** → detect when both sensors see white/black and re-center.
* **Debug output (display/log)** → print `error`, `steering`, `integral` for easy tuning.
* **Encoder feedback** → use wheel encoders for cascaded inner speed control.

---

## Next steps / improvements

* Add sensor calibration (white/black normalization).
* Use a 3–5 sensor array with centroid calculation for better accuracy.
* Implement cascaded control using `nMotorEncoder[]` for inner velocity loops.
* Add intersection handling and lost-line recovery behaviors.
* Add a small calibration program and logging to help tuning.


