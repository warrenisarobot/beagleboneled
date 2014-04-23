// prucode.p
.setcallreg r29.w0

.origin 0

.entrypoint START




#include "prucode.hp"


#define GPIO1 0x4804c000
#define GPIO_CLEARDATAOUT 0x190
#define GPIO_SETDATAOUT 0x194



	

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
	MOV r0, 10 // loop 10 times



LOOOP:
	//MOV r2, 1<<21
	//MOV r3, GPIO1 | GPIO_SETDATAOUT
	//SBBO r2, r3, 0, 4
	//MOV r0, 0x00f00000
	//SET R30.t14
	
	//SUB r0, r0, 1
	//QBNE DEL1, r0, 0
	//MOV R2, 1<<21
	//MOV r3, GPIO1 | GPIO_CLEARDATAOUT
	//SBBO r2, r3, 0, 4
	//MOV r0, 0x00f00000
	//CLR R30.t14

	SUB	r0, r0, 1
	CALL	BLINKME
	QBNE	LOOOP, r0, 0
	CLR	r30.t14

	// Send notification to Host for program completion
	MOV	r31.b0, PRU0_ARM_INTERRUPT+16

    // Halt the processor
    HALT



//0 =  High: 0.4us (400ns) , Low: 0.85us (850ns)
//Each clock cycle is 5ns
CODE0:
	SET	r30.t14
	MOV	r1, 39 //400ns = 80 clock cycles
CODE0_LOOP_HIGH:
	SUB	r1, r1, 1
	QBNE	CODE0_LOOP_HIGH, r1, 0
	CLR	r30.t14
	MOV	r1, 84//850ns = 170 clock cycles
CODE0_LOOP_LOW:	
	SUB	r1, r1, 1
	QBNE	CODE0_LOOP_LOW, r1, 0
	//leave it at low, we're done now
	RET
	

//1 =  High: 0.8us (800ns) , Low: 0.45us (450ns)
//Each clock cycle is 5ns
CODE1:
	SET	r30.t14
	MOV	r1, 79 //800ns = 160 clock cycles
CODE1_LOOP_HIGH:
	SUB	r1, r1, 1
	QBNE	CODE1_LOOP_HIGH, r1, 0
	CLR	r30.t14
	MOV	r1, 44//450ns = 90 clock cycles
CODE1_LOOP_LOW:	
	SUB	r1, r1, 1
	QBNE	CODE1_LOOP_LOW, r1, 0
	//leave it at low, we're done now
	RET


BLINKME:
	SET	r30.t14
	MOV	r1, 25000000
BLINKME_ON:
	SUB	r1, r1, 1
	QBNE	BLINKME_ON, r1, 0
	CLR	r30.t14
	MOV	r1, 25000000
BLINKME_OFF:	
	SUB	r1, r1, 1
	QBNE	BLINKME_OFF, r1, 0
	RET