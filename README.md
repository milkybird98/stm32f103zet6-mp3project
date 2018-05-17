# stm32f103zet6-mp3project

一个基于stm32f103zet6的MP3

## Functions

- MP3模式：
  - 搜索文件并建立索引
  - 播放.mp3文件
  - 打开并显示.bmp文件
  - 打开并显示.txt文件（我也不知道为啥做这个）
- 定时播放器模式：
  - 搜索文件并建立索引
  - 较为友善的时间设定界面
  - 建立定时播放规则
  - 根据已有规矩播放.mp3文件

## Dependence

- HAL lib (CubeMX)
- Keil v5

## Hardware

- MCU: STM32F103zet6
- Sound Decode: vs1053b
- LCD Driver: ili9341
- one SD card

## Bugs

- 不稳定，有时会卡死
- 时钟在关机后无法保存
- 测试在开发板上进行，spi信号质量较差，导致时钟频率较低
