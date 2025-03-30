/**
 * @file lv_port_lcd.c
 *
 * Example implementation of the LVGL LCD display drivers on the STM32 platform
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "stm32f7xx.h"

#include "lv_port_disp.h"
#include "./src/drivers/display/lcd/lv_lcd_generic_mipi.h"

/*********************
 *      DEFINES
 *********************/
#ifndef MY_DISP_HOR_RES
#define MY_DISP_HOR_RES 240
#endif

#ifndef MY_DISP_VER_RES
#define MY_DISP_VER_RES 320
#endif

// The display buffer is located in external SDRAM memory area and commands and data are sent via FMC
#define DISPLAY_MEMORY_BASE 0x60000000
#define DISPLAY_MEMORY_PIN 20

#define CMD_FRMCTR1 0xB1 /* Frame Rate Control (In Normal Mode/Full Colors) */
#define CMD_FRMCTR2 0xB2 /* Frame Rate Control (In Idle Mode/8 colors) */
#define CMD_FRMCTR3 0xB3 /* Frame Rate control (In Partial Mode/Full Colors) */
#define CMD_INVCTR 0xB4  /* Display Inversion Control */
#define CMD_DFUNCTR 0xB6 /* Display Function Control */
#define CMD_PWCTR1 0xC0  /* Power Control 1 */
#define CMD_PWCTR2 0xC1  /* Power Control 2 */
#define CMD_VMCTR1 0xC5  /* VCOM Control 1 */
#define CMD_VMCTR2 0xC7  /* VCOM Control 2 */
#define CMD_PWCTRA 0xCB  /* Power Control A */
#define CMD_PWCTRB 0xCF  /* Power Control B */
#define CMD_GMCTRP1 0xE0 /* Positive Gamma Correction */
#define CMD_GMCTRN1 0xE1 /* Negative Gamma Correction */
#define CMD_DTCTRA 0xE8  /* Driver timing control A */
#define CMD_DTCTRB 0xEA  /* Driver timing control B */
#define CMD_PONSEQ 0xED  /* Power On Sequence */
#define CMD_RDINDEX 0xD9 /* ili9341 */
#define CMD_IDXRD 0xDD   /* ILI9341 only, indexed control register read */
#define CMD_ENA3G 0xF2   /* Enable 3 Gamma control */
#define CMD_IFCTR 0xF6   /* Interface Control */
#define CMD_PRCTR 0xF7   /* Pump ratio control */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
// static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi);
// static int32_t lcd_io_init(void);

static void lcd_init_dma(void);
static void dma_transfer_complete(DMA_HandleTypeDef *hdma);
static void dma_transfer_error(DMA_HandleTypeDef *hdma);

static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                         size_t param_size);
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_display_t *lcd_disp;
static DMA_HandleTypeDef dma_handle; // DMA handle for sending display data to LCD RAM
// static volatile int lcd_bus_busy = 0;

static const uint8_t init_cmd_list[] = {
    CMD_PWCTRB, 3, 0x00, 0xAA, 0xE0,
    CMD_PONSEQ, 4, 0x67, 0x03, 0x12, 0x81,
    CMD_DTCTRA, 3, 0x8A, 0x01, 0x78,
    CMD_PWCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    CMD_PRCTR, 1, 0x20,
    CMD_DTCTRB, 2, 0x00, 0x00,
    CMD_PWCTR1, 1, 0x23,
    CMD_PWCTR2, 1, 0x11,
    CMD_VMCTR1, 2, 0x43, 0x4c,
    CMD_VMCTR2, 1, 0xA0,
    // CMD_FRMCTR1, 2, 0x00, 0x13,
    CMD_DFUNCTR, 2, 0x0A, 0x02,
    // CMD_IFCTR, 3, 0x09, 0x30, 0x00,
    CMD_ENA3G, 1, 0x00,
    LV_LCD_CMD_SET_GAMMA_CURVE, 1, 0x01,
    CMD_GMCTRP1, 15, 0x1F, 0x36, 0x36, 0x3A, 0x0C, 0x05, 0x4F, 0x87, 0x3C, 0x08, 0x11, 0x35, 0x19, 0x13, 0x00,
    CMD_GMCTRN1, 15, 0x00, 0x09, 0x09, 0x05, 0x13, 0x0A, 0x30, 0x78, 0x43, 0x07, 0x0E, 0x0A, 0x26, 0x2C, 0x1F,
    LV_LCD_CMD_DELAY_MS, LV_LCD_CMD_EOF};

/**********************
 *      MACROS
 **********************/
// External SDRAM memory area for commands (REG) and data (RAM)
#define LCD_REG (*((__IO uint8_t *)((uint32_t)(DISPLAY_MEMORY_BASE))))
#define LCD_RAM (*((__IO uint8_t *)((uint32_t)(DISPLAY_MEMORY_BASE | (1 << DISPLAY_MEMORY_PIN)))))
#define LCD_COMMAND(X) (LCD_REG) = (X)
#define LCD_DATA(X) (LCD_RAM) = (X)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /* Initialize LCD I/O */
    // if (lcd_io_init() != 0)
    //     return;

    /* Create the LVGL display object and the ST7789 LCD display driver */
    lcd_disp = lv_lcd_generic_mipi_create(MY_DISP_HOR_RES, MY_DISP_VER_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);
    lv_lcd_generic_mipi_send_cmd_list(lcd_disp, init_cmd_list);
    // lv_display_set_rotation(lcd_disp, LV_DISPLAY_ROTATION_270); /* set landscape orientation */
    // lv_ili9341_set_invert(lcd_disp, true); /* do not invert colors */
    // lv_lcd_generic_mipi_set_address_mode(lcd_disp, true, false, true, false);

    /* Example: two dynamically allocated buffers for partial rendering */
    uint8_t *buf1 = NULL;
    uint8_t *buf2 = NULL;

    uint32_t buf_size = MY_DISP_HOR_RES * MY_DISP_VER_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    // lv_disp_set_rotation(lcd_disp, LV_DISP_ROTATION_90);

    buf1 = lv_malloc(buf_size);
    if (buf1 == NULL)
    {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = lv_malloc(buf_size);
    if (buf2 == NULL)
    {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Initialize the DMA */
    lcd_init_dma();

    lv_disp_set_default(lcd_disp);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Callback is called when background transfer finished */
// static void lcd_color_transfer_ready_cb(SPI_HandleTypeDef *hspi)
// {
//     /* CS high */
//     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//     lcd_bus_busy = 0;
//     lv_display_flush_ready(lcd_disp);
// }

static void lcd_init_dma(void)
{
    /* Initialize the DMA */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /*##-2- Select the DMA functional Parameters ###############################*/
    dma_handle.Init.Channel = DMA_CHANNEL_0;
    dma_handle.Init.Direction = DMA_MEMORY_TO_MEMORY;
    dma_handle.Init.PeriphInc = DMA_PINC_DISABLE;                  /* Peripheral increment mode */
    dma_handle.Init.MemInc = DMA_MINC_DISABLE;                     /* Memory increment mode */
    dma_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; /* Peripheral data alignment */
    dma_handle.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;    /* memory data alignment */
    dma_handle.Init.Mode = DMA_NORMAL;                             /* Normal DMA mode */
    dma_handle.Init.Priority = DMA_PRIORITY_HIGH;                  /* priority level */
    dma_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;                /* FIFO mode disabled */
    dma_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_handle.Init.MemBurst = DMA_MBURST_SINGLE;    /* Memory burst */
    dma_handle.Init.PeriphBurst = DMA_PBURST_SINGLE; /* Peripheral burst */

    dma_handle.Instance = DMA2_Stream0;

    if (HAL_DMA_Init(&dma_handle) != HAL_OK)
    {
        //    Throw(GD_EXCEPTION_DMA_INIT);
        return;
    }

    HAL_DMA_RegisterCallback(&dma_handle, HAL_DMA_XFER_CPLT_CB_ID, dma_transfer_complete);
    HAL_DMA_RegisterCallback(&dma_handle, HAL_DMA_XFER_ERROR_CB_ID, dma_transfer_error);

    /*##-6- Configure NVIC for DMA transfer complete/error interrupts ##########*/
    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);

    /* Enable the DMA STREAM global Interrupt */
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
}

void dma_transfer_complete(DMA_HandleTypeDef *hdma)
{
    // uint32_t dma_continue;
    // uint32_t dma_source_address;

    // if (current_operation.dma_index < current_operation.pixel_count)
    // {
    //     dma_continue = current_operation.dma_index;
    //     if (current_operation.dma_index + 0xFFFF < current_operation.pixel_count)
    //     {
    //         current_operation.dma_index = current_operation.dma_index + 0xFFFF;
    //     }
    //     else
    //     {
    //         current_operation.dma_index = current_operation.pixel_count;
    //     }
    //     if (INCREASE_ENABLE == current_operation.increase_mode)
    //     {
    //         dma_source_address = (uint32_t)current_operation.mem_source + dma_continue;
    //     }
    //     else
    //     {
    //         dma_source_address = (uint32_t)current_operation.mem_source;
    //     }
    //     HAL_DMA_Start_IT(&dma_handle, dma_source_address, (uint32_t)&LCD_RAM, current_operation.dma_index - dma_continue);
    // }
    // else
    // {
    //     //    free_lock(&transfer_lock);
    //     if (current_operation.user_transfer_complete_callback != NULL)
    //         current_operation.user_transfer_complete_callback();
    // }
    lv_disp_flush_ready(lcd_disp);
}

void dma_transfer_error(DMA_HandleTypeDef *dmah)
{
    //  free_lock(&transfer_lock);
    //  Throw(GD_EXCEPTION_DMA_NOT_READY);
}

void DMA2_Stream0_IRQHandler(void)
{
    /* Check the interrupt and clear flag */
    HAL_DMA_IRQHandler(&dma_handle);
}

/* Platform-specific implementation of the LCD send command function. In general this should use polling transfer. */
static void lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param,
                         size_t param_size)
{
    LV_UNUSED(disp);

    // Send the command to the external SDRAM memory area
    for (int i = 0; i < cmd_size; i++)
    {
        LCD_COMMAND(cmd[i]);
    }

    // Send the parameters to the external SDRAM memory area
    for (int i = 0; i < param_size; i++)
    {
        LCD_DATA(param[i]);
    }
}

/* Platform-specific implementation of the LCD send color function. For better performance this should use DMA transfer.
 * In case of a DMA transfer a callback must be installed to notify LVGL about the end of the transfer.
 */
static void lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param,
                           size_t param_size)
{
    LV_UNUSED(disp);

    if (dma_handle.State != HAL_DMA_STATE_READY)
    {
        return;
    }

    // Send the command to the external SDRAM memory area
    for (int i = 0; i < cmd_size; i++)
    {
        LCD_COMMAND(cmd[i]);
    }

    for (int i = 0; i < param_size; i++)
    {
        LCD_DATA(param[i]);
    }
    lv_disp_flush_ready(lcd_disp);
    // HAL_DMA_Start_IT(&dma_handle, (uint32_t)param, (uint32_t)&LCD_RAM, param_size);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
