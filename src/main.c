#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * printf() => printw()
 * puts() => addstr()
 * putchar() => addch()
 *
 * cbreak() && nocbreak() - enable/disable C-c command
 * echo() && noecho() - enable/disable echo
 * raw() && noraw() - enable/disable raw mode
 *
 * wborder(win, left, right, top, bottom, top_left, top_right, bottom_left,
 * bottom_right) getyx(win, y, x) - get cursor position getbegyx(win, y, x) -
 * get window position getmaxyx(win, y, x) - get window size keypad(win,
 * boolean) - enable/disable keypad wgetch(win) - get char from window
 */
int load_state(char *todos, char *dones, char *file_path);

typedef struct {
  char *todos[3];
  unsigned int todo_curr;
  char *dones[3];
  unsigned int done_curr;
} state;

int load_state(char *todos, char *dones, char *file_path){
  
  FILE *file = fopen(file_path, "r");
  
  if (file == NULL) {
    fprintf(stderr, "ERROR: could not open file %s\n", file_path);
    return 1;
  }
  // read file by string and find if it is a todo or done and add it to the array
  return 0;
}


int main(int argc, char **argv) {

  /* Argument parsing */
  if (argc < 2) {
    fprintf(stderr, "\nUsage: todoc <file-path>\n");
    fprintf(stderr, "ERROR: no file path provided\n\n");
    return 1;
  }
  
  char *file_path = argv[1];
  /* Argument parsing */

  bool quit = false;
  char *todos[3] = {"Buy bread", "Buy milk", "Buy eggs"};
  unsigned int todo_curr = 0;
  char *dones[3] = {"Buy bread", "Buy milk", "Buy eggs"};
  unsigned int done_curr = 0;

  if (load_state(*todos, *dones, file_path) <= 0){
    fprintf(stderr, "Loaded file %s\n", file_path);
  }
  else {

  }


  initscr();

  noecho();
  keypad(stdscr, true);
  timeout(16);
  curs_set(0);


  while (!quit) {

    for (unsigned int i = 0; i < 3; i++) {
      int pair = todo_curr == i ? A_REVERSE : A_NORMAL;
      attron(pair);
      move(i, 1);
      addstr(todos[i]);
      attroff(pair);
    }
    refresh();

    int key = getch();
    switch (key) {
    case 'q':
      quit = true;
      break;
    case KEY_UP:
    case 'k':
      if (todo_curr > 0)
        todo_curr--;
      break;
    case KEY_DOWN:
    case 'j':
      if (todo_curr < 2) // TODO: replace with sizeof(todos) / sizeof(char *)
        todo_curr++;
      break;
    }
  }
  endwin();

  return 0;
}
