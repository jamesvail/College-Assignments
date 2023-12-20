// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

//start by initializing our instance variable i to 0
@i
M=0 //AKA writes 0 to initialized i.
//as well as initializing our R2 (our output) to 0.
// we initialize this to 0 because we need a clean output for our program,
// as well as our test script also tests this.
@R2
M=0 //writes 0 to R2.

//R0 Register
@R0 //selecting our R0 register (first multiple)
D=M //loads our first multiple (r) into our D register
@END
D;JEQ //0*n is always 0, so if first multiple (r0) is 0, we jump to the end


//R1 Register
//Same structure as above; loads R1 value (second multiple) into D register
//If value in D register is 0 (R1), we jump to end.
@R1
D=M
@END
D;JEQ


//START OF LOOP
(LOOP)
    // START LOOP CONDITION
    @i
    D=M //loads our i instance variable into D Register
    @R0 //selects first multiple
    D=D-M // sets D register to sum of i - R0
    @END
    // D register currently holds the output of i-R0. If these are equal
    // to eachother, our loop condition is met and we jump to END

    // The reason this works as a loop condition (IF R0 == i) is because
    //as we keep incrementing i, we keep executing the body of the loop 
    //which is incrementing R2 repeatedly.

    // Just to clarify, in this case I made R1 the value that is constantly
    //being added to R2, and R0 is the number of times it is to be added
    //so, (for(int i = 0; i < R0; i++))
    D;JEQ 
    // END LOOP CONDITION

    //BODY OF LOOP
    @R2 
    D=M //loads R2 (our output value) into the D register so we can wrangle it.
    @R1
    D=D+M //Adds our first multiple to the D register, D+=M OR R2+=R1
    @R2
    M=D //writes our edited D register into  R2.
    @i
    M=M+1 //increments i, our loop condition (i++)
    //END BODY OF LOOP

    //loads loop memory address into IR 
    @LOOP
    0;JMP //jumps to whatever is in the 0th register (in this case, LOOP ADRESS)
    //this is not a conditional statement, but an unconditional one. We rely on our
    //loop condition to jump to END.

//The start of end, initialized
(END)
    @END //loads END body address into our IR
    0;JMP //Same code as above but we have @END in our 0th register, or IR
    //this will keep jumping up and down in our END body, effectively ending the loop.