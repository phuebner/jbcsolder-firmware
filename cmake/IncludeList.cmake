### INCLUDE DIRECTORIES STM32 DRIVERS
set(INCLUIDE_DIRS_STM32_DRIVERS
    ${PROJ_PATH}/Drivers/CMSIS/Include
    ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJ_PATH}/USB_DEVICE/App
    ${PROJ_PATH}/USB_DEVICE/Target
)

### INCLUDE DIRECTORIES LVGL
set(INCLUDE_DIRS_LVGL
    ${PROJ_PATH}/Drivers/lvgl
    ${PROJ_PATH}/Drivers/lvgl/src/lv_misc
)

set(INCLUDE_DIRS_UI
    ${PROJ_PATH}/UI
    ${PROJ_PATH}/UI/Components
    ${PROJ_PATH}/UI/Screens
    ${PROJ_PATH}/UI/Themes
)

set(INCLUDE_DIRS_APPLICATION
    ${PROJ_PATH}/Application
)

set(INCLUDE_DIRS_FIRMWARE
    ${INCLUIDE_DIRS_STM32_DRIVERS}
    ${INCLUDE_DIRS_LVGL}
    ${PROJ_PATH}/Config
    ${PROJ_PATH}/Core/Inc
    ${PROJ_PATH}/Core/Src
    ${PROJ_PATH}/Utilities
    ${PROJ_PATH}/Hardware/Devices
    ${PROJ_PATH}/Application
    ${PROJ_PATH}/UI/Driver
    ${INCLUDE_DIRS_UI}
)

set(INCLUDE_DIRS_SIMULATOR
    ${INCLUDE_DIRS_LVGL}
    ${PROJ_PATH}/Application
    ${PROJ_PATH}/Utilities
    ${INCLUDE_DIRS_UI}
    ${PROJ_PATH}/UI/Driver
    ${PROJ_PATH}/Simulator
)