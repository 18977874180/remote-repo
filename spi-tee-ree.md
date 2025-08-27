## 指纹在REE和TEE环境的功能区别
**`REE`**
* 接受中断
* reset pin 控制
* 上下电
* 上报亮灭屏消息
* open close spi-clk
* spi被tee接管前读一下chip-id

**`TEE`**
* 传输指纹数据
* 存储 匹配 处理 指纹数据
* 返回结果