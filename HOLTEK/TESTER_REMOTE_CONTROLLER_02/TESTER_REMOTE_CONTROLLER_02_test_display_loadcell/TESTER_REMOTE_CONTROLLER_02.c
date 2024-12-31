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
#define 				INTERUPT_TIMER0_TOCREATE_SMALLPULSE							0x14
#define 				INTERUPT_TIMER1_TOCREATE_TIMEBASE							0x18
#define 				INTERUPT_INT1_EXTERNAL										0x08


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
#define					bStopDataReceive											rTaskflag.bit3
#define					bCreateDataOut												rTaskflag.bit4
#define					bStable														rTaskflag.bit5
//#define					bStopDataReceive											rTaskflag.bit6
//#define					bCreateDataOut												rTaskflag.bit7


#define					bStepperUp													rTaskflag1.bit0
#define					bStepperDown												rTaskflag1.bit1
//#define					bSegmentDisplay												rTaskflag1.bit2
//#define					bStartReceiveHx711											rTaskflag1.bit3
//#define					stateStepper												rTaskflag1.bit4
//#define					stateStepper												rTaskflag1.bit5
//#define					bStopDataReceive											rTaskflag1.bit6
//#define					bCreateDataOut												rTaskflag1.bit7


volatile 				BitField 			rTaskflag ;
volatile 				BitField 			rTaskflag1 ;

volatile 				uint16 				rCountTask ;

volatile				uint8				segDigitSelect ;
volatile				uint32				segDisplayNumPreparing ;
volatile				uint8				segDigitDisplayNum ;
volatile				uint8				dataReceiveHelp ;
volatile				uint32				receiveData ;
volatile				uint32				shiftData ;
volatile				uint8				receiveDataShifter ;
volatile				uint32				segmentDisplayNumber ;
volatile				uint8				stableInteger ;

volatile				int					receiveDataBit [24] ;

volatile				uint8				k ;

volatile				uint8				stateStepper ;
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
			
			//createDataOutFun();
			
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
void __attribute((interrupt(INTERUPT_TIMER0_TOCREATE_SMALLPULSE))) TIMER0_INT_FUN(void)
{
	//run every 1us
	_ctm0af = FALSE ;
	

	if(_pb6 && dataReceiveHelp < 48){
		if(_pb7){
			receiveDataBit [receiveDataShifter] = 1 ;
		}
		else{
			receiveDataBit [receiveDataShifter] = 0 ;
		}
		receiveDataShifter = receiveDataShifter - 1 ;
	}
	
	

	if(dataReceiveHelp < 50){
		dataReceiveHelp ++ ;
		_pb6 = !_pb6 ;
		k = 0 ;
		stableInteger = 0 ;
	}
	else{
		if(stableInteger < 26){
			stableInteger++;
			if(k < 24){
				shiftData = ((uint32)receiveDataBit[k]) << (k) ;
				receiveData = receiveData + shiftData;
				k++ ;
			}
			else if(k == 24){
				segmentDisplayNumber = receiveData ;
				receiveData = 0 ;
				k++ ;
			}
		}
		else{
			receiveDataShifter = 23 ;
				
			_ctm0ae = 0 ;
			_mf0e = 0 ;
			_ct0on = 0 ;
				
			_int1e = 1 ;
		}
	}
	
}


//1) TIMER1 Interupt -----------------------------------------------------
void __attribute((interrupt(INTERUPT_TIMER1_TOCREATE_TIMEBASE))) TIMER1_INT_FUN(void){
	
	_ctm1af = FALSE ;
	
	bIdleLoop  = FALSE ;
	bCountTask = TRUE ;

}


//1) INT1 Interupt -----------------------------------------------------
void __attribute((interrupt(INTERUPT_INT1_EXTERNAL))) INT1_INT_FUN(void){
	
	if(bStable){
		//_pb5 = !_pb5 ;
	

		_ctm0ae = 1 ;
		_mf0e = 1 ;
		_ct0on = 1 ;
	
	
		_int1e = 0 ;
	
		dataReceiveHelp = 0 ;
	}
	bStable = !bStable;
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
		
		_ctm1al = 0xFA ;
		_ctm1ah = 0x00 ; //compare with 625 with 8u clock equal 625 * 8us = 5ms
		
	//interrupt
		_mfi0 = 0b00000000 ;
		_mfi1 = 0b00000010 ;
		_intc1 = 0b00000100 ;
		_intc0 = 0b00000100 ;
		
		_integ = 0b00001000 ;

	//I/O Function
		
		//port A
			_pac = 0b00000000 ;
			_pa = 0b00000000 ;
				
		//port B
			_pbc = 0b10001100 ;
			_pb = 0b00000000 ;
		
		//port C
			_pcc = 0b00000000 ;
			_pc = 0b00001111 ;
			
		//port E
			_pec = 0b00000011 ;
			_pe = 0b00000000 ;
				
}

// --------------------------------------------------------------------------------------------------
void initializeSoftware (void)
{
	rCountTask 						= 0 ;
	segDigitSelect 					= 0 ;
	segDisplayNumPreparing 			= 0 ;
	segDigitDisplayNum 				= 0 ;
	dataReceiveHelp 				= 0 ;
	receiveData 					= 0 ;
	shiftData 						= 0 ;
	receiveDataShifter 				= 23 ;
	segmentDisplayNumber 			= 367558 ;
	stateStepper 					= 0 ;
	k 								= 0 ;
	stableInteger					= 0 ;

}


void runCountTask (void){
	
	//every 5ms
	

	bCreateDataOut = TRUE ;

	
	if(!(rCountTask & 0x07)){
		if(_pb2 == 0){
			bStepperUp = TRUE ;
		}
		else if(_pb3 == 1){
			_pe5 = FALSE ;
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe2 = FALSE ;
		}
	}
	
	if(!(rCountTask & 0x07)){
		if(_pb3 == 0){
			bStepperDown = TRUE ;
		}
		else if(_pb2 == 1){
			_pe5 = FALSE ;
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe2 = FALSE ;
		}
	}
	
	bSegmentDisplay = TRUE ;
	
	if(rCountTask < 247){
		rCountTask = rCountTask + 1 ;
	}
	else{
		rCountTask = 0 ;
		bStopDataReceive = FALSE ;
	}
}

//void createDataOutFun (void){	
	
//	if(k < 24){
//		shiftData = ((uint32)receiveDataBit[k]) << (k) ;
//		receiveData = receiveData + shiftData;
//		k++ ;
//	}
//	else if(k == 24){
//		segmentDisplayNumber = receiveData ;
//		receiveData = 0 ;
//		k++ ;
//	}
//	if(rCountTask == 248){
//		k = 0 ;
//	}
//}
	
	
void stepperUp (void){
	
	switch(stateStepper) {
		case 0:
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe5 = TRUE ;
			_pe2 = TRUE ;
			stateStepper = 1 ;
			break;
		
		case 1:
			_pe2 = FALSE ;
			_pe4 = FALSE ;
			_pe5 = TRUE ;
			_pe3 = TRUE ;
			stateStepper = 2 ;
			break;
		case 2:
			_pe5 = FALSE ;
			_pe2 = FALSE ;
			_pe4 = TRUE ;
			_pe3 = TRUE ;
			stateStepper = 3 ;
			break;
		
		case 3:
			_pe5 = FALSE ;
			_pe3 = FALSE ;
			_pe4 = TRUE ;
			_pe2 = TRUE ;
			stateStepper = 0 ;
			break;
	}
}

void stepperDown (void){
	
	switch(stateStepper) {
		case 0:
			_pe5 = FALSE ;
			_pe2 = FALSE ;
			_pe4 = TRUE ;
			_pe3 = TRUE ;
			stateStepper = 1 ;
			break;
		
		case 1:
			_pe2 = FALSE ;
			_pe4 = FALSE ;
			_pe5 = TRUE ;
			_pe3 = TRUE ;
			stateStepper = 2 ;
			break;
		case 2:
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe5 = TRUE ;
			_pe2 = TRUE ;
			stateStepper = 3 ;
			break;
		
		case 3:
			_pe5 = FALSE ;
			_pe3 = FALSE ;
			_pe4 = TRUE ;
			_pe2 = TRUE ;
			stateStepper = 0 ;
			break;
	}
}


void fSegmentDisplay (void){
	
	//every 3ms
	switch(segDigitSelect) {
	  case 0:
	  	segDigitDisplayNum = 0 ;
	    segDisplayNumPreparing = segmentDisplayNumber >> 5 ;
	    while(segDisplayNumPreparing > 99999){
	    	segDisplayNumPreparing = segDisplayNumPreparing - 100000 ;
	    	segDigitDisplayNum = segDigitDisplayNum + 1 ;
	    }
	    _pc3 = TRUE ;
	    _pc2 = TRUE ;
	    _pc1 = TRUE ;
	    _pb0 = TRUE ;
	    _pb1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pc0 = FALSE ;
	    segDigitSelect = 1 ;
	    break;
	  case 1:
	  	segDigitDisplayNum = 0 ;
		while(segDisplayNumPreparing > 9999){
		    segDisplayNumPreparing = segDisplayNumPreparing - 10000 ;
		    segDigitDisplayNum = segDigitDisplayNum + 1 ;
		}
	    _pc3 = TRUE ;
	    _pc2 = TRUE ;
	    _pc0 = TRUE ;
	    _pb0 = TRUE ;
	    _pb1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pc1 = FALSE ;
	    segDigitSelect = 2 ;
	    break;
	  case 2:
	  	segDigitDisplayNum = 0 ;
		while(segDisplayNumPreparing > 999){
		    segDisplayNumPreparing = segDisplayNumPreparing - 1000 ;
		    segDigitDisplayNum = segDigitDisplayNum + 1 ;
		}
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc3 = TRUE ;
	    _pb0 = TRUE ;
	    _pb1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pc2 = FALSE ;
	    segDigitSelect = 3 ;
	    break;
	  case 3:
	  	segDigitDisplayNum = 0 ;
		while(segDisplayNumPreparing > 99){
		    segDisplayNumPreparing = segDisplayNumPreparing - 100 ;
		    segDigitDisplayNum = segDigitDisplayNum + 1 ;
		}
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc2 = TRUE ;
	    _pb0 = TRUE ;
	    _pb1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pc3 = FALSE ;
	    segDigitSelect = 4 ;
	    break;
	  case 4:
	  	segDigitDisplayNum = 0 ;
		while(segDisplayNumPreparing > 9){
		    segDisplayNumPreparing = segDisplayNumPreparing - 10 ;
		    segDigitDisplayNum = segDigitDisplayNum + 1 ;
		}
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc2 = TRUE ;
	    _pc3 = TRUE ;
	    _pb1 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pb0 = FALSE ;
	    segDigitSelect = 5 ;
	    break;
	  case 5:
	    segDigitDisplayNum = segDisplayNumPreparing ;
	    _pc0 = TRUE ;
	    _pc1 = TRUE ;
	    _pc2 = TRUE ;
	    _pc3 = TRUE ;
	    _pb0 = TRUE ;
	    _pa = tblSegmentNumberSelect [segDigitDisplayNum] ;
	    _pb1 = FALSE ;
	    segDigitSelect = 0 ;
	    break;
	}
}