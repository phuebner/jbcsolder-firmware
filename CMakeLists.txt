cmake_minimum_required(VERSION 3.22)

# Project Name    --- MUST EDIT ---
project(solderiron)
enable_language(C CXX ASM)

# Define the options to select build target
option(BUILD_SIMULATOR "Build the LVGL GUI simulator for PC" OFF)


# Setup cmake module path and compiler settings
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message("Build type: "              ${CMAKE_BUILD_TYPE})
set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS	ON)
set(PROJ_PATH                       ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler settings based on the target
if(BUILD_SIMULATOR)
	message(STATUS "Configuring build for simulator")	
	set(TARGET_NAME simulator)
	add_definitions(-DSIMULATOR)
	set(CMAKE_C_COMPILER            gcc)
	set(CMAKE_CXX_COMPILER          g++)
else()
	message(STATUS "Configuring build for firmware")
	set(TARGET_NAME firmware)
	add_definitions(-DFIRMWARE)
	set(CMAKE_TOOLCHAIN_FILE		gcc-arm-none-eabi)
endif()


# Include List, Source List, and MCU Sepecific Setting
# Must Update these files in cmake/ when adding new source files
include(SourceList)
include(IncludeList)

if(BUILD_SIMULATOR)
	set(EXECUTABLE		${CMAKE_PROJECT_NAME}-simulator)
	add_executable(${EXECUTABLE} ${SIMULATOR_SOURCES})
	target_include_directories(${EXECUTABLE} PRIVATE ${INCLUDE_DIRS_SIMULATOR})

	target_compile_options(${EXECUTABLE} PRIVATE -O0 -g -Wall -Wextra -Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wno-discarded-qualifiers 
	-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing -Wno-error=cpp -Wuninitialized 
	-Wno-unused-parameter -Wno-missing-field-initializers -Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default 
	-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic -Wno-sign-compare -Wno-error=missing-prototypes -Wdouble-promotion -Wclobbered -Wdeprecated 
	-Wempty-body -Wshift-negative-value -Wstack-usage=2048 -Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith)

	# target_compile_definitions(${EXECUTABLE} PRIVATE "SIMULATOR=1")
	# Linker options
	target_link_options(${EXECUTABLE} PRIVATE
		-lm
	)
	find_package(SDL2 REQUIRED SDL2)
	include_directories(${SDL2_INCLUDE_DIRS})
	target_link_libraries(${EXECUTABLE} ${SDL2_LIBRARIES})

	target_compile_definitions(${EXECUTABLE} PRIVATE "LV_LVGL_H_INCLUDE_SIMPLE")
	#target_link_libraries(${EXECUTABLE} PRIVATE SDL2)
    #target_link_libraries(${EXECUTABLE} SDL2) # Link SDL for the simulator
else()
	set(EXECUTABLE		${CMAKE_PROJECT_NAME}-fw)
	# Linker File     --- MUST EDIT ---
	set(MCU_LINKER_SCRIPT       ${PROJ_PATH}/STM32F767VITX_FLASH.ld)

	# MCU Parameters (FPU, HAL, etc.)
	include(STM32F767xx_HAL_PARA)

	add_executable(${EXECUTABLE} ${FIRMWARE_SOURCES})
	target_include_directories(${EXECUTABLE} PRIVATE ${INCLUDE_DIRS_FIRMWARE})

	# Project symbols
	target_compile_definitions(${EXECUTABLE} PRIVATE ${compiler_define} "LV_LVGL_H_INCLUDE_SIMPLE")

	# Compiler options
	set(WARNINGS
		all
		extra
		maybe-uninitialized
		strict-overflow=2
		switch
		write-strings
	)

	list(TRANSFORM WARNINGS PREPEND "-W")
	list(TRANSFORM DISABLE_WARNINGS PREPEND "-Wno-")

	target_compile_options(${EXECUTABLE} PRIVATE
		${CPU_PARAMETERS}
	)

	# Linker options
	target_link_options(${EXECUTABLE} PRIVATE
		-T${MCU_LINKER_SCRIPT}
		${CPU_PARAMETERS}
		-Wl,-Map=${CMAKE_PROJECT_NAME}.map
		--specs=nosys.specs
		#-u _printf_float                # STDIO float formatting support
		-Wl,--start-group
		-lc
		-lm
		-lstdc++
		-lsupc++
		-Wl,--end-group
		-Wl,--print-memory-usage
	)

	# Execute post-build to print size
	add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${EXECUTABLE}>
	)

	# Convert output to hex and binary
	add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.hex
	)

	# Convert to bin file -> add conditional check?
	add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${EXECUTABLE}> ${EXECUTABLE}.bin
	)
endif()