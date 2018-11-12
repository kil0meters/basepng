#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "encode.h"
#include "decode.h"

// procedurally generates help message
void print_help(
    const char *program_name,
    struct option *long_opt
) {
    printf("%s v0.1\n", program_name);
    printf("USAGE: %s -{e,d} <INPUT> -o <OUTPUT>\n", program_name);
    printf("OPTIONS\n");
    int i = 0;
    while (long_opt[i].name != NULL) {
        printf("    -%c, --%s%s\n", long_opt[i].val, long_opt[i].name,
                                    long_opt[i].has_arg == required_argument ? " <FILE>" : "");
        i++;
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int           opt;
    const char   *short_opt = "he:d:o:";
    struct option long_opt[] = {
        {"help",   no_argument,       NULL, 'h'},
        {"encode", required_argument, NULL, 'e'},
        {"decode", required_argument, NULL, 'd'},
        {"output", required_argument, NULL, 'o'},
        {NULL,     0,                 NULL, 0  }
    };

    bool decode_flag = true;
    char *input = NULL;
    char *output = NULL;

    if (argc == 1) {
        print_help(argv[0], long_opt);
    }

    while((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(opt) {
            case -1:
            case 0:
                break;
            case 'h':
                print_help(argv[0], long_opt);
                break;
            case 'e':
                input = strdup(optarg);
                decode_flag = false;
                break;
            case 'd':
                input = strdup(optarg);
                break;
            case 'o':
                output = strdup(optarg);
                break;
            case '?':
                fprintf(stderr, "Try \"%s --help\" for more information.\n", argv[0]);
                return 1;
        };
    }

    if (input == NULL || output == NULL) {
        fprintf(stderr, "error: Input and output file required\n");
        fprintf(stderr, "Try \"%s --help\" for more information.\n", argv[0]);
        return 1;
    }

    if (decode_flag)
        decode_png(input, output);
    else
        encode_png(input, output);
    return 0;
}
