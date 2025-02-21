#include "lvgl.h"

/*******************************************************************************
 * Size: 24 px
 * Bpp: 4
 * Opts:
 ******************************************************************************/

#ifndef LV_FONT_SYMBOLS_24
#define LV_FONT_SYMBOLS_24 1
#endif

#if LV_FONT_SYMBOLS_24

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t gylph_bitmap[] = {
    /* U+F011 "" */
    0x0, 0x0, 0x0, 0x0, 0x0, 0xaf, 0xf8, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xef, 0xfd, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x8, 0x60, 0x0, 0xef, 0xfd, 0x0,
    0x7, 0x70, 0x0, 0x0, 0x0, 0x1, 0xcf, 0xf3,
    0x0, 0xef, 0xfd, 0x0, 0x5f, 0xfb, 0x0, 0x0,
    0x0, 0xc, 0xff, 0xfb, 0x0, 0xef, 0xfd, 0x0,
    0xcf, 0xff, 0xb0, 0x0, 0x0, 0x9f, 0xff, 0xf6,
    0x0, 0xef, 0xfd, 0x0, 0x7f, 0xff, 0xf7, 0x0,
    0x3, 0xff, 0xff, 0x60, 0x0, 0xef, 0xfd, 0x0,
    0x8, 0xff, 0xff, 0x20, 0xa, 0xff, 0xf9, 0x0,
    0x0, 0xef, 0xfd, 0x0, 0x0, 0xbf, 0xff, 0x90,
    0x1f, 0xff, 0xe0, 0x0, 0x0, 0xef, 0xfd, 0x0,
    0x0, 0x1f, 0xff, 0xf0, 0x5f, 0xff, 0x90, 0x0,
    0x0, 0xef, 0xfd, 0x0, 0x0, 0xa, 0xff, 0xf3,
    0x8f, 0xff, 0x40, 0x0, 0x0, 0xef, 0xfd, 0x0,
    0x0, 0x6, 0xff, 0xf6, 0x9f, 0xff, 0x20, 0x0,
    0x0, 0xef, 0xfd, 0x0, 0x0, 0x4, 0xff, 0xf8,
    0x9f, 0xff, 0x20, 0x0, 0x0, 0xef, 0xfc, 0x0,
    0x0, 0x4, 0xff, 0xf7, 0x8f, 0xff, 0x40, 0x0,
    0x0, 0x37, 0x72, 0x0, 0x0, 0x6, 0xff, 0xf6,
    0x5f, 0xff, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xa, 0xff, 0xf4, 0x1f, 0xff, 0xe0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x1f, 0xff, 0xf0,
    0xb, 0xff, 0xf8, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0xaf, 0xff, 0xa0, 0x3, 0xff, 0xff, 0x60,
    0x0, 0x0, 0x0, 0x0, 0x7, 0xff, 0xff, 0x20,
    0x0, 0xaf, 0xff, 0xf8, 0x0, 0x0, 0x0, 0x1,
    0x9f, 0xff, 0xf8, 0x0, 0x0, 0xd, 0xff, 0xff,
    0xe8, 0x42, 0x24, 0x9e, 0xff, 0xff, 0xb0, 0x0,
    0x0, 0x1, 0xdf, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xfb, 0x10, 0x0, 0x0, 0x0, 0xa, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0x90, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x3b, 0xff, 0xff, 0xff, 0xff,
    0xb3, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x15, 0x89, 0x97, 0x51, 0x0, 0x0, 0x0, 0x0,

    /* U+F0C9 "" */
    0x13, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xfe, 0x13, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x10, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x57, 0x77, 0x77, 0x77,
    0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x5f, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xfc, 0xee, 0xee, 0xee, 0xee,
    0xee, 0xee, 0xee, 0xee, 0xee, 0xeb, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x33, 0x33,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x31,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe1,
    0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
    0x33, 0x31};

/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 384, .box_w = 24, .box_h = 24, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 288, .adv_w = 336, .box_w = 21, .box_h = 20, .ofs_x = 0, .ofs_y = -1}};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xb8};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
    {
        {.range_start = 61457, .range_length = 185, .glyph_id_start = 1, .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY}};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

/*Store all the custom data of the font*/
static lv_font_fmt_txt_dsc_t font_dsc = {
    .glyph_bitmap = gylph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 0,
    .bitmap_format = 0};

/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
lv_font_t lv_font_symbols_24 = {
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt, /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt, /*Function pointer to get glyph's bitmap*/
    .line_height = 24,                              /*The maximum line height required by the font*/
    .base_line = 3,                                 /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0)
    .underline_position = -9,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};

#endif /*#if LV_FONT_SYMBOLS_24*/
