MOVC,R1,#4 
MOVC,R2,#5  
MUL,R2,R1,R2 
ADD,R3,R1,R2
MOVC,R4,4000
STR,R1,R4,R2
HALT 
