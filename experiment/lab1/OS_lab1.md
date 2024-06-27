# OS_lab1实验报告

## 思考题

### Thinking 1.1 编译链接

#### gcc命令

使用gcc编译器的`gcc -E file.c`， `gcc -c file.c` ，`gcc -o file file.c`命令可以分别生成编译器预处理、编译、链接之后产生的文件，通过查看相应代码可以看出，在实例`hello.c`生成可执行文件的过程中，`printf`的实现是在链接的过程中完成的。

#### ld命令

ld可用于将目标文件和库链接成可执行文件，它相当于执行了链接部分的功能。

命令格式：

```bash
ld [options] objfile...
```

gcc命令中，也会使用ld命令来完成链接工作。

#### readelf命令

readelf命令用于显示ELF可执行文件的相关信息，命令格式：

```bash
readelf -h <file> #显示头文件信息
readelf -S <file> #显示节表信息
readelf -s <file> #显示符号表信息
# ...
readelf -a <file> #显示所有信息
```

![readelf](.\readelf.png)

#### objdump命令

objdump命令用于反汇编目标文件或者可执行文件，可用于查看预处理，编译，链接之后产生的二进制文件。

传入参数的含义：

```bash
# objdump --help
-D, --disassemble-all    Display assembler contents of all sections
    --disassemble=<sym>  Display assembler contents from <sym>
-S, --source             Intermix source code with disassembly
    --source-comment[=<txt>] Prefix lines of source code with <txt>
```

可以看出，`-D`表示反汇编，而`-S`用于将源代码和反汇编代码共同显示出来。

#### MIPS 交叉编译工具链

使用MIPS 交叉编译工具链重新进行编译链接的过程，也就是在上述命令上加上`mips-linux-gnu-`前缀：

```shell
mips-linux-gnu-gcc -E hello.c > hello_mips.i #预处理

mips-linux-gnu-gcc -c hello.c -o hello_mips.o #编译
mips-linux-gnu-objdimp -DS hello_mips.o > hello_mips_o_DS

mips-linux-gnu-gcc -o hello_mips hello.c #链接
mips-linux-gnu-objdump -DS hello_mips > hello_mips_DS
```

反汇编产生的文件中，文件格式由`elf64-x86-64`变为`elf32-tradbigmips`。

### Thinking 1.2 readelf

使用`readelf`解析`target/mos`之后：

![readelf](.\readelf_mos.png)

使用`readelf -h`后：

![readelf](.\readelf_h_hello.png)

![readelf](.\readelf_h_readelf.png)

从`Makefile`文件中可以看出，在编译`hello`的时候，使用了`-m32 -static -g`选项，使得编译产生了一个32位的静态可执行文件，而`readelf`是64位的，因此无法解析`readelf`自己。

```shell
readelf: main.o readelf.o
	$(CC) $^ -o $@
hello: hello.c
	$(CC) $^ -o $@ -m32 -static -g
```

### Thinking 1.3 启动入口地址

大部分bootloader括stage1和stage2，stage1在ROM或者FLASH中执行，初始化硬件设备并为加载stage2准备RAM空间，在stage2中，会执行GRUB，GRUB会一步一步的加载自身代码，从而识别文件系统，从而将内核加载到内存并跳转，保证能够跳转到正确的内核位置，所以启动入口地址不一定是内核入口地址。

## 难点分析

本次实验的难点我觉得有两点：**理解ELF文件的结构**和**补全`vprintfmt()`函数**。

### ELF文件

首先，我们应该明确ELF文件的结构：

![elf](D:\lzq123\os\OS\experiment\lab1\elf.png)

明确在一个ELF文件中，ELF头、段头表（程序头表，program header table）、节头表（section header table）等概念，程序头表中包含`segment`的信息，节头表中包含`section`信息。

在exercise1.1中，我们需要在给定ELF头地址的情况下，输出文件中所有节头的地址信息。根据`elf.h`文件中ELF头结构体中的内容，我们发现可以在ELF头中找到节头表的地址偏移量和节头的数量以及大小，根据给出的ELF头地址，我们可以访问出以上信息，然后去遍历每一个节头即可。

```c
// Get the address of the section table, the number of section headers and the size of a
// section header.
const void *sh_table;
Elf32_Half sh_entry_count;
Elf32_Half sh_entry_size;
/* Exercise 1.1: Your code here. (1/2) */
sh_table = binary + ehdr->e_shoff;
sh_entry_count = ehdr->e_shnum;
sh_entry_size = ehdr->e_shentsize;
// For each section header, output its index and the section address.
// The index should start from 0.
for (int i = 0; i < sh_entry_count; i++) {
	const Elf32_Shdr *shdr;
	unsigned int addr;
	/* Exercise 1.1: Your code here. (2/2) */
	shdr =(const Elf32_Shdr *)(sh_table + i * sh_entry_size);
	addr = shdr->sh_addr;
	printf("%d:0x%x\n", i, addr);
}
```

我认为这部分内容中的难点在于搞清楚ELF文件的结构，以及去读懂`readelf.c`、`main.c`、`elf.h`文件中代码的功能，然后根据给出的提示将代码补全即可。

### `vprintfmt()`函数

我认为补全该函数的难点在于，需要去阅读附录和代码内容，明确printk格式，然后确定函数中是如何解析待输出的字符串的。以下是流程图：

<img src=".\vprintfmt_struct.png" alt="struct" style="zoom:18%;" />

## 实验体会

这次实验我认为还是有一定难度的，主要在于我对指导书的很多内容在第一次阅读的时候都不太理解，在进行实验的时候几乎都是摸索着去完成的，我认为想要做好这次的实验，必须要将指导书和代码相结合，在学习指导书内容是时候也要自己动手去试一试，明确不同文件之间的关系，然后多加尝试，同时还可以多和同学们进行讨论，互相分享心得。虽然过程比较痛苦，但是在完成实验，看到`print.c`文件正常运行的时候，我还是感到非常开心的，这时候再看一看指导书上的内容，感觉比刚开始理解的也更加深入了。
