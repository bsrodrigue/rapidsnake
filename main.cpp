#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
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

//-----------[COLORS CONFIGURATIONS]-----------------
#define RED_COLOR                                                              \
  Color { 255, 0, 0, 255 }
#define GREEN_COLOR                                                            \
  Color { 0, 255, 0, 255 }
#define BLUE_COLOR                                                             \
  Color { 0, 0, 255, 255 }

#define PRIMARY_COLOR                                                          \
  Color { 173, 204, 96, 255 }
#define SECONDARY_COLOR                                                        \
  Color { 43, 51, 24, 255 }
//------------------------------------------

#define FPS 30
#define RAND_SEED 0xBFA

// GAME SCREEN
#define CELL_COUNT 40
#define CELL_SIZE WINDOW_HEIGHT / CELL_COUNT

// PLAYER
#define MAX_SNAKE_BODY_LENGTH 10
#define PLAYER_UPDATE_INTERVAL 0.01
#define MAX_SPEED_DELAY 0.01
#define MIN_SPEED_DELAY 0.1

using namespace std;

enum Direction { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT };

namespace rapid_snake {

enum COMMON_INPUT_MAPPING {
  KEY_UP,
  KEY_DOWN,
  KEY_RIGHT,
  KEY_LEFT,
};
}

//-----------[COMMANDS AND KEYBOARD]-----------------
const uint8_t MAX_INPUTBUFFER_SIZE = 3;
queue<int> global_input_buffer;
//------------------------------------------

float get_random_pos() {
  float pos = GetRandomValue(1, CELL_COUNT - 2);
  return pos;
}

void draw_cell(int x, int y, Color color) {
  DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
}

void draw_walls() {
  for (int i = 0; i < CELL_COUNT; i++) {
    draw_cell(i, 0, RED_COLOR);
  }

  for (int i = 0; i < CELL_COUNT; i++) {
    draw_cell(CELL_COUNT - 1, i, RED_COLOR);
  }

  for (int i = CELL_COUNT - 1; i > 0; i--) {
    draw_cell(i, CELL_COUNT - 1, RED_COLOR);
  }

  for (int i = CELL_COUNT - 1; i > 0; i--) {
    draw_cell(0, i, RED_COLOR);
  }
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

float from_player_length_to_delay(int length) {

  const double len_start = 1.0;
  const double len_end = 25.0;
  const double delay_start = log10(MIN_SPEED_DELAY);
  const double delay_end = log10(MAX_SPEED_DELAY);

  const double delay = Lerp(delay_start, delay_end,
                            (length - len_start) / (len_end - len_start));

  return pow(10, delay);
}

class Player {
public:
  char name[256];
  int inputs[4];
  queue<int> input_buffer;
  Direction player_direction = DIR_UP;
  float player_speed = PLAYER_UPDATE_INTERVAL;
  Color color;

  deque<Vector2> body = {
      {6, 9},
      {5, 9},
      {4, 9},
  };

  Vector2 last_tail_position;

  Player(const char *name_input, Color color_input = BLUE_COLOR) {
    strcpy(name, name_input);
    inputs[rapid_snake::KEY_LEFT] = KEY_LEFT;
    inputs[rapid_snake::KEY_RIGHT] = KEY_RIGHT;
    inputs[rapid_snake::KEY_UP] = KEY_UP;
    inputs[rapid_snake::KEY_DOWN] = KEY_DOWN;

    color = color_input;
  }

  void config_inputs(int left, int right, int up, int down) {
    inputs[rapid_snake::KEY_LEFT] = left;
    inputs[rapid_snake::KEY_RIGHT] = right;
    inputs[rapid_snake::KEY_UP] = up;
    inputs[rapid_snake::KEY_DOWN] = down;
  }

  bool is_input(int pressed_key) {
    for (int i = 0; i < 4; i++) {
      if (inputs[i] == pressed_key)
        return true;
    }

    return false;
  }

  void get_input(int pressed_key) {

    if (pressed_key == 0) {
      return;
    }

    while (input_buffer.size() >= MAX_INPUTBUFFER_SIZE) {
      input_buffer.pop();
    }

    if (is_input(pressed_key)) {
      input_buffer.push(pressed_key);
    }

    else {
      // Handle global input buffer
    }
  }

  void handle_movement(int key) {
    bool is_horizontal =
        (player_direction == DIR_LEFT || player_direction == DIR_RIGHT);

    if (is_horizontal) {
      if (key == (inputs[rapid_snake::KEY_UP])) {
        player_direction = DIR_UP;
      }

      else if (key == (inputs[rapid_snake::KEY_DOWN])) {
        player_direction = DIR_DOWN;
      }
    }

    else {
      if (key == (inputs[rapid_snake::KEY_RIGHT])) {
        player_direction = DIR_RIGHT;
      }

      else if (key == (inputs[rapid_snake::KEY_LEFT])) {
        player_direction = DIR_LEFT;
      }
    }
  }

  void update_position() {
    last_tail_position = body.back();
    body.pop_back();
    Vector2 front = body.front();
    Vector2 direction;

    switch (player_direction) {

    case DIR_UP:
      direction = {0, -1};
      break;

    case DIR_DOWN:
      direction = {0, 1};
      break;

    case DIR_RIGHT:
      direction = {1, 0};
      break;

    case DIR_LEFT:
      direction = {-1, 0};
      break;
    }

    body.push_front(Vector2Add(front, direction));
  }

  void dispatch_input() {

    int key = input_buffer.front();

    input_buffer.pop();

    handle_movement(key);
  }

  void draw() {
    for (uint8_t i = 0; i < body.size(); i++) {
      int x = body[i].x;
      int y = body[i].y;

      draw_cell(x, y, color);
    }
  }

  void grow() {
    if (body.size() >= MAX_SNAKE_BODY_LENGTH)
      return;

    body.push_back(last_tail_position);
  }

  void update_speed() {
    float delay = from_player_length_to_delay(body.size());
    player_speed = delay;
  }
};

class Food {
public:
  Vector2 position;

  Food() { position = get_random_vec2(); }

  void draw() { draw_cell(position.x, position.y, SECONDARY_COLOR); }
};

double last_updatetime = 0;

bool event_triggered(double interval) {
  double current_time = GetTime();

  if (current_time - last_updatetime >= interval) {
    last_updatetime = current_time;
    return true;
  }

  return false;
}

void player_loop(Player *player, Food *food) {
  if (player->input_buffer.size()) {
    player->dispatch_input();
  }

  player->update_position();

  // Check food collision
  if (Vector2Equals(player->body.front(), food->position)) {
    food->position = get_random_vec2();
    player->grow();
  }

  // Check player->collision
  for (uint8_t i = player->body.size() - 1; i > 0; i--) {
    Vector2 body_part = player->body.at(i);

    if (Vector2Equals(body_part, player->body.front())) {
      // player->Self Damage
      CloseWindow();
    }
  }
}

int main(int argc, char *argv[]) {
  const char *window_title = "Rapid Snake";
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title);
  SetTargetFPS(FPS);

  auto time = std::chrono::system_clock::now();
  auto duration = time.time_since_epoch();
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

  SetRandomSeed(RAND_SEED * millis);

  Food food = Food();
  Player player = Player("Rachid");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(PRIMARY_COLOR);
    DrawFPS(CELL_SIZE, CELL_SIZE);
    int pressed_key = GetKeyPressed();

    player.get_input(pressed_key);

    if (event_triggered(0.1)) {
      player_loop(&player, &food);
    }

    draw_walls();

    // Check wall collision
    if (player.body.front().x == 0 || player.body.front().x == CELL_COUNT - 1 ||
        player.body.front().y == 0 || player.body.front().y == CELL_COUNT - 1) {
      CloseWindow();
    }

    player.draw();

    food.draw();

    EndDrawing();
  }

  CloseWindow();
  return EXIT_SUCCESS;
}
