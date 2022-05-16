#include 	<Stdio.h>

#define		DIVISOR_LENGTH		4

#define 	NUM_OF_MESSAGES		3

#define 	CRC_STEPS			8		//counter++ until data = 0   ----> number of (Data bits)	

#define 	MSG_LENGTH			11

#define 	REMAINDER_MSK		0x007F

 	
unsigned char Global_GlbU8Divisor = 0b00001010;

unsigned char   Crc    [3] = {0};

unsigned char 	Result [3] = {0};

void SenderConstructMsg (void)
{
	unsigned short int  Message[3] = {0}; 
	unsigned char Data[3] = {0b10010101, 0b11010011, 0b10111100};
	unsigned char Loc_u8GetDivisor = 0;
	unsigned char CrcSizeCounter   = MSG_LENGTH -1;
	unsigned short int RemainderMask = REMAINDER_MSK;
	unsigned short int Loc_u16CrcBitFieldsMask = 0;
	unsigned short int crcTemp = 0;
	signed char ShiftingFactor = 0;
	
	for (int i = 0; i< (DIVISOR_LENGTH-1); i++)     
	{
		Loc_u16CrcBitFieldsMask |= (1<<i);
	}
	
	
	for (int i = 0; i < NUM_OF_MESSAGES; i++)
	{
		Message[i] = ( Data[i]<<(DIVISOR_LENGTH-1) );
		
		crcTemp = 0;
		
		CrcSizeCounter   = MSG_LENGTH -1;

		for (int j = 0; j < CRC_STEPS; j++)
		{
			if (j == 0)
			{
				Loc_u8GetDivisor = ( (Message[i] >> (MSG_LENGTH-1)) & 1);
				
				crcTemp = ( Message[i] ^ ( ( (Global_GlbU8Divisor* Loc_u8GetDivisor) << (MSG_LENGTH - DIVISOR_LENGTH) ) ) ); 
			}
			else
			{
				Loc_u8GetDivisor = ( (crcTemp >> (CrcSizeCounter-1)) & 1);
				
				ShiftingFactor = CrcSizeCounter - DIVISOR_LENGTH;
				
				if (ShiftingFactor == 0 || ShiftingFactor < 0)
				{
					ShiftingFactor = 0;
				}
				
				crcTemp = ( crcTemp ^  ( (Global_GlbU8Divisor* Loc_u8GetDivisor) << (ShiftingFactor) ) ); 
				CrcSizeCounter --;
			}
			
			crcTemp |= (Message[i]&RemainderMask);
			
			RemainderMask = (RemainderMask >> 1);
		}
		
		crcTemp = (crcTemp & Loc_u16CrcBitFieldsMask);
		
		Crc[i] = (unsigned char)crcTemp;
					
		Message[i] |= Crc[i];
	}
	
}

void ReceiverDecodeMsg (void)
{
	unsigned short int  xMessage[3] = {0}; 
	unsigned char xData[3] = {0b10010101, 0b11010011, 0b10101100};
	unsigned char Loc_u8GetDivisor = 0;
	unsigned char CrcSizeCounter   = MSG_LENGTH -1;
	unsigned short int RemainderMask = REMAINDER_MSK;
	unsigned short int Loc_u16CrcBitFieldsMask = 0;
	unsigned short int resultTemp = 0;
	signed char ShiftingFactor = 0;
	
	for (int i = 0; i < (DIVISOR_LENGTH-1); i++)     
	{
		Loc_u16CrcBitFieldsMask |= (1<<i);
	}
	
	for (int i = 0; i < NUM_OF_MESSAGES; i++)
	{
		xMessage[i] = ( ( xData[i]<<(DIVISOR_LENGTH-1) ) | Crc[i] ) ;
		
		resultTemp = 0;
		
		CrcSizeCounter   = MSG_LENGTH -1;
		
		for (int j = 0; j < CRC_STEPS; j++)
		{
			if (j == 0)
			{
				Loc_u8GetDivisor = ( (xMessage[i] >> (MSG_LENGTH-1)) & 1);
				
				resultTemp = ( xMessage[i] ^ ( ( (Global_GlbU8Divisor* Loc_u8GetDivisor) << (MSG_LENGTH - DIVISOR_LENGTH) ) ) ); 
			}
			else
			{
				Loc_u8GetDivisor = ( (resultTemp >> (CrcSizeCounter-1)) & 1);
				
				ShiftingFactor = CrcSizeCounter - DIVISOR_LENGTH;
				
				if (ShiftingFactor == 0 || ShiftingFactor < 0)
				{
					ShiftingFactor = 0;
				}
				resultTemp = ( resultTemp ^  ( (Global_GlbU8Divisor* Loc_u8GetDivisor) << (ShiftingFactor) ) ); 
				
				CrcSizeCounter --;
			}
			
			resultTemp |= (xMessage[i]&RemainderMask);
			
			RemainderMask = (RemainderMask >> 1);
		}
		
		resultTemp = (resultTemp & Loc_u16CrcBitFieldsMask);
		
		Result[i] = (unsigned char)resultTemp;
	}
	
}

int main (void)
{
		
		SenderConstructMsg();
		ReceiverDecodeMsg();
		
		for (int i = 0; i<NUM_OF_MESSAGES; i++)
		{
			if (Result[i] == 0)
			{
				printf("\n%d - Positive ACK\n\n", i);
			}
			else
			{
				printf("\n%d - Negative ACK\n\n", i);
			}
		}
		
		
	return 0;
}

