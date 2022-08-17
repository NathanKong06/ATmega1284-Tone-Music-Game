# ATmega1284-Tone-Music-Game
Tone music game played on a Nokia screen with a joystick, controlled by Atmega1284. 

# Description
The project consists of two different parts, each selectable from the first menu. The introductory screen prompts the user to either move the joystick upwards or downwards, one for each part. The program will stay on the screen until the user makes an input using the joystick. Left and right movement on the joystick are ignored. Moving the joystick upwards will select the music game. Two tones are played in succession and the user has to determine which tone has the higher pitch. Moving the joystick upwards indicates that the user believes the second pitch is higher than the first pitch. Moving the joystick downwards indicates the opposite, being that the
first pitch is higher than the second pitch. Left and right movement on the joystick is ignored. There is a button which the user can press to reset the program back to the introductory screen. Every time the user makes an input on the joystick, their current score will be displayed on the Nokia screen. Once the user guesses incorrectly, their score will be displayed and the program will go back to the introductory screen. The objective of the game is to reach a score of 21, which wins the game. The second part of the program plays when the user moves the joystick downwards in the introductory screen. A musical melody plays immediately when the music part of the program is selected. After the initial music melody is finished playing, the user can use the mic as an input to play another musical tone. The mic is set to play another melody at a certain loudness level.

# User Guide

## Rules:
Using the joystick, select which tone has the higher pitch. Hitting the reset button at any time will reset the game and set the score to 0, meaning the user has lost the game. The goal of the game is to reach 21 points by guessing 21 times correctly successively. There is no time limit on any aspect of the game.

## Controls:
The joystick and the button are the only controls of the game. The button only serves as a reset button while the joystick is the main input.

## Special Considerations:
The microphone takes multiple user inputs before activating, probably due to not having an amplifier or possibly some mechanical fault. Tapping on the microphone is by far the best way to give input to the microphone, as it is the loudest to it. Claps and snaps are also valid but are less reliable due to probable reasons previously mentioned. The joystick is also not anchored onto the breadboard due to the bottom of the joystick not having long enough pins to fit into the breadboard holes.

## Inputs
- Joystick
- Electret Microphone
- Button

## Outputs
- Nokia 5110 Screen
- Speaker

# Component Visualization
![image](https://user-images.githubusercontent.com/81611522/185221423-827905ed-afa6-4e19-a72e-23498b26db35.png)

# Other Files Used
- [nokia5110.c](https://github.com/LittleBuster/avr-nokia5110)
- [nokia5110.h](https://github.com/LittleBuster/avr-nokia5110)
- [nokia5110_chars.h](https://github.com/LittleBuster/avr-nokia5110)

