
//файл  dacAlarm.c

 /*! 
 * \brief dacAlarm - ограничение значений кода ЦАПа и задание сигналов аварии
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

       // здесь добавить  аварийные токи .. 
       // 0x0000  - авария 3,75ма
       // 0xFFFF - авария 23,5ма
   // DAC_Code_Low_Limit 3.75ma
   // DAC_Code_High_Limit = 21.5ma

/*   раскомментировать, когда будет решение по обработке аварий
if (("alarm") && (Actual.Math_Mode & FCurrent_Alarm_Enable))
   {
     if (Actual.Math_Mode & FCurrent_Alarm_High)
       DAC_Code.w = 0xFFFF;
      else 
        DAC_Code.w = 0x0000;
   }
else
*/

        
DAC_Code_Tmp =   DAC_Code_Tmp+.5 + Actual.DAC_Code_Shift;     // сдвиг кода    

if ((Actual.Math_Mode & FSw_5mA_Off) /* && (Actual.Math_Mode & FCurrent_Alarm_Enable)*/)
 {
  if (/*(Actual.Math_Mode & FCurrent_Alarm_High)&&*/(DAC_Code_Tmp > DAC_Code_High_Limit))
    {
     DAC_Code.w = DAC_Code_High_Limit;
     Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
     Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
    }
   else
    {
     if (/*(!(Actual.Math_Mode & FCurrent_Alarm_High)) && */(DAC_Code_Tmp  < DAC_Code_Low_Limit))
        {
          DAC_Code.w = DAC_Code_Low_Limit;
          Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
        }
       else
        {        
          DAC_Code.w = (unsigned int)(DAC_Code_Tmp);
          Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
        }
     }
 }
else
 {
  if (DAC_Code_Tmp<2)
   {
     DAC_Code_Tmp = 2;
     Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
     Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated;
   }
  else 
  {
   if (DAC_Code_Tmp>0xfffd)
    {
       DAC_Code_Tmp = 0xfffd;
       Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
       Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated;
    }
   else
    {
      DAC_Code.w = (unsigned int)(DAC_Code_Tmp);
      Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
      Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
    }
  }
 }

  /*      
DAC_Code_Tmp =   DAC_Code_Tmp+.5 + Actual.DAC_Code_Shift;     // сдвиг кода    
if (DAC_Code_Tmp<2)
  {
    DAC_Code_Tmp = 2;
    Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
    Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated;
  }
 else 
   if (DAC_Code_Tmp>0xfffd)
    {
       DAC_Code_Tmp = 0xfffd;
       Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
       Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated;
    }

if ((Actual.Math_Mode & FSw_5mA_Off) && (Actual.Math_Mode & FCurrent_Alarm_Enable))
 {
  if ((Actual.Math_Mode & FCurrent_Alarm_High)&&(DAC_Code_Tmp > DAC_Code_High_Limit))
    {
     DAC_Code.w = DAC_Code_High_Limit;
     Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
     Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
    }
   else
    {
     if ((!(Actual.Math_Mode & FCurrent_Alarm_High)) && (DAC_Code_Tmp  < DAC_Code_Low_Limit))
        {
          DAC_Code.w = DAC_Code_Low_Limit;
          Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
        }
       else
        {        
          DAC_Code.w = (unsigned int)(DAC_Code_Tmp);
          Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
        }
     }
 }
 else
        {
          DAC_Code.w = (unsigned int)(DAC_Code_Tmp);
          Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
        }
*/
/*
if ((Actual.Math_Mode & FSw_5mA_Off) && (Actual.Math_Mode & FCurrent_Alarm_Enable))
 {
  if ((Actual.Math_Mode & FCurrent_Alarm_High)&&((DAC_Code_Tmp + Actual.DAC_Code_Shift) > DAC_Code_High_Limit))
    {
     DAC_Code.w = DAC_Code_High_Limit;
     Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
     Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
    }
   else
    {
     if ((!(Actual.Math_Mode & FCurrent_Alarm_High)) && ((DAC_Code_Tmp + Actual.DAC_Code_Shift) < DAC_Code_Low_Limit))
        {
          DAC_Code.w = DAC_Code_Low_Limit;
          Device_Status_Byte.Primary_Master |= FLoop_Current_Saturated;        // одновременно устанавливаются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master |= FLoop_Current_Saturated; 
        }
       else
        {
          DAC_Code.w = (unsigned int)(DAC_Code_Tmp+.5 + Actual.DAC_Code_Shift);
          Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
        }
     }
 }
 else
        {
          DAC_Code.w = (unsigned int)(DAC_Code_Tmp+.5 + Actual.DAC_Code_Shift);
          Device_Status_Byte.Primary_Master &= ~FLoop_Current_Saturated;        // одновременно сбрасываются флаги Loop_Current_Saturated для первого и второго мастера
          Device_Status_Byte.Secondary_Master &= ~FLoop_Current_Saturated;           
        }
*/




