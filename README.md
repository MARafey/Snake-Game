# Snake Game

This code is a two-player Snake Game using the SFML library. The main components of the game are the Snake, Food, and Grid. The key features of this code include:

## Snake class:

* init(bool): Initializes the snake with a starting position and direction.
* update(float): Updates the snake's position based on its speed and time elapsed.
* draw(RenderWindow&): Draws the snake on the window.
* grow(): Increases the size of the snake by adding a segment.
* checkCollision(): Checks if the snake has collided with itself or the boundaries.
* getHeadPosition(): Returns the position of the snake's head.
* setDirection(int, int): Sets the direction of the snake's movement.

## Food class:

* consumed(Snake&): Checks if a snake has consumed the food and triggers growth.
* update(Snake&): Updates the food's position when consumed by a snake.
* draw(RenderWindow&): Draws the food on the window.
* spawn(): Generates a random position for the food.
* Game struct: Holds pointers to the game's components (window, snakes, and foods).

###
_'gameLogic(void*)'_: Main game loop that updates the game state.

_'main()'_: Initializes the game and manages the game loop, event handling, drawing, and timers.

### Additional features:

* pthread: Used for creating a separate thread for the game logic.
* Grid: Visual representation of the game's play area.
* Countdown timer: Displays the remaining time for the game.
* Score display: Shows the score of each snake.
* Background music and image: Adds a background music loop and image to the game window


[Screencast from 03-04-2023 18:12:40.webm](https://user-images.githubusercontent.com/115175167/229520385-bee0eeb4-c1a9-45eb-b28e-0c8e54899cac.webm)


-> Thank you I hope you enjoy

_>P1 is single player 
_>P2 is 2 player
