# WS-RPi2350-1.47-LCD-with-SD-Card
Arduino: RPi2350 1.47 LCD with SD-Card on GPIO- Interface. The actual version shows some System informations and an analog clock.
![IMG_0594_cut_comp](https://github.com/user-attachments/assets/b310cbd5-01ac-4ed5-817f-367a4c0c8aa5)

Analog Clock

![Pico_SD-OS](https://github.com/user-attachments/assets/2fbff618-61a3-431b-95c4-c61678a272d7)

This project based on the Waveshare RP2350 board witch 1.47in LCD, RGB-LED and SD- Card. It is really a compact board with a small and nice LCD.
But it should work with other Boards in different configurations.

Available commands are :
**CD, MD, RD** - directory handling

**CONFIG** - shows the SPI- configuration

**DEL** - removing file

**DIR** - shows the directory

**DATE** and **TIME** - get and set time/date value

**TYPE** - list an ASCII-File on your terminal

**CLS** - clear the VT100 screen and set the cursor position top-left

**TEMP** - get the RTC- chip (DS3231 if available) and CPU-internal (Die) temperature

**VER** - shows some software informations

**VOL** - list the SD-Card information

**XTRAN** - file transfer from mircocontroller to host via XMODEM protocol


Some of the Waveshare LCD- functions are modified and the memory transfer to the LCD are fixed. 
![IMG_1962-1_cut_comp](https://github.com/user-attachments/assets/44041312-dc29-402c-8022-37abfc868a69)
![IMG_0584_cut_comp](https://github.com/user-attachments/assets/25883f65-68be-4058-8885-b6e8334d9b79)

Software was tested with some other Raspberry Picos and Pico II
![IMG_0591_cut_comp](https://github.com/user-attachments/assets/859e0a6f-6c51-4a00-8087-96e6e0bd9fa5)
