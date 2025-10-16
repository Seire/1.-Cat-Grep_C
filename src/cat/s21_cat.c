#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct flag {
  int fl[10];
} flags;

void output(int argc, char *argv[], flags flg);
void flagsymbol(unsigned char *next, flags flg, int *lc, FILE *ptr, long *x,
                unsigned char prev);
void emptylinesdelete(FILE *ptr, long *x);
void vflag(unsigned char *next);
void v_print(unsigned char c);
void flagprior(flags *flg);

int main(int argc, char *argv[]) {
  char opt;
  flags flg;
  int option_index = 1;
  const struct option long_options[] = {
      {"number-nonblank", no_argument, &flg.fl[0], 1},
      {"number", no_argument, &flg.fl[1], 1},
      {"squeeze-blank", no_argument, &flg.fl[3], 1},
      {0, 0, 0, 0}};
  memset(flg.fl, 0, sizeof(flg.fl));
  while ((opt = getopt_long(argc, argv, "bnestvTE", long_options,
                            &option_index)) != -1) {
    switch (opt) {
      case 'b':
        flg.fl[0] = 1;
        break;
      case 'n':
        flg.fl[1] = 1;
        break;
      case 'e':
        flg.fl[2] = 1;
        break;
      case 's':
        flg.fl[3] = 1;
        break;
      case 't':
        flg.fl[4] = 1;
        break;
      case 'v':
        flg.fl[5] = 1;
        break;
      case 'T':
        flg.fl[6] = 1;
        break;
      case 'E':
        flg.fl[7] = 1;
        break;
      case 1:
        break;
      default:
        fprintf(stderr,
                "cat: invalid option -- '%c'\nTry 'cat --help' for more "
                "information.\n",
                opt);
        break;
    }
  }
  flagprior(&flg);
  output(argc, argv, flg);
  return 0;
}

void flagprior(flags *flg) {
  if (flg->fl[2] || flg->fl[4]) {
    flg->fl[5] = 1;
  }

  if (flg->fl[0]) {
    flg->fl[1] = 0;
  }
}

void output(int argc, char *argv[], flags flg) {
  unsigned char next = '\0', prev = '\n';
  int count = 1;
  int linecount = 1;
  long x;
  while (count < argc) {
    if (argv[count][0] != '-') {
      FILE *ptr;
      if ((ptr = fopen(argv[count], "r")) == NULL) {
        fprintf(stderr, "cat: %s: No such file or directory\n", argv[count]);
      } else {
        if (prev == 10 && next == 255) {
          while ((next = getc(ptr)) == '\n') {
          }
        } else {
          next = fgetc(ptr);
        }
        while (next != 255) {
          flagsymbol(&next, flg, &linecount, ptr, &x, prev);
          prev = next;
          next = fgetc(ptr);
        }
        fclose(ptr);
      }
    }
    count++;
  }
}

void flagsymbol(unsigned char *next, flags flg, int *linecount, FILE *ptr,
                long *x, unsigned char prev) {
  if (flg.fl[0] && prev == '\n' && *next != '\n') {
    printf("%6d\t", (*linecount)++);
  }

  if (flg.fl[1] && prev == '\n') {
    printf("%6d\t", (*linecount)++);
  }

  if (flg.fl[2] && *next == '\n') {
    printf("$");
  }

  if (flg.fl[3] && *next == '\n' && prev == '\n') {
    emptylinesdelete(ptr, x);
  }

  if ((flg.fl[4] || flg.fl[6]) && *next == '\t') {
    printf("^");
    *next = 'I';
  }

  if (flg.fl[5] == 1) {
    vflag(next);
  }

  printf("%c", *next);
}

void emptylinesdelete(FILE *ptr, long *x) {
  int c = 0;
  *x = ftell(ptr);
  char cc = fgetc(ptr);
  while (cc == '\n') {
    c++;
    cc = getc(ptr);
  }
  fseek(ptr, *x, SEEK_SET);
  fseek(ptr, c, SEEK_CUR);
}

void vflag(unsigned char *next) {
  if ((*next) == 9 || (*next) == 10) {
  } else if ((*next) >= 32 && (*next) < 127) {
  } else if ((*next) == 127) {
    printf("^");
    (*next) = '?';
  } else if ((*next) >= 128 + 32) {
    printf("M-");
    if ((*next) < 128 + 127) {
      (*next) = (*next) - 128;
    } else {
      printf("^");
      (*next) = '?';
    }
  } else {
    if ((*next) > 32) {
      printf("M-^");
      (*next) = (*next) - 128 + 64;
    } else {
      printf("^");
      (*next) = (*next) + 64;
    }
  }
}