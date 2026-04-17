# 🔐 Secure Keypad System (Embedded Project)

## 📌 Overview

This project implements a **password-based locking system** using an embedded platform.
The system is designed using a **Mealy State Machine** and integrates **hardware interfacing**, **external interrupts**, and **modular drivers**.

The system is fully **simulated using Proteus** to validate both hardware behavior and system logic.

---

## 🧠 System Features

* 4x4 Keypad for password input
* 7-Segment display for failure count
* LEDs for:

  * Progress indication
  * Success (Unlocked)
  * Alarm state
* External interrupts (EXTI):

  * Emergency Reset
  * Door Bell

---

## 🧩 System Architecture

The system is divided into layers:

```text
Drivers Layer (GPIO, RCC, Keypad, SevenSeg, EXTI)
        ↓
Application Layer (app.c)
        ↓
State Machine (state_machine.c)
```

---

## 🧪 Simulation (Proteus)

The system is implemented and tested using **Proteus simulation environment**.

### Simulation Setup:

* Microcontroller (STM32 or equivalent)
* 4x4 Keypad connected via GPIO
* External buttons:

  * Emergency Reset (EXTI)
  * Door Bell (EXTI)
* 7-Segment Display for failure count
* LEDs for:

  * Progress indication
  * Success state
  * Alarm state
  * Door Bell indicator

### Purpose:

* Validate hardware interfacing
* Test interrupt behavior
* Verify state machine transitions
* Ensure correct system response

---

## ⚙️ Hardware Components

### 🔢 Keypad (GPIO)

* Used for password input
* Implemented using polling

---

### 🚨 External Interrupts (EXTI)

#### 1. Emergency Reset

* Configured as external interrupt
* Used to recover the system from ALARM state

Behavior:

* ISR sets:

```c
reset_flag = 1;
```

* State machine handles:

  * Turn OFF alarm LED
  * Clear progress LEDs
  * Reset failure counter
  * Clear 7-segment display
  * Clear input buffer
    → Transition to STATE_LOCKED

---

#### 2. Door Bell

* Configured as external interrupt

Behavior:

* ISR sets:

```c
doorbell_flag = 1;
```

* State machine:

  * Generates pulse (LED / buzzer)

IMPORTANT:

* Does NOT affect system state
* Does NOT interrupt password process

---

## 🧠 Interrupt Design

* ISR contains NO logic
* Only sets flags
* All behavior handled inside state machine

---

## ⚡ Priority Concept

* Emergency Reset is treated as **highest priority event**
* Door Bell is treated as **lower priority event**

Behavior:

* If Door Bell interrupt is running and Emergency Reset occurs
  → Emergency Reset is handled first

* After finishing
  → system resumes Door Bell execution

* If no higher-priority interrupt occurs
  → interrupts execute normally according to system logic

---

## 🔁 State Machine Design (Mealy Model)

### States:

1. STATE_LOCKED (Initial)
2. STATE_UNLOCKED
3. STATE_ALARM

---

### 🔒 STATE_LOCKED

#### Valid Input

* Update progress LEDs
* If complete & correct → STATE_UNLOCKED

#### Invalid Input

* Clear progress

* Increment failure counter

* Update 7-segment

* If failures < 3 → remain

* If failures == 3 → STATE_ALARM

#### Door Bell Event

* Pulse indicator
* Remain in LOCKED

---

### 🔓 STATE_UNLOCKED

#### Lock Command

* Turn OFF success LED
* Clear progress
  → STATE_LOCKED

#### Keypad Input

* Ignored

---

### 🚨 STATE_ALARM

* Triggered after 3 failures
* No timer-based reset

#### Emergency Reset Event

* Clear alarm
* Reset system بالكامل
  → STATE_LOCKED

#### Keypad Input

* Ignored

---

## 📁 Project Structure

```text
/drivers
    gpio.c / gpio.h
    rcc.c / rcc.h
    keypad.c / keypad.h
    sevenseg.c / sevenseg.h
    exti.c / exti.h

/app
    app.c / app.h

/state_machine
    state_machine.c / state_machine.h

main.c
```

---

## 🔧 Key Design Decisions

* Mealy State Machine for control logic
* Interrupt-driven external events
* Separation between hardware and logic
* No logic inside ISR


