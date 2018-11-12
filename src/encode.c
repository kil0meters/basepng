#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <png.h>

#define RETURN_IF_NULL(val, err_msg) if (val == NULL) { fputs(err_msg, stderr); return 1; }

typedef struct Dimentions {
    int x;
    int y;
} Dimentions;

typedef struct ImageData {
    char *data;
    long length;
} ImageData;

// finds the two most similar factors.
// it's quite unfortunate when a file has a prime
// numebr of bytes...
static Dimentions find_image_dimentions(int length) {
    Dimentions dimentions = {0, 0};
    int max = floor(sqrt(length));
    for (int i = max; i > 0; i -= 1) {
        if (length % i == 0) {
            printf("max: %d\ni: %d\n", max, i);
            dimentions.x = i;
            dimentions.y = length / i;
            break;
        }
    }

    return dimentions;
}

static ImageData read_file_into_data(const char *filename) {
    FILE *f = fopen(filename, "rb");
    char *string = NULL;
    long fsize = 0;
    if (f != NULL) {
        fseek(f, 0, SEEK_END);
        fsize = ftell(f);
        fseek(f, 0, SEEK_SET);

        string = malloc(fsize + 1);
        fread(string, fsize, 1, f);
        fclose(f);

        string[fsize] = '\0';
    }
    ImageData image_data = {
        .data = string,
        .length = fsize
    };
    return image_data;
}

bool encode_png(const char *input, const char *dest) {
    FILE *file_to_write = fopen(dest, "wb");
    RETURN_IF_NULL(file_to_write, "error: Could not open file");

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                  NULL, NULL, NULL);
    RETURN_IF_NULL(png_ptr, "error: Not enough memory");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    RETURN_IF_NULL(info_ptr, "error: Not enough memory");

    png_init_io(png_ptr, file_to_write);

    ImageData image_data = read_file_into_data(input);
    RETURN_IF_NULL(image_data.data, "error: Could not open file");

    char *content_to_encode = image_data.data;
    int content_length = image_data.length;

    Dimentions image_dimentions = find_image_dimentions(content_length / 3);
    printf("size: %d\nlength: %d\nwidth: %d\n\n", content_length / 3, image_dimentions.x, image_dimentions.y);

    png_init_io(png_ptr, file_to_write);

    png_set_IHDR(png_ptr, info_ptr, image_dimentions.x,
                 image_dimentions.y, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    png_bytep row = malloc(3 * image_dimentions.x * sizeof(png_byte));
    for (int y = 0; y < image_dimentions.y; y++) {
        for (int x = 0; x < image_dimentions.x; x++) {
            int index = y * 3* image_dimentions.x + x * 3;
            row[x * 3] = content_to_encode[index];
            row[x * 3 + 1] = content_to_encode[index + 1];
            row[x * 3 + 2] = content_to_encode[index + 2];
        }
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, NULL);
    fclose(file_to_write);

    return true;
}
