/**
 * @file      pic16F84-isa.cpp
 * @author    Gabriel Renaldo Laureano - laureano@inf.ufsc.br
 *            Leonardo Taglietti       - leonardo@inf.ufsc.br
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:33:28 -0300
 * 
 * @brief     The ArchC PIC 16F84 functional model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 *
 */

#include "pic16F84-isa.H"
#include <systemc.h>
#include "ac_isa_init.cpp"
#include "constants.h"

//Uncomment this line to view the debug model 
#define DEBUG_MODEL
#ifdef DEBUG_MODEL

#include <stdio.h>
#include <stdarg.h>

inline int mydprintf(const char *format, ...)
{
    int ret;  
    va_list args;
    va_start(args, format);
    ret = vfprintf(stdout, format, args);
    va_end(args);
    return ret;
}
#else
#define mydprinf(str, args...)
#endif


//Auxiliar Status Register
sc_uint<8> status;

/**
 * Stack of instruction addresses.
 */
class {
  private:
    char tos;
    int addresses[8];
  public:
    void push(int address) {
      addresses[tos] = address;
      mydprintf("Position in Stack=%X\n", tos);
      tos = (tos + 1) % 8;
    }

    int pop() {
      tos = (tos + 7) % 8;
      mydprintf("Position in Stack=%X\n", tos);
      return addresses[tos];
    }

    int top() {
      return addresses[(tos + 7) % 8];
    }
} STACK;

/**
 * Method to read a register from reg bank.
 */
ac_word read( unsigned address )
{
    if (address == INDF)
    {
        address = BANK.read(FSR);
    }
    
    return BANK.read(address);
}

/**
 * Method to write in a register from reg bank.
 */
void write( unsigned address, ac_word datum  )
{
    if (address == INDF)
    {
        address = BANK.read(FSR);
    }
    
    BANK.write(address, datum);        
}

/**
 * Method to verify whether there is a carry out.
 */
bool carry_out(char value1, char value2)
{
    if ((value1 < 0) && (value2 < 0))
    {
        return true;
    }
    else
    {
        if ( !((value1 >= 0) && (value2 >= 0)) )
        {
           if (value2 >= -value1)
                return true;
        }
    }
    
    return false;
}

/**
 * Method to verify whether there is digit carry out.
 */
bool digit_carry_out(char value1, char value2)
{
    char sum;
    bool carry_out;

    value1 &= 0x0F;
    value2 &= 0x0F;

    sum  = value1 + value2;

    carry_out = sum >> 4;

    return carry_out;
}

/**
 * Method to verify whether there is a borrow.
 */
bool inline not_borrow(char value1, char value2)
{
    return ((unsigned char)value1 >= (unsigned char)value2);
}

/**
 * Method to verify whether there is a digit borrow.
 */
bool inline not_digit_borrow(char value1, char value2)
{
    return not_borrow(value1 << 4, value2 << 4);
}

/**
 * Method to read all STATUS Register.
 */
void read_status () {    
    status=read(STATUS);
    mydprintf("Current Value of STATUS Register: %X \nBITS: \n", read(STATUS));
    mydprintf("Z DC C\n");
    mydprintf("%d  %d %d", (status[Z] == 1?1:0), (status[DC] == 1?1:0), (status[C] == 1?1:0));
     
    mydprintf("\n\n");
}

/**
 * Method to write in the status reg.
 */
void write_status () {
    write(STATUS, status);
    mydprintf("STATUS Register Updated\n");
}

//!Generic instruction behavior method.
void ac_behavior( instruction ){
    mydprintf("PC= 0x%x \n", (int)ac_pc);
    ac_pc = ac_pc + 2;
    mydprintf("Instruction number ********************************************=  %X \n", ac_instr_counter);
    read_status();
};

//! Instruction Format behavior methods.
void ac_behavior( Format_Byte ){
  //Empty
}

void ac_behavior( Format_Bit ) {
  //Empty
}

void ac_behavior( Format_Literal ){
  //Empty
}

void ac_behavior( Format_Control ){
  //Empty    
}

//----------------------------------------------------------------------------------------------------------------
//---------Byte-Oriented File Register Operations-----------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------


//!Instruction ADDWF behavior method.
void ac_behavior( ADDWF ){
   mydprintf("%s 0x%x,%X \n","ADDWF",f,d);
   unsigned char result;
   result=read(f) + W.read();

   status[C]=carry_out(read(f), W.read());
   status[DC]=digit_carry_out(read(f), W.read());

   if (d==1) {
    write(f,result);
   }
   else {
    W.write(result);
    }

   status[Z]=(result == 0);
   write_status();
   mydprintf("Result: %X \n\n",result);
}

//!Instruction ANDWF behavior method.
void ac_behavior( ANDWF ){
   mydprintf("%s 0x%x,0x%x \n","ANDWF",f,d);
   unsigned char result;
   result=W.read() & read(f);

   if (d==1) {
      write(f,result);
   }
   else {
        W.write(result);
        }

   status[Z]=(result == 0);
   write_status();
   mydprintf("Result: %X \n\n",result);

}

//!Instruction CLRF behavior method.
void ac_behavior( CLRF ){
   mydprintf("%s 0x%x \n","CLRF",f);
   write(f,0);

   status[Z]=1;
   write_status();
   mydprintf("Result: %X \n\n",read(f));
}

//!Instruction CLRW behavior method.
void ac_behavior( CLRW ){
  mydprintf("%s\n","CLRW");
  W.write(0);

  status[Z]=1;
  write_status();
  mydprintf("Result: %X \n\n",W.read());
}

//!Instruction COMF behavior method.
void ac_behavior ( COMF ) {
   mydprintf("%s 0x%x,%X \n","COMF",f,d);
   unsigned char result;
   result=~read(f);

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
   }

  status[Z]=(result == 0);
  write_status();
  mydprintf("Result: %X \n\n",result);

}


//!Instruction DECF behavior method.
void ac_behavior( DECF ){
   mydprintf("%s 0x%x,%X \n","DECF",f,d);
   unsigned char result;
   result=read(f) - 1;

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
        }

  status[Z]=(result == 0);
  write_status();
  mydprintf("Result: %X \n\n",result);

}

//!Instruction DECFSZ behavior method.
void ac_behavior ( DECFSZ ) {
   mydprintf("%s 0x%x,%X \n","DECFSZ",f,d);
   unsigned char result;
   result=read(f) - 1;
   if (d==1) {
       write(f,result);
   }
   else {
        W.write(result);
   }

  if (result == 0 ) {
   ac_pc += 2;
   mydprintf("Result=0. Skip next instruction.\n");
  }
  else {
   mydprintf("Result=1. Run next instrucao! \n");
  }

  mydprintf("Result: %X \n\n",result);
}

//!Instruction INCF behavior method.
void ac_behavior ( INCF ) {
   mydprintf("%s 0x%x,%X \n","INCF",f,d);
   unsigned char result;
   result=read(f) + 1;

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
    }

  status[Z]=(result == 0);
  write_status();
  mydprintf("Result: %X \n\n",result);
}

//!Instruction INCFSZ behavior method.
void ac_behavior ( INCFSZ ) {
   mydprintf("%s 0x%x,%X \n","INCFSZ",f,d);
   unsigned char result;
   result=read(f) + 1;

  if (d==1) {
       write(f,result);
   }
   else {
        W.write(result);
   }

  if (result == 0 ) {
   ac_pc += 2;
   mydprintf("Result=0. Skip next instruction. \n");
  }
  else {
   mydprintf("Result=1. Run next instruction. \n");
  }

  mydprintf("Result: %X \n\n",result);
}

//!Instruction IORWF behavior method.
void ac_behavior ( IORWF ) {
   mydprintf("%s 0x%x,%X \n","IORWF",f,d);
   unsigned char result;
   result=W.read() | read(f);

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
   }

  status[Z]=(result == 0);
  write_status();
  mydprintf("Result: %X \n\n",result);
}


//!Instruction MOVF behavior method.
void ac_behavior ( MOVF ) {
   mydprintf("%s 0x%x,%X \n","MOVF",f,d);
   
   unsigned char result;
   result=read(f);

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
      }

  status[Z]=(result == 0);
  write_status();
  mydprintf("Result: %X \n\n",result);
}

//!Instruction MOVWF behavior method.
void ac_behavior( MOVWF ){      
    mydprintf("%s 0x%x\n","MOVWF", f);
    write(f, W.read());
    mydprintf("Result: %X \n\n", read(f));
}

//!Instruction NOP behavior method.
void ac_behavior ( NOP ) {
   mydprintf("%s\n\n","NOP");
}

//!Instruction RLF behavior method.
void ac_behavior ( RLF ) {
   mydprintf("%s 0x%x,%X \n","RLF",f,d);
   unsigned char result;
   result=read(f);
   status=read(STATUS);

   sc_uint<8> aux_carry;
   aux_carry=result;
   result=(result << 1) | status[C];

   if (d==1) {
       write(f,result);
   }
   else {
        W.write(result);
   }

   status[C]=aux_carry[7];
   write_status();
   mydprintf("Result: %X \n\n", result);
}

//!Instruction RRF behavior method.
void ac_behavior ( RRF ) {
   mydprintf("%s 0x%x,%X \n","RRF",f,d);
   unsigned char result;
   result=read(f);
   status=read(STATUS);

   sc_uint<8> aux_carry;
   aux_carry=result;
   result=(result >> 1) | (status[C]<<7);

   if (d==1) {
        write(f,result);
   }
   else {
        W.write(result);
   }

   status[C]=aux_carry[0];
   write_status();
   mydprintf("Result: %X \n\n", result);
}

//Instruction SUBWF behavior method.
void ac_behavior ( SUBWF ) {
   mydprintf("%s 0x%x,%X \n","SUBWF",f,d);
   mydprintf("%X - %X\n", read(f), W.read());
   unsigned char result;
   result=read(f) - W.read();

   status[C]= not_borrow(read(f), W.read());
   status[DC]= not_digit_borrow(read(f), W.read());
   status[Z]=(result == 0);  

   if (d==1) {
     write(f,result);
   }
   else {
     W.write(result);
   }

   write_status();
   mydprintf("Result: %X \n\n",result);
}

//!Instruction SWAPF behavior method.
void ac_behavior ( SWAPF ) {
   mydprintf("%s 0x%x,%X \n","SWAPF",f,d);
   unsigned char high,low,result;
   high= read(f) << 4;
   low = read(f) >> 4;
   mydprintf("New Values: HIGH: %X   LOW: %X \n", high,low);
   result = high | low;
   if (d==1) {
       write(f,result);
   }
   else {
        W.write(result);
   }

   mydprintf("Result: %X \n\n",result);
}

//!Instruction XORWF behavior method.
void ac_behavior ( XORWF ) {
   mydprintf("%s 0x%x,%X \n","XORWF",f,d);
   unsigned char result;
   result=W.read() ^ read(f);

   if (d==1) {
        write(f, result);
   }
   else {
        W.write(W.read() ^ read(f));
   }

   status[Z]=(result == 0);
   write_status();
   mydprintf("Result: %X \n\n",result);

} //------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------
//---------Bit Oriented File Register Operations-----------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

//!Instruction BCF behavior method.
void ac_behavior ( BCF ) {
   mydprintf("%s 0x%x,%X \n","BCF",f,b);
   unsigned char result,mask;

   mask = 0x01 << b;
   mask = ~mask;
   result = read(f);
   result = result & mask;
   write(f, result);

   mydprintf("Result: %X \n\n",result);

}

//!Instruction BSF behavior method.
void ac_behavior ( BSF ) {
   mydprintf("%s 0x%x,%X \n","BSF",f,b);
   unsigned char result,mask;
   mask = 0x01 << b;
   result = read(f);
   result = result | mask;
   write(f, result);

   mydprintf("Result: %X \n\n",result);

}

//!Instruction BTFSC behavior method.
void ac_behavior ( BTFSC ) {
  mydprintf("%s 0x%x,%X \n","BTFSC",f,b);
  unsigned char result,mask;

  mask = 0x01 << b;
  result=read(f) & mask;
  if (result == 0) {
    mydprintf("Bit %X = 0. Skip next instruction.\n", b );
    ac_pc += 2;
  }
  else {
    mydprintf("Bit %X = 1. Run next instruction.\n", b );
  }

  mydprintf("Result: %X \n\n",result);
}

//!Instruction BTFSS behavior method.
void ac_behavior ( BTFSS ) {
   mydprintf("%s 0x%x,%X \n","BTFSS",f,b);
   unsigned char result,mask;
   mask = 0x01 << b;
   result=read(f) & mask;
   if ( result != 0)   {
      mydprintf("Bit %X = 1. Skip next instruction. \n", b );
      ac_pc += 2;
   }
   else {
      mydprintf("Bit %X = 0. Run next instruction. \n", b );
   }

  mydprintf("Result: %X \n\n",result);
}
//----------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------
//---------Literal Operations-------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//!Instruction ADDLW behavior method.
void ac_behavior( ADDLW ){
   mydprintf("%s %X\n","ADDLW",k);
   unsigned char result;
   result=W.read() + k;

   status[C]=carry_out(W.read(), k);
   status[DC]=digit_carry_out(W.read(), k);
   status[Z]=(result == 0);

   W.write(result);

   write_status();
   mydprintf("Result: %X \n\n",result);
}

//!Instruction ANDLW behavior method.
void ac_behavior( ANDLW ){
    mydprintf("%s %X\n","ANDLW",k);
    unsigned char result;
    result = W.read() & k;
    W.write(result);

    status[Z]=(result == 0);
    write_status();
    mydprintf("Result: %X \n\n",result);
}


//!Instruction CLRWDT behavior method.
void ac_behavior ( CLRWDT ) {
    //Empty
}


//!Instruction IORLW behavior method.
void ac_behavior ( IORLW ) {
    mydprintf("%s %X\n","IORLW",k);
    unsigned char result;

    result=W.read() | k;
    W.write(result);

    status[Z]=(result == 0);
    write_status();
    mydprintf("Result: %X \n\n",result);
}

//!Instruction MOVLW behavior method.
void ac_behavior( MOVLW ){
   mydprintf("%s %X\n","MOVLW",k);
   W.write(k);

   mydprintf("Result: %X \n\n",W.read());
}

//!Instruction SUBLW ehavior method.
void ac_behavior( SUBLW ){
    mydprintf("%s %X\n","SUBLW",k);
    unsigned char result;
    result = k - W.read();

   status[C]=not_borrow(k, W.read());
   status[DC]=not_digit_borrow(k, W.read());
   status[Z]=(result == 0);

    W.write(result);
 
    write_status();
    mydprintf("Result: %X \n\n",result);
}

//!Instruction XORLW behavior method.
void ac_behavior( XORLW ){
    mydprintf("%s %X\n","XORLW",k);
    unsigned char result;
    result = W.read() ^ k;

    W.write(result);

    status[Z]=(result == 0);
    write_status();
    mydprintf("Result: %X \n\n",result);
}

//!Instruction RETLW behavior method.
void ac_behavior ( RETLW ) {
    mydprintf("%s %X\n","RETLW",k);
    W.write(k);
    ac_pc = STACK.pop();

    mydprintf("Result: %X \n\n",W.read());
}
//----------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------
//---------Control Operations-------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//!Instruction CALL behavior method.
void ac_behavior ( CALL ) {
  mydprintf("%s 0x%x\n","CALL",kaddress);

  STACK.push(ac_pc);
  ac_pc = kaddress;

  mydprintf("Value of PC in stack = 0x%x \n\n", STACK.top());
}


//!Instruction GOTO behavior method.
void ac_behavior ( GOTO ) {

  mydprintf("%s 0x%x\n","GOTO",kaddress);
  mydprintf("\t\t\t\t STATE: %X ",read(15));
  ac_pc = kaddress;

  mydprintf("Branch PC to value = 0x%x \n\n", (unsigned int) ac_pc);
}


//!Instruction RETFIE behavior method.
void ac_behavior ( RETFIE ) {
  mydprintf("%s\n","RETFIE");
  //Empty

  mydprintf("Branch PC to value = 0x%x \n\n", (unsigned int) ac_pc);
}

//!Instruction RETURN behavior method.
void ac_behavior ( RETURN ) {
  mydprintf("%s\n","RETURN");
  ac_pc = STACK.pop();

  mydprintf("Branch PC to value = 0x%x \n\n", (unsigned int) ac_pc);
}


//!Instruction SLEEP behavior method.
void ac_behavior ( SLEEP ) {
    //Empty
}

//----------------------------------------------------------------------------------------------------------------
