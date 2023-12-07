#include <raylib.h>
#include <stdlib.h>

#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 640

using namespace std;

int main(int argc, char *argv[]) {
  const char *window_title = "Super Snake";
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, window_title);

  CloseWindow();
  return EXIT_SUCCESS;
}
