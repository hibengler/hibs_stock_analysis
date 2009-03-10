/* lwcreate -- create a backpropagation neural network
 * copyright (c) 2003-2006 Peter van Rossum
 * released under the GNU Lesser General Public License
 * $Id$ */

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/****************************************
 * Configuration
 ****************************************/

#define MAX_LAYERS 10
#define MAX_FILENAME_LENGTH 100

#include "lwneuralnet.h"

/****************************************
 * Command line options
 ****************************************/

char output_filename[MAX_FILENAME_LENGTH + 1] = "";
int no_of_neurons[MAX_LAYERS];
int no_of_layers;
int use_bias = 1;
int use_binary = 0;

void parse_options(int argc, char **argv)
{
  int c;

  static const struct option long_options[] = {
    {"binary", no_argument, NULL, 'b'},
    {"disable-bias", no_argument, NULL, 'd'},
    {"output", required_argument, NULL, 'o'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {0, 0, 0, 0}
  };
  static const char short_options[] = "bdo:hv";

  while (1) {
    c = getopt_long(argc, argv, short_options, long_options, NULL);
    if (c == -1)
      break;
    switch (c) {
      case 'h':
        printf("Usage:\n");
        printf("  lwcreate [OPTIONS] m0 ... m(n-1)\n");
        printf("\nDescription:\n");
        printf
            ("  Create a neural network with n layers; m0 is the number\n");
        printf
            ("  of neurons in the  input layer, m(n-1) is the number of\n");
        printf
            ("  neurons in the output layer, and m1, ..., m(n-2) are the\n");
        printf("  numbers of neurons in the hidden layers.\n");
        printf("\nOptions:\n");
        printf("  -b, --binary\n");
        printf("       create a binary file instead of a text file\n");
        printf("  -d, --disable-bias\n");
        printf("       disable usage of bias neurons\n");
        printf("  -h, --help\n");
        printf("       display this help and exit\n");
        printf("  -o filename, --output=filename\n");
        printf("       send output to specified file instead of stdout\n");
        printf("  -v, --version\n");
        printf("       display version information and exit\n");
        exit(0);
      case 'v':
        printf("lwneuralnet version %s\n", net_version());
        exit(0);
      case 'b':
        use_binary = 1;
        break;
      case '?':
        printf("Try `lwcreate --help' for more information.\n");
        exit(1);
      case 'o':
        if (strlen(optarg) > MAX_FILENAME_LENGTH) {
          printf("lwcreate: filename too long\n");
          exit(1);
        }
        strcpy(output_filename, optarg);
        break;
    }
  }

  no_of_layers = 0;
  while (optind < argc) {
    if (sscanf(argv[optind], "%i", &no_of_neurons[no_of_layers]) == 0) {
      printf("lwcreate: invalid argument\n");
      exit(1);
    }
    no_of_layers++;
    optind++;
    if (no_of_layers > MAX_LAYERS) {
      printf("lwcreate: too many layers\n");
      exit(1);
    }
  }
  if (no_of_layers < 2) {
    printf("lwcreate: not enough layers\n");
    exit(1);
  }
}

/****************************************
 * Main
 ****************************************/

int main(int argc, char **argv)
{
  network_t *net;

  srandom(time(0));

  parse_options(argc, argv);

  /* allocate a neural network and fill the weights with 
   * random numbers */
  net = net_allocate_l(no_of_layers, no_of_neurons);

  /* disable bias if requested */
  if (!use_bias) {
    net_use_bias(net, 0);
  }

  /* write the network to stdout or the specified file */
  if (use_binary && strlen(output_filename) == 0) {
    net_fbprint(stdout, net);
  } else if (use_binary) {
    net_bsave(output_filename, net);
  } else if (strlen(output_filename) == 0) {
    net_print(net);
  } else {
    net_save(output_filename, net);
  }

  /* free resources */
  net_free(net);

  return 0;
}
