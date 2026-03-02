# Kcstring / Nand2Tetris

> 本仓库记录了我基于 Nand2Tetris（从 NAND 到计算机系统）课程的学习与项目实现，从最基本的逻辑门到操作系统/编译器，逐步构建完整的简化计算机系统。官方课程网站：[nand2tetris.org](https://www.nand2tetris.org/)

## 项目概览

Nand2Tetris 是一门经典计算机系统构建课程，目标是从最基础的 NAND 门构建硬件电路，再逐步实现汇编器、虚拟机、编译器及简单操作系统，最终完成一台可运行程序的简化计算机。

本仓库包含了课程中硬件与软件部分的实现与练习，并在部分项目中参考了他人的实现。

## 仓库目录结构

/projects/                ← 各项目源码及测试文件  
/tools/                   ← 工具脚本或可执行文件（如官方工具 / 自实现脚本）  
其他实现文件（.hdl、.asm、.vm、.jack 等）根据项目分类存放  

- `projects/`：每个 project 文件夹包含官方模板、测试脚本、示例输入/输出及本人实现代码。官方项目编号从 `01` 到 `12`。  
- `tools/`：存放桌面版工具或脚本，可用于模拟 HDL 电路、汇编器、虚拟机等。  

## 项目映射（课程对应项目概览）

| 项目 | 内容概述 |
|------|----------|
| Project 01 | Boolean Logic：用 HDL 从 NAND 构建基本逻辑门（Not, And, Or, Xor, Mux 等） |
| Project 02 | Boolean Arithmetic：实现 16 位加法、布尔运算等 ALU 组合逻辑 |
| Project 03 | Sequential Logic / Memory：用触发器/寄存器构建 RAM 和寄存器单元 |
| Project 04 | Machine Language：汇编程序练习，理解机器语言与汇编关系 |
| Project 05 | Computer Architecture：将前面模块组合成完整 Hack CPU |
| Project 06 | Assembler：实现汇编器，将汇编代码转为机器码 |
| Project 07 | VM I: Stack Arithmetic：实现 VM 语言栈操作子集 |
| Project 08 | VM II: Program Control：扩展 VM 支持函数调用与程序控制 |
| Project 09 | High-Level Language：使用 Jack 编写应用与库 |
| Project 10 | Compiler I: Parsing：实现 Jack 语法解析器，输出 XML 表示 |
| Project 11 | Compiler II: Code Generation：实现 Jack → VM 编译器 |
| Project 12 | Operating System：实现简单操作系统服务（输入输出等） |

## 本仓库进度

- 硬件部分（Project 01–06）  
  主要在官方在线 IDE 完成，因此大部分源码未托管。  
- 软件部分（Project 07–12）  
  已基本完成，其中 OS 与编译器部分参考了一些博主/教程实现。


## 参考资料

- 官方课程网站：[nand2tetris.org](https://www.nand2tetris.org/)  
- 官方书籍：*The Elements of Computing Systems*  

