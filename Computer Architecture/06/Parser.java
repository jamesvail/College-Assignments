import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.lang.String;
import java.io.File;


public class Parser extends HackAssembler{
   //"https://stackoverflow.com/questions/1419835/understanding-enums-in-java"
   //enums over static constants, no need to assign ints or values
   
   public enum InstructionType{
   A_INSTRUCTION, C_INSTRUCTION, L_INSTRUCTION
   }

   
   //two global variables, one for main file, another as a current cursor.
   private BufferedReader asm;
   private String currentInstruction;

   //constructor, opens the file and gets ready to parse it.
   //as talked about, catches nofilefound exception
   
   //"https://docs.oracle.com/javase/8/docs/api/java/io/BufferedReader.html"
   //after doing some research with multiple readers, I decided bufferedreader
   //was the best choice.
   public Parser(File file) throws IOException {
      //reads new file and sets it to global variable
      //"= new BufferedReader(new FileReader("foo.in"));"
      asm = new BufferedReader(new FileReader(file));
      advance(); //advances to the next instruction, essentially moving the line.
   }

   //check to see if the next line is null or not
   public boolean hasMoreLines() throws IOException {
      //marks the current spot in the file 
      asm.mark(100);
      //only read one character
      String nextLine = asm.readLine(); //set to 100 as we are in assembly language, max num of bytes which we can reset too
      if(nextLine == null) {
      asm.reset();
      return false;
      }
      asm.reset();
      return true;
   }

   //advances to next instruction. ignore every comment as well as whitespace
   //PREREQUISITE: HAS MORE LINES
   public void advance() throws IOException {
      String line;
      if(hasMoreLines()){
         //trim the line in order to get rid of all leading whitespaces (and trailing)
         // found from java docs string library 
         // "https://docs.oracle.com/javase/8/docs/api/java/lang/String.html"
         line = asm.readLine();
         line = line.trim();
      }else{
            currentInstruction = null;
            return;
      }
      //startsWith(String prefix) (java docs string library)
      //Tests if this string starts with the specified prefix.
      if(line.isEmpty() || line.startsWith("//") || line.startsWith("*")){
            currentInstruction = null;
            advance();
      }
      else{
            currentInstruction = line;
      }
   }

   // Returns the type of the current instruction
   public InstructionType instructionType() {
      //to figure out if it is an a instruction, we look for code like
      //@xxx (code with an @ in front of it, as specified by in parser API)
      if (currentInstruction.startsWith("@")) {
         return InstructionType.A_INSTRUCTION;
      //if we are about to loop or if we are reading a label,
      //we look for the parentheses in the beginning of the line. 
      } else if (currentInstruction.startsWith("(")) {
         return InstructionType.L_INSTRUCTION;
      //all other cases will lead to a C instruction, after trimming the line
      //and ruling out comments or whitespaces in the advance() function.
      } else {
         return InstructionType.C_INSTRUCTION;
      }
   }

   // returns just the plain symbol of a command
   //PREREQUISITE: ONLY A OR L INSTRUCTION
   
   //"If the current instruction is @17 or @sum, a call to symbol() would return the string "17"
   // or "sum", respectively. If the current instruction is (LOOP), a call to symbol() would return
   // the string "LOOP"." page 158
   public String symbol() {
      //first check to see if it is an A instruction, where we only need to remove the '@'
      if (instructionType() == InstructionType.A_INSTRUCTION) {
      //we return the substring starting at index 1, inclusive, gatheriung the rest of the symbol.
         return currentInstruction.substring(1);
         //ELSE IF L instruction
      } else if (instructionType() == InstructionType.L_INSTRUCTION){
         //just removing the parentheses, as starting the substring at the 0+1 index and
         // ending it at the length-1.
         return currentInstruction.substring(1, currentInstruction.length() - 1); // Exclude the '(' and ')'
      }else {}
      return null;
   }
   
   //returns only the plain symbol
   //just like symbol function, but for C Instruction
   //PREREQUISITE: ONLY CALLED IF ITS A C_INSTRUCTION
   
   //"If the current instruction is a call to dest (), comp (), and jump () (on D=D+1; JLE)
   // would return the strings "D", "D+1" and "JLE", respectively." page 158
   public String dest() {
      if(instructionType() != InstructionType.C_INSTRUCTION) {return null;}
      //find out where the = sign is so we can only grab the destination of the
      //command.
      int equalIndex = currentInstruction.indexOf('=');
      //not sure if this is needed, but just makes sure that the equals 
      //sign actually exists in the code.
      if (equalIndex != -1) {
         //if it does exist in the code, all we do is grab the substring up until the '='
         return currentInstruction.substring(0, equalIndex);
      }
         return null; //return null default for no dest
 
   }

   //COMP RETURNS D+1 WHEN D=D+1; JLE
   
   //PREREQUISITE: MUST BE C INSTRUCTION
   public String comp() {
      //find index of the = sign, just like dest, so we can grab the computation
      int equalIndex = currentInstruction.indexOf('=');
      //since there are so many possiblities as listed in the book api, we need to make sure we grab all the way till the semi colon
      //which is where the jump is located.

      int colonIndex = currentInstruction.indexOf(';');
      //check to make sure there is a = sign in the asm code
      if (equalIndex != -1) {
         //next check to see if there is a semicolon
         if (colonIndex != -1) {
            //if both is true, (aka if there is a JMP call) we take the whole substring
            //of everything after the equal sign to the semicolon, non inclusive of the semicolon.
            return currentInstruction.substring(equalIndex + 1, colonIndex);
         } else {
            //this is the else statement to the nested if, (if there is a semicolon)
            //if there is not, that means we don't have to worry about a jump call getting
            //read, and therefore can grab the whole rest of the line starting after the equal sign.
            return currentInstruction.substring(equalIndex + 1);
         }
        //if there is no equal sign, we still have possibilities to check as there can still
        //be a comp without one.
      } else {
         //may not be needed, but makes sure that this is a valid C instruction by making sure there
         //is indeed a semicolon.
         if (colonIndex != -1) {
            //no equal sign, so no dest, so we grab everything before the semicolon
            return currentInstruction.substring(0, colonIndex);
         } else {
            return null; //again, just a check to make sure is a c instruction.
         }
      }
   }

   //JUMP RETURNS JLE WHEN -> D=D+1; JLE
   //8 possibilities
   
   //PREREQUISITE: MUST BE C INSTRUCTION
   public String jump() {
      //grabs the semicolon index again for the substring return
      int colonIndex = currentInstruction.indexOf(';');
      //checks to make sure that there actually is a jump
      if (colonIndex != -1) {
         //returns everything after the semicolon, so would be JLE if the code was 
         // D=D+1; JLE
         return currentInstruction.substring(colonIndex + 1);
      } else {
         return null; //no JMP, may remove
      }
   }

   //closes asm reader at the end of a program; may integrate this into hasMoreLines when
   //the next line is NULL
   public void closeReader() throws IOException {
      asm.close();
   }
}