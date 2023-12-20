import java.util.Hashtable;
//explanation for hashtable comes from book the book on page 161

public class SymbolTable extends HackAssembler{

   

   //same declaration as Code.java hash table.
   
   //"A natural means for representing this <symbol, address> mapping is any data
   //structure designed to handle <key, value> pairs. Every modern high- level 
   //programming language features such a ready-made abstraction, typically called
   // a hash table, map, dictionary, among other names."
   private Hashtable<String, Integer> address; 
   
   //Creates new empty symbol table.
   public SymbolTable(){
      address = new Hashtable<String, Integer>();
   }
   
   //Adds <symbol,address> to the table.
   //Symbol is a string and address is an int
   public void addEntry(String symbol, int address){
   //I didn't have time, but here would go the code to add Registers into RAM
   // at 1-15
   //as well as adding new variables into ROM starting at 16
   //then I would add all of the known values like SCREEN, KBD, SP, etc starting at around 16k
  
   }
   //check the symbol table to see if it contains the parameter symbol.
   public boolean contains(String symbol) {
      if(address.containsKey(symbol)){
         return true;
      }
   return false;
   
   }
   //returns the address asociated with the parameter symbol.
   public int getAddress(String symbol) {
      return address.get(symbol);
   
   }
   
   
}
