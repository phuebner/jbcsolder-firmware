#include "stm32f7xx_hal.h"
#include "ili9341v.h"

/* Private defines */
#define ILI9341_RESET 0x01
#define ILI9341_SLEEP_OUT 0x11
#define ILI9341_GAMMA 0x26
#define ILI9341_DISPLAY_OFF 0x28
#define ILI9341_DISPLAY_ON 0x29
#define ILI9341_COLUMN_ADDR 0x2A
#define ILI9341_PAGE_ADDR 0x2B
#define ILI9341_GRAM 0x2C
#define ILI9341_MAC 0x36
#define ILI9341_PIXEL_FORMAT 0x3A
#define ILI9341_WDB 0x51
#define ILI9341_WCD 0x53
#define ILI9341_RGB_INTERFACE 0xB0
#define ILI9341_FRC 0xB1
#define ILI9341_BPC 0xB5
#define ILI9341_DFC 0xB6
#define ILI9341_POWER1 0xC0
#define ILI9341_POWER2 0xC1
#define ILI9341_VCOM1 0xC5
#define ILI9341_VCOM2 0xC7
#define ILI9341_POWERA 0xCB
#define ILI9341_POWERB 0xCF
#define ILI9341_PGAMMA 0xE0
#define ILI9341_NGAMMA 0xE1
#define ILI9341_DTCA 0xE8
#define ILI9341_DTCB 0xEA
#define ILI9341_POWER_SEQ 0xED
#define ILI9341_3GAMMA_EN 0xF2
#define ILI9341_INTERFACE 0xF6
#define ILI9341_PRC 0xF7

#define DISPLAY_MEMORY_BASE 0x60000000
#define DISPLAY_MEMORY_PIN 20


#define LCD_REG (*((__IO uint8_t *) ((uint32_t)(DISPLAY_MEMORY_BASE))))
#define LCD_RAM (*((__IO uint8_t *)((uint32_t)(DISPLAY_MEMORY_BASE | (1 << DISPLAY_MEMORY_PIN)))))
#if 0
#define FIX_REV1(X) ((X & 0xFC) | ((X >> 1) & 0x01) | ((X << 1) & 0x02))
#define COMMAND(X) (LCD_REG) = (FIX_REV1(X))
#define DATA(X) (LCD_RAM) = (FIX_REV1(X))
#else
#define COMMAND(X) (LCD_REG) = (X)
#define DATA(X) (LCD_RAM) = (X)
#endif

typedef enum
{
  INCREASE_ENABLE,
  INCREASE_DISABLE
} Transfer_Source_Pointer_Increase;

typedef struct
{
  uint16_t color_buffer;
  uint16_t *mem_source;
  uint32_t pixel_count;
  uint32_t dma_index;
  Transfer_Source_Pointer_Increase increase_mode;
  void (*user_transfer_complete_callback)(void);
} Display_Operation;

typedef enum {
	VERTICAL,
	HORIZONTAL,
	VERTICAL_180,
	HORIZONTAL_180,
} screen_rotation_t;

DMA_HandleTypeDef dma_handle;
Display_Operation current_operation;

uint32_t transfer_lock = 0;

void init_display_pins(void);
void init_fsmc(void);
void init_dma(void);
void delay_ms(uint32_t delay);
void dma_transfer_complete(DMA_HandleTypeDef *dmah);
void dma_transfer_error(DMA_HandleTypeDef *dmah);

void set_cursor_position(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void transfer_pixels(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *pixels,
                     Transfer_Source_Pointer_Increase, void (*operation_complete_callback)());

void dma_transfer_complete(DMA_HandleTypeDef *dmah)
{
  uint32_t dma_continu;
  uint32_t dma_source_address;

  if (current_operation.dma_index < current_operation.pixel_count)
  {
    dma_continu = current_operation.dma_index;
    if (current_operation.dma_index + 0xFFFF < current_operation.pixel_count)
    {
      current_operation.dma_index = current_operation.dma_index + 0xFFFF;
    }
    else
    {
      current_operation.dma_index = current_operation.pixel_count;
    }
    if (INCREASE_ENABLE == current_operation.increase_mode)
    {
      dma_source_address = (uint32_t)current_operation.mem_source + dma_continu;
    }
    else
    {
      dma_source_address = (uint32_t)current_operation.mem_source;
    }
    HAL_DMA_Start_IT(&dma_handle, dma_source_address, (uint32_t)&LCD_RAM, current_operation.dma_index - dma_continu);
  }
  else
  {
    //    free_lock(&transfer_lock);
	  if(current_operation.user_transfer_complete_callback != NULL)
		  current_operation.user_transfer_complete_callback();
  }
}

void dma_transfer_error(DMA_HandleTypeDef *dmah)
{
  //  free_lock(&transfer_lock);
  //  Throw(GD_EXCEPTION_DMA_NOT_READY);
}

void init_dma(void)
{
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

void display_init(void)
{
	//  if(false == get_lock(&transfer_lock)){
	//    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
	//  }

	/* HW reset of display */
	HAL_Delay(20); // Allows display to start up.

	/* Software reset */
	COMMAND(ILI9341_RESET);
	HAL_Delay(120);

	//************* Start Initial Sequence **********
	COMMAND(0xCF);
	DATA(0x00);
	DATA(0xAA);
	DATA(0XE0);
	COMMAND(0xED);
	DATA(0x67);
	DATA(0x03);
	DATA(0X12);
	DATA(0X81);
	COMMAND(0xE8);
	DATA(0x8A);
	DATA(0x01);
	DATA(0x78);
	COMMAND(0xCB);
	DATA(0x39);
	DATA(0x2C);
	DATA(0x00);
	DATA(0x34);
	DATA(0x02);
	COMMAND(0xF7);
	DATA(0x20);
	COMMAND(0xEA);
	DATA(0x00);
	DATA(0x00);
	COMMAND(0xC0); //Power control
	DATA(0x23); //VRH[5:0]
	COMMAND(0xC1); //Power control
	DATA(0x11); //SAP[2:0];BT[3:0]
	COMMAND(0xC5); //VCM control
	DATA(0x43);
	DATA(0x4c);
	COMMAND(0xC7); //VCM control2
	DATA(0xA0);

	COMMAND(0x36); // Memory Access Control / Display rotation
#if TFT_HOR_RES < TFT_VER_RES
	#ifdef TFT_FLIP
		DATA(0x48); //Vertical
	#else
		DATA(0x88); //Vertical flip
	#endif
#else
	#ifdef TFT_FLIP
		DATA(0x28); //Horizontal
	#else
		DATA(0xE8); //Horizontal flip
	#endif
#endif

	COMMAND(0x3A); // Memory Access Control
	DATA(0x05);
	COMMAND(0xB6); //Set Gamma
	DATA(0x0A);
	DATA(0x02);
	COMMAND(0xF2); // 3Gamma Function Disable
	DATA(0x00);
	COMMAND(0x26); //Gamma curve selected
	DATA(0x01);

	/* GAMMA */
	COMMAND(0xE0);
	DATA(0x1f); //p1 //0f
	DATA(0x36); //p2 //30
	DATA(0x36); //p3
	DATA(0x3A); //p4
	DATA(0x0C); //p5
	DATA(0x05); //p6
	DATA(0x4F); //p7
	DATA(0x87); //p8
	DATA(0x3C); //p9
	DATA(0x08); //p10
	DATA(0x11); //p11
	DATA(0x35); //p12
	DATA(0x19); //p13
	DATA(0x13); //p14
	DATA(0x00); //p15
	COMMAND(0xE1);
	DATA(0x00); //p1
	DATA(0x09); //p2
	DATA(0x09); //p3
	DATA(0x05); //p4
	DATA(0x13); //p5
	DATA(0x0A); //p6
	DATA(0x30); //p7
	DATA(0x78); //p8
	DATA(0x43); //p9
	DATA(0x07); //p10
	DATA(0x0E); //p11
	DATA(0x0A); //p12
	DATA(0x26); //p13
	DATA(0x2C); //p14
	DATA(0x1f); //p15

	COMMAND(0x11); //Exit Sleep
	HAL_Delay(120);
//	COMMAND(0x13); //Normal display mode
//	HAL_Delay(120);
//	COMMAND(0x21);
	COMMAND(0x29); //Display on

	HAL_Delay(50);

	init_dma();

  //  free_lock(&transfer_lock);
}

void set_cursor_position(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  if (x0 > x1 || x1 > DISP_HOR || y0 > y1 || y1 > DISP_VER)
  {
    //    Throw(GD_EXCEPTION_POS_OUT_OF_SCREEN);
  }
  COMMAND(ILI9341_COLUMN_ADDR);
  DATA(x0 >> 8);
  DATA(x0 & 0xFF);
  DATA(x1 >> 8);
  DATA(x1 & 0xFF);

  COMMAND(ILI9341_PAGE_ADDR);
  DATA(y0 >> 8);
  DATA(y0 & 0xFF);
  DATA(y1 >> 8);
  DATA(y1 & 0xFF);
}

void display_fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color,
                  void (*operation_complete_callback)())
{
  //  if(false == get_lock(&transfer_lock)){
  ////    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
  //  }
  current_operation.color_buffer = color;
  transfer_pixels(x0, y0, x1, y1, &current_operation.color_buffer, INCREASE_DISABLE, operation_complete_callback);
}

void display_bitmap(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *bitmap,
                    void (*operation_complete_callback)())
{
  //  if(false == get_lock(&transfer_lock)){
  ////    Throw(GD_EXCEPTION_LOCK_NOT_FREE);
  //  }
  transfer_pixels(x0, y0, x1, y1, bitmap, INCREASE_ENABLE, operation_complete_callback);
}

void transfer_pixels(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t *pixels,
                     Transfer_Source_Pointer_Increase source_pointer_increase, void (*operation_complete_callback)())
{
  if (dma_handle.State != HAL_DMA_STATE_READY)
  {
    //    Throw(GD_EXCEPTION_DMA_NOT_READY);
  }
  current_operation.pixel_count = (x1 - x0 + 1) * (y1 - y0 + 1);
  current_operation.mem_source = pixels;
  current_operation.user_transfer_complete_callback = operation_complete_callback;
  current_operation.increase_mode = source_pointer_increase;
  if (current_operation.pixel_count > 0xFFFF)
  {
    current_operation.dma_index = 0xFFFF;
  }
  else
  {
    current_operation.dma_index = current_operation.pixel_count;
  }

  if (INCREASE_DISABLE == source_pointer_increase)
  {
    dma_handle.Instance->CR &= ~((uint32_t)1 << 9); // Disable PINC bit (9) in CR register
  }
  else
  {
    dma_handle.Instance->CR |= DMA_PINC_ENABLE;
  }

  set_cursor_position(x0, y0, x1, y1);
  COMMAND(ILI9341_GRAM);
  HAL_DMA_Start_IT(&dma_handle, (uint32_t)current_operation.mem_source, (uint32_t)&LCD_RAM, current_operation.dma_index);
}

void DMA2_Stream0_IRQHandler(void)
{
  /* Check the interrupt and clear flag */
  HAL_DMA_IRQHandler(&dma_handle);
}
