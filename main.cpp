#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <vector>
#include <string>
#include <cmath>

// Window settings
#define WIDTH 1400
#define HEIGHT 1000
#define FPS 120

// Game settings
#define BRICK_SIZE_X (WIDTH/GRID_SIZE_X)
#define BRICK_SIZE_Y (HEIGHT/5/GRID_SIZE_Y)

#define PADDLE_WIDTH 300
#define PADDLE_HEIGHT 50

#define BALL_SPEED_START 5
#define BALL_SIZE 10

#define GRID_SIZE_X 10
#define GRID_SIZE_Y 5

#define AMOUNT_OF_LIFES 3

// Textures files
#define FONT_FILE "../fonts/ARCADECLASSIC.TTF"
#define BACKGROUND_MENU_TEXTURE_FILE "../textures/background_menu.jpg"
#define BACKGROUND_GAME_TEXTURE_FILE "../textures/background_game.jpg"
#define BALL_TEXTURE_FILE "../textures/ball.png"
#define PADDLE_TEXTURE_FILE "../textures/paddle.png"
#define BRICK_ORANGE_TEXTURE_FILE "../textures/brick_orange.png"
#define BRICK_PURPLE_TEXTURE_FILE "../textures/brick_purple.png"
#define BRICK_RED_TEXTURE_FILE "../textures/brick_red.png"
#define BRICK_GREEN_TEXTURE_FILE "../textures/brick_green.png"
#define BRICK_UNBREAKABLE_TEXTURE_FILE "../textures/brick_unbreakable.png"
#define BONUS_TEXTURE_FILE "../textures/bonus_brick.png"
#define CIRCLE_TEXTURE_FILE "../textures/circle.png"

// Block types
enum BrickType {
    UNBREAKABLE, BREAKABLE, BONUS, HIDDEN_BONUS, SPEED_UP
};

// Bonus types
enum BonusType {
    CHANGE_SPEED_BALL, CHANGE_SIZE_PADDLE, SAVE_ONCE, CHANGE_DIRECTION, STICKY_PADDLE
};

class Moveable {
public:
    sf::Sprite sprite;
    float dx;
    float dy;
    bool is_moving;

    Moveable() : dx(0), dy(0), is_moving(false) {}

    Moveable(sf::Texture &texture, sf::Vector2f position) : dx(0), dy(0), is_moving(false) {

        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(5 * BALL_SIZE / sprite.getLocalBounds().width, 5 * BALL_SIZE / sprite.getLocalBounds().height);
    }



    void move(float _dx, float _dy) {
        is_moving = true;
        dx = _dx;
        dy = _dy;
        sprite.move(dx, dy);
    }

    sf::Vector2f get_position() const {
        return sprite.getPosition();
    }


    void set_position(sf::Vector2f position) {
        sprite.setPosition(position);
    }

    void change_move_direction_xy() {
        is_moving = true;
        dx = -dx;
        dy = -dy;
    }


    void change_move_direction_y() {
        is_moving = true;
        dy = -dy;
    }

    void change_move_direction_x() {
        is_moving = true;
        dx = -dx;
    }

    bool is_collision(const Moveable &other) const {
        return sprite.getGlobalBounds().intersects(other.sprite.getGlobalBounds());
    }
};
class Bonus : public Moveable {
private:
    BonusType bonusType;

public:
    Bonus() {
        int number = rand() % 50;
        if (number < 10) {
            bonusType = CHANGE_SPEED_BALL;
        } else if (number < 20) {
            bonusType = CHANGE_SIZE_PADDLE;
        } else if (number < 30) {
            bonusType = SAVE_ONCE;
        } else if (number < 40) {
            bonusType = CHANGE_DIRECTION;
        } else {
            bonusType = STICKY_PADDLE;
        }
    }

    void initialize(sf::Texture &texture, sf::Vector2f position) {
        sprite.setTexture(texture);
        sprite.setPosition(position);
        sprite.setScale(5 * BALL_SIZE / sprite.getLocalBounds().width, 5 * BALL_SIZE / sprite.getLocalBounds().height);
        dx = 0;
        dy = 5;
        switch (bonusType) {
            case CHANGE_SPEED_BALL:
                sprite.setColor(sf::Color::Green);
                break;
            case CHANGE_SIZE_PADDLE:
                sprite.setColor(sf::Color::Blue);
                break;
            case SAVE_ONCE:
                sprite.setColor(sf::Color::Red);
                break;
            case CHANGE_DIRECTION:
                sprite.setColor(sf::Color::White);
                break;
            case STICKY_PADDLE:
                sprite.setColor(sf::Color::Magenta);
                break;
        }
        is_moving = true;
    }

    BonusType getBonusType() const {
        return bonusType;
    }
};


class Brick : public Moveable {
public:
    BrickType type;
    int health;
    bool hidden_bonus;

    Brick() {}

    Brick(sf::Vector2f position, int health, sf::Texture &brick_green, sf::Texture &brick_red,
          sf::Texture &brick_orange, sf::Texture &brick_purple, sf::Texture &brick_unbreakable,
          sf::Texture &brick_bonus)
            : Moveable(), health(health), hidden_bonus(false) {

        brick_random_color(brick_green, brick_red, brick_orange, brick_purple, brick_unbreakable, brick_bonus);
        sprite.setPosition(position);
    }

    void brick_random_color(sf::Texture &brick_green, sf::Texture &brick_red, sf::Texture &brick_orange,
                            sf::Texture &brick_purple, sf::Texture &brick_unbreakable, sf::Texture &brick_bonus) {
        int number = rand() % 100;
        if (number < 20) {
            type = SPEED_UP;
            sprite.setTexture(brick_green); // green
        } else if (number < 40) {
            type = HIDDEN_BONUS;
            sprite.setTexture(brick_orange); // orange
        } else if (number < 60) {
            type = BREAKABLE;
            sprite.setTexture(brick_red); // red
        } else if (number < 80) {
            type = BREAKABLE;
            sprite.setTexture(brick_purple); // purple
        } else if (number < 90) {
            type = BONUS;
            sprite.setTexture(brick_bonus); // bonus
        } else {
            type = UNBREAKABLE;
            sprite.setTexture(brick_unbreakable); // unbreakable
        }
        sprite.setScale(BRICK_SIZE_X / sprite.getLocalBounds().width, BRICK_SIZE_Y / sprite.getLocalBounds().height);

    }

    void take_hit() {
        if (type != UNBREAKABLE) {
            health -= 1;
        }
    }

    bool is_destroyed() const {
        return (type != UNBREAKABLE && health <= 0);
    }
};

class Paddle : public Moveable {
public:
    int width;
    int height;
    bool sticky_paddle;

    Paddle() : width(PADDLE_WIDTH), height(PADDLE_HEIGHT), sticky_paddle(false) {}

    Paddle(sf::Texture &texture, sf::Vector2f position)
            : Moveable(texture, position), width(PADDLE_WIDTH), height(PADDLE_HEIGHT), sticky_paddle(false) {

        sprite.setScale(PADDLE_WIDTH / sprite.getLocalBounds().width,
                        PADDLE_HEIGHT / sprite.getLocalBounds().height);
    }

    void size_up() {
        width = static_cast<int>(width * 1.2); // update the width variable
        sprite.setScale(static_cast<float>(width) / sprite.getLocalBounds().width,
                        static_cast<float>(height) / sprite.getLocalBounds().height);
    }
    void sizeStandard() {
        width = PADDLE_WIDTH;
        sprite.setScale(static_cast<float>(width) / sprite.getLocalBounds().width,
                        static_cast<float>(height) / sprite.getLocalBounds().height);
    }


    void sticky_paddle_bonus() {
        sticky_paddle = true;
    }

    void remove_sticky_paddle_bonus() {
        sticky_paddle = false;
    }


};

class Ball : public Moveable {
public:
    bool save_once;

    Ball() : save_once(false) {}

    Ball(sf::Texture &texture, sf::Vector2f position) : Moveable(texture, position), save_once(false) {}

    void speed_up() {
        if (dx < 100 && dy < 100) {
            dx *= 1.1f;
            dy *= 1.1f;
        }
    }
    void changeSpeed() {
        if (rand() % 10 == 0) {
            dx *= 1.5f;
            dy *= 1.5f;
        } else{
            dx /= 1.5f;
            dy /= 1.5f;
        }

    }

    void save_once_bonus(bool _save_once) {
        save_once = _save_once;
    }
    void remove_save_once_bonus() {
        save_once = false;
    }
};

class Window {
private:
    sf::RenderWindow window;
    int width;
    int height;
    int fps;

public:
    Window() : width(WIDTH), height(HEIGHT), fps(FPS) {
        window.create(sf::VideoMode(width, height), "Arkanoid", sf::Style::Default);
        window.setFramerateLimit(fps);
    }

    void clear() {
        window.clear();
    }

    void draw(sf::Drawable &drawable) {
        window.draw(drawable);
    }

    void update() {
        window.display();
    }

    void close() {
        window.close();
    }

    bool isOpen() const {
        return window.isOpen();
    }

    bool pollEvent(sf::Event &event) {
        return window.pollEvent(event);
    }
};

class Game : public Window {
private:
    std::vector<Bonus> drops;
    Moveable droping_sprite;
    Paddle paddle;
    Ball ball;
    std::vector<Brick> bricks;

        sf::Texture backgroundTexture;
        sf::Sprite backgroundSprite;

        sf::Texture ballTexture;
        sf::Texture paddleTexture;

        sf::Texture brickTextureGreen;
        sf::Texture brickTextureRed;
        sf::Texture brickTextureOrange;
        sf::Texture brickTexturePurple;
        sf::Texture brickTextureUnbreakable;
        sf::Texture bonusTexture;
        sf::Texture dropTexture;

    sf::Text scoreText;
    sf::Text lifeText;
    sf::Text winText;
    sf::Text loseText;
    sf::Font font;

    int lifes;
    int score;
    bool game_started;
    bool is_win;
    bool is_lose;


    void loadTextures() {
        backgroundTexture.loadFromFile(BACKGROUND_GAME_TEXTURE_FILE);
        ballTexture.loadFromFile(BALL_TEXTURE_FILE);
        paddleTexture.loadFromFile(PADDLE_TEXTURE_FILE);

        brickTextureGreen.loadFromFile(BRICK_GREEN_TEXTURE_FILE);
        brickTextureRed.loadFromFile(BRICK_RED_TEXTURE_FILE);
        brickTextureOrange.loadFromFile(BRICK_ORANGE_TEXTURE_FILE);
        brickTexturePurple.loadFromFile(BRICK_PURPLE_TEXTURE_FILE);
        brickTextureUnbreakable.loadFromFile(BRICK_UNBREAKABLE_TEXTURE_FILE);
        bonusTexture.loadFromFile(BONUS_TEXTURE_FILE);
        dropTexture.loadFromFile(CIRCLE_TEXTURE_FILE);
    }

    void initializeBricks() {
        for (int i = 0; i < GRID_SIZE_X; i++) {
            for (int j = 0; j < GRID_SIZE_Y; j++) {
                Brick brick(sf::Vector2f(i * BRICK_SIZE_X, 100 + j * BRICK_SIZE_Y), rand() % 5, brickTextureGreen,
                            brickTextureRed, brickTextureOrange, brickTexturePurple, brickTextureUnbreakable,
                            bonusTexture);
                bricks.push_back(brick);
            }
        }
    }


    void updateScore() {
        scoreText.setString("Score: " + std::to_string(score));
    }

    void updateLife() {
        lifeText.setString("Lifes: " + std::to_string(lifes));
    }

    void reset() {
        lifes = AMOUNT_OF_LIFES;
        score = 0;
        game_started = false;
        is_win = false;
        is_lose = false;
        ball.set_position(sf::Vector2f(WIDTH / 2, HEIGHT - 2 * PADDLE_HEIGHT - BALL_SIZE));
        ball.is_moving = false;
        ball.dx = 5;
        ball.dy = 7;
        paddle.sizeStandard();
        paddle.set_position(sf::Vector2f(WIDTH / 2 - PADDLE_WIDTH / 2, HEIGHT - PADDLE_HEIGHT));
        bricks.clear();
        initializeBricks();
        update();
    }


public:
    void checkBricksCollision() {
        for (auto it = bricks.begin(); it != bricks.end();) {
            if (ball.is_collision(*it)) {
                if (it->type == SPEED_UP) {
                    ball.speed_up();
                    ball.change_move_direction_y();
                }
                if (it->type != UNBREAKABLE) {
                    it->take_hit();
                    if (it->is_destroyed()) {
                        if (it->type == BONUS || it->type == HIDDEN_BONUS) {
                            Bonus drop;
                            drop.initialize(dropTexture, it->get_position());
                            drops.push_back(drop);
                        }
                        score += 10;
                        updateScore();
                        it = bricks.erase(it);
                    } else {
                        ++it;
                    }
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }



    void checkDropsCollision() {
        for (auto it = drops.begin(); it != drops.end();) {
            if (paddle.is_collision(*it)) {
                // Apply the effect of the bonus
                switch (it->getBonusType()) {
                    case CHANGE_SPEED_BALL:
                        ball.changeSpeed(); // Assuming you have a method to change the ball's speed
                        break;
                    case CHANGE_SIZE_PADDLE:
                        paddle.size_up(); // Assuming you have a method to change the paddle's size
                        break;
                    case SAVE_ONCE:
                        ball.save_once_bonus(true);
                        // Implement the logic for saving the game once
                        break;
                    case CHANGE_DIRECTION:
                        ball.change_move_direction_xy(); // Assuming you have a method to change the ball's direction
                        break;
                    case STICKY_PADDLE:
                        paddle.sticky_paddle_bonus(); // Assuming you have a method to make the paddle sticky
                        ball.is_moving = false;
                        ball.set_position(paddle.get_position());
                        break;
                }
                // Erase the drop after applying the bonus
                it = drops.erase(it);
            } else {
                ++it;
            }
        }
    }


    void checkCollision() {
        if (ball.is_collision(paddle)) {
            ball.change_move_direction_y();
        }
        if (ball.get_position().y > HEIGHT || ball.get_position().y < 0) {
            ball.change_move_direction_y();
        }
        if (ball.get_position().x > WIDTH || ball.get_position().x < 0) {
            ball.change_move_direction_x();
        }
        checkDropsCollision();
        checkBricksCollision();

    }


    void moveEntities() {
        if (ball.is_moving) {
            ball.move(ball.dx, ball.dy);
        }
        for (auto &it: drops) {
            it.move(it.dx, it.dy);
        }


    }

    void render() {
        clear();
        draw(backgroundSprite);
        draw(ball.sprite);
        draw(paddle.sprite);

        for (auto &brick: bricks) {
            draw(brick.sprite);
        }
        for (auto &bonus: drops) {
            draw(bonus.sprite);
        }
        updateScore();
        updateLife();
        draw(scoreText);
        draw(lifeText);

        if (is_win) {
            draw(winText);
        } else if (is_lose) {
            draw(loseText);
        }

        update();
    }

    // Track key states
    bool moveLeft = false;
    bool moveRight = false;

    void check_events() {
        sf::Event event;
        while (pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                close();
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                    moveLeft = true;
                }
                if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                    moveRight = true;
                }
                if (event.key.code == sf::Keyboard::Space) {
                    ball.is_moving = !ball.is_moving;
                }
                if (event.key.code == sf::Keyboard::R) {
                    reset();
                }
            }
            if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A) {
                    moveLeft = false;
                }
                if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D) {
                    moveRight = false;
                }
            }
        }
    }

    void update_paddle() {
        if (moveLeft && paddle.get_position().x > 0) {
            paddle.move(-11, 0);
        }
        if (moveRight && paddle.get_position().x < (WIDTH - PADDLE_WIDTH)) {
            paddle.move(11, 0);
        }
    }


    void check_win_lose() {
        if (bricks.empty()) {
            is_win = true;
        }
        bool only_unbreakable = true;
        for (const auto &brick: bricks) {
            if (brick.type != UNBREAKABLE) {
                only_unbreakable = false;
                break;
            }
        }
        if (!bricks.empty() && only_unbreakable) {
            is_win = true;
        }

        if (lifes <= 0) {
            is_lose = true;
        }
    }

public:
    Game() : lifes(AMOUNT_OF_LIFES), score(0), game_started(false), is_win(false), is_lose(false) {
        font.loadFromFile(FONT_FILE);

        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        lifeText.setFont(font);
        lifeText.setCharacterSize(30);
        lifeText.setFillColor(sf::Color::White);
        lifeText.setPosition(WIDTH - 150, 10);

        winText.setFont(font);
        winText.setString("You Win!");
        winText.setCharacterSize(50);
        winText.setFillColor(sf::Color::Green);
        winText.setPosition(WIDTH / 2 - 100, HEIGHT / 2 - 25);

        loseText.setFont(font);
        loseText.setString("Game Over!");
        loseText.setCharacterSize(50);
        loseText.setFillColor(sf::Color::Red);
        loseText.setPosition(WIDTH / 2 - 150, HEIGHT / 2 - 25);

        loadTextures();

        backgroundSprite.setTexture(backgroundTexture);

        ball = Ball(ballTexture, sf::Vector2f(WIDTH / 2 - BALL_SIZE, HEIGHT - PADDLE_HEIGHT - 2 * BALL_SIZE));

        ball.sprite.setScale(2 * BALL_SIZE / ball.sprite.getLocalBounds().width,
                             2 * BALL_SIZE / ball.sprite.getLocalBounds().height);
        paddle = Paddle(paddleTexture, sf::Vector2f(WIDTH / 2 - PADDLE_WIDTH / 2, HEIGHT - PADDLE_HEIGHT));

        initializeBricks();
    }

    void run() {

        while (isOpen()) {
            check_events();

            update_paddle();
            checkCollision();
            moveEntities();
            render();
            check_win_lose();

        }
    }

};

int main() {
    srand((time(nullptr)));

    Game game;
    game.run();

    return 0;
}