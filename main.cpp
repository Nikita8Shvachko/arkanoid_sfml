#include <iostream>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <vector>
#include <string>
/*
 * Арканоид.
Вверху поля множество прямоугольных блоков. Внизу горизонтально перемещается каретка.
 По полю летает «мяч», отражаясь от стен, блоков и каретки. Если мяч попадает
 вниз (мимо каретки) начисляются отрицательные очки /уменьшается каретка/увеличивается число поражений.
Блоки бывают разных типов:
1) Неразрушаемые. От них мяч просто отскакивает.
2) Блоки могут быть со спрятанными бонусами: при попадании вертикально вниз падает бонус

3) Блоки, увеличивающие скорость шарика при столкновении
4) Блоки имеют уровень здоровья = число попаданий, чтобы блок исчез. За каждое попадание +1 очко игроку, -1 очко здоровья блоку.
5) Бонусы: меняют размер каретки, меняют скорость шарика, меняют прилипание шарика к каретке.
6) Бонус: одноразовое дно для шарика – можно один раз пропустить, дно сработает как каретка, и исчезнет.
 //
7) Бонус: шарик в произвольный момент меняет траекторию.
8) Бонус: появляется двигающий блок, не задевающий при движении остальные блоки (надо проверять коллизии), и живущий до 3 ударов
9) Бонус: появляется второй шарик, способный оттолкнуться и от первого, и от блоков/стенок
Для сдачи: 1-6 + 1 на выбор из 7-9
 * */
//window settings
#define WIDTH 1400
#define HEIGHT 1000
#define FPS 120

//Game settings
#define BRICK_SIZE_X WIDTH/GRID_SIZE_X
#define BRICK_SIZE_Y HEIGHT/4/GRID_SIZE_Y
#define BRICK_SHIFT_CENTER_X WIDTH/4
#define PADDLE_WIDTH 3001
#define PADDLE_HEIGHT 40
#define BALL_SPEED_START 50
#define BALL_SIZE 36
#define GRID_SIZE_X 15
#define GRID_SIZE_Y 5
#define AMOUNT_OF_LIFES 311
#define POSSIBILITY_HIDDEN_BONUS 5 //%
#define POSSIBILITY_BONUS 20
#define POSSIBILITY_UNBREAKEABLE 10


//Textures files
#define FONT_FILE "../fonts/ARCADECLASSIC.TTF"
#define FONT_2_FILE "../fonts/Apple Symbols.ttf"
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


//blocs types
enum BrickType {
    UNBREAKABLE, BREAKABLE, BONUS, HIDDEN_BONUS
};

//Types of bonus
enum BonusType {
    SPEED_UP_BALL, SIZE_UP_PADDLE, SAVE_ONCE, ADD_BALL, STICKY_PADDLE
};
//Types of collision
enum CollisionSide {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    NONE
};


class Brick {
public:
    BrickType type;
    int health;
    bool hiddenBonus;
    sf::Sprite sprite;

    Brick(BrickType type, int health, bool hiddenBonus, sf::Texture &texture, sf::Vector2f position)
            : type(type), health(health), hiddenBonus(hiddenBonus) {
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }


    void take_hit() {
        if (type != UNBREAKABLE and health > 0) {
            health--;
        }
    }

    bool is_destroyed() {
        return type != UNBREAKABLE and health <= 0;
    }

};


class Bonus : public Brick {
public:
    BonusType bonusType;
    sf::Sprite sprite;

    Bonus(BonusType bonusType, sf::Texture &texture, sf::Vector2f position)
            : Brick(BONUS, 1, false, texture, position), bonusType(bonusType) {}


    void move() {
        float dy = 5;
        sprite.move(0, dy);
    }

};

class Paddle {
public:
    sf::Sprite sprite;
    int width;
    int height;
    bool sticky_paddle;

    Paddle(sf::Texture &texture) {
        sprite.setTexture(texture);
        width = PADDLE_WIDTH;
        height = PADDLE_HEIGHT;
        sprite.setScale((float) width / texture.getSize().x, (float) height / texture.getSize().y);
        sprite.setPosition(WIDTH / 2 - width / 2, HEIGHT - height - 10);
    }

    void move(float dx) {
        sprite.move(dx, 0);
    }

    void size_up() {
        height = height * 1.2;
        sprite.setScale((float) width / sprite.getLocalBounds().width, (float) height / sprite.getLocalBounds().height);
    }

    void sticky_paddle_bonus() {
        sticky_paddle = true;
    }
};

class Ball {
public:
    sf::Sprite sprite;
    float dx, dy;
    bool save_once;
    bool add_ball;

    Ball(sf::Texture &texture) {
        sprite.setTexture(texture);
        sprite.setPosition(WIDTH / 2, HEIGHT / 2);
        sprite.setScale(BALL_SIZE / sprite.getLocalBounds().width,
                        BALL_SIZE / sprite.getLocalBounds().height);
        add_ball = false;
        save_once = false;
        dx = 0;
        dy = 0;


    }

    void add_bonus_ball(sf::Texture &texture, sf::Vector2f position, std::vector<Ball> &bonus_balls) {
        bonus_balls.push_back(Ball(texture));
        bonus_balls.back().sprite.setPosition(position);
        bonus_balls.back().dx = dx;
        bonus_balls.back().dy = dy;
        bonus_balls.back().save_once = save_once;
        bonus_balls.back().add_ball = false;

    }

    void move() {
        sprite.move(dx, dy);
    }

    void bounce_horizontal() {
        // Convert degrees to radians
        dx = -dx; // Reflect the ball horizontally with some randomness
        sprite.rotate((rand() % 35 + 1) / 100);
    }

    void bounce_vertical() {
        // Convert degrees to radians
        dy = -dy; // Reflect the ball vertically with some randomness
        sprite.rotate((rand() % 35 + 1) / 100);
    }

    void speed_up() {
        dx = dx * 1.2;
        dy = dy * 1.2;
    }

    void save_once_bonus() {
        save_once = true;
    }

    void add_ball_bonus() {
        add_ball = true;
    }

};

// Load all game textures
std::vector<sf::Texture> load_all_game_textures(std::vector<std::string> paths) {
    std::vector<sf::Texture> textures;
    for (auto path: paths) {
        sf::Texture texture;
        if (texture.loadFromFile(path)) {
            textures.push_back(texture);
        }
    }
    return textures;

}

// Returns a random type of brick
BrickType determineBrickType() {

    int randomValue = rand() % 100;
    if (randomValue < 20) {
        return BREAKABLE; // Orange
    } else if (randomValue < 40) {
        return BREAKABLE; // Purple
    } else if (randomValue < 60) {
        return BREAKABLE; // Red
    } else if (randomValue < 80) {
        return BREAKABLE; // Green
    } else if (randomValue < 90) {
        return UNBREAKABLE;
    } else {
        return BONUS;
    }
}

// Returns a random bonus
BonusType determineBonusType() {
    int randomValue = rand() % 100;
    if (randomValue < 20) {
        return SPEED_UP_BALL;
    } else if (randomValue < 40) {
        return SIZE_UP_PADDLE;
    } else if (randomValue < 60) {
        return SAVE_ONCE;
    } else if (randomValue < 80) {
        return ADD_BALL;
    } else {
        return STICKY_PADDLE;
    }
}

// Function to determine brick texture
Brick determineBrickTexture(BrickType type, sf::Texture &orangeTexture, sf::Texture &purpleTexture,
                            sf::Texture &redTexture, sf::Texture &greenTexture, sf::Texture &unbreakableTexture,
                            sf::Texture &bonusTexture, sf::Vector2f position) {

    switch (type) {
        case BREAKABLE: {
            // You need to randomly select a texture for the breakable bricks.
            switch (rand() % 4) {
                case 0: {
                    return Brick(type, 1, false, greenTexture, position);
                }
                case 1: {
                    return Brick(type, 2, false, orangeTexture, position);
                }
                case 2: {
                    return Brick(type, 3, false, redTexture, position);
                }
                case 3: {
                    return Brick(type, 4, false, purpleTexture, position);
                }
            }
        }
        case BONUS: {
            return Brick(type, 1, false, bonusTexture, position);
        }
        case HIDDEN_BONUS: {
            switch (rand() % 4) {
                case 0: {
                    return Brick(type, 1, false, greenTexture, position);
                }
                case 1: {
                    return Brick(type, 2, false, orangeTexture, position);
                }
                case 2: {
                    return Brick(type, 3, false, redTexture, position);
                }
                case 3: {
                    return Brick(type, 4, false, purpleTexture, position);
                }
            }
        }
        default: {
            return Brick(type, 1, false, unbreakableTexture, position);
        }
    }
}

// Function to calculate position
sf::Vector2f calculatePosition(int i, int j) {
    return sf::Vector2f(i * (BRICK_SIZE_X), 70 + j * (BRICK_SIZE_Y));
}

// Creates a random brick
Brick createRandomBrick(int i, int j, sf::Texture &orangeTexture, sf::Texture &purpleTexture, sf::Texture &redTexture,
                        sf::Texture &greenTexture, sf::Texture &unbreakableTexture, sf::Texture &bonusTexture) {

    BrickType type = determineBrickType();
    bool hiddenBonus = false;
    if (type == HIDDEN_BONUS) {
        hiddenBonus = true;
    }
    // You need to return the result of the function call here.
    return determineBrickTexture(type, orangeTexture, purpleTexture, redTexture, greenTexture, unbreakableTexture,
                                 bonusTexture, calculatePosition(i, j));
}

// Creates all bricks
void create_bricks(std::vector<Brick> &bricks, sf::Texture &brick_texture_red, sf::Texture &brick_texture_purple,
                   sf::Texture &brick_texture_orange, sf::Texture &brick_texture_green, sf::Texture &bonus_texture,
                   sf::Texture &brick_texture_unbreakable) {
    srand(time(NULL));
    for (int i = 0; i < GRID_SIZE_X; i++) {
        for (int j = 0; j < GRID_SIZE_Y; j++) {
            Brick brick = createRandomBrick(i, j, brick_texture_orange, brick_texture_purple, brick_texture_red,
                                            brick_texture_green, brick_texture_unbreakable, bonus_texture);
            brick.sprite.setScale((float) BRICK_SIZE_X / brick.sprite.getTexture()->getSize().x,
                                  (float) BRICK_SIZE_Y / brick.sprite.getTexture()->getSize().y);
            bricks.push_back(brick);
        }

    }
}

void draw_bricks(sf::RenderWindow &window, std::vector<Brick> &bricks) {
    for (auto &brick: bricks) {
        window.draw(brick.sprite);
    }
}

CollisionSide is_collision(const sf::Sprite &sprite1, const sf::Sprite &sprite2) {
    sf::FloatRect bounds1 = sprite1.getGlobalBounds();
    sf::FloatRect bounds2 = sprite2.getGlobalBounds();

    if (!bounds1.intersects(bounds2)) {
        return NONE;
    }

    // Calculate the difference in positions
    double dx = (bounds1.left + bounds1.width / 2) - (bounds2.left + bounds2.width / 2);
    double dy = (bounds1.top + bounds1.height / 2) - (bounds2.top + bounds2.height / 2);

    // Calculate the combined half-widths and half-heights
    double w = (bounds1.width + bounds2.width) / 2;
    double h = (bounds1.height + bounds2.height) / 2;

    // Calculate overlap
    double wy = w * dy;
    double hx = h * dx;

    if (wy > hx) {
        return (wy > -hx) ? BOTTOM : LEFT;
    } else {
        return (wy < -hx) ? RIGHT : TOP;
    }
}


sf::Font load_font(std::string path) {
    sf::Font font;
    if (!font.loadFromFile(path)) {
        std::cout << "Error loading font" << std::endl;
    }
    return font;
}

sf::Text create_text(std::string text, int size, sf::Color color, sf::Vector2f position, sf::Font &font) {
    sf::Text text1;
    text1.setFont(font);
    text1.setCharacterSize(size);
    text1.setFillColor(color);
    text1.setPosition(position);
    text1.setString(text);
    return text1;
}

sf::Sprite create_sprite(sf::Texture &texture, sf::Vector2f position, sf::Color color) {
    sf::Sprite sprite(texture);
    sprite.setColor(color);
    sprite.setScale(WIDTH / sprite.getLocalBounds().width,
                    HEIGHT / sprite.getLocalBounds().height);

    sprite.setPosition(position);
    return sprite;
}


Bonus create_bonus_sprite(BonusType type, sf::Vector2f position, sf::Texture &texture) {
    switch (type) {
        case SPEED_UP_BALL: {
            Bonus bonus = Bonus(SPEED_UP_BALL, texture, position);
            bonus.sprite.setColor(sf::Color::Green);
            return bonus;
        }
        case SIZE_UP_PADDLE: {
            Bonus bonus = Bonus(SIZE_UP_PADDLE, texture, position);
            bonus.sprite.setColor(sf::Color::Magenta);
            return bonus;
        }
        case SAVE_ONCE: {
            Bonus bonus = Bonus(SAVE_ONCE, texture, position);
            bonus.sprite.setColor(sf::Color::Red);
            return bonus;
        }
        case ADD_BALL: {
            Bonus bonus = Bonus(ADD_BALL, texture, position);
            bonus.sprite.setColor(sf::Color::Yellow);
            return bonus;


        }
        case STICKY_PADDLE: {
            Bonus bonus = Bonus(STICKY_PADDLE, texture, position);
            bonus.sprite.setColor(sf::Color::Cyan);
            return bonus;
        }

    }

}

void apply_bonus(BonusType type, Paddle &paddle, Ball &ball) {
    switch (type) {
        case SPEED_UP_BALL: {
            ball.speed_up();
            return;
        }
        case SIZE_UP_PADDLE: {
            paddle.size_up();
            return;
        }
        case SAVE_ONCE: {
            ball.save_once_bonus();
            return;
        }
        case ADD_BALL: {
            ball.add_ball_bonus();
            return;
        }
        case STICKY_PADDLE: {
            paddle.sticky_paddle_bonus();
            return;
        }

    }

}

void check_ball_collision_window(Ball &ball, int &lifes_count, bool &game_over) {
    if (ball.sprite.getPosition().x < 0 ||
        ball.sprite.getPosition().x + ball.sprite.getGlobalBounds().width > WIDTH) {
        ball.dx = -ball.dx;
    }
    if (ball.sprite.getPosition().y < 0) {
        ball.dy = -ball.dy;
    }
    if (ball.sprite.getPosition().y + ball.sprite.getGlobalBounds().height > HEIGHT) {
        ball.dy = -ball.dy;
        if (lifes_count > 0 and ball.save_once == false) {
            lifes_count -= 1;
            if (lifes_count == AMOUNT_OF_LIFES) {
                game_over = true;
                // You should handle game over state here without breaking the loop
            }
        } else if (ball.save_once == true) {
            ball.save_once = false;
        }
    }
}

void check_ball_collision_paddle(Ball &ball, Paddle &paddle) {
    if (paddle.sprite.getGlobalBounds().intersects(ball.sprite.getGlobalBounds())) {
        ball.bounce_vertical();
    }
}

void check_bonus_balls_ball_collision(Ball &ball, std::vector<Ball> &bonus_balls) {
    for (auto it = bonus_balls.begin(); it != bonus_balls.end(); ++it) {
        if (it->sprite.getGlobalBounds().intersects(ball.sprite.getGlobalBounds())) {
            float temp_dx = it->dx;
            float temp_dy = it->dy;
            it->dx = ball.dx;
            it->dy = ball.dy;
            ball.dx = temp_dx;
            ball.dy = temp_dy;

        }
    }
}

void check_bonus_paddle_collision(Paddle &paddle, std::vector<Bonus> &bonuses, Ball &ball) {
    for (auto it = bonuses.begin(); it != bonuses.end();) {
        if (paddle.sprite.getGlobalBounds().intersects(it->sprite.getGlobalBounds())) {
            apply_bonus(it->bonusType, paddle, ball);
            std::cout << "bonus applied" << std::endl;
            it = bonuses.erase(it);
        } else {
            ++it;
        }
    }
}

std::tuple<std::vector<Brick>, int, std::vector<Bonus>>
check_bricks_health(std::vector<Brick> &bricks, Ball &ball, int &score, std::vector<Bonus> &bonuses,
                    sf::Texture &texture) {
    for (auto it = bricks.begin(); it != bricks.end();) {
        if (it->is_destroyed()) {
            score += 2;
            if (it->type == BONUS or it->type == HIDDEN_BONUS) {
                BonusType bonus_type = determineBonusType();  // Assuming there are 5 types of bonuses
                bonuses.push_back(create_bonus_sprite(bonus_type, it->sprite.getPosition(),
                                                      texture));  // Bonus randomly created
                std::cout << "bonus created \t type :" << bonus_type << std::endl;
                (bonuses.back().sprite);
            }
            it = bricks.erase(it);
        } else {
            ++it;
        }
    }

    return std::make_tuple(bricks, score, bonuses);
}

void check_keyboard_input(Paddle &paddle, Ball &ball, bool &start) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) and paddle.sprite.getPosition().x > 0) {
        paddle.move(-5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) and
        paddle.sprite.getPosition().x < WIDTH - PADDLE_WIDTH) {
        paddle.move(5);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) and start) {
        ball.sprite.setPosition(sf::Vector2f(WIDTH / 2, HEIGHT / 2));
        ball.dx = BALL_SPEED_START / 2;
        ball.dy = -BALL_SPEED_START;
//            start = false;
    }
}
int check_bricks_colision_ball(std::vector<Brick> &bricks, Ball &ball, int &score) {
for (auto it = bricks.begin(); it != bricks.end(); ++it) {
if (it->sprite.getGlobalBounds().intersects(ball.sprite.getGlobalBounds())) {
if (it->type != UNBREAKABLE) {
it->health -= 1;
score += 1;
}
ball.bounce_vertical();
}
}
return score;
}
void game() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arkanoid ");
    window.setFramerateLimit(FPS);
    //load textures
    std::vector<std::string> texturePaths = {
            BACKGROUND_MENU_TEXTURE_FILE, BACKGROUND_GAME_TEXTURE_FILE,
            BALL_TEXTURE_FILE, PADDLE_TEXTURE_FILE,
            BRICK_RED_TEXTURE_FILE, BRICK_PURPLE_TEXTURE_FILE, BRICK_ORANGE_TEXTURE_FILE, BRICK_GREEN_TEXTURE_FILE,
            BONUS_TEXTURE_FILE, BRICK_UNBREAKABLE_TEXTURE_FILE};
    std::vector<sf::Texture> textures = load_all_game_textures(texturePaths);
    //load fonts
    sf::Font font = load_font(FONT_FILE);
    //game variables
    int lifes_count = 3;
    int score = 0;
    //Create game objects
    sf::Text Score = create_text(std::to_string(0), 50, sf::Color::White, sf::Vector2f(100, 10), font);
    sf::Text Timer = create_text("3", 50, sf::Color::Magenta, sf::Vector2f(WIDTH - 200, 10), font);
    sf::Text lifes = create_text("3", 50, sf::Color::Red, sf::Vector2f(WIDTH - 800, 10), font);

    sf::Clock clock;

    sf::Sprite background_menu = create_sprite(textures[0], sf::Vector2f(0, 0), sf::Color::White);
    sf::Sprite background_game = create_sprite(textures[1], sf::Vector2f(0, 0), sf::Color::White);

    Ball ball(textures[2]);
    ball.sprite.setPosition(sf::Vector2f(WIDTH / 2, HEIGHT / 2));

    std::vector<Ball> bonus_balls;

    Paddle paddle(textures[3]);
    paddle.sprite.setPosition(sf::Vector2f(WIDTH / 2, HEIGHT - PADDLE_HEIGHT));
    paddle.sprite.setScale(PADDLE_WIDTH / paddle.sprite.getLocalBounds().width,
                           PADDLE_HEIGHT / paddle.sprite.getLocalBounds().height);

    std::vector<Brick> bricks;
    std::vector<Bonus> bonuses;
    create_bricks(bricks, textures[4], textures[5], textures[6], textures[7], textures[8], textures[9]);
    bool start = true;
    bool game_over = false;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        score = check_bricks_colision_ball(bricks, ball, score);

        std::tuple<std::vector<Brick>, int, std::vector<Bonus>> result = check_bricks_health(bricks, ball, score,   bonuses, textures[8]);
        bricks = std::get<0>(result);
        score = (std::get<1>(result));
        bonuses = std::get<2>(result);

        check_bonus_paddle_collision(paddle, bonuses, ball);

        check_ball_collision_paddle(ball, paddle);

        check_bonus_balls_ball_collision(ball, bonus_balls);

        check_ball_collision_window(ball, lifes_count, game_over); // check collision with window and ball,

        if (ball.add_ball == true) {
            ball.add_ball = false;
            ball.add_bonus_ball(textures[11], ball.sprite.getPosition(), bonus_balls);
            std::cerr<<(bonus_balls.size())<<std::endl;
            std::cout << "bonus ball created" << std::endl;

        }


        check_keyboard_input(paddle, ball, start);


        window.clear();
        Score.setString("Score\t" + std::to_string(score));
        Timer.setString("Time\t" + std::to_string(llround(clock.getElapsedTime().asSeconds())));
        lifes.setString("lifes\t" + std::to_string(lifes_count));

//        window.draw(background_menu);
//        window.draw(background_game);
        window.draw(Score);
        window.draw(Timer);
        window.draw(lifes);
        window.draw(paddle.sprite);
        ball.move();
        for (auto &bonus_ballt: bonus_balls) {
            window.draw(bonus_ballt.sprite);
            bonus_ballt.move();
            std::cerr << bonus_ballt.dx << " " << bonus_ballt.dy << std::endl;
        }
        window.draw(ball.sprite);

        for (auto &bonust: bonuses) {
            bonust.move();
            window.draw(bonust.sprite);
        }
        draw_bricks(window, bricks);
        window.display();
    }
}


int main() {
    game();
    return 0;
}