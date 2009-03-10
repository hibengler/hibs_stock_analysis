/* lwcopy.c -- copy a backpropagation neural network
 * copyright (c) 2003-2006 Peter van Rossum
 * $Id$ */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lwneuralnet.h"

/****************************************
 * Hard coded bounds
 ****************************************/

#define MAX_FILENAME_LENGTH 100

/****************************************
 * Command line options
 ****************************************/

char input_filename[MAX_FILENAME_LENGTH + 1] = "";
char output_filename[MAX_FILENAME_LENGTH + 1] = "";

void parse_options(int argc, char **argv)
{
  int c;

  static const struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"output", required_argument, NULL, 'o'},
    {0, 0, 0, 0}
  };
  static const char short_options[] = "ho:";

  while (1) {
    c = getopt_long(argc, argv, short_options, long_options, NULL);
    if (c == -1)
      break;
    switch (c) {
      case 'h':
        printf("Usage:\n");
        printf("  lwcopy [OPTIONS] input_network\n");
        printf("\nDescription:\n");
        printf
            ("  Read a neural network from the file input, make a copy of\n");
        printf
            ("  this network with net_copy, and write the result to the\n");
        printf("  file output, or to stdout.\n");
        printf("  The only use of the program is to test the reading,\n");
        printf("  writing, and copying routines of the neural network\n");
        printf("  library.\n");
        printf("\nOptions:\n");
        printf("  -h, --help\n");
        printf("       display this help and exit\n");
        printf("  -o filename, --output=filename\n");
        printf("       send output to specified file instead of stdout\n");
        exit(0);
      case 'o':
        if (strlen(optarg) > MAX_FILENAME_LENGTH) {
          printf("lwcopy: filename too long\n");
          exit(1);
        }
        strcpy(output_filename, optarg);
        break;
      case '?':
        printf("Try `lwcopy --help' for more information.\n");
        exit(1);
    }
  }

  if ((argc < 2) || (argc > 3)) {
    printf("lwcopy: invalid number of arguments\n");
    exit(1);
  }

  strcpy(input_filename, argv[1]);
}

/****************************************
 * Main
 ****************************************/

int main(int argc, char **argv)
{
  network_t *net1, *net2;

  parse_options(argc, argv);

  net1 = net_load(input_filename);
  net2 = net_copy(net1);
  if (strlen(output_filename) == 0) {
    net_print(net2);
  } else {
    net_save(output_filename, net2);
  }
  net_free(net2);
  net_free(net1);

  return 0;
}
