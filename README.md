# Paketdienst-Inator
Line Following robot 


LEGO NXT PID Line Follower
What it is
Small NXC program for a two-sensor (left/right) PID line follower. Reads two light sensors, computes error = (left-right)/ERROR_SCALE, runs PID (with derivative filtering and integral clamp), and maps steering to the two drive motors (differential drive).
Wiring
Left light sensor → S1
Right light sensor → S2
Left motor → B
Right motor → C
Core behavior
Normalize sensor difference → error (≈ −1..+1)
integral += error * dt (clamped)
derivative = filtered (error - lastError)/dt
steering = Kp*error + Ki*integral + Kd*derivative
left = base + steering*POWER_RANGE, right = base - steering*POWER_RANGE
Clamp motor outputs and apply with OnFwd/OnRev. Simple anti-windup undoes last integral step when outputs saturate. Optional slew limiter to smooth commands.
Where to change things
base_speed — start low (30–45)
kp, ki, kd — tune (see below)
ERROR_SCALE — matches sensor range (default 100)
DERIV_TAU — derivative smoothing (e.g. 0.02 s)
INTEGRAL_LIMIT — bounds integral term
Quick tuning recipe
Set ki = 0, kd = 0.
Increase kp until slightly oscillatory, then back off 20–30%.
Add kd to damp oscillation.
Add small ki only if there’s steady drift.
Use low base_speed while tuning.
Common gotchas
Lighting changes → normalize sensors (calibrate white/black).
DT must match Sleep(LOOP_MS) — keep loop timing stable.
If motors hit clamp often, reduce gains or lower base_speed.
Suggested starting values
base_speed = 40
kp = 1.2
kd = 0.04
ki = 0.00 (add later if needed)
ERROR_SCALE = 100.0
DERIV_TAU = 0.02
LOOP_MS = 10
INTEGRAL_LIMIT = 30
Next steps (easy wins)
Add sensor calibration (white/black normalization).
Print error/steering for debugging.
Use encoders later for cascaded outer (steering) + inner (wheel velocity) control.
Want me to convert this into a downloadable README.md file or produce a small calibration helper NXC program next?