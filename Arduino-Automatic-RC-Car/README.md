# Arduino Automatic RC Car

An Arduino-based two-motor robot car with ultrasonic obstacle detection and a servo-mounted sensor. It supports automatic obstacle avoidance and manual control through the Serial Monitor.

## Features

- Automatic driving by default
- Ultrasonic distance measurement
- Stops when an obstacle is too close
- Reverses and scans left/right using a servo
- Chooses a clearer direction to turn
- Manual serial commands for forward, backward, left, right, and stop
- Safety stop if the ultrasonic sensor receives no echo

## Hardware

- Arduino board compatible with the sketch (for example, Arduino Uno)
- Two DC geared motors and robot-car chassis
- Dual H-bridge motor driver (L298N-style driver)
- HC-SR04-compatible ultrasonic sensor
- Servo motor (for example, SG90)
- Motor battery and jumper wires

> Check your motor driver's voltage/current requirements and connect all grounds together. Do not power motors directly from Arduino GPIO pins.

## Wiring

| Component signal | Arduino pin |
|---|---|
| Left motor forward input (LF) | D13 |
| Left motor backward input (LR) | D12 |
| Right motor forward input (RF) | D11 |
| Right motor backward input (RR) | D10 |
| Ultrasonic TRIG | A0 |
| Ultrasonic ECHO | A1 |
| Servo signal | D6 |

Connect the motor driver inputs to the listed pins. Connect the ultrasonic sensor's VCC and GND according to its specifications, and connect the servo's signal wire to D6. Use a suitable external supply for motors/servo when needed, with a common ground.

## Getting started

1. Open `auto_rc_car.ino` in the Arduino IDE.
2. Select the correct board and port.
3. Ensure the `Servo` library is available (it is included with the standard Arduino AVR core).
4. Upload the sketch.
5. Open Serial Monitor at **9600 baud**.

## Serial commands

Send one character in the Serial Monitor:

| Command | Action |
|---|---|
| `A` | Switch to automatic mode |
| `F` | Forward |
| `B` | Backward |
| `L` | Turn left |
| `R` | Turn right |
| `S` | Stop |

The sketch starts in automatic mode. Sending a manual movement command switches to manual mode.

## Adjustable settings

Edit these constants near the top of the sketch to tune behavior:

- `STOP_DISTANCE`: obstacle stop threshold in centimetres (default 10 cm)
- `CLEAR_DISTANCE`: minimum side clearance used when choosing a direction (default 20 cm)
- `BACK_TIME`: reverse duration (default 400 ms)
- `TURN_TIME`: turn duration (default 500 ms)
- `ANGLE_LEFT`, `ANGLE_RIGHT`, `ANGLE_CENTER`: servo scan positions

If left/right movement is reversed due to motor wiring or chassis orientation, adjust the motor connections or scan angles.

## How it works

1. Read the ultrasonic distance.
2. In automatic mode, drive forward while the path is clear.
3. If an obstacle is within `STOP_DISTANCE`, stop and reverse briefly.
4. Scan left and right with the servo-mounted ultrasonic sensor.
5. Turn toward the side with more clearance; if both sides are blocked, turn longer.
6. In manual mode, accept serial commands while preventing forward movement when an obstacle is too close.

## Notes and limitations

- This sketch uses **Serial Monitor commands**, not Bluetooth control by itself.
- `pulseIn()` timeout or a missing echo is treated as an unsafe reading; automatic mode stops.
- Turning duration depends on motor speed, battery level, wheel traction, and chassis geometry, so calibrate `TURN_TIME` on your car.
- Test with the wheels lifted first, then test slowly in an open area.

## Project status

Prototype / educational embedded-systems project.

## License

Add a license if you want others to reuse the project. For a simple open-source repository, MIT License is a common option; only add it if you agree to its terms.
