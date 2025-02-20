### STARTUP SCRIPT
set(STARTUP_SCRIPT
    ${PROJ_PATH}/Core/Startup/startup_stm32f767vitx.s
)

### STM32 DRIVER SOURCES
file(GLOB_RECURSE STM32_DRIVER_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/*c
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*c
    ${PROJ_PATH}/USB_DEVICE/*c
)

### LVGL COMMON SOURCES
file(GLOB_RECURSE LVGL_COMMON_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Drivers/lvgl_custom_fonts/*.c
    ${PROJ_PATH}/Drivers/lvgl/src/*.c
)

### FIRMWARE APPLICATION SOURCES
file(GLOB_RECURSE FIRMWARE_APPLICATION_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Core/Src/*.c)

### SIMULATOR SOURCES
file(GLOB_RECURSE SIMULATOR_APPLICATION_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Simulator/*.c
    ${PROJ_PATH}/Core/Src/gui/*.c
)

set(FIRMWARE_SOURCES
    ${STM32_DRIVER_SOURCES}
    ${LVGL_COMMON_SOURCES}
    ${FIRMWARE_APPLICATION_SOURCES}
    ${STARTUP_SCRIPT}
)

set(SIMULATOR_SOURCES
    ${LVGL_COMMON_SOURCES}
    ${SIMULATOR_APPLICATION_SOURCES}
)