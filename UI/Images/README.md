# Image Asset Management

## Overview
This directory contains PNG image assets that are automatically converted to C source files during the build process using the LVGL image converter.

## How It Works
1. Place PNG images in this directory
2. During build, CMake automatically runs the LVGL image converter (`3rd_party/lvgl/scripts/LVGLImage.py`)
3. Each PNG is converted to a corresponding C file with the same name
4. The generated C files are automatically included in the build

## Conversion Details
- **Input Format**: PNG images
- **Output Format**: C source files with LVGL image data
- **Color Format**: AUTO (automatically determined based on image content)
- **Variable Naming**: Hyphens in filenames are converted to underscores for valid C variable names

## Example
- `back-button.png` → `back_button.c` (with variable `back_button`)
- `settings-icon.png` → `settings_icon.c` (with variable `settings_icon`)

## Using Converted Images

### Include the Header
```c
#include "UI/Images/ui_images.h"  // Contains all image declarations
```

### Basic Usage
```c
// Create an image widget
lv_obj_t *img = lv_img_create(parent);
lv_img_set_src(img, &back_button);  // Reference the converted image
lv_obj_set_pos(img, 10, 10);       // Position it
```

### Available Images
The build system automatically generates variables for all PNG files:
- `back-button.png` → `back_button` variable
- `test-icon.png` → `test_icon` variable
- `settings-gear.png` → `settings_gear` variable (example)

### Image Properties
Each converted image is a `lv_image_dsc_t` structure with:
```c
back_button.header.w        // Width in pixels
back_button.header.h        // Height in pixels  
back_button.data_size       // Size in bytes
back_button.data            // Pointer to pixel data
```

### Complete Example
See `usage_example.c` in this directory for working code examples.

## Requirements
The build system requires Python 3 with the following packages:
- `pypng` (python3-png)
- `lz4` (python3-lz4)

These are automatically installed during the first build if not present.