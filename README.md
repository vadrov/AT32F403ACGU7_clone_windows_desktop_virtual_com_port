Copyright (C)2023, VadRov, all right reserved / www.youtube.com/@VadRov / www.dzen.ru/vadrov
# Cloning a Windows desktop image on a display connected to a microcontroller. Virtual com port. JPEG encoding and decoding.
The project demonstrates working with a virtual com port. Play streaming video (motion jpeg). The server (computer) constantly takes screenshots of the desktop, encodes them in jpeg and transfers them to the client (microcontroller) via USB (virtual COM port). The microcontroller decodes the image and shows it on the display.\
MCU - blackpill-artery at32f403acgu7\
Display - st7789 (and compatible). Supported by ili9341 - initialization string needs to be changed (set prefix ILI9341 in display add function)\
AT32F403A high-performance microcontrollers, powered by 32-bit ARM® Cortex®-M4 core, utilize advanced process to achieve 240 MHz computing speed. The embedded single precision floating-point unit (FPU) and digital signal processor (DSP), rich peripherals and flexible clock control mechanism can meet an extensive range of applications. The superior memory design supports up to 1 MB Flash memory and 224 KB SRAM, with the excellent Flash access zero wait far beyond the same level of the chip industry.
![at32f403acgu7 board](https://github.com/vadrov/AT32F403ACGU7_display_spi_dma_st7789_ili9341/assets/111627147/e2e03925-22c7-4f26-88f4-a398f9c42ef4)
```
Connection:
LCD_BLK ---> PB1
LCD_DC  ---> PB6
LCD_RES ---> PB7
LCD_CS  ---> PB8
LCD_SCL ---> PB13
LCD_SDA ---> PB15
```
1. Connect the microcontroller board to the USB connector of the computer.
2. Launch the terminal program from the folder of the same name.
3. Select the com port to which the microcontroller is connected.

If everything went well, then a copy of the Windows screen should be displayed on the display connected to the microcontroller:
![1704679158070](https://github.com/vadrov/stm32f401ccu6-usb-vcp-clone-windows-desktop-to-display/assets/111627147/105a61d5-7477-465b-ad95-42c0ddf2ef16)

Выводим изображение рабочего стола Windows на внешний дисплей с помощью микроконтроллера. USB CDC Virtual COM Port, кодер и декодер jpeg. Клонирование изображения рабочего стола Windows на дисплее, подключенном к микроконтроллеру. Виртуальный COM порт. JPEG кодирование и декодирование. Проект демонстрирует работу с виртуальным COM портом. Воспроизведение потокового видео (motion jpeg). Сервер (компьютер) постоянно делает скриншоты рабочего стола, кодирует их в jpeg и передает клиенту (микроконтроллеру) через USB (виртуальный COM порт). Микроконтроллер декодирует изображение и отображает его на дисплее. Что потребуется:\
Отладочная плата на базе микроконтроллера at32 - blackpill-artery at32f403acgu7\
Дисплей - st7789 или ili9341 (и совместимые с ними. Проект собран для дисплея st7789!).\
Как использовать:
1. Подключите дисплей к отладочной плате согласно таблице подключений:\
LCD_BLK ---> PB1\
LCD_DC  ---> PB6\
LCD_RES ---> PB7\
LCD_CS  ---> PB8\
LCD_SCL ---> PB13\
LCD_SDA ---> PB15
2. Подключите прошитую плату микроконтроллера к USB разъему компьютера (папка Release, файл для прошивки AT32F403ACGU7_clone_windows_desktop_virtual_com_port.hex Внимание. Прошивка для дисплея ST7789. При использовании дисплея ILI9341 необходимо пересобрать проект, указав требуемые параметры дисплея при вызове функции LCD_DisplayAdd в main).
3. Запустите серверную программу из папки Terminal\Release\Terminal.exe (это проект для компьютера).
4. Выберите com-порт, к которому подключен микроконтроллер. Узнать требуемый COM порт можно из диспетчера устройств. Он будет подписан, как "Устройство с последовательным интерфейсом USB (COM номер такой-то)" в разделе  "Порты (COM и LPT)". \
Если все сделано правильно, то на дисплее, подключенном к микроконтроллеру, вы увидите копию того  же изображения, что можете наблюдать на мониторе компьютера.

Author: **VadRov**\
Contacts: [Youtube](https://www.youtube.com/@VadRov) [Дзен](https://dzen.ru/vadrov) [VK](https://vk.com/vadrov) [Telegram](https://t.me/vadrov_channel)\
Donate: [donate.yoomoney](https://yoomoney.ru/to/4100117522443917)
