import java.util.Hashtable;
//decided to use a hashtable instead of a ton of if elses. got this ideaa from the textbook
//on page 161
//"A natural means for representing this <comp, address> mapping is any data structure designed to handle 
//<key, value> pairs. Every modern high- level programming language features such a ready-made abstraction, 
//typically called a hash table, map, dictionary,"
//while this was for the comp table, I thought it would also work very well in here as well.

//i was going to include an extra .txt file to read and loop it to add it into the hash table, but
//the assignment details dont call for extra files so i just thought it would be best to add it here.
//normally, to reduce code and improve cleanliness, i would have another file included.

public class Code extends HackAssembler{
   //"https://docs.oracle.com/javase/8/docs/api/java/util/Hashtable.html"
   //Grabbed how to initialize the hashtable from the java docs as well
   //as functions like put() and contains()
   
   //"https://www.geeksforgeeks.org/hashtable-in-java/"
   //also glanced at this website a little bit as i havent used a hash table
   //in a while. did not reference it too much, mostly consulted java docs.
   
   
   //different hashtables for different binary values, as dest and comp 
   //have some similar keys
   //declared up here so that all methods can use them
   private Hashtable<String, String> dest = new Hashtable<String, String>();
   private Hashtable<String, String> jump = new Hashtable<String, String>();
   private Hashtable<String, String> comp = new Hashtable<String, String>();
   
   //constructor for intiializing hashtable and 'putting'
   public Code(){
   
   
   //DEST CODES BELOW
   dest.put("null", "000");
   dest.put("M", "001");
   dest.put("D", "010");
   dest.put("DM", "011");
   dest.put("A", "100");
   dest.put("AM", "101");
   dest.put("AD", "110");
   dest.put("ADM", "111");

   
   //JUMP CODES BELOW
   jump.put("null", "000");
   jump.put("JGT", "001");
   jump.put("JEQ", "010");
   jump.put("JGE", "011");
   jump.put("JLT", "100");
   jump.put("JNE", "101");
   jump.put("JLE", "110");
   jump.put("JMP", "111");

   
   //COMP CODES BELOW, RETURNS 7 BITS (a==0 or a==1)
   
   comp.put("0", "0101010");
   comp.put("1", "0111111");
   comp.put("-1", "0111010");
   comp.put("D", "0001100");
   comp.put("A", "0110000");
   comp.put("M", "1110000");
   comp.put("!D", "0001101");
   comp.put("!A", "0110001");
   comp.put("!M", "1110001");
   comp.put("-D", "0001111");
   comp.put("-A", "0110011");
   comp.put("-M", "1110011");
   comp.put("D+1", "0011111");
   comp.put("A+1", "0110111");
   comp.put("M+1", "1110111");
   comp.put("D-1", "0001110");
   comp.put("A-1", "0110010");
   comp.put("M-1", "1110010");
   comp.put("D+A", "0000010");
   comp.put("D+M", "1000010");
   comp.put("D-A", "0010011");
   comp.put("D-M", "1010011");
   comp.put("A-D", "0000111");
   comp.put("M-D", "1000111");
   comp.put("D&A", "0000000");
   comp.put("D&M", "1000000");
   comp.put("D|A", "0010101");
   comp.put("D|M", "1010101");
 
   }
   
   
   //returns the binary code of the dest mnemonic, 
   //3 bits as a string
   public String dest(String destBits){
      return dest.get(destBits);

   
   }
   //returns the binary code of the comp mnemonic
   //returns 7 bits as a string
   public String comp(String compBits){
      return comp.get(compBits);
   }
   
   
   //returns the binary code of the jump mnemonic
   //returns 3 bits as a string
   public String jump(String jumpBits){
      return dest.get(jumpBits);
   }

}