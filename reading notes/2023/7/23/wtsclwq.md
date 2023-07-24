# 第一章 部署工作环境

## 编译器

1. C语言编译器：GCC（我这里先用clang试一下）
2. 汇编语言编译器：NASM

## 宿主环境

> 什么是虚拟机？
>
> 用软件来模拟硬件。虚拟机只是一个普通的进程，该进程**模拟了硬件资源**，在虚拟机中运行的程序其所 做出的任何行为都**先被虚拟机检查**，由虚拟机分析后，**代为向操作系统申请。**

1. 外部环境：VisualBox+Centos6.5（我这里先用原生macos试一下）
2. 内部环境：bochs虚拟机（类似于qemu）

## 起步

1. 安装虚拟机

`brew install bochs`

2. 安装nasm

`brew install nasm`

3. 写一个汇编文件 `hx mbr.S `

   ```assembly
   SECTION MBR vstart=0x7c00                                                                                                                                                                                         
       mov ax, cs
       mov ds, ax
       mov es, ax
       mov ss, ax
       mov fs, ax
       mov sp, 0x7c00
       mov ax, 0xb800
       mov gs, ax
   
       mov ax, 0x600
       mov bx, 0x700
       mov cx, 0
       mov dx, 0x184f
   
       int 0x10
       
       mov byte [gs:0x00], '1' 
       mov byte [gs:0x01], 0xA4
       
       mov byte [gs:0x02], '2' 
       mov byte [gs:0x03], 0xA4
       
       mov byte [gs:0x04], '1' 
       mov byte [gs:0x05], 0xA4
       
       mov byte [gs:0x06], '1' 
       mov byte [gs:0x07], 0xA4
       
       jmp $
   
       times 510-($-$$) db 0
   
       db 0x55, 0xaa
   
   ```

4. 编译汇编文件

   `nasm -o mbr.bin mbr.S `

5. 创建磁盘镜像

   `bximage -hd=60M -imgmode="create" -q hd60M.img`

6. 将编译好的.bin程序写入磁盘镜像

   `dd if=mbr1.bin of=hd60M.img bs=512 count=1 conv=notrunc`

7. 创建虚拟机参数文件 `hx bochsrc.disk`

   ```json
   # 设置虚拟机内存为32MB                                                                                                                                                                                            
   megs: 32
   
   # 设置BIOS镜像
   romimage: file=$BXSHARE/BIOS-bochs-latest 
   
   # 设置VGA BIOS镜像
   vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest
   
   # 设置从硬盘启动
   boot: disk
   
   # 设置日志文件
   log: bochsout.txt
   
   # 关闭鼠标
   mouse: enabled=0
   
   # 打开键盘
   keyboard: type=mf, serial_delay=250
   
   # 设置硬盘
   ata0: enabled=1, ioaddr1=0x1f0, ioaddr2=0x3f0, irq=14
   
   ata0-master: type=disk, path="hd60M.img", mode=flat
   ```

8. 运行虚拟机

   `bochs -f bochsrc.disk`bochs -f bochsrc.disk

![image-20230724120223346](https://wtsclwq.oss-cn-beijing.aliyuncs.com/image-20230724120223346.png)
