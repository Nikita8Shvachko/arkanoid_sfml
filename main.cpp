#include <iostream>
#include <SFML/Graphics.hpp>

struct brick {
    int hardness;
    sf::Color color;
    int x;
    int y;
};
struct bonus : public brick {
    std::string type;

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
    int power;
};

int main() {
    return 0;
}
