### STARTUP SCRIPT
set(STARTUP_SCRIPT
    ${PROJ_PATH}/Core/Startup/startup_stm32f767vitx.s
)

### THIRD PARTY SOURCES
file(GLOB_RECURSE THIRD_PARTY_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Src/*c
    ${PROJ_PATH}/Drivers/lvgl/src/*.c
    ${PROJ_PATH}/Drivers/lvgl_custom_fonts/*.c
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*c
    ${PROJ_PATH}/USB_DEVICE/*c
)

### PROJECT SOURCES
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    ${PROJ_PATH}/Core/Src/*.c)

set(source_list ${source_list}
    ${THIRD_PARTY_SOURCES}
    ${PROJECT_SOURCES}
    ${STARTUP_SCRIPT}
)