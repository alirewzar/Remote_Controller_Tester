// Test program
// This is just a test program to show how a C program is organized
//===================================================================================================
//Chip Type : BA45F0082
//Clock Frequency : 8MHz
//Oscillator Type : INTERNAL
//---------------------------------------------------------------------------------------------------
//Revision History:
// Author 		Version 	Date 		Comments
// A.RAJABI 	01.0 		402/05/08 	Project created, initializations made
// A.Moallemi 	02.0 		402/0x/xx 	software config added
// ...
//=====================================================================================================

//=====================================================================================================
// File inclusions
//=====================================================================================================
#include <BS66F340C.h>
#include "generalDefs.h"


//=====================================================================================================


//=====================================================================================================
//Variables Declarations
//=====================================================================================================
#define 				INTERUPT_timer0_TOCREATE_1USECOND							0x14
#define 				INTERUPT_timer1_TOCREATE_5MSECOND							0x18

#define 				SEGMENT_INTEGER_0											0x3F //0
#define 				SEGMENT_INTEGER_1											0x06 //1
#define 				SEGMENT_INTEGER_2											0x5B //2
#define 				SEGMENT_INTEGER_3											0x4F //3
#define 				SEGMENT_INTEGER_4											0x66 //4
#define 				SEGMENT_INTEGER_5											0x6D //5
#define 				SEGMENT_INTEGER_6											0x7D //6
#define 				SEGMENT_INTEGER_7											0x07 //7
#define 				SEGMENT_INTEGER_8											0x7F //8
#define 				SEGMENT_INTEGER_9											0x6F //9



#define					bCountTask													rTaskflag.bit0
#define					bIdleLoop													rTaskflag.bit1
#define					bSegmentDisplay												rTaskflag.bit2
#define					bStartReceiveHx711											rTaskflag.bit3
//#define					stateStepperUp												rTaskflag.bit4
//#define					stateStepperDown											rTaskflag.bit5
#define					bStopDataReceive												rTaskflag.bit6
#define					bCreateDataOut												rTaskflag.bit7


#define					bStepperUp													rTaskflag1.bit0
#define					bStepperDown												rTaskflag1.bit1
//#define					bSegmentDisplay												rTaskflag1.bit2
//#define					bStartReceiveHx711											rTaskflag1.bit3
//#define					stateStepperUp												rTaskflag1.bit4
//#define					stateStepperDown											rTaskflag1.bit5
//#define					bStopDataReceive												rTaskflag1.bit6
//#define					bCreateDataOut												rTaskflag1.bit7


volatile 				BitField 			rTaskflag ;
volatile 				BitField 			rTaskflag1 ;

volatile 				uint16 				rCountTask = 0 ;

volatile				uint8				rSegmentDispla = 0 ;
volatile				uint8				segCountBit = 0 ;
volatile				uint16				segDisplaynum = 0 ;
volatile				uint8				segDisplaynumFin = 0 ;
volatile				uint8				receiveBitCount = 0 ;
volatile				uint32				receiveData = 0 ;
volatile				uint32				shiftData = 0 ;
volatile				uint8				receiveDataShifter = 23 ;
volatile				uint16				testSampleSegmentDisplay = 2388 ;
volatile				uint16				SegmentDisplayNumber = 0 ;

volatile				int					receiveDataBit [24] ;

volatile				uint8				k = 11 ;

volatile				uint8				stateStepperUp = 0 ;
volatile				uint8				stateStepperDown = 0 ;
// ==================================================================================================================================================
//table
		
const 					uint8 				tblSegmentNumberSelect[10] = {SEGMENT_INTEGER_0, SEGMENT_INTEGER_1, SEGMENT_INTEGER_2, 
																		  SEGMENT_INTEGER_3, SEGMENT_INTEGER_4, SEGMENT_INTEGER_5, 
																		  SEGMENT_INTEGER_6, SEGMENT_INTEGER_7, SEGMENT_INTEGER_8, 
																		  SEGMENT_INTEGER_9};

// ==================================================================================================================================================


//===================================================================================================

//=====================================================================================================
//just Function Prototypes
//=====================================================================================================
void 				initializeHardware 								(void);
void 				initializeSoftware 								(void);
void 				runCountTask 									(void);
void 				fSegmentDisplay 								(void);
void 				stepperUp		 								(void);
void 				stepperDown		 								(void);
void 				createDataOutFun		 						(void);
//=====================================================================================================


//=====================================================================================================
//just stable Tables
//=====================================================================================================

//=====================================================================================================



void main(void)
{	
	initializeHardware(); //hardware initialization function
	initializeSoftware(); //software initialization function
	
	//global interupt
		_emi = TRUE ;
		

	
	while(TRUE){
		asm("clr wdt");
		
		if (bCountTask){
			
			runCountTask();
			
			bCountTask = FALSE;
		}
		
		if (bSegmentDisplay){
			
			fSegmentDisplay();
			
			bSegmentDisplay = FALSE;
		}
		
		if (bCreateDataOut){
			
			createDataOutFun();
			
			bCreateDataOut = FALSE;
		}
		
		if (bStepperUp){
			
			stepperUp();
			
			bStepperUp = FALSE;
		}
		
		if (bStepperDown){
			
			stepperDown();
			
			bStepperDown = FALSE;
		}
		
		bIdleLoop = TRUE;
		while (bIdleLoop){
			
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
			asm("NOP");
		}
	}
}


//1) TIMER0 Interupt -----------------------------------------------------for receive Data
void __attribute((interrupt(INTERUPT_timer0_TOCREATE_1USECOND))) TIMER0_INT_FUN(void)
{
	//run every 1us
	_ctm0af = FALSE ;

	if(rCountTask < 120 && bStopDataReceive == FALSE && _pb0 == 0){
		bStartReceiveHx711 = TRUE ;
		bStopDataReceive = TRUE ;
	}
	
	if(bStartReceiveHx711){
		
		if(receiveBitCount < 48){
			receiveBitCount ++ ;
		}
		else{
			receiveBitCount = 0 ;
			bStartReceiveHx711 = FALSE ;
		}
		if(_pb1){
			if(_pb0){
				receiveDataBit [receiveDataShifter] = 1 ;
			}
			else{
				receiveDataBit [receiveDataShifter] = 0 ;
			}
			receiveDataShifter = receiveDataShifter - 1 ;
		}
		_pb1 = !_pb1 ;
	}
	else{
		_pb1 = FALSE ;
		receiveDataShifter = 23 ;
		_ct0on = FALSE ;
	}
	

}


//1) TIMER0 Interupt -----------------------------------------------------
void __attribute((interrupt(INTERUPT_timer1_TOCREATE_5MSECOND))) TIMER1_INT_FUN(void){
	
	_ctm1af = FALSE ;
	
	bIdleLoop  = FALSE ;
	bCountTask = TRUE ;

}


//===================================================================================================
void initializeHardware (void)
{
	//System clock ENABLED     Fsys = FH / 1     FH = 8MHz       Fsys = 8MHz
		_scc = 0b00000000 ;
		_hircc = 0b00000001 ;
		
	//watchdog timer     
		_wdtc = 0b10101101 ;  
		
	//TIMER0     CTMN counter clock = Fsys / 1
		_ctm0c0 = 0b00100000 ;
		_ctm0c1 = 0b11000001 ;
		
		_ctm0al = 0x0F ;
		_ctm0ah = 0x00 ; //compare with 1000 with 100u clock
		
	//TIMER1	 CTMN counter clock = FH / 64
		_ctm1c0 = 0b00111000 ;
		_ctm1c1 = 0b11000001 ;
		
		_ctm1al = 0x71 ;
		_ctm1ah = 0x02 ; //compare with 625 with 8u clock equal 625 * 8us = 5ms
		
	//interrupt
		_mfi0 = 0b00000010 ;
		_intc1 = 0b00000110 ;
		
		_mfi1 = 0b00000010 ;

	//I/O Function
		
		//port A
			_pac = 0b00000000 ;
			_pa = 0b00000000 ;
				
		//port B
			_pbc = 0b11001101 ;
			_pb = 0b00000000 ;
		
		//port C
			_pcc = 0b00000000 ;
			_pc = 0b00001111 ;
			
		//port E
			_pec = 0b00000000 ;
			_pe = 0b00000000 ;
				
}

// --------------------------------------------------------------------------------------------------
void initializeSoftware (void)
{
	rCountTask 						= 0 ;
	rSegmentDispla 					= 0 ;
	segCountBit 					= 0 ;
	segDisplaynum 					= 0 ;
	segDisplaynumFin 				= 0 ;
	receiveBitCount 				= 0 ;
	receiveData 					= 0 ;
	shiftData 						= 0 ;
	receiveDataShifter 				= 23 ;
	testSampleSegmentDisplay 		= 2388 ;
	SegmentDisplayNumber 			= 3658 ;
	stateStepperUp 					= 0 ;
	stateStepperDown 				= 0 ;
	k 								= 11 ;
	
	receiveDataBit [24] ;
}


void runCountTask (void){
	
	//every 5ms
	
	
	
	if(rCountTask < 248){
		rCountTask = rCountTask + 1 ;
	}
	else{
		rCountTask = 0 ;
		_ct0on = TRUE ;
		bStopDataReceive = FALSE ;
	}
	

	bCreateDataOut = TRUE ;

	
	if(!(rCountTask & 0x07)){
		if(_pb7 == 0){
			bStepperUp = TRUE ;
		}
		else if(_pb6 == 1){
			_pe5 = FALSE ;
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe2 = FALSE ;
			_pe1 = FALSE ;
		}
	}
	
	if(!(rCountTask & 0x07)){
		if(_pb6 == 0){
			bStepperDown = TRUE ;
		}
		else if(_pb7 == 1){
			_pe5 = FALSE ;
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe2 = FALSE ;
			_pe1 = FALSE ;
		}
	}
	
	bSegmentDisplay = TRUE ;
}

void createDataOutFun (void){	
	
	if(k < 24){
		shiftData = ((uint32)receiveDataBit[k]) << (k - 11) ;
		receiveData = receiveData + shiftData;
		k++ ;
	}
	else if(k == 24){
		SegmentDisplayNumber = receiveData ;
		receiveData = 0 ;
		k++ ;
	}
	if(rCountTask == 248){
		k = 11 ;
	}
}
	
	
void stepperUp (void){
	
	switch(stateStepperUp) {
		case 0:
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe5 = TRUE ;
			_pe1 = TRUE ;
			_pe2 = TRUE ;
			stateStepperUp = 1 ;
			break;
		
		case 1:
			_pe2 = FALSE ;
			_pe4 = FALSE ;
			_pe5 = TRUE ;
			_pe1 = TRUE ;
			_pe3 = TRUE ;
			stateStepperUp = 2 ;
			break;
		case 2:
			_pe5 = FALSE ;
			_pe1 = FALSE ;
			_pe2 = FALSE ;
			_pe4 = TRUE ;
			_pe3 = TRUE ;
			stateStepperUp = 3 ;
			break;
		
		case 3:
			_pe5 = FALSE ;
			_pe1 = FALSE ;
			_pe3 = FALSE ;
			_pe4 = TRUE ;
			_pe2 = TRUE ;
			stateStepperUp = 0 ;
			break;
	}
}

void stepperDown (void){
	
	switch(stateStepperDown) {
		case 0:
			_pe5 = FALSE ;
			_pe1 = FALSE ;
			_pe2 = FALSE ;
			_pe4 = TRUE ;
			_pe3 = TRUE ;
			stateStepperDown = 1 ;
			break;
		
		case 1:
			_pe2 = FALSE ;
			_pe4 = FALSE ;
			_pe5 = TRUE ;
			_pe1 = TRUE ;
			_pe3 = TRUE ;
			stateStepperDown = 2 ;
			break;
		case 2:
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe5 = TRUE ;
			_pe1 = TRUE ;
			_pe2 = TRUE ;
			stateStepperDown = 3 ;
			break;
		
		case 3:
			_pe5 = FALSE ;
			_pe1 = FALSE ;
			_pe3 = FALSE ;
			_pe4 = TRUE ;
			_pe2 = TRUE ;
			stateStepperDown = 0 ;
			break;
	}
}


void fSegmentDisplay (void){
	
	//every 10ms
	switch(segCountBit) {
	  case 0:
	  	segDisplaynumFin = 0 ;
	    segDisplaynum = SegmentDisplayNumber ;
	    while(segDisplaynum > 999){
	    	segDisplaynum = segDisplaynum - 1000 ;
	    	segDisplaynumFin = segDisplaynumFin + 1 ;
	    }
	    _pc3 = TRUE ;
	    _pc2 = TRUE ;
	    _pc1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDisplaynumFin] ;
	    _pc0 = FALSE ;
	    segCountBit = 1 ;
	    break;
	  case 1:
	  	segDisplaynumFin = 0 ;
		while(segDisplaynum > 99){
		    segDisplaynum = segDisplaynum - 100 ;
		    segDisplaynumFin = segDisplaynumFin + 1 ;
		}
	    _pc3 = TRUE ;
	    _pc2 = TRUE ;
	    _pc0 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDisplaynumFin] ;
	    _pc1 = FALSE ;
	    segCountBit = 2 ;
	    break;
	  case 2:
	  	segDisplaynumFin = 0 ;
		while(segDisplaynum > 9){
		    segDisplaynum = segDisplaynum - 10 ;
		    segDisplaynumFin = segDisplaynumFin + 1 ;
		}
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc3 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDisplaynumFin] ;
	    _pc2 = FALSE ;
	    segCountBit = 3 ;
	    break;
	  case 3:
	    segDisplaynumFin = segDisplaynum ;
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc2 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDisplaynumFin] ;
	    _pc3 = FALSE ;
	    segCountBit = 0 ;
	    break;
	}
}