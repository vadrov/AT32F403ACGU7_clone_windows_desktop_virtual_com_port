/*
 *  Copyright (C)2023, VadRov, all right reserved.
 *
 *  Проект демонстрирует работу с виртуальным com портом.
 *  Воспроизведение стримингового видео.
 *	Версия для м/к ARTERY AT32F403A
 *
 *	Клиентская часть
 *  Данная программа для м/к в реальном времени непрерывно получает закодированные jpeg
 *  изображения по USB (виртуальному COM-порту клиенту) от компьютера (сервера),
 *  на стороне которого они и формируются (см. описание и код серверной программы).
 *  Полученные MCU-клиентом данные изображений декодируются из jpeg и выводятся на дисплей.
 *  Таким образом, изображение на мониторе компьютера дублируется на дисплее,
 *  подключенном к микроконтроллеру, выполняющему функции клиента, подключенного к серверу
 *  (компьютеру) посредством USB.
 *
 *  Допускается свободное распространение.
 *  При любом способе распространения указание автора ОБЯЗАТЕЛЬНО.
 *  В случае внесения изменений и распространения модификаций указание первоначального автора ОБЯЗАТЕЛЬНО.
 *  Распространяется по типу "как есть", то есть использование осуществляется на свой страх и риск.
 *  Автор не предоставляет никаких гарантий.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 */
#include "main.h"
#include "display.h" 		/* Драйвер дисплея */
#include "ili9341.h" 		/* Поддержка дисплея на базе контроллера ili9341 */
#include "jpeg_chan.h"

#include "usbd_core.h"
#include "cdc_class.h"
#include "cdc_desc.h"
#include "usbd_int.h"

#include <stdlib.h>
#include <string.h>

#define DISPLAY_DEBUG_DATA	1 /* Вывод некоторой отладочной информации на дисплей
								 (количество кадров в секунду, количество принимаемых байт в секунду)
							  	 1 - да, иное - нет
							   */

usbd_core_type usb_core_dev;
uint8_t buffer[24000];
volatile uint16_t buf_offs = 0;
volatile uint16_t len_buf = 0;
volatile uint8_t recieve_fl = 0;
volatile uint32_t millis = 0;


void usb_clock48m_select(usb_clk48_s clk_s)
{
  if(clk_s == USB_CLK_HICK)
  {
    crm_usb_clock_source_select(CRM_USB_CLOCK_SOURCE_HICK);

    /* enable the acc calibration ready interrupt */
    crm_periph_clock_enable(CRM_ACC_PERIPH_CLOCK, TRUE);

    /* update the c1\c2\c3 value */
    acc_write_c1(7980);
    acc_write_c2(8000);
    acc_write_c3(8020);

    /* open acc calibration */
    acc_calibration_mode_enable(ACC_CAL_HICKTRIM, TRUE);
  }
  else
  {
    switch(system_core_clock)
    {
      /* 48MHz */
      case 48000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1);
        break;

      /* 72MHz */
      case 72000000:
        crm_usb_clock_div_set(CRM_USB_DIV_1_5);
        break;

      /* 96MHz */
      case 96000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2);
        break;

      /* 120MHz */
      case 120000000:
        crm_usb_clock_div_set(CRM_USB_DIV_2_5);
        break;

      /* 144MHz */
      case 144000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3);
        break;

      /* 168MHz */
      case 168000000:
        crm_usb_clock_div_set(CRM_USB_DIV_3_5);
        break;

      /* 192MHz */
      case 192000000:
        crm_usb_clock_div_set(CRM_USB_DIV_4);
        break;

      default:
        break;

    }
  }
}

/* Настройка таймера с PWM (ШИМ) каналом для подсветки дисплея. */
static void Init_TMR(void)
{
	/*--------- Таймер TMR3 с каналом 4 PWM (для подсветки дисплея посредством) ----------*/
	crm_periph_clock_enable(CRM_TMR3_PERIPH_CLOCK, TRUE); //Тактирование таймера TIM3

	gpio_init_type  gpio_init_struct = {0};
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE); //Тактирование порта B

	/* Конфигурирование вывода PB1 - 4 канал таймера TMR3 */
	gpio_init_struct.gpio_pins = GPIO_PINS_1;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init(GPIOB, &gpio_init_struct);

	crm_clocks_freq_type crm_clocks_freq_struct = {0};
	crm_clocks_freq_get(&crm_clocks_freq_struct);

	/* Период счетчика таймера для частоты ШИМ 400 Гц с делителем таймера 1000 */
	uint16_t timer3_period = (crm_clocks_freq_struct.sclk_freq / (400 * 1000)) - 1;
	/* Скважность/заполнение 50% */
	uint16_t channel4_pulse = (timer3_period + 1) / 2 - 1;

	/* Базовая инициализация таймера: период счетчика, делитель частоты, направление счета */
	tmr_base_init(TMR3, timer3_period, 1000 - 1);
	tmr_cnt_dir_set(TMR3, TMR_COUNT_UP);

	tmr_output_config_type tmr_output_struct;
	tmr_output_default_para_init(&tmr_output_struct);
	tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
	tmr_output_struct.oc_output_state = TRUE;
	tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.oc_idle_state = TRUE;
	tmr_output_struct.occ_output_state = TRUE;
	tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
	tmr_output_struct.occ_idle_state = FALSE;

	/* Конфигурация 4 канала таймера */
	tmr_output_channel_config(TMR3, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
	/* Скважность ШИМ для 4 канала таймера */
	tmr_channel_value_set(TMR3, TMR_SELECT_CHANNEL_4, channel4_pulse);

	/* Разрешение выхода таймера */
	tmr_output_enable(TMR3, TRUE);

	/* Включение счетчика таймера */
	tmr_counter_enable(TMR3, TRUE);
}

/*
 * Настройка выводов м/к для использования в качестве управляющих сигналов
 * дисплея (CS, DC, RES), sd карты (CS) и индикаторного диода чтения sd карты
 * (LED_INDCTR).
 */
static void Init_GPIO(void)
{
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE); //Тактирование порта B
	crm_periph_clock_enable(CRM_GPIOC_PERIPH_CLOCK, TRUE); //Тактирование порта C

	/* Конфигурирование выводов PB6 (LCD_DC), PB7 (LCD_RES), PB8 (LCD_CS) */
	gpio_init_type gpio_init_struct;
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
	gpio_init_struct.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7 | GPIO_PINS_8;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOB, &gpio_init_struct);
	GPIOB->scr = GPIO_PINS_6 | GPIO_PINS_7 | GPIO_PINS_8; /* На выводах PB6, PB7, PB8 логическая 1. */
}

/* Инициализация SPI2 (дисплей) */
static void Init_SPI(void)
{
	gpio_init_type gpio_init_struct;
	gpio_default_para_init(&gpio_init_struct);

	dma_init_type dma_init_struct;
	dma_default_para_init(&dma_init_struct);

	spi_init_type spi_init_struct;
	spi_default_para_init(&spi_init_struct);

	/*-------------------------------- SPI2 - дисплей ---------------------------*/
	crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);  /* Тактирование SPI2 */
	crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE); /* Тактирование порта B */
		/*
	  	  Конфигурирование выводов PB13, PB15:
	  	  PB13 ---> SPI2_SCK
	  	  PB15 ---> SPI2_MOSI
		 */
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_MODERATE;
	gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = GPIO_PINS_13 | GPIO_PINS_15;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOB, &gpio_init_struct);

	/* Параметры DMA1_Channel1 - канал приема */
	crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE); //Тактирование контроллера DMA1
	nvic_irq_enable(DMA1_Channel1_IRQn, 2, 0); /* дисплей - передача (TX) */
	/* Привязываем к 1 каналу DMA1 исходящий поток SPI2 */
	dma_flexible_config(DMA1, FLEX_CHANNEL1, DMA_FLEXIBLE_SPI2_TX);
	dma_reset(DMA1_CHANNEL1);
	dma_init_struct.peripheral_base_addr = (uint32_t)&SPI2->dt;
	dma_init_struct.memory_base_addr = 0x0;
	dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
	dma_init_struct.buffer_size = 0x0;
	dma_init_struct.peripheral_inc_enable = FALSE;
	dma_init_struct.memory_inc_enable = TRUE;
	dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_BYTE;
	dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_BYTE;
	dma_init_struct.loop_mode_enable = FALSE;
	dma_init_struct.priority = DMA_PRIORITY_LOW;
	dma_init(DMA1_CHANNEL1, &dma_init_struct);

	/* Параметры SPI2 */
	spi_init_struct.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
	spi_init_struct.master_slave_mode = SPI_MODE_MASTER;
	spi_init_struct.mclk_freq_division = SPI_MCLK_DIV_2;
	spi_init_struct.first_bit_transmission = SPI_FIRST_BIT_MSB;
	spi_init_struct.frame_bit_num = SPI_FRAME_8BIT;
	spi_init_struct.clock_polarity = SPI_CLOCK_POLARITY_LOW;
	spi_init_struct.clock_phase = SPI_CLOCK_PHASE_1EDGE;
	spi_init_struct.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
	spi_init(SPI2, &spi_init_struct);
	/*------------------------------------------------------------------------*/
}

int main(void)
{
	/* Выбор 16 групп приоритетов прерываний (от 0 до 15) */
	NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_4);

	/* Конфигурирование частоты тактирования м/к */
	system_clock_config();

	/* Выбор линии тактирования SysTick */
	systick_clock_source_config(SYSTICK_CLOCK_SOURCE_AHBCLK_NODIV);

	/* Прерывания от SysTick каждую миллисекунду */
	SysTick_Config(system_core_clock/1000);

	/* Включение тактирования контроллера IOMUX (мультиплексор I/O) */
	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);

	/* select usb 48m clcok source */
	usb_clock48m_select(USB_CLK_HICK);

	/* enable usb clock */
	crm_periph_clock_enable(CRM_USB_PERIPH_CLOCK, TRUE);

	/* enable usb interrupt */
	nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 0, 0);

	/* usb core init */
	usbd_core_init(&usb_core_dev, USB, &cdc_class_handler, &cdc_desc_handler, 0);

	/* enable usb pull-up */
	usbd_connect(&usb_core_dev);

	Init_GPIO();	/* Настройка выводов м/к для использования в качестве управляющих сигналов
					   дисплея (CS, DC, RES), sd карты (CS) и индикаторного диода чтения sd карты
					   (LED_INDCTR). */
	Init_TMR(); 	/* Настройка таймера с PWM (ШИМ) каналом для подсветки дисплея. */
	Init_SPI();		/* Настройка SPI2 (для дисплея) */

	/*------------------- Настройка параметров и инициализация дисплея ---------------*/
	/* Подсветка ШИМ */
	LCD_BackLight_data bl_dat = { .htim_bk = TMR3,							/* Указатель на таймер (либо 0, если ШИМ не используется) */
		  	  	  	  	  	  	  .channel_htim_bk = TMR_SELECT_CHANNEL_4,	/* Канал таймера */
    							  .blk_port = 0,							/* Указатель на порт для подсветки по типу вкл./выкл.
    							  	  	  	  	  	  	  	  	  	  	  	   Либо 0 для ШИМ подсветки */
    							  .blk_pin = 0,								/* Номер вывода порта для подсветки по типу вкл./выкл.
    							                                               Либо 0 для ШИМ подсветки */
    							  .bk_percent = 90 };						/* Яркость подсветки (0...100), %
    							                                               Для 2 варианта подсветки > 0 - вкл. 0 - выкл.) */

	/* Данные используемого канала DMA для передачи данных по spi */
	LCD_DMA_TypeDef hdma = { .dma = DMA1,					/* Контроллер DMA (DMA1 или DMA) */
							 .stream = FLEX_CHANNEL1 };		/* Канал DMA (FLEX_CHANNEL1...FLEX_CHANNEL7) */

	/* Данные подключения (макросы портов и пинов см. в файле main.h) */
	LCD_SPI_Connected_data spi_dat = { .spi = SPI2,							/* Указатель на используемый spi */
  		  							   .dma_tx = hdma,						/* Данные DMA */
   									   .reset_port = LCD_RES_GPIO_Port,		/* Порт и номер вывода RES */
   									   .reset_pin = LCD_RES_Pin,
   									   .dc_port = LCD_DC_GPIO_Port,			/* Порт и номер вывода DC */
   									   .dc_pin = LCD_DC_Pin,
   									   .cs_port = LCD_CS_GPIO_Port,			/* Порт и номер вывода CS */
									   .cs_pin = LCD_CS_Pin  };
	#ifndef LCD_DYNAMIC_MEM
	LCD_Handler lcd1;
	#endif
	/* Для дисплея на контроллере ILI9341 */
	LCD = LCD_DisplayAdd( LCD,
	#ifndef LCD_DYNAMIC_MEM
		  	  	  	  	  &lcd1,
	#endif
		  	  	  	  	  240,
		   				  320,
						  ILI9341_CONTROLLER_WIDTH,
						  ILI9341_CONTROLLER_HEIGHT,
						  /* Задаем смещение по ширине и высоте для нестандартных или бракованных дисплеев: */
						  0,	/* смещение по ширине дисплейной матрицы */
						  0,	/* смещение по высоте дисплейной матрицы */
						  PAGE_ORIENTATION_LANDSCAPE,
						  ILI9341_Init,
						  ILI9341_SetWindow,
						  ILI9341_SleepIn,
						  ILI9341_SleepOut,
						  &spi_dat,
						  LCD_DATA_16BIT_BUS,
						  bl_dat );

	LCD_Handler *lcd = LCD; 	/* Указатель на первый дисплей в списке */
	LCD_Init(lcd); 				/* Инициализация дисплея */
	LCD_Fill(lcd, 0x319bb1);	/* Заливка дисплея заданным цветом */
	iPicture_jpg file;
	uint32_t old_time = millis, bytes = 0;
	char fps_txt[10] = "???";
	char bytes_txt[10] = "???";
	uint32_t frames_s = 0;
	char message[100];
	error_status res = ERROR;

	//Формируем сообщение, содержащее графическое разрешение дисплея
	/* sprintf(message, "MCU:%dX%d.", lcd->Width, lcd->Height); */
	strcpy(message, "MCU:");
	utoa(lcd->Width, &message[strlen(message)], 10);
	strcat(message, "X");
	utoa(lcd->Height, &message[strlen(message)], 10);
	strcat(message, ".");
	//Отправляем это сообщение компьютеру-серверу
	while (res != SUCCESS) {
		res = usb_vcp_send_data(&usb_core_dev, (uint8_t*)message, strlen(message));
	}

	while (1) {
		if (recieve_fl) {
			file.data = &buffer[buf_offs];
			file.size = len_buf;
			bytes += len_buf;

			buf_offs = 12000 - buf_offs;
			recieve_fl = len_buf = 0;

			LCD_Load_JPG_chan(lcd, 0, 0, lcd->Width, lcd->Height, &file, PICTURE_IN_MEMORY);
#if (DISPLAY_DEBUG_DATA == 1)
			LCD_WriteString(lcd, 8, lcd->Height - Font_8x13.height, fps_txt, &Font_8x13, COLOR_BLACK, COLOR_WHITE, LCD_SYMBOL_PRINT_FAST);
			LCD_WriteString(lcd, lcd->Width / 2, lcd->Height - Font_8x13.height, bytes_txt, &Font_8x13, COLOR_BLACK, COLOR_WHITE, LCD_SYMBOL_PRINT_FAST);
#endif
			frames_s++;
			if (millis - old_time >= 1000) {
				utoa(frames_s, fps_txt, 10);
				frames_s = 0;
				old_time = millis;
				utoa(bytes, bytes_txt, 10);
				bytes = 0;
			}
		}
		__NOP();
	}
}

/**
  * @brief  this function handles usb interrupt.
  * @param  none
  * @retval none
  */
void USBFS_L_CAN1_RX0_IRQHandler(void)
{
	usbd_irq_handler(&usb_core_dev);
}
