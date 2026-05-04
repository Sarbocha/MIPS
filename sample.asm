# Demo: all required ISA ops. NOPs space dependent ops for this no-forwarding pipeline.
# R1=10, R2=5 -> ALU chain -> Mem[0]=50, LW to R8, BEQ skips ADDI 999, J skips ADDI 100.
ADDI 1, 0, 10
NOP
NOP
ADDI 2, 0, 5
NOP
NOP
ADD 3, 1, 2
NOP
NOP
SUB 4, 1, 2
NOP
NOP
MUL 5, 1, 2
NOP
NOP
AND 6, 3, 4
NOP
NOP
OR 7, 3, 4
NOP
NOP
SLL 1, 4, 2
NOP
NOP
SRL 2, 3, 1
NOP
NOP
SW 5, 0, 0
NOP
NOP
LW 8, 0, 0
NOP
NOP
BEQ 8, 5, toJump
NOP
NOP
ADDI 10, 0, 999
NOP
NOP
toJump: J endLabel
NOP
NOP
ADDI 11, 0, 100
endLabel: NOP
