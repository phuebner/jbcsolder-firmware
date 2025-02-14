### INCLUDE DIRECTORIES
set(INCLUIDE_DIRS_THIRD_PARTY
    ${PROJ_PATH}/Drivers
    ${PROJ_PATH}/Drivers/CMSIS/Include
    ${PROJ_PATH}/Drivers/CMSIS/Device/ST/STM32F7xx/Include
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Inc
    ${PROJ_PATH}/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
    ${PROJ_PATH}/Drivers/lvgl/src
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
    ${PROJ_PATH}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc
    ${PROJ_PATH}/USB_DEVICE/App
    ${PROJ_PATH}/USB_DEVICE/Target
)

set(include_list ${include_list}
    ${INCLUIDE_DIRS_THIRD_PARTY}
    ${PROJ_PATH}/Core/Inc
    ${PROJ_PATH}/Core/Src
)