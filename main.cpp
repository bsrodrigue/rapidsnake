#include <chrono>
#include <cstdint>
#include <deque>
#include <queue>
#include <raylib.h>
#include <raymath.h>
#include <stdlib.h>

// TODO: Make support for heterogenous screen sizes

//-----------[WINDOW CONFIGURATIONS]-----------------
#define GLOBAL_WINDOW_SIZE 750
#define WINDOW_HEIGHT GLOBAL_WINDOW_SIZE
#define WINDOW_WIDTH GLOBAL_WINDOW_SIZE
//------------------------------------------

#define FPS 60 // Nintendo will never be able achieve this anymore
#define RAND_SEED 0xBFA

// BASE COLORS
#define RED_COLOR                                                              \
  Color { 255, 0, 0, 255 }
#define GREEN_COLOR                                                            \
  Color { 0, 255, 0, 255 }
#define BLUE_COLOR                                                             \
  Color { 0, 0, 255, 255 }

// GAME COLORS
#define PRIMARY_COLOR                                                          \
  Color { 173, 204, 96, 255 }
#define SECONDARY_COLOR                                                        \
  Color { 43, 51, 24, 255 }

// GAME SCREEN
#define CELL_COUNT 50
#define CELL_SIZE WINDOW_HEIGHT / CELL_COUNT

// PLAYER
#define MAX_SNAKE_BODY_LENGTH 25
#define PLAYER_UPDATE_INTERVAL 0.1

using namespace std;

enum Direction { UP, DOWN, RIGHT, LEFT };

Direction player_direction = UP;

//-----------[SUPER POWERS]-----------------
bool can_backtrack = false;
bool can_selftouch = false;
bool can_shoot = false;
//------------------------------------------

//-----------[COMMANDS AND KEYBOARD]-----------------
const uint8_t MAX_INPUTBUFFER_SIZE = 3;
queue<int> input_buffer;
//------------------------------------------

float get_random_pos() {
  float pos = GetRandomValue(0, CELL_COUNT - 1);
  return pos;
}

void draw_cell(int x, int y, Color color) {
  // TODO: Make this look better
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

void draw_rand_cell(Color color) {
  float x = get_random_pos();
  float y = get_random_pos();

  draw_cell(x, y, color);
}

Vector2 get_random_vec2() {
  Vector2 vec;
  float x = get_random_pos();
  float y = get_random_pos();

  vec.x = x;
  vec.y = y;

  return vec;
}

class Player {
public:
  deque<Vector2> body = {
      {6, 9}, {5, 9}, {4, 9}, {3, 9}, {2, 9}, {1, 9},
  };

  Vector2 last_tail_position;

  void draw() {
    for (uint8_t i = 0; i < body.size(); i++) {
      int x = body[i].x;
      int y = body[i].y;

      draw_cell(x, y, BLUE_COLOR);
    }
  }

  void grow() {
    // TODO: This is not safe

    body.push_back(last_tail_position);
  }
};

class Food {
public:
  Vector2 position;

  Food() { position = get_random_vec2(); }

  void draw() { draw_cell(position.x, position.y, SECONDARY_COLOR); }
};

void sync_pos(Vector2 *dest, Vector2 origin) {
  dest->x = origin.x / CELL_COUNT;
  dest->y = origin.y / CELL_COUNT;
}

void update_input_buffer() {
  // TODO: The handle input is too fast comparing to UI update

  int pressed_key = GetKeyPressed();

  if (pressed_key == 0) {
    return;
  }

  while (input_buffer.size() >= MAX_INPUTBUFFER_SIZE) {
    input_buffer.pop();
  }

  input_buffer.push(pressed_key);
}

void handle_input() {
  bool is_horizontal = (player_direction == LEFT || player_direction == RIGHT);

  int latest_pressed_key = input_buffer.front();

  input_buffer.pop();

  if (is_horizontal) {
    if (latest_pressed_key == (KEY_UP)) {
      player_direction = UP;
    }

    else if (latest_pressed_key == ((KEY_DOWN))) {
      player_direction = DOWN;
    }
  }

  else {
    if (latest_pressed_key == ((KEY_RIGHT))) {
      player_direction = RIGHT;
    }

    else if (latest_pressed_key == ((KEY_LEFT))) {
      player_direction = LEFT;
    }
  }
}

void update_position(Player *player) {
  player->last_tail_position = player->body.back();
  player->body.pop_back();
  Vector2 front = player->body.front();
  Vector2 direction;

  switch (player_direction) {

  case UP:
    direction = Vector2{0, -1};
    break;

  case DOWN:
    direction = Vector2{0, 1};
    break;

  case RIGHT:
    direction = Vector2{1, 0};
    break;

  case LEFT:
    direction = Vector2{-1, 0};
    break;
  }

  player->body.push_front(Vector2Add(front, direction));
}

double last_updatetime = 0;

bool event_triggered(double interval) {
  double current_time = GetTime();

  if (current_time - last_updatetime >= interval) {
    last_updatetime = current_time;
    return true;
  }

  return false;
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

  Player player = Player();

  Food pasta = Food();

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(PRIMARY_COLOR);

    DrawFPS(CELL_SIZE, CELL_SIZE);

    update_input_buffer();

    if (event_triggered(PLAYER_UPDATE_INTERVAL)) {
      if (input_buffer.size()) {
        handle_input();
      }
      update_position(&player);
    }

    player.draw();
    food.draw();

    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
