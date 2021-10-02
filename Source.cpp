// Author: L.Dey(2021)
                               
#define _CRT_SECURE_NO_WARNINGS
#include <string>     // for string class
#include <chrono>     // date & time functions
#include <ctime>      // date & time string functions
#include <iostream>   // for cout <<
#include <fstream>    // for file I/O
#include <iomanip>    // for fancy output
#include <functional> // for std::reference_wrapper
#include <vector>     // for vector container class


constexpr char ENCRYPTION_KEY = 's';                         
constexpr char const * STUDENT_NAME = "Luigi Dey"; 
constexpr int MAX_CHARS = 6;                                 // Minimum 6 character per user input                          
constexpr char STRING_TERMINATOR = '$';                      // custom string terminator
                               
char original_chars[MAX_CHARS];            // Original character string
char encrypted_chars[MAX_CHARS];           // Encrypted character string
char decrypted_chars[MAX_CHARS]; // Decrypted character string,

//---------------------------------------------------------------------------------------------------------------
//----------------- C++ FUNCTIONS -------------------------------------------------------------------------------

void get_char(char& a_character) //Here we get a reference to "a_character"; //a_character is an address ; get a character
{
    /* WHAT I DID FOR CONVERTING CHARS TO LOWER CASE:
       By looking at the binary representations of the characters in the ASCII table, you can notice that in the Hex column, there is a difference of 20 between 
       the lower case and upper case characters. 
       Therefore, to convert the user input from upper, to lower case, I just added 0x20 (in hex) to the user input.
       However, after debugging, I noticed that only new variable containing 'a_character', which is called 'new_character', had changed.
       **Check on line 106 for my solution to this. */


	bool keep_going = true;	     //This defines whether the loop should keep looping or not.
	
	__asm 
	{
whileloop:						//Beginning of the while loop
	}

	a_character = (char)_getwche();//https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getche-getwche //single character input	

	if (a_character == STRING_TERMINATOR) // skip further checks if user entered string terminator character
	{
		return;//Gets out of the loop
	}

	if (a_character == '\r' || a_character == '\n')  // treat new line/enter/carriage return characters as terminating character
	{
		a_character = STRING_TERMINATOR;//if \r and \n are the value of a_character then count it as a STRING_TERMINATOR ($)
		return;
	}

	int new_character = a_character; /*I did this because instead of treating a_character as a char, it would
									   treat it as an int - makes it easier to compare it to ints, as seen below. 
									   This also fixes the issue where if it was left as 'a_character', 
									   then even if I inputted 'h', it would output the error message.*/

	
	__asm 
	{
		cmp		new_character, 65  //65 is 'A' in Dec ASCII
		jnge    endfor			   //Check character is in upper case. If (new_character is not >= than A).
		cmp		new_character, 90  //90 is 'Z' in Dec ASCII
		jnle    endfor			   //Check character is in upper case. If (new_character is not <= than Z).
        add     new_character, 0x20 //Check text on start of function on why I did this.
	}

	__asm
	{
	endfor:
		cmp		new_character, 97  //97 is 'a' in Dec ASCII
		jnge	endif			   //Check character is in lower case if (new_character is not >= than a)
		cmp		new_character, 122 //122 is 'z' in Dec ASCII
		jnle	endif			   //Check character is in lower case if (new_character is not >= than z)
		jmp     done			   //No further checks needed if character is alphabetic: get out of the loop and process the character.
	}
	__asm
	{
	endif:						   //If new_character doesn't match the criteria above, then output error message.
	}
	
	std::cout << "\nAlphabetic characters only, please try again > "; 
	std::cout << original_chars; /* This makes it so that the user, after entering a wrong character, will continue from the
								    previously entered alphabetic character. */

	__asm
	{
		jmp		whileloop //Keep re-iterating until new_character is only alphabetic.
	done:				  //To get out of the loop.
	}

    a_character = new_character; /* Did this because just adding 0x20 to input to make it lowercase won't work: a_character, which is what the output is, did not change, 
                                    just 'new_character' did; for the change to be shown on the cmd, a_character needs to be changed too.*/

} 

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINE --------------------------------------------------------------------------


void encrypt_chars (int length, char EKey)
{
  char temp_char;                  //Character temporary store

  int test = length;               //Below, is the explanation on why I did this. Line 121.
                          
    __asm                          
    {             
      /* Used inlining for the following code. This used to have CDECL calling convention, but now I just replaced 'call encrypt_18' with actual routine. */
      /* I had an issue with 'int length' as 'LENGTH' operators have a limited meaning in inline ASM: they just return the num of elements in an array */
      /* So to sort this, I just tried putting length into another variable, which fixed this issue. */
      /* This new variable 'test' is only used within the FOR loop. */


     // Inputs: register EAX = the encrypted value of the source character (in the low 8-bit field, AL)
     //                  ECX = the encrypted value of the source character (in the low 8-bit field, CL)
     // Output: register EDX = the encrypted value of the source character (in the low 8-bit field, DL)


      /* for (int i = 0; i < length; ++i) */
      mov    ecx, 0                //Set counter to 0
      jmp    checkend              //See if loop should terminate immediately

      forloop : add ecx, 1         //Increment counter


      checkend : cmp ecx, test     //Sets flags based on result of counter - test (containing length of character input)
                 jge endfor        //If counter is greater than the length of the user input, now inside an integer variable test, then jump out of loop
                                   



      /*  temp_char = original_chars[i] */
      lea    ebx, [original_chars + ecx] //Quantity original_chars + ecx is placed in EBX. This because it will then be placed in EAX, then in temp_char, ready to be processed.
      movzx  eax, byte ptr[ebx]    //Using movzx because I am storing a char, and not an int: char is not as big as an int and therefore it needs to be padded out with zero's.
      mov    temp_char, al         //Then as on C++, move al - containing first letter of user input - in temp_char, ready to be encrypted.
                                   /* So if I use 'hello' as input, on first iteration, temp_char = 'h' */


      
      push   eax                   //Saving registers on the stack for future use.
      push   ecx                   //They need to be saved by the caller function.
      push   edx         
   
     
    
      movzx  ecx, temp_char        /* It extends zeroes out from the temp_char to fill up the rest of the space with 0's */
      lea    eax, EKey             /* 'lea' is used in this case because each routine alters the 'EKey' and its address and 'lea', by loading it to eax, makes sure it stays intact */
      push   eax                   /* Pushing parameters to the stack to allow string to be encrypted */
      push   ecx                   /* 'eax' = contains address of EKey ; 'ecx' = contains value of temp_char */

      mov   esi, eax                  //Copy value of EAX in ESI - as part of encryption routine. ESI is used as a placeholder for EAX.
      xchg  eax, [eax]                /* Exchange the EKey address with its value. EAX has now got the value of Ekey */
      and eax, 0x79                   /* This encrypts the character */
      ror   al, 3                     //rotate 3 bits right in al, lower bit of EAX 
                                      
      add   eax, 0x03                 //This is done as part of encryption routine.
      mov   edx, eax                  //This is done for future use - value of EDX will be added to ecx containing the actual ekey as part of encryption routine. 

      add   ecx, 0x02                 //adding 2 to temp_char which is part of encryption routine, this is the beginning of the string encryption routine.
      add   ecx, edx                  //Done as part of encryption routine.
      ror   cl, 1                     //rotate 1 bit right in cl (low bit right area of ECX) - this is done as part of encryption routine.
      mov[esi], eax                   //copy address of EAX into value of ESI.
      mov   edx, ecx                  //move the encrypted string into EDX, which will then be passed into EAX 


      mov eax, edx                    //This is done because on the following instructions, the lower 8-bit of eax, AL, is copied into temp_char


      add    esp, 8
      mov    temp_char, al        /*move back the contents of ecx into the variable*/
                                  /*because at this point the string has been encrypted*
                                  /*al stores the encrypted value of the source character, 
                                    which is then moved into 'temp_char', as this is the character temporary store*/
      
      
      pop    edx                   //pop is used to restore registers to previous value.
      pop    ecx                   
      pop    eax  
   

      /* encrypted_chars[i] = temp_char */
      lea    ebx, [encrypted_chars + ecx] /* This block of 3 lines of code, is the reverse of the block in line 131 */
      movzx  eax, temp_char               /* This step and the one below, ensure that the encrypted character is placed back in the encrypted_chars array */
      mov    byte ptr[ebx], al

      jmp    forloop                      /* Keep the FOR loop going until all charaters in temp_char have been encrypted */

      endfor:
  }
  
  return;

  //--- End of Assembly code
}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------






//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINE --------------------------------------------------------------------------

void decrypt_chars (int length, char EKey)
{
    char temp_char;                  // Character temporary store

    for (int i = 0; i < length; ++i) // Decrypt characters one at a time
    {
        temp_char = encrypted_chars[i]; // Get the next char from encrypted_chars array to start the decryption

        __asm
        {

            //EPILOGUE
            push   eax                   //Saving registers on the stack for future use.
            push   ecx                   //They need to be saved by the caller function.
            push   edx

            //CALL SITE BODY
            movzx  ecx, temp_char        /* It extends zeroes out from the temp_char to fill up the rest of the space with 0's */
            lea    eax, EKey             /* 'lea' is used in this case because each routine alters the 'EKey' and its address and 'lea', by loading it to eax, makes sure it stays intact */
            push   eax                   /* Pushing parameters to the stack to allow string to be decrypted */
            push   ecx                   /* 'eax' = contains address of EKey ; 'ecx' = contains value of temp_char, actual string inputted by user. */

            call   decrypt_18            /* run the 'decrypt_18' function below (from line 221). Used to decrypt the user string */


            add    esp, 8                /* Point the stack pointer at EAX */
            mov    temp_char, al         /* AL, the low 8-bit field in EAX, contains the hex number for the decrypted string, in my case 68;
                                            which is then moved into 'temp_char', as this is the character temporary store */


                                         //PROLOGUE
            pop    edx                   //pop is used to restore registers to previous value.
            pop    ecx
            pop    eax
        }

        decrypted_chars[i] = temp_char;     // Store decrypted char in the decrypted_chars array, as a way of storing data.
    }

    return;

    /* Decryption subroutine. You copy and paste the encryption routine from above (what I wrote), 
    delete the callee and restart again as decryption routine is completely different from encryption routine */


    // Inputs: register EAX = this register contains the original EKey. (In the low 8-bit field, AL)
    //                  ECX = this register contains the encrypted string. (In the low 8-bit field, CL)

    /* Outputs: register EDX = this register carries the encrypted/altered EKey from - in my case - line 244 -> "mov edx, eax";  
       It is then subtracted to ECX, to backtrack the encryption process, to get the original string. (In the low 8-bit field, DL) */
    //          register EAX = this register outputs the original string, which at this point (line 255 in my case OR "mov eax, edx") contains the decrypted string.
    

    __asm
    {
    decrypt_18:
    
        //PROLOGUE
        push  ebp                       /*push the EBP register on the stack to save its present value*/
        mov   ebp, esp                  /*copy the ESP value into the EBP - this is its new fixed value*/


        mov   ecx, [ebp + 8]            /* Retrieving the parameters ECX and EAX which were previously pushed on the stack at the call site */
        mov   eax, [ebp + 12]           /* We now have the original EKey */


        //BODY
        //this block of code below deals with the EKey
        /*The way I decrypted this, is by encrypting/changing the EKey (in EAX), the same way it was done in the encryption bit.
          Then I just rotated left (rol), subtracted it from ecx (which had the encrypted string), and subtracted 0x02, which gave me the original string, decrypted.*/

        mov   esi, eax      //Copy value of EAX in ESI - as part of decryption routine. ESI is used as a placeholder for EAX.
        xchg  eax, [eax]    //This gets a copy of EKey - done as part of the decryption routine to encrypt the EKey.
        and   eax, 0x79     //This adds 2 to the Ekey - done as part of the decryption routine to encrypt the EKey. 
        ror    al, 3        //Rotate right 3 times - done as part of decryption routine to encrypt the EKey.
        add   eax, 0x03     //Done as part of decryption routine to encrypt the EKey.
        mov   edx, eax      //EDX now has the encrypted EKey which will then be used to subtract from string.

        rol    cl, 1        //The 3 lines below are the inverted instructions from the encryption routine,
        sub   ecx, edx      //This is done to backtrack the way the string has been encrypted,
        sub   ecx, 0x02     //Which then allows me to get the original, decrypted string.
     

        mov [esi], eax      //At every iteration, the Encryption Key changes.
        mov   edx, ecx      //Copy the decrypted string to EDX, which is then copied to EAX and then outputted.
        mov   eax, edx

        //EPILOGUE

        mov   esp, ebp     //close the stack frame
        pop   ebp


        ret
    }
    //--- End of Assembly code
}


//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------





//************ MAIN *********************************************************************************************

void get_original_chars (int& length)
{
  length = 0;

  char next_char = 0;
  get_char (next_char);

  while ((length < MAX_CHARS) && (next_char != STRING_TERMINATOR))
  {
    original_chars[length++] = next_char;
    next_char = 0;
    get_char (next_char);
  }
}

std::string get_date ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  std::strftime (buf, sizeof(buf), "%d/%m/%Y", std::localtime (&now));
  return std::string{ buf };
}

std::string get_time ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  std::strftime (buf, sizeof (buf), "%H:%M:%S", std::localtime (&now));
  return std::string{ buf };
}

// support class to help output to multiple streams at the same time
struct multi_outstream
{
  void add_stream (std::ostream& stream)
  {
    streams.push_back (stream);
  }

  template <class T>
  multi_outstream& operator<<(const T& data)
  {
    for (auto& stream : streams)
    {
      stream.get() << data;
    }
    return *this;
  }

private:
  std::vector <std::reference_wrapper<std::ostream>> streams;
};

int main()
{
  int char_count = 0;  // The number of actual characters entered (upto MAXCHARS limit).

  std::cout << "Please enter up to " << MAX_CHARS << " alphabetic characters:  ";
  get_original_chars (char_count);	// Input the original character string to be encrypted 


  //*****************************************************
  // Open a file to store results (you can view and edit this file in Visual Studio)

  std::ofstream file_stream;
  file_stream.open ("EncryptDump.txt", std::ios::app);
  file_stream << "Name: " << STUDENT_NAME << "\nEncryption key = '" << ENCRYPTION_KEY << "'";
  file_stream << "\nDate: " << get_date () << " Time: " << get_time ();

  multi_outstream output;
  output.add_stream (file_stream);
  output.add_stream (std::cout);


  //*****************************************************
  // Display and save to the EDump file the string just input

  output << "\n\nOriginal string  = ";
  output << std::left << std::setw (MAX_CHARS) << std::setfill (' ') << original_chars;
  output << " Hex = ";

  // output each original char's hex value
  for (int i = 0; i < char_count; ++i)
  {
    int original_char = static_cast<int>(original_chars[i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::setw (2) << std::setfill ('0') << original_char << " ";
  }


  //*****************************************************
  // Encrypt the string and display/save the result

  encrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nEncrypted string = ";
  output << std::left << std::setw (MAX_CHARS) << std::setfill (' ') << encrypted_chars;
  output << " Hex = ";

  // output each encrypted char's hex value
  for (int i = 0; i < char_count; ++i)
  {
    int encrypted_char = static_cast<int>(encrypted_chars[i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::setw (2) << std::setfill ('0') << encrypted_char << " ";
  }


  //*****************************************************
  // Decrypt the encrypted string and display/save the result

  decrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nDecrypted string = ";
  output << std::left << std::setw (MAX_CHARS) << std::setfill (' ') << decrypted_chars;
  output << " Hex = ";

  // output each decrypted char's hex value
  for (int i = 0; i < char_count; ++i)
  {
    int decrypted_char = static_cast<int>(decrypted_chars[i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::setw (2) << std::setfill ('0') << decrypted_char << " ";
  }


  //*****************************************************
  // End program

  output << "\n\n";
  file_stream << "-------------------------------------------------------------\n\n";
  file_stream.close ();

  system("PAUSE"); // do not use in your programs! just a hack to pause the program before exiting

  return 0;
}
//*** end of whole program
//---------------------------------------------------------------------------------------------------------------
