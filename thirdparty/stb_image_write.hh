#pragma once

extern "C" {
int stbi_write_png(char const *filename, int w, int h, int comp, const void *data, int stride_in_bytes);
int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
int stbi_write_tga(char const *filename, int w, int h, int comp, const void *data);
int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
int stbi_write_hdr(char const *filename, int w, int h, int comp, const float *data);
}