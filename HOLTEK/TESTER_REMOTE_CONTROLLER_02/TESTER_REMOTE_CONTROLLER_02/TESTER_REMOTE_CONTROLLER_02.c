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
#define					bPollingPins												rTaskflag.bit3
#define					bCounterSwitching											rTaskflag.bit4
#define					bStable														rTaskflag.bit5
#define					bBeforState													rTaskflag.bit6
#define					bLoadEeprom													rTaskflag.bit7


#define					bStepperUp													rTaskflag1.bit0
#define					bStepperDown												rTaskflag1.bit1
//#define					bSegmentDisplay												rTaskflag1.bit2
//#define					bStartReceiveHx711											rTaskflag1.bit3
//#define					stateStepper												rTaskflag1.bit4
//#define					stateStepper												rTaskflag1.bit5
//#define					bStopDataReceive											rTaskflag1.bit6
//#define					bCounterSwitching												rTaskflag1.bit7

#define 			T_WAIT_TO_READ_EEPROM					10//x NOP


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
volatile				uint32				segmentDisplayNumberLoad ;
volatile				uint32				segmentDisplayNumberCount ;
volatile				uint8				stableInteger ;
volatile				uint8				stateStorage ;
volatile				uint8				rReadFromEeromFail;
volatile				uint8				rWaitToReadFromEeprom;
volatile				uint8				tempEEPROMread;

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
void 				counterSwitching		 						(void);
void 				pollingPins				 						(void);
uint8 				readEeprom 										(uint8);
void 				writeEeprom 									(uint8,uint8);
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
		
		if (bCounterSwitching){
			
			counterSwitching();
			
			bCounterSwitching = FALSE;
		}
		
		if (bStepperUp){
			
			stepperUp();
			
			bStepperUp = FALSE;
		}
		
		if (bStepperDown){
			
			stepperDown();
			
			bStepperDown = FALSE;
		}
		
		if (bPollingPins){
			
			pollingPins();
			
			bPollingPins = FALSE;
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
	

	if(_pb6 && dataReceiveHelp < 30){
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
	}
	else{
		if(stableInteger < 15){
			stableInteger++;
			if(receiveDataBit[k] == 1){
				switch((k-10)){
					case 0 : 
						shiftData = 2 ;
						break;
					case 1 : 
						shiftData = 4 ;
						break;
					case 2 : 
						shiftData = 8 ;
						break;
					case 3 : 
						shiftData = 16 ;
						break;
					case 4 : 
						shiftData = 32 ;
						break;
					case 5 : 
						shiftData = 64 ;
						break;
					case 6 : 
						shiftData = 128 ;
						break;
					case 7 : 
						shiftData = 256 ;
						break;
					case 8 : 
						shiftData = 512 ;
						break;
					case 9 : 
						shiftData = 1024 ;
						break;
					case 10 : 
						shiftData = 2048 ;
						break;
					case 11 : 
						shiftData = 4096 ;
						break;
					case 12 : 
						shiftData = 8192 ;
						break;
					case 13 : 
						shiftData = 16384 ;
						break;
					case 14 : 
						shiftData = 32768 ;
						break;
					case 15 : 
						shiftData = 65536 ;
						break;
					case 16 : 
						shiftData = 131072 ;
						break;
					case 17 : 
						shiftData = 262144 ;
						break;	
					case 18 : 
						shiftData = 524288 ;
						break;
					case 19 : 
						shiftData = 1048576 ;
						break;
					case 20 : 
						shiftData = 1097152 ;
						break;
					case 21 : 
						shiftData = 4194304 ;
						break;
					case 22 : 
						shiftData = 8388608 ;
						break;
					case 23 : 
						shiftData = 16777216 ;
						break;	
				}
				receiveData = receiveData + shiftData;	
			}

		}	
		else{
			segmentDisplayNumberLoad = receiveData ;
			_ctm0ae = 0 ;
			_mf0e = 0 ;
			_ct0on = 0 ;
				
			_int1e = 1 ;
		}
		k ++;
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
		
		k = 10 ;
		stableInteger = 0 ;
		receiveDataShifter = 23 ;
		
		receiveData = 0 ;
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
		
		_ctm0al = 0x15 ;
		_ctm0ah = 0x00 ; //compare with 1000 with 100u clock
		
	//TIMER1	 CTMN counter clock = FH / 64
		_ctm1c0 = 0b00111000 ;
		_ctm1c1 = 0b11000001 ;
		
		_ctm1al = 0x2c ;
		_ctm1ah = 0x01 ; //compare with 625 with 8u clock equal 250 * 8us = 1.25ms
		
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
	//EEPROM
		_def  = FALSE;//end write eeprom Interrupt request flag
		_mf3f = FALSE;//multi function3 Interrupt request flag
		_mf3e = TRUE;//multi function3 Interrupt En
				
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
	segmentDisplayNumberLoad 		= 367558 ;
	stateStepper 					= 0 ;
	k 								= 10 ;
	stableInteger					= 0 ;
	segmentDisplayNumberCount		= 0 ;
	stateStorage					= 0 ;
	bLoadEeprom						= 0 ;
	

	//read EEPROM
	//check switches' LED active?
	rWaitToReadFromEeprom=0;
	rReadFromEeromFail=0;
	
	tempEEPROMread = readEeprom(0X00);
	if(tempEEPROMread == 0x01 || tempEEPROMread == 0xFF)
	{		
		writeEeprom(0X00,0X00);
	}	
}


void runCountTask (void){
	
	//every 5ms
	
	
	if(rCountTask == 244){
		bCounterSwitching = TRUE ;
	}

	
	//if(!(rCountTask & 0x07)){
	//	if(_pb2 == 0){
	//		bStepperUp = TRUE ;
	//	}
	//	else if(_pb3 == 1){
	//		_pe5 = FALSE ;
	//		_pe4 = FALSE ;
	//		_pe3 = FALSE ;
	//		_pe2 = FALSE ;
	//	}
	//}
	
	//if(!(rCountTask & 0x07)){
	//	if(_pb3 == 0){
	//		bStepperDown = TRUE ;
	//	}
	//	else if(_pb2 == 1){
	//		_pe5 = FALSE ;
	//		_pe4 = FALSE ;
	//		_pe3 = FALSE ;
	//		_pe2 = FALSE ;
	//	}
	//}
	
	if(!(rCountTask & 0x07)){
		if(segmentDisplayNumberLoad<60){
			bStepperUp = TRUE ;
		}
		else if(segmentDisplayNumberLoad<100){
			_pe5 = FALSE ;
			_pe4 = FALSE ;
			_pe3 = FALSE ;
			_pe2 = FALSE ;
		}
	}
	
	if(!(rCountTask & 0x07)){
		if(segmentDisplayNumberLoad>100){
			bStepperDown = TRUE ;
		}
		else if(segmentDisplayNumberLoad>60){
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
		bPollingPins = TRUE ;
	}
}

void counterSwitching (void){	
	
	if(bLoadEeprom == 0){
		segmentDisplayNumberCount = readEeprom(0x00) ;
		bLoadEeprom = 1 ; 
	} 
	
	else{
		if(_pe1 == 0 && bBeforState == 0){
			segmentDisplayNumberCount = segmentDisplayNumberCount + 1 ;
			bBeforState = 1 ;
			writeEeprom(0X00,segmentDisplayNumberCount) ;
		}
		else if(_pe1){
			bBeforState = 0 ;
		}
	}
}
	
	
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

void pollingPins (void){
	if(_pb3 == FALSE){
		writeEeprom(0X00,0) ;
		segmentDisplayNumberCount = 0 ;
	}
}


void fSegmentDisplay (void){
	
	//every 3ms
	switch(segDigitSelect) {
	  case 0:
	  	segDigitDisplayNum = 0 ;
	    segDisplayNumPreparing = segmentDisplayNumberCount ;
	    //segDisplayNumPreparing = segmentDisplayNumberLoad ;
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





uint8 readEeprom (uint8 address)
{
	rReadFromEeromFail=0;
	_eea = address;//user defined address

	
	do
	{
		rWaitToReadFromEeprom=0;
		_mp1l = 0x40;//setup memory pointer MP1L
		_mp1h = 0x01;//setup memory pointer MP1H, points to EEC reg
		_iar1 = 0;
		_iar1 = (_iar1|BIT1);// EEC register : RDEN = 1, enable read operation
		_iar1 = (_iar1|BIT0);// EEC register : RD = 1, start Read cycle
		
		//wait for end-up read operation
		while ((_iar1&BIT0)==TRUE && rWaitToReadFromEeprom <= T_WAIT_TO_READ_EEPROM)
		{
			rWaitToReadFromEeprom++;
		}
		
		
		if (rWaitToReadFromEeprom > T_WAIT_TO_READ_EEPROM) //time-out for wait for end-up read operation
		{
			rReadFromEeromFail++;
		}	
		else
		{
			if(rReadFromEeromFail>0)
			{
				rReadFromEeromFail--;
			}
		}				
		_iar1 = 0;//disable eeprom read	
		_mp1h = 0;
		_mp1l = 0;
		
	}while(rReadFromEeromFail==1);
	


	if(rReadFromEeromFail==0)
	{
		
		return(_eed);//move read data to register
		
	}
	else
	{
		return 0xFF;
	}
}

//-----------------------------------------------------------------------------------------------------
void writeEeprom (uint8 address, uint8 data)
{
	if ( (_iar1 & BIT2) == FALSE) //Has last write been finished?
	{
		_eea   = address;//user defined address
		_eed   = data;//user defined data
		
		_mp1l = 0x40;//setup memory pointer MP1L
		_mp1h = 0x01;//setup memory pointer MP1H, points to EEC reg
		
		_emi   = FALSE;//since next two command must be done successively we disable global interrupt
		_iar1 |= BIT3;//set WREN bit, enable write operations
		_iar1 |= BIT2;//start Write Cycle - set WR bit
		_emi   = TRUE;//enable global interrupt
		
		_mf3f = FALSE;//multi function3 Interrupt request flag
		_dee  = TRUE;
		
	}

}