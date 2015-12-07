/*
 * mytest.c
 */

/******************************************************************************
* Include Files                                                               *
******************************************************************************/

// Standard header files
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

// Driver header file
#include "prussdrv.h"
#include <pruss_intc_mapping.h>	 

// LED manipulation library
#include "lightmodes.cpp"

/******************************************************************************
* Explicit External Declarations                                              *
******************************************************************************/

/******************************************************************************
* Local Macro Declarations                                                    *
******************************************************************************/

#define PRU_NUM 	 0
#define ADDEND1	 	 0x98765400u
#define ADDEND2		 0x12345678u
#define ADDEND3		 0x10210210u

#define DDR_BASEADDR     0x80000000
#define OFFSET_DDR	 0x00001000 
#define OFFSET_SHAREDRAM 2048		//equivalent with 0x00002000

#define PRUSS0_SHARED_DATARAM    4

//#define PRU_INTERRUPT_1  21
#define PRU_INTERRUPT_1  22
#define LED_COUNT        1500
#define MODE_LENGTH      30

/******************************************************************************
* Local Typedef Declarations                                                  *
******************************************************************************/


/******************************************************************************
* Local Function Declarations                                                 *
******************************************************************************/

static int LOCAL_exampleInit ( int pruNum);
static unsigned short LOCAL_examplePassed ( unsigned short pruNum );


/******************************************************************************
* Local Variable Definitions                                                  *
******************************************************************************/


/******************************************************************************
* Intertupt Service Routines                                                  *
******************************************************************************/


/******************************************************************************
* Global Variable Definitions                                                 *
******************************************************************************/

static void *pruDataMem;
static unsigned int *pruDataMem_int;
static unsigned char *pruDataMem_byte;


/******************************************************************************
* Global Function Definitions                                                 *
******************************************************************************/
int InitPRU(int pruNum){
    unsigned int ret;
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
    void *DDR_regaddr1, *DDR_regaddr2, *DDR_regaddr3;
    int i=0;
    
    printf("\nINFO: Starting %s example.\r\n", "PRU_memAccess_DDR_PRUsharedRAM");
    /* Initialize the PRU */
    prussdrv_init();	
    
    /* Open PRU Interrupt */
    printf("\nOpening PRU");
    ret = prussdrv_open(PRU_EVTOUT_0);
    if (ret)
    {
        printf("prussdrv_open open failed\n");
        return (ret);
    }
    printf("\nInitializing interrupts");
    /* Get the interrupt initialized */
    prussdrv_pruintc_init(&pruss_intc_initdata);

    /* Initialize example */
    //printf("\tINFO: Initializing example.\r\n");

    printf("\nInitializing PRU memory");
    //Initialize pointer to PRU data memory
    if (pruNum == 0)
    {
      prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
    }
    else if (pruNum == 1)
    {
      prussdrv_map_prumem (PRUSS0_PRU1_DATARAM, &pruDataMem);
    }
    pruDataMem_int = (unsigned int*) pruDataMem;
    pruDataMem_byte = (unsigned char*) &pruDataMem_int[1];

    /* interrupt tests */
    printf("\tINT: Interrupt event %d on channel %d and host interrupt %d\r\n",  PRU_INTERRUPT_1, prussdrv_get_event_to_channel_map(PRU_INTERRUPT_1), prussdrv_get_event_to_host_map(PRU_INTERRUPT_1));
    
    /* Execute example on PRU */
    printf("\tINFO: Executing example.\r\n");
    prussdrv_exec_program (PRU_NUM, "./prucode.bin");
    return (0);
}

void RGBToPRU(RGB *leds, int length) {
  //ws2812b takes bytes in GRB order
  int i;
  //printf("Copying Memory\r\n");
  pruDataMem_int[0] = length * 3;  
  for (i=0; i<length; i++) {
    pruDataMem_byte[(i*3)] = (char) leds[i].green;
    pruDataMem_byte[(i*3) + 1] = (char) leds[i].red;
    pruDataMem_byte[(i*3) + 2] = (char) leds[i].blue;
  }
  //printf("Memory count: %d\r\n", pruDataMem_int[0]);
  prussdrv_pru_send_event(PRU_INTERRUPT_1);
  //printf("Done\r\n");

}

void WaitForPRU() {
    /* Wait until PRU0 has finished rendering the lights */
    //printf("\tINFO: Waiting for HALT command.\r\n");
    prussdrv_pru_wait_event (PRU_EVTOUT_0);
    //printf("\tINFO: PRU completed transfer.\r\n");
    prussdrv_pru_clear_event (PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
}

LightMode *nextLightMode(int cycleNum, RGB *leds, int numberOfLights) {
  LightMode* lm;
  const int num_modes = 3;
  switch (cycleNum % num_modes) {
  case 0: lm = new ChristmasTwinkleMode(leds, numberOfLights); break;
  case 1: lm = new CandleMode(leds, numberOfLights); break;
  case 2: lm = new MovingCandycaneMode(leds, numberOfLights); break;
  }
  return lm;
}


int main (void)
{
  int ret;
  RGB leds[LED_COUNT];
  LightMode *currentMode;
  time_t startTime, currentTime;
  double elapsedTime;
  int lightCycle = 0;
  
  ret = InitPRU(0);
  if (ret) {
    return(ret);
  }


  
  while (lightCycle < 10) {
    startTime = time(NULL);
    currentMode = nextLightMode(lightCycle, leds, LED_COUNT);
    lightCycle += 1;
    elapsedTime = difftime(time(NULL), startTime);
    currentTime = time(NULL);
    // Just in case the clock has updated ( on a reboot our time may be way off) we don't
    // want this to go forever so use the elapsed time as a lower bound as well
    while (elapsedTime < MODE_LENGTH && elapsedTime >= 0) {
      //while (1) {
      currentMode->cycle();
      RGBToPRU(leds, LED_COUNT);
      usleep(currentMode->delayTime() * 1000);    
      WaitForPRU();
      currentTime = time(NULL);
      elapsedTime = difftime(currentTime, startTime);
    }
    delete(currentMode);
  }


    
    /* Disable PRU and close memory mapping*/
    prussdrv_pru_disable(PRU_NUM); 
    prussdrv_exit ();

    return(0);
}

/*****************************************************************************
* Local Function Definitions                                                 *
*****************************************************************************/

static int LOCAL_exampleInit ( int pruNum )
{
    void *DDR_regaddr1, *DDR_regaddr2, *DDR_regaddr3;
    int i=0;

    //Initialize pointer to PRU data memory
    if (pruNum == 0)
    {
      prussdrv_map_prumem (PRUSS0_PRU0_DATARAM, &pruDataMem);
    }
    else if (pruNum == 1)
    {
      prussdrv_map_prumem (PRUSS0_PRU1_DATARAM, &pruDataMem);
    }
    pruDataMem_int = (unsigned int*) pruDataMem;
    pruDataMem_byte = (unsigned char*) &pruDataMem_int[1];
    // Flush the values in the PRU data memory locations
    pruDataMem_int[0] = 900;
    //pruDataMem_int[1] = 0x08;
    //pruDataMem_int[2] = 0x00;
    for (i=0; i < 300; i++ ) {
      pruDataMem_byte[i*3] = 0;
      pruDataMem_byte[(i*3) + 1] = 0;
      pruDataMem_byte[(i*3) + 2] = 0;
    }
    /*
    pruDataMem_byte[0] = 0;
    pruDataMem_byte[1] = 0;
    pruDataMem_byte[2] = 0;
    pruDataMem_byte[3] = 0;
    pruDataMem_byte[4] = 0;
    pruDataMem_byte[5] = 0;
    */
    prussdrv_pru_send_event(PRU_INTERRUPT_1);
    return(0);
}

static unsigned short LOCAL_examplePassed ( unsigned short pruNum )
{
    unsigned int result_0, result_1, result_2;

    printf("counter=%d\r\n", pruDataMem_int[0]);
    printf("Array starting  in: %hhu, %hhu, %hhu\r\n",
	   pruDataMem_byte[0], pruDataMem_byte[1], pruDataMem_byte[2]);
    return 1;

}


