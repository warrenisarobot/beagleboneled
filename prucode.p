// prucode.p
.setcallreg r29.w0

.origin 0

.entrypoint START




#include "prucode.hp"


#define GPIO1 0x4804c000
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194



//The WS2812b takes bytes in GRB order
//Bits are sent with the most significant bit first


//Use registers r0-r9 for passing params
//		r10-r19 for local data that can be re-written from other functions
//		r20-28 for global data that needs to persist within other functions
	

START:
          // Enable OCP master port
	
        LBCO      r0, CONST_PRUCFG, 4, 4
	CLR     r0, r0, 4         // Clear SYSCFG[STANDBY_INIT] to enable OCP master port
	SBCO      r0, CONST_PRUCFG, 4, 4

	// Configure the programmable pointer register for PRU0 by setting c28_pointer[15:0]
	// field to 0x0120.  This will make C28 point to 0x00012000 (PRU shared RAM).
	MOV     r0, 0x00000120
	MOV       r1, CTPPR_0
	ST32      r0, r1
	
	// Configure the programmable pointer register for PRU0 by setting c31_pointer[15:0]
	// field to 0x0010.  This will make C31 point to 0x80001000 (DDR memory).
	MOV     r0, 0x00100000
	MOV       r1, CTPPR_1
	ST32      r0, r1

	//Load values from external DDR Memory into Registers R0/R1/R2
	LBCO      r0, CONST_DDR, 0, 12

	//Store values from read from the DDR memory into PRU shared RAM
	SBCO      r0, CONST_PRUSHAREDRAM, 0, 12

	// test GP output
	LBCO      r0, CONST_PRUDRAM, 0, 4 //Load 4 bytes from memory location c3(PRU0/1 Local Data).  This is the lED count
	MOV	  r1, 4
	// Send notification to Host for program completion



BYTE_LOOP:
	// r0 - the number of bytes to process
	// r1 - the offset to start from for the first byte
	SUB	r0, r0, 1
	LBCO	r2, CONST_PRUDRAM, r1, 4	//value of current byte in r2
	CALL	SEND_BITS
	ADD	r1, r1, 1
	QBNE	BYTE_LOOP, r0, 0
	CLR	r30.t14
	// Send notification to Host for program completion
	MOV	r31.b0, PRU0_ARM_INTERRUPT+16

    // Halt the processor
    MOV		r0, 1
    MOV		r1, 255
    LSL		r0, r0, 2
    AND		r0, r0, r1
    SBCO      r4, CONST_PRUDRAM, 0, 4 //Load 4 bytes from memory location c3(PRU0/1 Local Data).  This is the lED count	
    HALT


//this expects R2 to have a byte value we are going to traverse the bits of
//
SEND_BITS:
	MOV	r3, 0	//r3 is bit counter
	MOV	r5, 255	//The 1 bit AND mask, this will rightt shift as we iterate (sending most-significant bit first)
	//r29 is used for storig the address of this function on a CALL, but we were
	//CALLed from above, we want to store this address since it will be overwritten
	//through our CALLs below
	MOV	r28, r29
SEND_BITS_LOOP:
	ADD	r3, r3, 1	//increase bit position counter
	MOV	r4, r2		//copy the byte we're using into r4 - to be masked
	AND	r4, r4, r5	//r5 has the AND mask to use
	QBNE	SEND_BITS_0, r4, 0
//This is a non-zero value, meaning the bit is a 1
	CALL	CODE0
	//CALL	BLINKLONG
	JMP	SEND_BITS_DONE
SEND_BITS_0:
	CALL	CODE1
	//CALL 	BLINKSHORT
SEND_BITS_DONE:
	LSR	r5, r5, 1
	QBNE	SEND_BITS_LOOP, r3, 8
	MOV	r29, r28
	RET

	
	
//0 =  High: 0.4us (400ns) , Low: 0.85us (850ns)
//Each clock cycle is 5ns
CODE0:
	SET	r30.t14
	MOV	r10, 39 //400ns = 80 clock cycles
CODE0_LOOP_HIGH:
	SUB	r10, r10, 1
	QBNE	CODE0_LOOP_HIGH, r10, 0
	CLR	r30.t14
	MOV	r10, 84//850ns = 170 clock cycles
CODE0_LOOP_LOW:	
	SUB	r10, r10, 1
	QBNE	CODE0_LOOP_LOW, r10, 0
	//leave it at low, we're done now
	RET
	

//1 =  High: 0.8us (800ns) , Low: 0.45us (450ns)
//Each clock cycle is 5ns
CODE1:
	SET	r30.t14
	MOV	r10, 79 //800ns = 160 clock cycles
CODE1_LOOP_HIGH:
	SUB	r10, r10, 1
	QBNE	CODE1_LOOP_HIGH, r10, 0
	CLR	r30.t14
	MOV	r10, 44//450ns = 90 clock cycles
CODE1_LOOP_LOW:	
	SUB	r10, r10, 1
	QBNE	CODE1_LOOP_LOW, r10, 0
	//leave it at low, we're done now
	RET


BLINKLONG:
	SET	r30.t14
	MOV	r10, 25000000
BLINKLONG_ON:
	SUB	r10, r10, 1
	QBNE	BLINKLONG_ON, r1, 0
	CLR	r30.t14
	MOV	r10, 25000000
BLINKLONG_OFF:	
	SUB	r10, r10, 1
	QBNE	BLINKLONG_OFF, r10, 0
	RET


BLINKSHORT:
	SET	r30.t14
	MOV	r10, 5000000
BLINKSHORT_ON:
	SUB	r10, r10, 1
	QBNE	BLINKLONG_ON, r1, 0
	CLR	r30.t14
	MOV	r10, 25000000
BLINKSHORT_OFF:	
	SUB	r10, r10, 1
	QBNE	BLINKLONG_OFF, r10, 0
	RET

DELAY:
	SUB	r7, r7, 1
	QBNE	DELAY, r7, 0
	RET


TEST:
	//TEMP
	MOV	r1, 900
	MOV	r7, 4
	CALL	DELAY
ERASEME:
	CALL	CODE0
	MOV	r7, 8
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	CALL	CODE0
	MOV	r7, 2
	CALL 	DELAY
	SUB	r1, r1, 1
	QBNE	ERASEME, r1, 0
	//TEMP
	
	MOV	r31.b0, PRU0_ARM_INTERRUPT+16
HALT	
	