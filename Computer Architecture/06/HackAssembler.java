import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.BufferedWriter;

//also, just for reference, reminded myself of parent and child classes in java
//"https://www.codecademy.com/learn/learn-java/modules/learn-java-inheritance-and-polymorphism/cheatsheet#:~:text=In%20Java%2C%20a%20child%20class,inside%20the%20child%20class%20constructor."
//didn't forget about inheritancy, just had to be reminded of the syntax and
//just wanted to make sure I stated anytime I looked at other sources.


public class HackAssembler{ 
   //Information on how to read a file was gained from OS class, but as I haven't used java
   //in a while I confirmed on stackoverflow, link below:
   //"https://stackoverflow.com/questions/57949345/java-read-a-file-and-print-it-from-command-line-arguments"
   public static void main(String[] args) throws IOException {
      if(args.length==0){
         System.out.println("Correct Usage: HackAssembler.asm <.asm file>");
         return;
      }
      File asm = new File(args[0]);
      if(asm.canRead()&&asm.exists()){
      }else{
         System.out.println("Error with file");
         return;
      }
      
      //"https://www.w3schools.com/java/java_files_create.asp"
      //referenced this website on the bare basics of writing to files.
      //didn't look at much.
      String fileName = asm.getName().trim();
      int index = fileName.indexOf(".");
      //get the substring of everything up to the period, this is for the name of the new file.
      String tempName = fileName.substring(0, index);
      File binary = new File(tempName + ".hack");
      try (BufferedWriter binaryOutput = new BufferedWriter(new FileWriter(binary))){
      Parser parser = new Parser(asm);
      Code code = new Code();
      //scanner to read/parse the file written from args.
      //Scanner scnr = new Scanner(asm);
      //java docs file for interpreting file library
      //"https://docs.oracle.com/javase/8/docs/api/java/io/File.html"
      String instruction;
      String symbol;
      System.out.println(parser.hasMoreLines());
      while(parser.hasMoreLines()){
         System.out.println("in loop: " + parser.hasMoreLines());
         parser.advance();
         symbol = parser.symbol();
         if(symbol != null){
            //A or L Instruction case
            //do something with symbol here
         }
         else{
            //NEEDS TO BE C INSTRUCTION BELOW
               String comp = parser.comp();
               String dest = parser.dest();
               String jump = parser.jump();
               
               //"https://docs.oracle.com/javase/8/docs/api/java/lang/String.html"
               //java docs for learning how to concat strings
               instruction = "111";
               instruction = instruction.concat(code.comp(comp));
               if(dest!="null"){
               instruction = instruction.concat(code.dest(dest));
               }else {instruction = instruction.concat("000");}
               if(jump!="null"){
               instruction = instruction.concat(code.jump(jump));
               }else {instruction = instruction.concat("000");}

               System.out.println("instruction to write: " + instruction);
               binaryOutput.write(instruction+"\n");
            }
         
      }
      parser.closeReader();

      }
   }
}

