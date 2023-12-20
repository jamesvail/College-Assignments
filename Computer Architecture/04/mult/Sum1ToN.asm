// Non nand2tetris file, modeled after a java file Sum1ToN



// Adds 1+…+100
	@i		// I refers to some mem. Location.
	M=1		//i = 1
	@sum		// sum refers to some mem. location
	N=0 		// sum=0
(LOOP)
	@I
	D=M		// D=i
	@100
	D=D-A 	// D=i-100
	@END
	D;JGT		// If (i-100)>0 goto END
	@i
	D=M	// D=i
	@sum
	M=D+M
	@i
	M=D+M	// sum=sum+i
	@i
	M=M+1	// i=i+1
	@LOOP
	D;JMP		// Goto LOOP
(END)
	@END
	D;JMP		// infinite loop