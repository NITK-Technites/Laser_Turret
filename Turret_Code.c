#include <msp430g2553.h>


/* Seven segment display
                  1
			 ----------
            |          |
		2	|          |   6
            |     7    |
			 ----------
            |          |
		3   |          |   5
            |          |
			 ----------
                 4
*/
//#define N_channel 12

#define SIN595  	 BIT4//port2
#define SCLK595      BIT7//port1
#define LTCH595 	 BIT6//port1
#define RESET		 BIT0 //port2

#define INTP1 BIT1                                   //Interrupt bit for Player 1 (port2)
#define INTP2 BIT2                                   //Interrupt bit for Player 2 (port2)

char number[10]={0x3F,0x30,0x6D,0x79,0x72,0x5B,0x5F,0x31,0x7F,0x7B};
//char disp_arr[N_channel]={0x01,0x02,0x04,0x08,0x10,0x40,0x20,0x80,0x04,0x08,0x10,0x20};
char ledArray[9]={0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};

char array[7];                                      //Array to be sent to SIN
unsigned int score1=0,score2=0;                              //Scores of Player1 and Player2
unsigned int player1_flag=0,player2_flag=0;                              
int MAXledCount=30;
int ledCount=0;
char gameFin=0;

void sendArray(void)
{
	char temp;
	char i,j;
	for (i = 0; i < 6; i++)
	{
		temp=array[5-i];
		for(j=0;j<8;j++)
		{
	    	if(temp&BIT7)
	    		P2OUT|=SIN595;
			else
				P2OUT&=~SIN595;
			P1OUT|=SCLK595;
			P1OUT&=~SCLK595;
			temp=temp<<1;

		}
	}
	P1OUT|=LTCH595;
    P1OUT&=~(LTCH595);

}

//switching on 1 LED for player 1 and 1 for player 2
void ledON()
{
	int rand1, rand2;
	
	//generating random number from 1 to 12. USING TIMER A COUNTER HERE
	rand1=TAR0%12;
	rand2=TAR0%12;
	
	if(rand1<=8)
		array[0]=ledArray[rand1];
	else
	{
		array[0]=ledArray[0];
		array[1]=ledArray[rand1-8];
	}
	if(rand2<=4)
	{
		array[1]^=ledArray[rand2+4]
		array[2]=ledArray[0];
	}
	else
		array[2]=ledArray[rand2-4];
		
}

void main()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop WDT
	BCSCTL1 = CALBC1_1MHZ;                  // Set DCO to 1MHz
	DCOCTL = CALDCO_1MHZ;


	P1DIR = 0;
	P1DIR |= SCLK595+LTCH595+RESET;
	P2DIR |= SIN595;

	P2DIR &=~(INTP1 + INTP2 + RESET);
	P2IE |= (INTP1 + INTP2+ RESET);
	P2IES &= ~(INTP1 + INTP2 + RESET);
	P2REN|= (INTP1 + INTP2);
	P2IFG =0;
	P2OUT&=~(INTP1 + INTP2);

	_enable_interrupt();
	
	CCR0 = 50000;
	TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

	score1=0;
	score2=0;
	_bis_SR_register(GIE+LPM0_bits);
	
	while(1)
	{
		if(!gameFin)
		{
			if(ledCount==MAXledCount)
			{
				array[0]=0x00;
				array[1]=0x00;
				array[2]=0x00;
				gameFin=1;
			}	

			else if(ledCount<(MAXledCount/2))
			{
				player1_flag=0;
				player2_flag=0;
				ledON();
				sendArray();
				_delay_cycles(1000000);
			}
	
			else
			{
				player1_flag=0;
				player2_flag=0;
				ledON();
				sendArray();
				_delay_cycles(700000);
			}
			//_delay_cycles(5000000);
			sendArray();
		}
	}
}



#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
	P1OUT^=BIT0;
	_delay_cycles(300);
	if(P2IFG&BIT1)
	{
		if(player1_flag==0)                      //to prevent increase in shooting on a double hit on the same led
		{
		score1=score1+1;
		array[5]=number[score1%10];
		array[6]=number[score1/10];
		player1_flag=1;
		}
	}

	else if(P2IFG&BIT2)
	{
		if(player2_flag==0)
		{
		score2=score2+1;
		array[3]=number[score2%10];
		array[4]=number[score2/10];
		player2_flag=1;
		}
	}

	else if(P2IFG&RESET)
	{
		if(gameFin)
			gameFin=0;
		
		score1=0;
		score2=0;
		array[3]=array[4]=array[5]=array[6]=number[0];
		array[0]=array[1]=array[2]=0x00;
		sendArray();
		_delay_cycles(1000000);
		ledCount=0;
		
		_bic_SR_register_on_exit(LPM0_bits);
	}
	P2IFG = 0;							  // Clear P2IFG
}


