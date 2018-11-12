#include <stdio.h>
#include <getopt.h>

#include "encode.h"
#include "decode.h"

// procedurally generates help message
void print_help(
    const char *program_name,
    struct option *long_opt
) {
    printf("%s v0.1\n", program_name);
    printf("OPTIONS\n");
    int i = 0;
    while (long_opt[i].name != NULL) {
        printf("    -%c, --%s%s\n", long_opt[i].val, long_opt[i].name,
                                    long_opt[i].has_arg == required_argument ? " <file>" : "");
        i++;
    }
}

int main(int argc, char *argv[]) {
    int           opt;
    const char   *short_opt = "he:d:";
    struct option long_opt[] = {
        {"help",   no_argument,       NULL, 'h'},
        {"encode", required_argument, NULL, 'e'},
        {"decode", required_argument, NULL, 'd'},
        {NULL,     0,                 NULL, 0  }
    };

    while((opt = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch(opt) {
            case -1:
            case 0:
                break;
            case 'h':
                print_help(argv[0], long_opt);
                break;
            case 'e':
                printf("Encoding file: \"%s\"\n", optarg);
                encode_png(optarg, "output");
                break;
            case 'd':
                printf("Decoding file: \"%s\"\n", optarg);
                decode_png(optarg, "output");
                break;
            case '?':
                fprintf(stderr, "Try \"%s --help\" for more information.\n", argv[0]);
                return(1);
        };
    }
    return 0;
}
