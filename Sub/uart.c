#include  "msp430x54xA.h"
//#include  <types_define.h>
#include  <sub_main_data.h>


/*! 
 * \brief UART_init - инициализация последжовательного порта для технологического обмена
 *
 * 
 *
 *  Stack:     не использует \par
 *  Return:    возвращает значение скомпенсированного давления \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void UART_init(void)
{ 
  Work_Key_Z;                  //задать направление для порта - ввод для кнопки в случае, когда выключен передатчик    
 // Pren_Key_Z |= Key_Z;         // разрешить подтягивающие резисторы
//  Pout_Key_Z |= Key_Z;         // подтяжка вверх 
  
//  UARTht_TXenable(); //отладка - убрать при нормальной работе
  
  P3SEL |= 0x20;                             // P3.5 = USCI_A0 RXD
  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  
/*  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
 */ 
 
 UCA0CTL1 |= UCSSEL1 + UCSSEL0;       // SMCLK  
 UCA0BRW = UCBR_HT;                // надо разделить на 16 если устанавливаешь UCOS16
  UCA0MCTL =  UCBRS_HT;            // Modulation UCBRSx = ?
  UCA0CTL1 &= ~UCSWRST;            // **Initialize USCI state machine**
  
  flagHTechT = 0;
  flagHTechR = 0;
  UARTht_IERXenable();
  UARTht_IETXdesable();

  
  __bis_SR_register(GIE);
}
//------------------------------------------------------------------------------            
// Подпрограмма подготовки переменных для передачи по UART с прерыванием
WORD InitBufUSART_Tx(void){
 BYTE i, dcnt, CRC = 0;
 WORD jj=30000;
 while((flagHTechT&TPocket)&&(jj--));
 if(jj){
    ptrURX_Tx = TransmitBufer;
    dcnt = TransmitBufer[3]-1;
    cntURX_Tx=6+dcnt;
 for(i=0;i<4;i++)
     CRC = crc_table[CRC^TransmitBufer[i]] ; //CRC += TransmitBufer[i];
     TransmitBufer[4] =  CRC;
     CRC = 0;
  for(i=0; i<dcnt; i++)
      CRC = crc_table[CRC^TransmitBufer[i+5]] ; //CRC += TransmitBufer[i+5];
     TransmitBufer[dcnt+5] = CRC;
    flagHTechT &= CLRTrans;
    flagHTechT |=  (Start55 + TPocket);
//    cntURX_Tx--;
// Запрет передачи при нажатой кнопке сброса   
   Key_Z_Status = Get_Key_Z; 
   if (!(Key_Z_Status))        // Key_Z нажата?    
     UARTht_TXdesable();       // запретить выход передатчика
   else 
      UARTht_TXenable();       // разрешить выход передатчика
// проверка кнопки завершена   
    NextCHT(0xFF);             // Отправить первый байт пакета
    AA_FF_counter = 0;
    flagHTechR = FindeRPocket; // Разрешить прием пакетов
    UARTht_IETXenable();       // Запуск передачи по прерываниям
 } 
  return(jj); 
}
/*
//------------------------------------------------------------------------------            
// Подпрограмма передачи по UART с прерыванием
// Используется буфер bufURX размером SizeRxBuf и счетчик cntURX_Tx
void StartTx(void){
    flagHTechT &= CLRTrans;
    flagHTechT |=  (Start55 + TPocket);
    NextCHT(0xFF);
    UARTht_IERXenable();
}
*/
/*
//------------------------------------------------------------------------------            
// Подпрограмма копирования переменных в буфер для передачи по UART
void putc( char x){
   *ptrURX_Tx++ = x; 
   cntURX_Tx++;
}
*/
/*
//============================================================================== 
// Замена сандартной программы вывода символа на std output
int putchar(int outch)
{ WORD ich = 60000;
	WhileTX(ich);  // USART TX buffer ready?
        NextCHT((BYTE)outch);        // Output character to TXBUF
  return outch;
}
*/
/*
//------------------------------------------------------------------------------
// Вывод двух байт ASCII кода выводимого байта в UART без прерывания
void putHEX(unsigned char ah)
{
  unsigned char al;
    al = (ah>>4) & 0xF;
    putchar(al<0x0A?al+ 0x30:al+0x37);
    al = ah & 0xF;
        putchar(al<0x0A?al+ 0x30:al+0x37);
}
*/
/*
// Вывод строки символов определенной длины в UART
void putstr(BYTE * ah, char lng )
{ char i =0;
 for(i=0;i<lng;i++) putchar(ah[i]);
}
*/
/*
// Вывод строки символов определенной длины в UART 
// с переводом на следующую строку
void putstrl(BYTE * ah, char lng )
{ for(char i=0;i<lng;i++) 
      putchar(ah[i]);
 putchar(0x0D);
 putchar(0x0A);
}
*/
/*
// Вывод в UART символа перевод на следующую строку
void putln(void)
{putchar(0x0D);
 putchar(0x0A);
}
*/
/*
//------------------------------------------------------------------------------            
// Подпрограмма передачи по UART фиксированной строки
// mptr - указатель на начало строки
// mptr[0] - длина передаваемой строки
BYTE putConstStr( char * mptr){
 BYTE i;
  for(i=0;i<mptr[0];i++)
   TransmitBufer[i] = mptr[i+1];
   return( InitBufUSART_Tx());
}
*/
//------------------------------------------------------------------------------

// прерывание UART0
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)

{BYTE tch;

if(UCA0IV & 0x0002)         // прерывание по приему?
  {
   ReadRXUART(tch);           // да, прочитать байт
   #include <sub_HTp.c>       // обработка прочитатнного 
  }
 else
 {                            // прерывание по передаче
  if(flagHTechT & TPocket)
   {
    if(flagHTechT & Start55)
     { 
       if( ++AA_FF_counter >3)
       {
        AA_FF_counter=0; 
        NextCHT(0x55); 
        flagHTechT &= (~Start55);
       }
       else
          NextCHT(0xff);
     }
     else
      if(flagHTechT & EndAA)
      { 
         NextCHT(0xAA); 
       if( ++AA_FF_counter >3)
       {
         AA_FF_counter=0;         
         UARTht_IETXdesable();
         flagHTechT &= ~(EndAA + Start55 + TPocket);
       }
        /* 
       
        NextCHT(0xAA); 
        if (flagHTechT & EndTX)
        {
          UARTht_IETXdesable();  
        //  UARTht_TXdesable();
          flagHTechT &= ~(EndAA + Start55 + TPocket + EndTX);
        }
        else
         {
           flagHTechT |= EndTX;
         }
        */
      }
      else
         if(flagHTechT & Ferst55)
          { 
            NextCHT(0x55);
            flagHTechT &= ~Ferst55;
          }
      else
       if(flagHTechT & End55)
        { 
          NextCHT(0x55); 
          flagHTechT &= (~End55);
          flagHTechT |= (EndAA);
          AA_FF_counter =0;
        }
          else
           { 
             tch = *ptrURX_Tx++;
             if(!(--cntURX_Tx)) flagHTechT |= End55;
             NextCHT(tch);
             if(tch == 0x55)     flagHTechT |= Ferst55;
           }
   }
 }
}



