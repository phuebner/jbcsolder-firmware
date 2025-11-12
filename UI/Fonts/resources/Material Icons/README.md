# Google Material Design Icons

This directory contains **Material Design Icons** font files from Google, which are part of the classic Material Icons set. These fonts provide a comprehensive collection of icons that follow Google's Material Design guidelines.

## 📁 Contents

This directory includes the following Material Icons font variants:

| Font File | Style | Description |
|-----------|-------|-------------|
| `MaterialIcons-Regular.ttf` | **Filled** | The original and most commonly used filled style |
| `MaterialIconsOutlined-Regular.otf` | **Outlined** | Outlined versions of the icons |
| `MaterialIconsRound-Regular.otf` | **Rounded** | Icons with rounded corners |
| `MaterialIconsSharp-Regular.otf` | **Sharp** | Icons with sharp, angular corners |
| `MaterialIconsTwoTone-Regular.otf` | **Two Tone** | Icons with two-tone styling |

Each font is accompanied by a corresponding `.codepoints` file that maps icon names to their Unicode values.

## 🌐 Source

- **Repository**: https://github.com/google/material-design-icons/tree/master/font
- **License**: Apache License 2.0
- **Browse Icons Online**: https://fonts.google.com/icons?icon.set=Material+Icons

## 🔍 Finding Icon Codes

### Method 1: Using Codepoints Files
Each `.codepoints` file contains mappings in the format:
```
icon_name unicode_hex_value
```

For example, in `MaterialIcons-Regular.codepoints`:
```
home e88a
settings e8b8
search e8b6
menu e5d2
```

### Method 2: Online Icon Browser
1. Visit https://fonts.google.com/icons
2. Use the dropdown menu (top left) to select "Material Icons"
3. Search for your desired icon
4. Click on the icon to see its name and Unicode value

## 🔧 Using with LVGL

### Step 1: Convert Font
Use the LVGL Font Converter to generate C arrays from these font files:
- **Online Converter**: https://lvgl.io/tools/fontconverter
- **Documentation**: https://docs.lvgl.io/master/details/main-modules/fonts/overview.html#font-symbols

### Step 2: Include Specific Icons
When converting, specify the Unicode ranges for the icons you need. For example:
- Home icon: `0xe88a`
- Settings icon: `0xe8b8`
- Multiple icons: `0xe88a,0xe8b8,0xe8b6`

### Step 3: Define Symbols in Code
Convert Unicode values to UTF-8 and create defines:

```c
// Example: Home icon (Unicode: 0xe88a)
// UTF-8 bytes: EE A2 8A
#define ICON_HOME "\xEE\xA2\x8A"

// Example: Settings icon (Unicode: 0xe8b8)  
// UTF-8 bytes: EE A2 B8
#define ICON_SETTINGS "\xEE\xA2\xB8"
```

### Step 4: Use in LVGL
```c
// Declare your converted font
LV_FONT_DECLARE(material_icons_24);

// Create label with icon
lv_obj_t *label = lv_label_create(parent);
lv_obj_set_style_text_font(label, &material_icons_24, 0);
lv_label_set_text(label, ICON_HOME);

// Or combine with text
lv_label_set_text(label, ICON_HOME " Home");
```

## 🛠️ Conversion Tips

### Font Converter Settings
- **Size**: Choose appropriate size (16, 20, 24, 32, 48 px are common)
- **BPP**: 1-bit for simple icons, 2-4 bit for better quality
- **Range**: Use specific Unicode values instead of full ranges to reduce size
- **Compression**: Enable to reduce memory usage

### Example Conversion Parameters
```
Name: material_icons_24
Size: 24
BPP: 2
Range: 0xe88a,0xe8b8,0xe8b6,0xe5d2
```

## 📋 Common Icons Reference

Here are some frequently used Material Icons with their Unicode values:

| Icon Name | Unicode | UTF-8 Bytes | Visual |
|-----------|---------|-------------|--------|
| home | `0xe88a` | `\xEE\xA2\x8A` | 🏠 |
| menu | `0xe5d2` | `\xEE\x97\x92` | ☰ |
| settings | `0xe8b8` | `\xEE\xA2\xB8` | ⚙️ |
| search | `0xe8b6` | `\xEE\xA2\xB6` | 🔍 |
| favorite | `0xe87d` | `\xEE\xA1\xBD` | ❤️ |
| star | `0xe838` | `\xEE\xA0\xB8` | ⭐ |
| arrow_back | `0xe5c4` | `\xEE\x97\x84` | ← |
| arrow_forward | `0xe5c8` | `\xEE\x97\x88` | → |
| close | `0xe5cd` | `\xEE\x97\x8D` | ✕ |
| check | `0xe5ca` | `\xEE\x97\x8A` | ✓ |

## 🔗 Additional Resources

- **Material Design Guidelines**: https://material.io/design/iconography/
- **LVGL Font Documentation**: https://docs.lvgl.io/master/details/main-modules/fonts/overview.html
- **Unicode to UTF-8 Converter**: http://www.ltg.ed.ac.uk/~richard/utf-8.cgi
- **Material Symbols** (newer variable font version): https://fonts.google.com/icons

## ⚠️ Important Notes

- **Material Icons vs Material Symbols**: These are the classic Material Icons (no longer updated since 2022). For new projects, consider using Material Symbols instead.
- **Font Loading**: Only include the icons you actually use to minimize memory usage.
- **Style Consistency**: Stick to one icon style (filled, outlined, etc.) for visual consistency.
- **LVGL Version**: Ensure your LVGL version supports the font format you're generating.

## 📄 License

These icon fonts are available under the **Apache License 2.0**. Feel free to use them in your projects. Attribution in your app's about screen is appreciated but not required.
