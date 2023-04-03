// #include <X11/Xlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#
using namespace std;
using namespace sf;

const int blockSize = 16;
const int width = 800 / blockSize;
const int height = 600 / blockSize;

using namespace sf;
using namespace std;
// g++ P1.cpp -o run -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lX11 -pthread

class Snake
{
public:
    bool flag;
    int score;
    bool isMoving;
    Snake(bool startAtOppositeEnd = false) : flag(startAtOppositeEnd), score(0)
    {
        init(flag);
    }

    void init(bool startAtOppositeEnd)
    {
        srand(time(NULL));
        segments.clear();
        if (startAtOppositeEnd)
        {
            segments.emplace_back(width - width / 3, height - height / 3);
        }
        else
        {
            segments.emplace_back(width / 2, height / 2);
        }
        direction = Vector2i(rand() % 500, rand() % 500);
        timeElapsed = 0.0f;
        speed = 0.15f;

        // Initialize isMoving to false
        isMoving = false;
    }

    void update(float dt)
    {
        timeElapsed += dt;
        if (timeElapsed >= speed)
        {
            move();
            timeElapsed = 0.0f;
        }
    }

    void draw(RenderWindow &window)
    {
        RectangleShape block({blockSize, blockSize});
        if (flag == 0)
            block.setFillColor(Color::Green);
        else
            block.setFillColor(Color::Blue);
        for (const auto &segment : segments)
        {
            block.setPosition(segment.x * blockSize, segment.y * blockSize);
            window.draw(block);
        }
    }

    void grow()
    {
        segments.push_back(segments.back());
    }

    bool checkCollision()
    {
        for (std::size_t i = 1; i < segments.size(); ++i)
        {
            if (segments[0] == segments[i])
            {
                return true;
            }
        }
        if ((segments[0].x < 0 || segments[0].x >= width) || (segments[0].y < 0 || segments[0].y >= height))
        {
            if (isMoving)
            {
                score -= 5;
            }
            init(flag);
        }
    }

    Vector2i getHeadPosition() const
    {
        return segments.front();
    }

    void setDirection(int dx, int dy)
    {
        Vector2i newDirection(dx, dy);
        if (newDirection != -direction)
        {
            direction = newDirection;
            // Set isMoving to true when the snake's direction changes
            isMoving = true;
        }
    }

private:
    void move()
    {
        for (std::size_t i = segments.size() - 1; i > 0; --i)
        {
            segments[i] = segments[i - 1];
        }

        segments[0] += direction;

        if (checkCollision())
        {
            init(flag);
        }
    }

    std::vector<Vector2i> segments;
    Vector2i direction;
    float timeElapsed;
    float speed;
};

class Food
{
public:
    Food()
    {
        srand(time(nullptr));
        spawn();
        shape.setRadius(blockSize / 2);
        shape.setFillColor(Color::Red);
    }
    bool consumed(Snake &snake)
    {
        if (snake.getHeadPosition() == position)
        {
            spawn();
            snake.grow();
            snake.score++; // increment the score of the snake that ate the food
            return true;
        }
        return false;
    }

    void update(Snake &snake)
    {
        if (snake.getHeadPosition() == position)
        {
            spawn();
            snake.grow();
            snake.score++; // increment the score of the snake that ate the food
        }
    }

    void draw(RenderWindow &window)
    {
        shape.setPosition(position.x * blockSize, position.y * blockSize);
        window.draw(shape);
    }

private:
    void spawn()
    {
        position.x = rand() % width;
        position.y = rand() % height;
    }

    Vector2i position;
    CircleShape shape;
};

struct Game
{
    RenderWindow *window;
    Snake *snake;
    Snake *snake2;
    vector<Food> *foods;
};

void *gameLogic(void *arg)
{
    Game *game = (Game *)arg;

    Clock clock;
    while (game->window->isOpen())
    {
        float time = clock.restart().asSeconds();

        game->snake->update(time);
        game->snake2->update(time);
        for (auto &food : *game->foods)
        {
            food.update(*game->snake);
            food.update(*game->snake2); // Update food for the second snake
        }
        pthread_yield();
    }
    return nullptr;
}

int main()
{
    // Prompt the user to input the grid spacing
    int gridSpacing = 20;

    // Calculate the number of blocks needed for the grid
    const int width = 800 / gridSpacing;
    const int height = 600 / gridSpacing;

    RenderWindow window(VideoMode(800, 600), "Snake Game");
    window.setFramerateLimit(60);
    Snake snake, snake2(true);
    snake.flag = 0;
    snake.score = snake2.score = 0;
    snake2.flag = 1;
    int numFoods = 5;
    vector<Food> foods(numFoods);

    Game game{&window, &snake, &snake2, &foods};

    sf::Music backgroundMusic;
    if (!backgroundMusic.openFromFile("m.wav"))
    {
        std::cerr << "Failed to load background music" << std::endl;
        return -1;
    }
    backgroundMusic.setLoop(true);
    backgroundMusic.play();

    pthread_t gameThread;
    pthread_create(&gameThread, nullptr, gameLogic, &game);

    // Load the background image
    Texture texture;
    if (!texture.loadFromFile("B1.jpeg"))
    {
        std::cerr << "Failed to load background image" << std::endl;
        return -1;
    }
    Sprite background(texture);

    // Set up the grid
    RectangleShape gridLine({800, 1});
    gridLine.setFillColor(Color::Black);
    gridLine.setPosition(0, 0);

    Font font;
    if (!font.loadFromFile("GA1.ttf"))
    {
        std::cerr << "Failed to load font" << std::endl;
        return -1;
    }

    // Create a Text object for the countdown timer
    Text countdown;
    Text snakeScoreText;
    Text snake2ScoreText;

    countdown.setFont(font);
    countdown.setCharacterSize(24);
    countdown.setFillColor(Color::White);
    countdown.setPosition(10, 10);
    snakeScoreText.setFont(font);
    snakeScoreText.setCharacterSize(24);
    snakeScoreText.setFillColor(Color::White);
    snakeScoreText.setPosition(10, 40);

    snake2ScoreText.setFont(font);
    snake2ScoreText.setCharacterSize(24);
    snake2ScoreText.setFillColor(Color::White);
    snake2ScoreText.setPosition(10, 70);

    Clock countdownClock;
    float gameDuration = 60.0f;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed)
            {
                switch (event.key.code)
                {
                case Keyboard::Up:
                    snake.setDirection(0, -1);
                    break;
                case Keyboard::Down:
                    snake.setDirection(0, 1);
                    break;
                case Keyboard::Left:
                    snake.setDirection(-1, 0);
                    break;
                case Keyboard::Right:
                    snake.setDirection(1, 0);
                    break;
                case Keyboard::W:
                    snake2.setDirection(0, -1);
                    break;
                case Keyboard::S:
                    snake2.setDirection(0, 1);
                    break;
                case Keyboard::A:
                    snake2.setDirection(-1, 0);
                    break;
                case Keyboard::D:
                    snake2.setDirection(1, 0);
                    break;
                case Keyboard::Escape:
                    exit(1);
                    break;
                default:
                    break;
                }
            }
        }

        window.clear();
        window.draw(background);

        snake.draw(window);
        snake2.draw(window);
        for (auto &food : foods)
        {
            food.draw(window);
        }

        float timeRemaining = gameDuration - countdownClock.getElapsedTime().asSeconds();
        countdown.setString("Time remaining: " + to_string(int(timeRemaining)));
        snakeScoreText.setString("Snake 1 score: " + to_string(snake.score));
        snake2ScoreText.setString("Snake 2 score: " + to_string(snake2.score));

        // End the game if the remaining time is zero or negative
        if (timeRemaining <= 0.0f)
        {
            window.close();
        }

        window.clear();
        window.draw(background);

        snake.draw(window);
        snake2.draw(window);
        for (auto &food : foods)
        {
            food.draw(window);
        }

        // Draw the countdown timer
        window.draw(countdown);
        window.draw(snakeScoreText);
        window.draw(snake2ScoreText);

        window.display();

        // Draw the grid
        for (int i = 0; i <= height; ++i)
        {
            gridLine.setPosition(0, i * gridSpacing);
            window.draw(gridLine);
        }
        for (int i = 0; i <= width; ++i)
        {
            gridLine.setPosition(i * gridSpacing, 0);
            window.draw(gridLine);
        }
        pthread_yield();
    }

    pthread_join(gameThread, nullptr);
    return 0;
}
