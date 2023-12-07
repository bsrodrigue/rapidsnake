#include <cstdint>
#include <raylib.h>
#include <stdlib.h>

// TODO: Make support for heterogenous screen sizes
#define WINDOW_HEIGHT 750
#define WINDOW_WIDTH 750

#define FPS 60 // Nintendo will never be able achieve this
#define RAND_SEED 0xBFA

Color create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// BASE COLORS
#define RED_COLOR() create_color(255, 0, 0, 0)
#define GREEN_COLOR() create_color(0, 255, 0, 0)
#define BLUE_COLOR() create_color(0, 0, 255, 0)

// GAME COLORS
#define PRIMARY_COLOR() create_color(173, 204, 96, 255)
#define SECONDARY_COLOR() create_color(43, 51, 24, 255)

// GAME SCREEN
#define CELL_COUNT 25
#define CELL_SIZE WINDOW_HEIGHT / CELL_COUNT

using namespace std;

void draw_cell(int x, int y, Color color) {
  // TODO: Make this look better
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

void draw_rand_cell(Color color) {
  int x = GetRandomValue(0, WINDOW_HEIGHT - CELL_SIZE);
  int y = GetRandomValue(0, WINDOW_HEIGHT - CELL_SIZE);

  draw_cell(x, y, color);
}

Color create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  Color color = {r, g, b, a};
  return color;
}

class Food {

public:
  Vector2 position = {5, 6};

  Food() {}

  void draw() { draw_cell(position.x, position.y, SECONDARY_COLOR()); }
};

int main(int argc, char *argv[]) {
  const char *window_title = "Rapid Snake";
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title);
  SetRandomSeed(RAND_SEED);
  SetTargetFPS(FPS);

  Food food = Food();

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(PRIMARY_COLOR());

    // Place game elements
    food.draw();

    draw_rand_cell(RED_COLOR());

    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
