# MatrixProjectArduino

## Task :

Create an engaging game using an 8x8 LED Matrix. The game should have a menu with Start Game, Highscore, Settings, About, and How to Play options. Implement features like an intro message, high score tracking with EEPROM storage, adjustable LCD and matrix brightness, and sound settings. Gameplay must display relevant information like lives, level, and score. The game ending should show a congratulatory message and relevant game information. The project requires integrating components such as an LCD, joystick, and buzzer, focusing on intuitive gameplay and fun.

## Components

- Arduino Uno Board
- Joystick
- 8x8 LED Matrix
- MAX7219
- Buzzer
- Resistors and capacitors as needed
- Breadboard and connecting wires

## Pictures:

![poza_m_1](https://github.com/VladRo26/MatrixProjectArduino/assets/100710098/50ba8fcb-456e-487a-9a99-dd5c324ee12c=200x200)
![poza_m_3](https://github.com/VladRo26/MatrixProjectArduino/assets/100710098/f0224cee-0bb3-424f-932e-116b733453b3=200x200)
![poza_m_2](https://github.com/VladRo26/MatrixProjectArduino/assets/100710098/8c3e9468-2b16-40a1-af02-0ba7acb4e045=200x200)

## Video:

https://youtu.be/Y1YXCt6dkRg

## Code:
The code is uploaded with the file name: matrix_project.ino


## Game

### About the Game

For my project, I selected "Turis Game," a reinterpretation of the iconic Bomberman game. The game's original name eludes me, but its resemblance to Bomberman is striking. This variant, in my view, offers a more enjoyable experience than the classic version, although this is admittedly a subjective assessment. My initial plan was to recreate the original Bomberman, but I was drawn to the simplicity and engaging logic of this version. It struck me as an excellent fit for the project, blending straightforward gameplay with a personal touch of fun.


### Game description

In "Turis Game," players navigate through levels where walls are randomly generated in a selected portion of the map, adjustable in the settings. The player character is also randomly spawned on the map. The primary objective is to destroy these walls to complete a level. With each level progression, the number of bombs available to the player decreases, adding to the game's challenge. A level is successfully completed when all the walls are destroyed. 

### How to play

- Navigating the Player: Use the joystick to move the player around the game map
- Planting Bombs: Press the joystick button to plant a bomb, which is key for destroying walls
Menu Navigation:
- Move through menu options using the joystick.
- Select an option by pressing the joystick button
To go back or exit a menu, use the joystick button


