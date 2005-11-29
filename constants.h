 /*******************************************************
 * The ArchC PIC 16F84 functional model.                *
 *                                                      *
 * Author:                                              *
 * Gabriel Renaldo Laureano - laureano@inf.ufsc.br      *
 * Leonardo Taglietti       - leonardo@inf.ufsc.br      *
 *                                                      *
 * For more information on ArchC, please visit:         *
 * http://www.archc.org                                 *
 *******************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

//Special Function Registers in Bank0
#define INDF   00
#define TMR0   01
#define PCL    02
#define STATUS 03
#define FSR    04
#define PORTA  05
#define PORTB  06
//Address 07 is a unimplemented data memory location
#define EEDATA 08
#define EEADR  09
#define PCLATH 10 //0A
#define INTCON 11 //0B
#define OPTION 0x81;
#define TRISA  0x85;
#define TRISB  0x86;
#define EECON1 0x88;
#define EECON2 0x89;


//Bits in OPTION_REG Register
#define   RBPU 7
#define INTEDG 6
#define   TOCS 5
#define   T0SE 4
#define    PSA 3
#define    PS2 2
#define    PS1 1
#define    PS0 0


//Bits in INTCON Register
#define   GIE  7
#define  EEIE  6
#define  T0IE  5
#define  INTE  4
#define  RBIE  3
#define  TOIF  2
#define  INTF  1
#define  RBIF  0

//Bits in STATUS Register
#define IRP 7
#define RP1 6
#define RP0 5
#define  TO 4
#define  PD 3
#define   Z 2
#define  DC 1
#define   C 0

//Other constants
#define ENABLE 1
#define UNABLE 0
#define W_TARGET 0
#define REG_TARGET 1
#define SET 1
#define CLEAR 0

#endif /* CONSTANTS_H */
