#include <errno.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

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
typedef struct {
  char **dones;
  char **todos;
  unsigned int done_cnt;
  unsigned int todo_cnt;
} Tlist;

typedef enum {
    Vert,
    Horz,
} LayoutKind;

typedef struct {
  int x;
  int y;
} Cords;

typedef struct {
  LayoutKind kind;
  Cords pos;
  Cords size;
} Layout;

typedef struct {
  Layout *layouts;
  int key;
} Ui;

int load_state(Tlist *tlist, char *file_path);
void add_dones(Tlist *tlist, char *line, int cnt);

typedef enum { GOOD, NOT_FOUND, ERROR } status;
typedef enum { DONE, TODO } state;

Cords available_pos(Layout *layout){
  Cords pos;
  if (layout->kind == Vert) {
    pos.x = 0;
    pos.y = layout->pos.y + layout->size.y;
  } else if (layout->kind == Horz) {
    pos.x = layout->pos.x + layout->size.x;
    pos.y = 0;
  }
  return pos;
}

void add_widget(Layout *layout, Cords size) {
  if (layout->kind == Vert) {
    layout->size.y += size.y;
    layout->size.x = size.x > layout->size.x ? size.x : layout->size.x;
  } else if (layout->kind == Horz) {
    layout->size.x += size.x;
    layout->size.y = size.y > layout->size.y ? size.y : layout->size.y;
  }
}



// TODO[1]: rewrite code with tlist.done_cnt instead of cnt
// TODO[2]: insteas of srtcpy use cycle to copy line or use strdup
void add_dones(Tlist *tlist, char *line, int cnt) {

  if (cnt == 0) {
    tlist->dones = (char **)malloc(sizeof(char *));
    tlist->dones[cnt] = (char *)malloc(sizeof(char) * (strlen(line - 4) + 1));
    strcpy(tlist->dones[cnt], line + 4);
  } else {
    tlist->dones = (char **)realloc(tlist->dones, sizeof(char *) * (cnt + 1));
    tlist->dones[cnt] = (char *)malloc(sizeof(char) * (strlen(line - 4) + 1));
    strcpy(tlist->dones[cnt], line + 4);
  }
}

void add_todos(Tlist *tlist, char *line, int cnt) {

  if (cnt == 0) {
    tlist->todos = (char **)malloc(sizeof(char *));
    tlist->todos[cnt] = (char *)malloc(sizeof(char) * (strlen(line - 4) + 1));
    strcpy(tlist->todos[cnt], line + 4);
  } else {
    tlist->todos = (char **)realloc(tlist->todos, sizeof(char *) * (cnt + 1));
    tlist->todos[cnt] = (char *)malloc(sizeof(char) * (strlen(line - 4) + 1));
    strcpy(tlist->todos[cnt], line + 4);
  }
}

int load_state(Tlist *tlist, char *file_path) {

  int status = GOOD;
  char line[128];
  FILE *file = fopen(file_path, "r");

  if (file == NULL) {
    file = fopen(file_path, "a+r");
    status = NOT_FOUND;
  }

  while (fgets(line, sizeof(line), file)) {
    if (line[0] == '[') {
      if (line[1] == 'X' && line[2] == ']') {
        add_dones(tlist, line, tlist->done_cnt++);
      } else if (line[1] == ' ' && line[2] == ']') {
        add_todos(tlist, line, tlist->todo_cnt++);
      }
    } else {
      status = ERROR;
      break;
    }
  }
  return status;
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

  Tlist *tlist = (Tlist *)malloc(sizeof(Tlist));
  if (tlist == NULL) {
    fprintf(stderr, "ERROR: could not allocate memory\n");
    return 1;
  }
  unsigned int todo_curr = 0;
  unsigned int done_curr = 0;

  int ls_status = load_state(tlist, file_path);

  if (ls_status == GOOD) {
    fprintf(stderr, "Loaded file %s\n", file_path);
  } else if (ls_status == NOT_FOUND) {
    fprintf(stderr, "New file %s\n", file_path);
  } else if (ls_status == ERROR) {
    fprintf(stderr, "ERROR: could not load file %s, %d\n", file_path, errno);
    return 1;
  }

  initscr();

  noecho();
  keypad(stdscr, true);
  timeout(16);
  curs_set(0);

  bool quit = false;
  int panel = TODO;
  bool edit = false;
  bool editing_cursor = false;

  while (!quit) {
    erase();

    int x = 0;
    int y = 0;
    
    getmaxyx(stdscr, y, x);
    
    for (unsigned int i = 0; i < tlist->done_cnt; i++) {
      int pair = done_curr == i ? A_REVERSE : A_NORMAL;
      attron(pair);
      move(i, 1);
      addstr(tlist->dones[i]);
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
      if (done_curr > 0)
        done_curr--;
      break;
    case KEY_DOWN:
    case 'j':
      if (done_curr < tlist->done_cnt - 1)
        done_curr++;
      break;
    }
  }
  endwin();

  return 0;
}
