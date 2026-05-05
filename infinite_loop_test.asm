# Infinite Loop Test
# Tests that J instruction correctly jumps back
# WARNING: Run with ctrl+c to stop

ADDI $1, $0, 1     # R1 = 1
ADDI $2, $0, 0     # R2 = 0 (counter)
loop: ADD $2, $2, $1   # R2 = R2 + 1
J loop              # jump back to loop forever