// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

@24576 //Loads the max/end keyboard value (found in book, page 113)
    // into the D register.
D=A
@MAXPIXEL //I created a variable here for the value.
//writes the value in D, max pixel, into our instance variable.
M=D



//Start of loop, or  for lamens terms, where the program will always
//default to so that it can wait for a key value. continously loops back to itself
// to wait a key press.
(LOOP)

    //sets the A register to the address of RAM register (represents pixels at the screens top row)
    @SCREEN
    D=A //after fetching screen value from A register, loads it into D register.
    @address
    M=D //writes our screen value (or d register value) into a saved variable.

    //to check keyboard value, we need to constantly select our KBD
    //and load it into D register for wrangling.
    @KBD
    D=M


    //The book, on page 115, states that the KBD value has a
    //constant value of 0 whenever no key is pressed.
    //"When a key is pressed on the physical keyboard, its 16-bit character code appears at RAM[KBD]. When no key is pressed, the code 0 appears."
    //Since we're only worried about whether a key is pressed or not pressed,
    //we don't have to check for which specific key is pressed, just if its greater than 0.

    //jumps to keytrue if a key is pressed (if kbd value isnt 0)
    @KEYTRUE
    D;JGT

    //jumps to keyfalse if a key is not pressed (if kbd value equals 0)
    @KEYFALSE
    D;JEQ

    //to continue checking for KBD value, jump back to loop.
    @LOOP
    0;JMP

(KEYFALSE)
    //get saved address variable (our screen address)
    @address
    D=M //load our screen address into D register
    @MAXPIXEL //load our max pixel variable

    //time to compare our screen address and MAX value
    //if the two values equal eachother, there is no need to whiten screen.
    // so we jump back to listening for a key. if the screen is black, we continue down
    // the 'function'
    D=M-D
    @LOOP
    D;JEQ



    //change our A register to our screen address instead of our loop memory address.
    //AKA loads our A register.
    @address
    A=M
    //sets our A register to 0, which points to our address
    M=0

    //here we load our address into our A register again so we can increment it,
    //which is the whole point of the function. By continously incrementing our address
    //value and setting the screen address to 0, we slowly whiten the screen by making
    //every pixel equal to 0.
    @address
    M=M+1

    //as stated before, we need to loop through to whiten the screen. our loop condition
    //is on line 71
    @KEYFALSE
    0;JMP

(KEYTRUE)

    //same structure as the key false body.

    //We start off with our loop condition; when D, our screen address, is equal to 
    // our max pixel value, we go back to listening to keyboard input and leave our mini loop.

    //Again, we load address into our A register but this time, instead of setting it to 0
    //(which is for a white screen) we set it to -1 for a black screen.

    //Then it's the same as KEYFALSE; we load our adress variable back into A register,
    //increment it by 1 to get to the next pixel, then jump back to the beginning of our function.

    //Again, the way this ends is when the loop invariant (on line 115) is true.


    @address
    D=M
    @MAXPIXEL
    D=M-D
    @LOOP
    D;JEQ

    @address
    A=M
    M=-1
    @address
    M=M+1

    @KEYTRUE
    0;JMP