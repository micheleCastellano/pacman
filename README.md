# 👻 Pac-Man on LandTiger Board (LPC1768 Microcontroller) 🕹️

This project implements a version of the classic arcade game **Pac-Man** on the **LandTiger Board**, which utilizes the **NXP LPC1768** ARM Cortex-M3 microcontroller.

This implementation was developed as part of the "Architetture dei Sistemi Di Elaborazione" course requirements.

## 🚀 Overview

The game is implemented in C using the **Keil µVision** environment and is designed to run directly on the physical **LandTiger Board**.

It features core Pac-Man mechanics, including a maze, pills, scoring, lives, player controls via a joystick, and an auto-controlled ghost. It also incorporates sound effects and external communication via the CAN bus.

## ✨ Features

### Game Mechanics
* **Maze and Pills:** The game is displayed on the LandTiger's display.
    * A custom labyrinth is used, designed to fit exactly **240 Standard Pills**.
    * **6 Power Pills** are generated in random positions, replacing Standard Pills in their location.
    * The central "teleport" locations are implemented: Pac-Man exits one side and re-enters the other, maintaining direction.
* **Player Control:** Pac-Man is controlled by the **joystick**.
    * Pac-Man continues movement until a new direction is selected or a wall is encountered, in which case he stops.
* **Scoring and Lives:**
    * Each Standard Pill increases the Score by **10 points**.
    * Each Power Pill increases the Score by **50 points**.
    * A new life is earned every **1000 points** (starting with 1 life).
    * The current Score and Remaining Lives are displayed.
* **Game Flow:**
    * **Countdown Timer:** The game starts from **60 seconds** and counts down to 0.
    * **Pause Functionality:** Pressing **INT0** pauses the game and displays a “PAUSE” message; pressing it again resumes the game. The game starts in “PAUSE” mode.
    * **Winning:** If the player cleans up the labyrinth (by eating all the Pills), a **'Victory!'** screen is shown.
    * **Losing:** If the countdown timer expires before all pills have been eaten, a **“Game Over!”** screen is shown.
* **Auto-Controlled Ghost:** A single ghost is implemented that pursues Pac-Man, causing him to lose a life in case of contact.
    * The ghost has a unique AI and movement style. Blinky (Red) directly follows Pac-Man.
    * **Chase Mode:** The ghost actively tries to catch Pac-Man.
    * **Frightened Mode:** Activates when Pac-Man eats a "Power Pills". The ghost turns **blue** for **10 seconds**, runs away, and can be eaten by Pac-Man for an extra **100 points**.
    * **Respawn:** An eaten ghost respawns after **3 seconds** in its central spawning point.
    * **AI Implementation:** The AI can be implemented using a pathfinding algorithm like **A\*** to find the shortest path between the ghost's current position and its target.
* **Audio Output:** The **speaker** is configured to play sound effects and background music.
* **CAN Bus Communication:** The **CAN peripheral** is used to print the current **Score**, **Remaining Lives**, and the **Countdown timer** through the CAN bus.
