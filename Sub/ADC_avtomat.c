

unsigned char ADC_Buf_Count=0;
unsigned char iadc, jadc, ADC_Data_Count, ADC_Status=0;    
const char Base_ADC[3][5] =
{//режим +повторов  счетчик буфера   CONR1   CONR0       MR0 
       1,              0,             G01,    AIN1,      F16,    // первый канал
       1,              0,             G01,    AIN2,      F16,    // второй канал
       1,              0,             G01,    AIN3,      F16,    // третий канал
};
unsigned char Reg_ADC[3][5];
unsigned char Data_ADC[3][3];
unsigned char Data_Buf_ADC[3][3][8];


 switch (ADC_Status & 0x07)
   { case 0:             // начало работы
      if (Reg_ADC[iadc][0] & 0x7f)    //¬ыключен канал - ноль в счетчике?
        iadc++;                       // тогда - следующий
       else
       {
        ADC_Data_Count =  Reg_ADC[iadc][0]&0x0f;
        jadc=0;
        ADC_write (ConR);
        ADC_write (Reg_ADC[iadc][2]);
        ADC_write (Reg_ADC[iadc][3]);
        ADC_write (MR);
        if (Reg_ADC[iadc][0]&0x80)    // требуетс€ калибровка?
         {
           ADC_write (ZSC);
           ADC_Status |=1;
         }
         else
         {
           ADC_write (CCM); 
           ADC_Status |=3;
         }
         ADC_write (Reg_ADC[iadc][4]);
        }
      break; 
    case 1:  // запущена калибровка,продолжаем
       ADC_write (MR);
       ADC_write (FSC);
       ADC_write (Reg_ADC[iadc][4]);   // м.б. здесь только прерывание сбросить?
       ADC_Status++;                   // поставив флаг - "не читать" дл€ прерывани€?   
       break; 
      
    case 2:  //  калибровка закончена, запускаем чтение
       ADC_write (MR);
       ADC_write (CCM);
       ADC_write (Reg_ADC[iadc][4]);
       ADC_Status++;
       break; 
    case 3:  //   чтение
     
       Data_ADC[iadc][0]= ADC_DR_val[0];
       Data_ADC[iadc][1]= ADC_DR_val[1];
       Data_ADC[iadc][2]= ADC_DR_val[2];
       if (Reg_ADC[iadc][2]++ < ADC_Buf_Count )
       {
         Data_Buf_ADC[iadc][0][(Reg_ADC[iadc][2])]= Data_ADC[iadc][0];
         Data_Buf_ADC[iadc][1][(Reg_ADC[iadc][2])]= Data_ADC[iadc][1];
         Data_Buf_ADC[iadc][2][(Reg_ADC[iadc][2])]= Data_ADC[iadc][2];
       }
       else
         ADC_Status |= ADC_Buf_Ready;
       if ( ++jadc == ADC_Data_Count)
           if( ++iadc == 3)
          {
            iadc=0;
               
          }
        ADC_Status &= 0xf0; 
       break;        
   }









// PORT2 Interrupt Vector handler
#pragma vector=PORT2_VECTOR
__interrupt void Port2_Vector (void)
{ 
  if (Pifg_DoutRDY_ADC & DoutRDY_ADC)
  {
    ADC_write (DR+Read);
    ADC_DR_val[2] = ADC_read();
    ADC_DR_val[1] = ADC_read();
    ADC_DR_val[0] = ADC_read();
    ADC_Status |= ADC_Data_Ready; 
    Pifg_DoutRDY_ADC &= ~DoutRDY_ADC;  // сброс прерывани€
  }
   
}
