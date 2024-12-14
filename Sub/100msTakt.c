
/*! 
 * \brief 100msTakt - обработка прерывания таймера А0 
 *
 *  \par  модуль запускается каждые 100 мс 
 *  производит усреднение, преобразование кода АЦП в давление, формирует код для ЦАПа
 *  Обрабатывает счетчики технологических режимов и режима кнопки сброса в ноль
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

#include <math.h> 
unsigned int ADC_Error_Reg_tmp;

 __bis_SR_register(GIE);                 //  разрешить прерывания
 #include <dampfer.c>       // демпфирование

 //  PpEE = (PpE - Actual.Zero_Shift) * Actual.Span; // вставить коррекцию пользователя 
 
 if (Hart_Meander_Test_Counter)
 {
   #include <HART_meander_test.c>  
   Meander_Pulse_Counter ++;
 }
    /*
 Time_Stamp_Counter.l += 3200;
 if (Time_Stamp_Counter.l >= Time_Stamp_MAx_Limit)
   Time_Stamp_Counter.l = 0;
 */
 /*
           Temperature_Takt_Counter++;
          if (++Temperature_Takt_Counter > 40)
          {
            Device_Status_Reg |= FTemperature_Calculate; 
            Temperature_Takt_Counter = 0;
          }
 */
/*
//загрубление нуля
   if  (Pass.Math_Mode & 0x30)   // включен режим теста S0?
           #include <test_S0.c>


if (!(DAC_Count))         // технология - счетчик блокирования ЦАПа 
{
 #include <p2dac.c>          // пересчет давления в код ЦАП
 //#include <dacUshift.c>          //cдвиг ЦАПа
 #include <dacTcomp.c>          //термокомпенсация ЦАП
}
 #include <dacAlarm.c>          //ограничения аварийных сигналов ЦАП

 __bic_SR_register(GIE);   //Запретить прерывания
 P_Out = PpEE;               //записать значения Р в выходной буфер
 DAC_Out = DAC_Code.w;      //записать значения кода ЦАПа в выходной буфер
 __bis_SR_register(GIE);   // разрешить прерывания
*/
 #include <key_Z.c>  // обработка кнопки сброса в ноль
 
 if (++ADC_Dog_Count > 5) // АЦП повис :(((
 {
    if (ADC_Error_Reg)
     {
       if (0xffff != ADC_Error_Reg)
        {
          ADC_Error_Reg_tmp=ADC_Error_Reg;
          ADC_Error_Reg_tmp<<=1;
        }
        else
          ADC_Error_Reg_tmp=0xfffe;  
       //ADC_Error_Reg = ADC_Error_Reg_tmp;
       /* */
       
        if ( write_data2flash( (unsigned long int)((char *) &ADC_Error_Reg), (char *) &ADC_Error_Reg_tmp, 2) )
          Error_Code = Write2Flash_Error; 
      
      }
     ADC_reset();          // сброс АЦП 
     ADC_fast_start();
     ADC_Dog_Count =0;
 }
 if (ADC_HT_Mode_Count)  // выдержка времени работы технологического режима 
 {
   if (!(--ADC_HT_Mode_Count))
   {                     // выход из технологического режима
     ADC_reset();          // сброс АЦП 
     ADC_fast_start();  
     ADC_Func_Ptr = 0;         // перейти к Read_P_Run_T
   }
   if ((Group_ADC_Pause_Count != 0x07ff)&&(ADC_Status & ADC_Buf_Ready))
      Group_ADC_Pause_Count++;   // отсчет времени после готовности гр. чтения
 }  
   if (DAC_Count)                // выдержка времени блокировки ЦАПа
  //    if (DAC_Count < 255)   // раскоментировать - если хотим, чтоб уставка 255 держала до отмены
      DAC_Count--;

   if (Shift_Count)                 // выдержка времени блокировки P и Т
   { 
     Shift_Count--;  
     if (!(Shift_Count))
     {
       Actual.DAC_Code_Shift = User.DAC_Code_Shift;
       Actual.Zero_Shift = User.Zero_Shift;
     }
   }
#ifdef Debugging  
  if (PT_Count != 255)
#endif    
   if (PT_Count)                 // выдержка времени блокировки P и Т
       PT_Count--;   

