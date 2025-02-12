// Fill.asm
// 如果按下任意键，黑化屏幕；如果没有按键，清空屏幕

// 主循环开始
(LOOP)
@24576         // 加载键盘状态地址
D=M            // 将键盘状态值加载到 D 寄存器中
@BLACKEN       // 如果键盘状态值非零，跳转到 BLACKEN
D;JNE
@CLEAR         // 如果键盘状态值为零，跳转到 CLEAR
0;JMP

// 黑化屏幕
(BLACKEN)
@16384         
D=A             
@arr
M=D
@BLACK_LOOP
0;JMP          

(BLACK_LOOP)
@arr
D=M          
A=D
M=-1
D=D+1
@arr
M=D
D=M
@24576
D=A-D
@BLACK_LOOP
D;JGT          

@LOOP          // 回到主循环
0;JMP

// 清空屏幕
(CLEAR)
@16384         
D=A             
@arr
M=D
@CLEAR_LOOP
0;JMP
(CLEAR_LOOP)
@arr
D=M          
A=D
M=0
D=D+1
@arr
M=D
D=M
@24576
D=A-D
@CLEAR_LOOP
D;JGT          


@CLEAR_LOOP
D;JLT          // 如果未到达结束地址，继续清空屏幕

@LOOP          // 回到主循环
0;JMP

