#include <iostream>
#include <SFML/Graphics.hpp>
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

#define WIDTH 1600
#define HEIGHT 1200
#define FPS 120
//
#define BRICK_SIZE 40
#define PADDLE_WIDTH 200
#define PADDLE_HEIGHT 40
#define BALL_RADIUS 20
#define BALL_SPEED_START 10
#define GRID_SIZE_X 20
#define GRID_SIZE_Y 5
#define COLOR_BRICK_ORANGE "orange"
#define COLOR_BRICK_PURPLE "purple"
#define COLOR_BRICK_RED "red"

//
#define BACKGROUND_MENU_TEXTURE_PATH "images/background_menu.png"
#define BACKGROUND_GAME_TEXTURE_PATH "images/background.png"
//
#define BRICK_ORANGE__TEXTURE_PATH "images/brick_orange.png"
#define BRICK_PURPLE__TEXTURE_PATH "images/brick_purple.png"
#define BRICK_RED__TEXTURE_PATH "images/brick_red.png"
//
#define BONUS_TEXTURE_PATH "images/bonus.png"
#define BALL_TEXTURE_PATH "images/ball.png"
#define PADDLE_TEXTURE_PATH "images/paddle.png"
//blocs types
#define UNBREAKABLE_BLOCK "UNBREAKABLE"
#define BREAKABLE_BLOCK
#define BONUS_BLOCK 2
#define BONUS_BLOCK_SPEEDUP 3




struct brick {
    int health;
    bool destructible;
    std::string color;
    int x;
    int y;
};
struct bonus : public brick {
    std::string type;
    int x;
    int y;


};

struct paddle {
    int x;
    int y;
    int width;
    int height;
};
struct ball {
    int x;
    int y;
    int dx;
    int dy;
};
std::vector<sf::Texture> load_game_texture(std::vector<std::string> paths) {
    std::vector<sf::Texture> textures;
    for(auto path : paths) {
        sf::Texture texture;
        if(texture.loadFromFile(path)) {
            textures.push_back(texture);
        }
    }
    return textures;

}
void game(){
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Arkanoid");
    window.setFramerateLimit(FPS);

    sf::Sprite background_game, background_menu;
    sf::Sprite paddle;
    sf::Sprite ball;
    sf::Sprite brick[GRID_SIZE_X][GRID_SIZE_Y];


}
int main() {
    return 0;
}
