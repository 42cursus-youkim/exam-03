#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define ERR_ARG "Error: argument\n"
#define ERR_OP "Error: Operation file corrupted\n"
#define ZERO 0.000000

typedef enum { OUT, IN, EDGE } tile_t;

int str_len(char *str) {
  if (!str)
    return 0;
  int i = 0;
  while (str[i])
    i++;
  return i;
}

int str_err(char *str) {
  write(1, str, str_len(str));
  return 1;
}

void cleanup(FILE *stream, char *grid) {
  free(grid);
  fclose(stream);
}

tile_t get_tile_status(float x, float y, float rx, float rw, float ry, float rh) {
  float rx2 = rx + rw;
  float ry2 = ry + rh;
  const float E = 1.000000;
  if ((rx + E <= x && x <= rx2 - E) && (ry + E <= y && y <= ry2 - E))
    return IN;
  else if ((rx <= x && x <= rx2) && (ry <= y && y <= ry2))
    return EDGE;
  return OUT;
}

int main(int ac, char *av[]) {
  FILE *stream;
  int bg_width, bg_height;
  char bg_char;
  char *grid;
  int res_len;

//args
  if (ac != 2)
    return str_err(ERR_ARG);

//grid_setup
  if (!(stream = fopen(av[1], "r"))
      || (fscanf(stream, "%d %d %c\n", &bg_width, &bg_height, &bg_char) != 3)
      || (!(0 < bg_width && bg_width <= 300) || !(0 < bg_height && bg_height <= 300))
      || (!(grid = malloc(bg_height * bg_width))))
    return str_err(ERR_OP);
  memset(grid, bg_char, bg_width * bg_height);

//grid_write
  char rtype, rchar;
  float rx, ry, rw, rh;
  while ((res_len = fscanf(stream, "%c %f %f %f %f %c\n",
          &rtype, &rx, &ry, &rw, &rh, &rchar)) == 6) {
    // error check
    if (rw <= ZERO || rh <= ZERO || !(rtype == 'r' || rtype == 'R'))
      break;
    // write to grid
    for (int y = 0; y < bg_height; y++) {
      for (int x = 0; x < bg_width; x++) {
        tile_t tile = get_tile_status(x, y, rx, rw, ry, rh);
        if (tile == EDGE || (tile == IN && rtype == 'R'))
          grid[y * bg_width + x] = rchar;
      }
    }
  }
  if (res_len != EOF) {
    cleanup(stream, grid);
    return str_err(ERR_OP);
  }

// grid_print
  for (int y = 0; y < bg_height; y++) {
    write(1, grid + y * bg_width, bg_width);
    write(1, "\n", 1);
  }

//cleanup
  cleanup(stream, grid);
  return 0;
}
