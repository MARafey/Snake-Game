// #include <X11/Xlib.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;
const int blockSize = 16;
const int width = 800 / blockSize;
const int height = 600 / blockSize;

using namespace sf;
using namespace std;
// g++ P1.cpp -o run -lsfml-graphics -lsfml-window -lsfml-system -lX11 -pthread

class Snake
{
public:
    Snake()
    {
        init();
    }

    void init()
    {
        segments.clear();
        segments.emplace_back(width / 2, height / 2);
        direction = Vector2i(1, 0);
        timeElapsed = 0.0f;
        speed = 0.15f;
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
        block.setFillColor(Color::Green);

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

    bool checkCollision() const
    {
        for (std::size_t i = 1; i < segments.size(); ++i)
        {
            if (segments[0] == segments[i])
            {
                return true;
            }
        }

        return segments[0].x < 0 || segments[0].x >= width || segments[0].y < 0 || segments[0].y >= height;
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
            init();
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

    void update(const Snake &snake)
    {
        if (snake.getHeadPosition() == position)
        {
            spawn();
            const_cast<Snake &>(snake).grow();
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
    Food *food;
};

void *gameLogic(void *arg)
{
    Game *game = (Game *)arg;

    Clock clock;
    while (game->window->isOpen())
    {
        float time = clock.restart().asSeconds();

        game->snake->update(time);
        game->food->update(*game->snake);

        pthread_yield();
    }
    return nullptr;
}

void displayWinnerScreen(RenderWindow &window, bool didWin)
{
    // Create a font object and load the font file
    sf::Font font;
    if (!font.loadFromFile("GA1.ttf"))
    {
        cerr << "Failed to load font file" << endl;
        return;
    }

    // Create a text object and set its font and color
    Text text;
    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(Color::White);

    // Set the text content based on whether the player won or lost
    if (didWin)
    {
        text.setString("Congratulations, you won!");
    }
    else
    {
        text.setString("Sorry, you lost. Better luck next time!");
    }

    // Set the position of the text object to the center of the window
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f,
                   textRect.top + textRect.height / 2.0f);
    text.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

    // Draw the text object to the window and display it
    window.clear();
    window.draw(text);
    window.display();

    // Wait for the user to close the window
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }
    }
}

int main()
{
    //   XInitThreads();
    // Prompt the user to input the grid spacing
    int gridSpacing = 20;

    // Calculate the number of blocks needed for the grid
    const int width = 800 / gridSpacing;
    const int height = 600 / gridSpacing;

    RenderWindow window(VideoMode(800, 600), "Snake Game");
    window.setFramerateLimit(60);
    Snake snake;
    Food food;

    Game game{&window, &snake, &food};

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

        snake.draw(window);
        food.draw(window);
        window.display();

        pthread_yield();
    }

    pthread_join(gameThread, nullptr);

    return 0;
}
