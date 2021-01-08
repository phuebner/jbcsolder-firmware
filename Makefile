#
# Makefile
# WARNING: relies on invocation setting current working directory to Makefile location
# This is done in .vscode/task.json
#
MAKEFLAGS 			:= -j $(shell nproc)
SRC_EXT      		:= c
OBJ_EXT				:= o
CC 					:= gcc

SRC_DIR				:= ./
WORKING_DIR			:= ./build
BUILD_DIR			:= $(WORKING_DIR)/obj
BIN_DIR				:= $(WORKING_DIR)/bin
UI_DIR 				:= ui

WARNINGS 			:= -Wall -Wextra \
						-Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wno-discarded-qualifiers \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized \
						-Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default  \
					  	-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated  \
						-Wempty-body -Wshift-negative-value -Wstack-usage=2048 \
            			-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith

CFLAGS 				:= -O0 -g $(WARNINGS)

# Add simulator define to allow modification of source
DEFINES				:= -D SIMULATOR=1

# Include simulator inc folder first so lv_conf.h from custom UI can be used instead
INC 				:= -I./Simulator/ -I./ -I./Drivers/ -I./Drivers/lvgl/
LDFLAGS 			:= -lSDL2 -lm
BIN 				:= $(BIN_DIR)/simulation

COMPILE				= $(CC) $(CFLAGS) $(INC) $(DEFINES)

# Automatically include all source files
# SRCS 				:= $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '*/\.*')

SRCS				+= 	./Simulator/main.c \
						./Simulator/iron.c \
						./Simulator/hal_lvgl_encoder.c \
						./Simulator/mouse_cursor_icon.c \
					$(shell find $(SRC_DIR)Core/Src/gui -type f -name '*.c' -not -path '*/\.*') \
					$(shell find $(SRC_DIR)Simulator/lv_drivers -type f -name '*.c' -not -path '*/\.*') \
					$(shell find $(SRC_DIR)Drivers/lvgl_custom_fonts -type f -name '*.c' -not -path '*/\.*') \
					$(shell find $(SRC_DIR)Drivers/lvgl/src -type f -name '*.c' -not -path '*/\.*')



OBJECTS    			:= $(patsubst $(SRC_DIR)%,$(BUILD_DIR)/%,$(SRCS:.$(SRC_EXT)=.$(OBJ_EXT)))

all: default

$(BUILD_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(SRC_EXT)
	@echo 'Building project file: $<'
	@mkdir -p $(dir $@)
	@$(COMPILE) -c -o "$@" "$<"

default: $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $(BIN) $(OBJECTS) $(LDFLAGS)

clean:
	rm -rf $(WORKING_DIR)