### INCLUDE DIRECTORIES STM32 DRIVERS
set(INCLUIDE_DIRS_STM32_DRIVERS
    ${PROJ_PATH}/3rd_party/CMSIS/Include
    ${PROJ_PATH}/3rd_party/CMSIS/Device/ST/STM32F7xx/Include
    ${PROJ_PATH}/3rd_party/STM32F7xx_HAL_Driver/Inc
    ${PROJ_PATH}/3rd_party/STM32F7xx_HAL_Driver/Inc/Legacy
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJ_PATH}/USB_DEVICE/App
    ${PROJ_PATH}/USB_DEVICE/Target
)

### INCLUDE DIRECTORIES LVGL
set(INCLUDE_DIRS_LVGL
    ${PROJ_PATH}/3rd_party/lvgl
    ${PROJ_PATH}/3rd_party/lvgl/src/lv_misc
)

set(INCLUDE_DIRS_UI
    ${PROJ_PATH}/UI
    ${PROJ_PATH}/UI/Components
    ${PROJ_PATH}/UI/Screens
    ${PROJ_PATH}/UI/Themes
    ${PROJ_PATH}/UI/Images
    ${PROJ_PATH}/UI/Fonts
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
    ${PROJ_PATH}/Hardware/LvPort
    ${PROJ_PATH}/Application
    ${INCLUDE_DIRS_UI}
)

set(INCLUDE_DIRS_SIMULATOR
    ${INCLUDE_DIRS_LVGL}
    ${PROJ_PATH}/Application
    ${PROJ_PATH}/Utilities
    ${PROJ_PATH}/Hardware/Devices
    ${PROJ_PATH}/Simulator/Hardware/Devices
    ${INCLUDE_DIRS_UI}
    ${PROJ_PATH}/Simulator
)