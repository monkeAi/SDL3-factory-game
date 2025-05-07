#include "game.h"
#include "stdio.h"

int main(int argc, char* argv[]) {
    game_init();
    game_loop();
    game_shutdown();
    return 0;
}