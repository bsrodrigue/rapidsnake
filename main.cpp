#include <chrono>
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
#define RED_COLOR() create_color(255, 0, 0, 255)
#define GREEN_COLOR() create_color(0, 255, 0, 255)
#define BLUE_COLOR() create_color(0, 0, 255, 255)

// GAME COLORS
#define PRIMARY_COLOR() create_color(173, 204, 96, 255)
#define SECONDARY_COLOR() create_color(43, 51, 24, 255)

// GAME SCREEN
#define CELL_COUNT 25
#define CELL_SIZE WINDOW_HEIGHT / CELL_COUNT

using namespace std;

int get_random_pos() {
  int pos = GetRandomValue(0, CELL_COUNT);
  return pos;
}

void draw_cell(int x, int y, Color color) {
  // TODO: Make this look better
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

void draw_rand_cell(Color color) {
  int x = get_random_pos();
  int y = get_random_pos();

  draw_cell(x, y, color);
}

Vector2 get_random_vec2() {
  Vector2 vec;
  int x = get_random_pos();
  int y = get_random_pos();

  vec.x = x;
  vec.y = y;

  return vec;
}

Color create_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  Color color = {r, g, b, a};
  return color;
}

class Food {

public:
  Vector2 position;

  Food() { position = get_random_vec2(); }

  void draw() { draw_cell(position.x, position.y, SECONDARY_COLOR()); }
};

void sync_pos(Vector2 *dest, Vector2 origin) {
  dest->x = origin.x / CELL_COUNT;
  dest->y = origin.y / CELL_COUNT;
}

int main(int argc, char *argv[]) {
  const char *window_title = "Rapid Snake";
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title);

  auto time = std::chrono::system_clock::now();
  auto duration = time.time_since_epoch();
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  SetRandomSeed(RAND_SEED * millis);

  SetTargetFPS(FPS);

  Food food = Food();

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(PRIMARY_COLOR());

    sync_pos(&food.position, GetMousePosition());

    food.draw();

    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
