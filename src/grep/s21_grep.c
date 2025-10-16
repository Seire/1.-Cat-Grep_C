#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum bool { false, true };

typedef struct flags {
  unsigned int e : 1;
  unsigned int i : 1;
  unsigned int v : 1;
  unsigned int c : 1;
  unsigned int l : 1;
  unsigned int n : 1;
  unsigned int h : 1;
  unsigned int s : 1;
  unsigned int f : 1;
  unsigned int o : 1;
  unsigned int extra : 1;
} flags;

void grab_pat(char **patterns, char *optarg, int mode, int *err);
char *getstring(FILE *ptr, char *c, int mode);
void flagpriority(flags *flag);
void getpattern(int argc, char *argv[], char **patterns);
void regex(int argc, char *argv[], char **patterns, int *flags);
void run_patterns(FILE *ptr, regex_t gigapat2, flags *flag, char *argv[], int j,
                  int file_sum);
void output_multi(flags *flag, char *string, char *argv[], int i,
                  int *line_count, int file_sum);
void output_single(flags *flag, char *argv[], int i, int line_sum,
                   int file_sum);
int filecount(int argc);
void regexing(char *string, regex_t gigapat2, flags *flag, char *argv[], int j,
              int file_sum, int *line_count, int *linefound);
void realloc_and_check(char **arr_p, int x);

int main(int argc, char *argv[]) {
  enum bool;
  flags flag = {false};
  int opt = 0, err = 1, reg_option = 0;
  char *gigapat = calloc(1, 1);
  while ((opt = getopt(argc, argv, "e:ivclnhsf:o")) != -1 && err) {
    switch (opt) {
      case 'e':
        flag.e = true;
        grab_pat(&gigapat, optarg, 1, &err);
        break;
      case 'i':
        flag.i = true;
        reg_option = REG_ICASE;
        break;
      case 'v':
        flag.v = true;
        break;
      case 'c':
        flag.c = true;
        break;
      case 'l':
        flag.l = true;
        break;
      case 'n':
        flag.n = true;
        break;
      case 'h':
        flag.h = true;
        break;
      case 's':
        flag.s = true;
        break;
      case 'f':
        flag.f = true;
        grab_pat(&gigapat, optarg, 0, &err);
        break;
      case 'o':
        flag.o = true;
        break;
      case 1:
        break;
      default:
        fprintf(stderr,
                "Usage: grep [OPTION]... PATTERNS [FILE]...\nTry 'grep --help' "
                "for more information.\n");
        err = 0;
        break;
    }
  }
  if (err) {
    flagpriority(&flag);
    if (!flag.f && !flag.e && argc - optind > 1) {
      optind++;
      getpattern(argc, argv, &gigapat);
    } else if (flag.e || flag.f) {
      int giglen = strlen(gigapat);
      gigapat[giglen - 1] = '\0';
      gigapat[giglen - 2] = '\0';
    }
    if (argc - optind > 0) {
      regex_t gigapat2;
      int regerr = 0;
      if ((regerr = regcomp(&gigapat2, gigapat, reg_option))) {
        printf("Regcomp error: %d\n", regerr);
      }

      int file_sum = filecount(argc);

      for (int j = optind; j < argc; j++) {
        {
          FILE *ptr;
          if ((ptr = fopen(argv[j], "r")) == NULL) {
            if (!flag.s) {
              fprintf(stderr, "grep: %s: No such file or directory\n", argv[j]);
            }
          } else {
            run_patterns(ptr, gigapat2, &flag, argv, j, file_sum);
            fclose(ptr);
          }
        }
      }
      regfree(&gigapat2);
    } else {
      fprintf(stderr,
              "Usage: grep [OPTION]... PATTERNS [FILE]...\nTry 'grep --help' "
              "for more information.\n");
    }
  }
  free(gigapat);
  return err;
}
/* void init_new_vals(char **patterns){

} */

void grab_pat(char **patterns, char *optarg, int mode, int *err) {
  if (mode) {
    realloc_and_check(patterns,
                      (strlen(optarg) + strlen(*patterns) + 3) * sizeof(char));
    // init_new_vals(patterns);
    strcat(*patterns, optarg);
    strcat(*patterns, "\\|");
  } else {
    if (access(optarg, 0)) {
      (*err) = 0;
      fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
    } else {
      FILE *ptr;
      ptr = fopen(optarg, "r");
      char c = '\0';
      while (c != EOF) {
        char *string = getstring(ptr, &c, 1);
        realloc_and_check(
            patterns, (strlen(string) + strlen(*patterns) + 3) * sizeof(char));
        strcat(*patterns, string);
        strcat(*patterns, "\\|");
        free(string);
      }
      fclose(ptr);
    }
  }
}

void realloc_and_check(char **arr_p, int x) {
  char *temp;
  if ((temp = realloc((*arr_p), x * sizeof(char))) == NULL) {
    printf("Never happens\n");
  } else {
    (*arr_p) = temp;
  }
}

void run_patterns(FILE *ptr, regex_t gigapat2, flags *flag, char *argv[], int j,
                  int file_sum) {
  int line_sum = 0;
  int line_count = 0;
  int linefound = flag->v;
  char c = '\0';
  while (c != EOF) {
    char *string = getstring(ptr, &c, 0);
    if (string[0] == 0) {
      free(string);
      break;
    }
    line_count++;
    regexing(string, gigapat2, flag, argv, j, file_sum, &line_count,
             &linefound);

    if (linefound) {
      if (!flag->extra && !flag->o) {
        output_multi(flag, string, argv, j, &line_count, file_sum);
      }
      line_sum++;
    }
    linefound = flag->v;
    free(string);
  }
  if (flag->extra) {
    output_single(flag, argv, j, line_sum, file_sum);
  }
}

void regexing(char *string, regex_t gigapat2, flags *flag, char *argv[], int j,
              int file_sum, int *line_count, int *linefound) {
  regmatch_t matches[2];
  if (flag->o) {
    int ind = 0;
    while (regexec(&gigapat2, string + ind, 1, matches, 0) == 0) {
      (*linefound) = !flag->v;
      if (!flag->extra) {
        char *o_string =
            calloc(matches[0].rm_eo - matches[0].rm_so + 2, sizeof(char));
        memcpy(o_string, string + ind + matches[0].rm_so,
               matches[0].rm_eo - matches[0].rm_so);
        o_string[matches[0].rm_eo - matches[0].rm_so] = '\n';
        o_string[matches[0].rm_eo - matches[0].rm_so + 1] = '\0';
        output_multi(flag, o_string, argv, j, line_count, file_sum);
        free(o_string);
        ind += matches[0].rm_eo;
      } else {
        break;
      }
    }
  } else if ((regexec(&gigapat2, string, 2, matches, 0)) == 0) {
    (*linefound) = !flag->v;
  }
}

void output_multi(flags *flag, char *string, char *argv[], int j,
                  int *line_count, int file_sum) {
  int kstl = 0;
  if (file_sum > 1 && !flag->h) {
    printf("%s", argv[j]);
    kstl = 1;
  }
  if (flag->n) {
    if (kstl) {
      printf(":");
    }
    printf("%d", *line_count);
    kstl = 1;
  }

  if (kstl) {
    printf(":");
  }

  printf("%s", string);
}

void output_single(flags *flag, char *argv[], int j, int line_sum,
                   int file_sum) {
  int kstl = 0;
  if (flag->l) {
    if (line_sum > 0) {
      printf("%s", argv[j]);
      kstl = 1;
    }
  } else if (file_sum > 1 && !flag->h && ((flag->o + flag->v - flag->c) != 2)) {
    printf("%s", argv[j]);
    kstl = 1;
  }

  if (flag->c) {
    if (kstl) {
      printf(":");
    }
    printf("%d", line_sum);
    kstl = 1;
  }

  if (kstl) {
    printf("\n");
  }
}

char *getstring(FILE *ptr, char *c, int mode) {
  char *string = calloc(1, sizeof(char));
  int i = 0;
  (*c) = '\0';
  for (; (*c) != '\n' && (*c) != EOF; i++) {
    (*c) = getc(ptr);
    string[i] = (*c);
    realloc_and_check(&string, (i + 2));
  }

  if (mode || (*c) == EOF) {
    string[i - 1] = '\0';
  }

  if (!mode && (*c) == EOF) {
    if (i != 1 && string[i - 1] != '\n') {
      string[i - 1] = '\n';
    } else {
      string[i - 1] = '\0';
    }
  }

  string[i] = '\0';
  return string;
}

void flagpriority(flags *flag) {
  if (flag->l) {
    flag->c = false;
    flag->n = false;
    flag->h = false;
    flag->o = false;
  }

  if (flag->c) {
    flag->n = false;
  }

  if (flag->l || flag->c || (flag->o && flag->v)) {
    flag->extra = 1;
  }
}

void getpattern(int argc, char *argv[], char **patterns) {
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      realloc_and_check(patterns, strlen(argv[i]) + 1);
      strcpy(*patterns, argv[i]);
      break;
    }
  }
}

int filecount(int argc) {
  int filecounter = 0;
  for (int i = optind; i < argc; i++) {
    filecounter++;
  }
  return filecounter;
}