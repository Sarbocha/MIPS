# Memory Loop Test
# Stores values 1-5 into Mem[0] through Mem[4]

ADDI $1, $0, 0     # R1 = base address (0)
ADDI $2, $0, 1     # R2 = value to store
ADDI $3, $0, 5     # R3 = loop limit
loop: SW $2, 0($1)      # Mem[R1] = R2
ADDI $1, $1, 1     # R1 = R1 + 1
ADDI $2, $2, 1     # R2 = R2 + 1
BEQ  $2, $3, end   # if R2 == 5, exit
J loop              # else loop
end: NOP