// Mult.asm
// 该程序计算R0 * R1，并将结果存储在R2中。
  @R1
  D=M
  @END
  D;JEQ

  (LOOP)
  @R0
  D=M
  @R2
  M=M+D
  @R1
  M=M-1
  @R1
  D=M
  @LOOP
  D;JGT

  (END)
  @END
  0;JMP
