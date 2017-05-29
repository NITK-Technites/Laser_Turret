#include <msp430.h>

//#define N_channel 12
#define SIN595      BIT4    //port2
#define SCLK595     BIT7    //port1
#define LTCH595     BIT6    //port1
//RESET Button
#define RESET       BIT0    //port2

#define INTP1 BIT1          //Interrupt bit for Player 1 (port2)
#define INTP2 BIT2          //Interrupt bit for Player 2 (port2)

int score1=0,score2=0;                          //Scores of Player1 and Player2
int prev_score1=0, prev_score2=0;
int player1_flag=0, player2_flag=0;
int MAXledCount=30;
int ledCount=0;
int gameFin=0;

int array[7] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

int numbers[6][10] = {0xE3, 0xF3, 0xE3, 0xC3, 0xDD, 0xC1, 0xC1, 0xC1, 0xC1, 0xC1,
                    0xD9, 0xE3, 0xDD, 0xFD, 0xDD, 0xDF, 0xDF, 0xFD, 0xDD, 0xDD,
                    0xD5, 0xF3, 0xFD, 0xFD, 0xDD, 0xC3, 0xC1, 0xFD, 0xC1, 0xC1,
                    0xD5, 0xF3, 0xE3, 0xE1, 0xC1, 0xFD, 0xDD, 0xFD, 0xDD, 0xFD,
                    0xCD, 0xF3, 0xDF, 0xFD, 0xFD, 0xDD, 0xDD, 0xFD, 0xDD, 0xFD,
                    0xE3, 0xE1, 0xC1, 0xC3, 0xFD, 0xE3, 0xC1, 0xFD, 0xC1, 0xC1
};
int disp[6][4] = {0xE3, 0xE3, 0xE3, 0xE3,
                    0xD9, 0xD9, 0xD9, 0xD9,
                    0xD5, 0xD5, 0xD5, 0xD5,
                    0xD5, 0xD5, 0xD5, 0xD5,
                    0xCD, 0xCD, 0xCD, 0xCD,
                    0xE3, 0xE3, 0xE3, 0xE3
};
/*
* main.c
*/
/*int global = 0, global1 = 0;
void put_score_on_matrix() {
    int tenth, unit, punit, ptenth, i=0, j=0;
    if(prev_score1 != score1) {
        unit = score1%10;
        tenth = (score1/10)%10;
        punit = prev_score1%10;
        ptenth = (prev_score1/10)%10;

        for(i=global;i<6;i++) {
            if(unit != punit) {
                disp[i-1][1] = disp[i][1];
            }
            if(tenth != ptenth) {
                disp[i-1][0] = disp[i][0];
            }
        }

        if(global == 0) {
            if(unit != punit) {
                disp[5-global][1] = 0xFF;
            }
            if(tenth != ptenth) {
                disp[5-global][0] = 0xFF;
            }
        }
        else {
            for(i=0,j=global-1;i<global;i++,j--) {
                if(unit != punit) {
                    disp[5-i][1] = numbers[j][unit];
                }
                if(tenth != ptenth) {
                    disp[5-i][0] = numbers[j][tenth];
                }
            }
        }
        //prev_score1 = score1;
        global++;
        if(global == 8) {
            prev_score1 = score1;
            global = 0;
        }
    }
    if(prev_score2 != score2) {
        unit = score2%10;
                tenth = (score2/10)%10;
                punit = prev_score2%10;
                ptenth = (prev_score2/10)%10;

                for(i=global1;i<6;i++) {
                    if(unit != punit) {
                        disp[i-1][3] = disp[i][3];
                    }
                    if(tenth != ptenth) {
                        disp[i-1][2] = disp[i][2];
                    }
                }

                if(global1 == 0) {
                    if(unit != punit) {
                        disp[5-global1][3] = 0xFF;
                    }
                    if(tenth != ptenth) {
                        disp[5-global1][2] = 0xFF;
                    }
                }
                else {
                    for(i=0,j=global1-1;i<global1;i++,j--) {
                        if(unit != punit) {
                            disp[5-i][3] = numbers[j][unit];
                        }
                        if(tenth != ptenth) {
                            disp[5-i][2] = numbers[j][tenth];
                        }
                    }
                }
                //prev_score1 = score1;
                global1++;
                if(global1 == 8) {
                    prev_score2 = score2;
                    global1 = 0;
                }
    }
}*/

void put_score_on_matrix() {
    int tenth, unit, punit, ptenth, i=0, j=0;
    if(prev_score1 != score1) {
        unit = score1%10;
            tenth = (score1/10)%10;

            for(i=0;i<6;i++) {
                disp[i][1] = numbers[i][unit];
                disp[i][0] = numbers[i][tenth];
            }
            prev_score1 = score1;
    }
    if(prev_score2 != score2) {
        unit = score2%10;
            tenth = (score2/10)%10;

            for(i=0;i<6;i++) {
                disp[i][3] = numbers[i][unit];
                disp[i][2] = numbers[i][tenth];
            }
            prev_score2 = score2;
    }

}

int main(void) {
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

  BCSCTL1 = CALBC1_16MHZ;                  // Set DCO to 1MHz
  DCOCTL = CALDCO_16MHZ;

  P1DIR = 0x00;
  P2DIR = 0x00;
  P1DIR |= SCLK595+LTCH595;
  P2DIR |= SIN595;

  P2DIR &= ~(INTP1 + INTP2);
  P2IE |= (INTP1 + INTP2);
  P2IES &= ~(INTP1 + INTP2);
  P2REN |= (INTP1 + INTP2);
  P2IFG = 0x00;
  P2OUT &= ~(INTP1 + INTP2);

  CCR0 = 50000;
  TACTL = TASSEL_2 + MC_2;                  // SMCLK, contmode

  __enable_interrupt();

  volatile int i=0, j=0, temp, temp0;
  long long int k=0;
  volatile int row_select=0, board=0, rand1, rand2;

  for(i=0,row_select=0,k=0;;i++,k++) {
      if(k==2000||k==4000||k==6000||k==8000||k==10000||k==12000||k==14000) {
          put_score_on_matrix();
      }
      if(k==50000) {
          k=0;
          rand1 = rand()%7;
          rand2 = rand()%7;
          array[5] = BIT7>>rand1;
          array[6] = BIT7>>rand2;

          //array[5] = 212;
          //array[6] = 212;
          player1_flag=0;
          player2_flag=0;
      }

    if(i == 7) {
        i = 0;
        P1OUT |= LTCH595;
        P1OUT &= ~LTCH595;

    }
    if(row_select > 5) {
        row_select = 0;
        board++;
        board = board%2;
    }

    if(i == 0 || i == 5 || i == 6) {
      temp = array[i];
    }
    else {
        if(i == 4) {
          temp = 0x00;
          temp = BIT7>>row_select;
          if(board==1) {
              if(disp[row_select][3]&(BIT5)) {
                  temp |= BIT1;
              }
              if(disp[row_select][3]&(BIT4)) {
                  temp |= BIT0;
              }
              /*temp0 = disp[row_select][3]&(BIT5);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT4);
              temp0 = temp0<<3;
              temp += temp0;*/
          }
          else {
              //temp += 0xFF&(BIT4+BIT5);
              temp |= BIT1;
              temp |= BIT0;
          }
          //temp = 0x00;
          //temp = BIT6|BIT7|BIT2|BIT3|BIT4|BIT5;
          //temp = 212;
          row_select++;
        }
        else if(i == 3) {
          temp = 0x00;
          if(board==0) {
              /*temp0 = disp[row_select][0]&(BIT3);
              temp0 = temp0>>3;
              temp += temp0;

              temp0 = disp[row_select][0]&(BIT2);
              temp0 = temp0>>1;
              temp += temp0;

              temp0 = disp[row_select][0]&(BIT1);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][0]&(BIT0);
              temp0 = temp0<<3;
              temp += temp0;*/
              if(disp[row_select][0]&(BIT5)) {
                  temp |= BIT7;
              }
              if(disp[row_select][0]&(BIT4)) {
                  temp |= BIT6;
              }
              if(disp[row_select][0]&(BIT3)) {
                  temp |= BIT5;
              }
              if(disp[row_select][0]&(BIT2)) {
                  temp |= BIT4;
              }
              if(disp[row_select][0]&(BIT1)) {
                  temp |= BIT3;
              }
              if(disp[row_select][0]&(BIT0)) {
                  temp |= BIT2;
              }
          }
          else {
              temp = 0xFF&(BIT7|BIT6|BIT5|BIT4|BIT3|BIT2);
          }
          if(board==1) {
              /*temp0 = disp[row_select][1]&(BIT5);
              temp0 = temp0>>1;
              temp += temp0;

              temp0 = disp[row_select][1]&(BIT4);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][1]&(BIT3);
              temp0 = temp0<<3;
              temp += temp0;

              temp0 = disp[row_select][1]&(BIT2);
              temp0 = temp0<<5;
              temp += temp0;*/
              if(disp[row_select][3]&(BIT3)) {
                  temp |= BIT1;
              }
              if(disp[row_select][3]&(BIT2)) {
                  temp |= BIT0;
              }
          }
          else {
              temp += 0xFF&(BIT1|BIT0);
          }
          //temp = 0x00;
          //temp = 212;
        }
        else if(i == 2) {
          temp = 0x00;
          if(board == 0) {
              /*temp0 = disp[row_select][1]&(BIT0);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][1]&(BIT1);
              temp0 = temp0>>1;
              temp+= temp0;*/
              if(disp[row_select][1]&(BIT5)) {
                  temp |= BIT7;
              }
              if(disp[row_select][1]&(BIT4)) {
                  temp |= BIT6;
              }
              if(disp[row_select][1]&(BIT3)) {
                  temp |= BIT5;
              }
              if(disp[row_select][1]&(BIT2)) {
                  temp |= BIT4;
              }
              if(disp[row_select][1]&(BIT1)) {
                  temp |= BIT3;
              }
              if(disp[row_select][1]&(BIT0)) {
                  temp |= BIT2;
              }
          }
          else {
              temp = 0xFF&(BIT7|BIT6|BIT5|BIT4|BIT3|BIT2);
          }

          if(board == 1) {
              /*temp0 = disp[row_select][2]&(BIT5);
              temp0 = temp0>>3;
              temp += temp0;

              temp0 = disp[row_select][2]&(BIT4);
              temp0 = temp0>>1;
              temp += temp0;

              temp0 = disp[row_select][2]&(BIT3);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][2]&(BIT2);
              temp0 = temp0<<3;
              temp += temp0;

              temp0 = disp[row_select][2]&(BIT1);
              temp0 = temp0<<5;
              temp += temp0;

              temp0 = disp[row_select][2]&(BIT0);
              temp0 = temp0<<7;
              temp += temp0;*/
              if(disp[row_select][3]&(BIT1)) {
                  temp |= BIT1;
              }
              if(disp[row_select][3]&(BIT0)) {
                  temp |= BIT0;
              }
          }
          else {
              temp += 0xFF&(BIT1|BIT0);
          }
          //temp = 0x00;
          //temp = 212;
        }
        else if(i == 1) {
          temp = 0x00;
          if(board == 1) {
              /*temp0 = disp[row_select][3]&(BIT5);
              temp0 = temp0>>3;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT4);
              temp0 = temp0>>1;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT3);
              temp0 = temp0<<1;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT2);
              temp0 = temp0<<3;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT1);
              temp0 = temp0<<5;
              temp += temp0;

              temp0 = disp[row_select][3]&(BIT0);
              temp0 = temp0<<7;
              temp += temp0;

              temp += BIT1;
              temp += BIT0;*/
              if(disp[row_select][2]&(BIT5)) {
                  temp |= BIT7;
              }
              if(disp[row_select][2]&(BIT4)) {
                  temp |= BIT6;
              }
              if(disp[row_select][2]&(BIT3)) {
                  temp |= BIT5;
              }
              if(disp[row_select][2]&(BIT2)) {
                  temp |= BIT4;
              }
              if(disp[row_select][2]&(BIT1)) {
                  temp |= BIT3;
              }
              if(disp[row_select][2]&(BIT0)) {
                  temp |= BIT2;
              }
          }
          else {
              temp = 0xFF;
          }
          //temp = 0x00;
          //temp = 212;
        }
    }

    for(j=0;j<8;j++) {
      if(temp&BIT0) {
        P2OUT |= SIN595;
      }
      else {
        P2OUT &= ~SIN595;
      }

      P1OUT |= SCLK595;
      P1OUT &= ~SCLK595;
      temp = temp>>1;
    }
  }

  return 0;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void) {
  if(P2IFG&BIT1) {
      if(player1_flag==0) {
          score1++;
          player1_flag=1;
      }

  }

  else if(P2IFG&BIT2) {
      if(player2_flag==0) {
          score2++;
          player2_flag=1;
      }
  }
  P2IFG = 0x00;
}
