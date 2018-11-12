#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <png.h>

#define RETURN_IF_NULL(val, err_msg) if (val == NULL) { fputs(err_msg, stderr); return 1; }

bool decode_png(const char *input, const char *dest) {
    unsigned char header[8];
    FILE *fp = fopen(input, "rb");
    RETURN_IF_NULL(fp, "error: Could not open file");

    if (fread(header, 1, 8, fp) != 8) return false;
    if (png_sig_cmp(header, 0, 8)) return false;

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                 NULL, NULL, NULL);
    RETURN_IF_NULL(png_ptr, "error: Not enough memory");

    png_infop info_ptr = png_create_info_struct(png_ptr);
    RETURN_IF_NULL(info_ptr, "error: Not enough memory");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_byte channels = png_get_channels(png_ptr, info_ptr);

    if (bit_depth != 8)
        return false;

    printf("channels: %d\n", channels);
    printf("bit depth: %d\n", bit_depth);

    png_read_update_info(png_ptr, info_ptr);

    png_bytep *row_pointers = malloc(sizeof(png_bytep*) * height);
    for (int y=0; y<height; y++)
        row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

    printf("%ld, %d, %d\n", png_get_rowbytes(png_ptr, info_ptr), width, height);
    png_read_image(png_ptr, row_pointers);

    FILE *dest_file = fopen(dest, "wb");
    RETURN_IF_NULL(dest_file, "error: Unable to open file");
    printf("content size: %d\n", height * width * 3);

    if (channels == 3) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width * 3; x++) {
                fwrite(&row_pointers[y][x], sizeof(png_byte), 1, dest_file);
            }
        }
    }
    else if (channels == 4) {
        char chars[3];
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int r = row_pointers[y][x*4+0];
                int g = row_pointers[y][x*4+1];
                int b = row_pointers[y][x*4+2];
                int a = row_pointers[y][x*4+3];

                chars[0] = (r * a) / 255;
                chars[1] = (g * a) / 255;
                chars[2] = (b * a) / 255;

                // printf("%c%c%c", chars[0], chars[1], chars[2]);
                fwrite(chars, sizeof(char) * 3, 1, dest_file);
            }
        }
    }
    return true;
}
