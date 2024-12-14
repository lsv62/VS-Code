/* file 
*/
#include  "msp430x54xA.h"
#include  <ADC7799.h>

//#include  <types_define.h>
#include  <sub_main_data.h>


unsigned char Buf_ADC_Ptr[3]={0,0,0};
unsigned char iadc, jadc, ADC_Data_Count;   


/*! 
 * \brief AD7799_init - инициализация АЦП 
 *
 *  \par  Настраивается интерфейс для работы с  AD7799
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void AD7799_init(void)    // инициализация АЦП
{

  CS_ADC_1;                     // рекомендованное значение - 1 
  Pdir_CS_ADC |= CS_ADC;        // вкл. на выход
  
  Din_ADC_1;                      // рекомендованное значение - 1
  Pdir_Din_ADC |= Din_ADC;        // вкл. на выход
  
  SCLK_ADC_1;                       // рекомендованное значение - 1
  Pdir_SCLK_ADC |= SCLK_ADC;        //  вкл. на выход
  
  
  Pdir_DoutRDY_ADC &= ~DoutRDY_ADC;        //  вкл. на вход 
  Pren_DoutRDY_ADC |= DoutRDY_ADC;         // разрешить подтягивающие резисторы
  Pout_DoutRDY_ADC |= DoutRDY_ADC;         // подтяжка вверх
  Pies_DoutRDY_ADC |= DoutRDY_ADC;         // прерывание при переходе из 1 в 0
  Pifg_DoutRDY_ADC &= ~DoutRDY_ADC;        // сбросить PхIFG
   
   for (iadc=0;iadc<3;iadc++)   // перенос настроек
 {
   Reg_ADC[iadc][0]=1;
   for (jadc=1;jadc<4;jadc++)  //  в ОЗУ загнать базовые настройки АЦП
     Reg_ADC[iadc][jadc]=ADC_Flash.Base_ADC[iadc][jadc-1];
 }
 iadc=0;
}



/*! 
 * \brief ADC_read - чтение байта из АЦП
 *
 *  \par на время работы модуля блокируется система прерывания
 *       для получения результата измерения необходимо вычитать 3 байта 
 *
 *  Stack:     не использует \par
 *  Return:    байт, считанный из АЦП \par
 *  \param    None
 *  \return   Байт данных из АЦП
 *  \sa       None
 * 
 *****************************************************************************/
// Чтение байта из АЦП
__monitor unsigned char ADC_read(void)  // результат - считанный байт
{ unsigned char  i,d;
   for (i=8,d=0; i; i--)
    { SCLK_ADC_0;    //снять синхросигнал через t2 появится бит данных
      d<<=1;         // если будут сбои - то поставить перед SCLK_ADC_0
      d|=(P_DoutRDY_ADC & DoutRDY_ADC)?1:0;
      SCLK_ADC_1;    //подать синхросигнал
    }
  return  d;         //вернуть прочитанный байт
}



/*! 
 * \brief ADC_write - запись байта в АЦП
 *
 *  \par на время работы модуля блокируется система прерывания
 *       для получения результата измерения необходимо вычитать 3 байта 
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    Data - байт, передаваемый в АЦП
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

// Запись байта в АЦП
__monitor void  ADC_write(unsigned char  Data)  //Data: байт данных для записи
{ unsigned char  i;
 for (i=8;i;i--)
  {SCLK_ADC_0;                                  //снять синхросигнал
   if (Data & 0x80) Din_ADC_1; else Din_ADC_0;  //выставить бит данных
   SCLK_ADC_1;                                  //выдать синхросигнал
   Data <<= 1;}                                 //подготовить следующий бит
 Din_ADC_1;                                     //рекомендованное значение - 1 на время паузы
 return;                     //Возврат из подпрограммы
}




/*! 
 * \brief ADC_reset - сброс АЦП 
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
__monitor void  ADC_reset(void)
{ unsigned char  i;
 CS_ADC_0;  
 Din_ADC_1; 
 for (i=32;i;)
  {
    SCLK_ADC_0;  //снять синхросигнал
    i--;
    SCLK_ADC_1;  //выдать синхросигнал
  }
 return;         //Возврат из подпрограммы
}


/*! 
 * \brief ADC_fast_start - запуск АЦП в работу
 *
 *  \par  производится начальная инициализация внутренних регистров АЦП
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void ADC_fast_start(void)
{
//  unsigned char iadc, jadc; 
  
CS_ADC_0;                      // разрешить работу АЦП
ADC_Func_Ptr=0;                // Инициализировать автомат АЦП
 
 __bic_SR_register(GIE);      // запретить прерывания 

  for (iadc=0; iadc<3; iadc++)
        ADC_write (ConR);                          // запись в конфигурационный регистр
         ADC_write (Reg_ADC[iadc][1]);   // старший байт  - коэффициент усиления 
         ADC_write (Reg_ADC[iadc][2]);   // младший байт - канал 
        ADC_write (MR);                            // запись в регистр режима
         ADC_write (CCM);               // старший байт - постоянное преобразование
         ADC_write (Reg_ADC[iadc][3]);  // младший байт - частота преобразования
          
 
  Pifg_DoutRDY_ADC &= ~DoutRDY_ADC;      // сброс требования прерывания от АЦП
  Pie_DoutRDY_ADC |= DoutRDY_ADC;         // прерывание от АЦП разрешить 
  
  
  __bis_SR_register(GIE);      //разрешить прерывания
  
  
  
}

#include  "ADC_com.c"



/*! 
 * \brief ADC_Automat - модуль работы с АЦП в  технологическом режиме
 *
 *  \par  обеспечивается работа с АЦП с поддержкой специфики технологических команд 
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void ADC_Automat (void)  // автомат АЦП для НТ
{
  if (( ADC_Status & ADC_Buf_Start)&&((ADC_Status & 0x07) != 1))
   {                         // если есть команда группового чтения,
     ADC_Status =0;                  // инициализировать 
     ADC_Status &= ~ADC_Buf_Start;   // переменные
     Buf_ADC_Ptr[0]=0;               // для 
     Buf_ADC_Ptr[1]=0;               // группового
     Buf_ADC_Ptr[2]=0;               // чтения 
   } 
 
 switch (ADC_Status & 0x07)
   { case 0:             // начало работы
      if (!(Reg_ADC[iadc][0] & 0x0f))    //Выключен канал - ноль в счетчике?
      {
        if (++iadc>2) 
          iadc=0;                       // тогда - следующий
      }
       else
       {                                            // иначе:
        ADC_Data_Count =  Reg_ADC[iadc][0]&0x0f;    // установить счетчик циклов чтения
        jadc=0;
        ADC_write (ConR);                          // запись в конфигурационный регистр
        ADC_write (Reg_ADC[iadc][1]);
        ADC_write (Reg_ADC[iadc][2]);
        ADC_write (MR);                            // запись в регистр режима
        if (Reg_ADC[iadc][0]&0x80)    // требуется калибровка?
         {
           ADC_Status |= 1;           
           ADC_write (ZSC);           //калибровка нуля
         }
         else
         {
           ADC_Status |=3;
           ADC_write (CCM);            // обычное чтение
         }
         ADC_write (Reg_ADC[iadc][3]);
        }
      break; 
    case 1:  // запущена калибровка,продолжаем
       ADC_write (MR);
       ADC_write (FSC);               //калибровка полного значения
       ADC_write (Reg_ADC[iadc][3]);   
       ADC_Status++;                    
       break; 
      
    case 2:  //  калибровка закончена, запускаем чтение
       ADC_write (MR);
       ADC_write (CCM);
       ADC_write (Reg_ADC[iadc][3]);
       ADC_Status++;
       break; 
    case 3:  //   чтение
     
       Data_ADC[iadc][0]= ADC_DR_val.c[0];
       Data_ADC[iadc][1]= ADC_DR_val.c[1];
       Data_ADC[iadc][2]= ADC_DR_val.c[2];
       
       
#define HT_Debugiing       

#ifdef HT_Debugiing   // включен режим отладки технологии
      if(HT_Debugiing_Column_Row!=0xff)
       switch (iadc)
        {
          case 0: // давление
                P_int.l = (unsigned long)Point[HT_Debugiing_Column_Row].P_DAC ;
                Data_ADC[iadc][0]= P_int.c[0];
                Data_ADC[iadc][1]= P_int.c[1];
                Data_ADC[iadc][2]= P_int.c[2];            
            break;
          case 1: // температура
                T_int.l = (unsigned long)Point[HT_Debugiing_Column_Row].T_DAC ;
                Data_ADC[iadc][0]= T_int.c[0];
                Data_ADC[iadc][1]= T_int.c[1];
                Data_ADC[iadc][2]= T_int.c[2];                      
            break;  
          default:// 3-й канал?:))
                Data_ADC[iadc][0]= 0;
                Data_ADC[iadc][1]= 0;
                Data_ADC[iadc][2]= 0;
            break;
        }  
#endif       
       
       
       if (!(ADC_Status & ADC_Buf_Ready))// если групповое чтение не закончено 
       {// запись в буфер группового чтения:
         if((Buf_ADC_Ptr[iadc] < 8)&& (iadc < 3))
          {
            Data_Buf_ADC[iadc][0][(Buf_ADC_Ptr[iadc])]= Data_ADC[iadc][0];
            Data_Buf_ADC[iadc][1][(Buf_ADC_Ptr[iadc])]= Data_ADC[iadc][1];
            Data_Buf_ADC[iadc][2][(Buf_ADC_Ptr[iadc])++]= Data_ADC[iadc][2];
          }
         
         if (((Buf_ADC_Ptr[0]>= ADC_Buf_Cnt)||(!(Reg_ADC[0][0]&0x0f)))&&\
             ((Buf_ADC_Ptr[1]>= ADC_Buf_Cnt)||(!(Reg_ADC[1][0]&0x0f)))&&\
             ((Buf_ADC_Ptr[2]>= ADC_Buf_Cnt)||(!(Reg_ADC[2][0]&0x0f)))) 
                 ADC_Status |= ADC_Buf_Ready;  // если буфер заполнен - снять флаг групп. чтения
       }
       if ( ++jadc >= ADC_Data_Count)
       { 
         if( ++iadc >= 3)
          {
            iadc=0;   
          }
        ADC_Status &= 0xf0; 
       }
       break;        
   }
  // переменные температуры и давления - подготовить для работы:
  T_int.c[0] = Data_ADC[1][0];
  T_int.c[1] = Data_ADC[1][1];
  T_int.c[2] = Data_ADC[1][2];
  T_int.c[3] = 0;
  
  P_int.c[0] = Data_ADC[0][0];
  P_int.c[1] = Data_ADC[0][1];
  P_int.c[2] = Data_ADC[0][2];
  P_int.c[3] = 0;
 }








// PORT2 Interrupt Vector handler
#pragma vector=PORT1_VECTOR
__interrupt void Port1_Vector (void)
{ 
 if (Pifg_DoutRDY_ADC & DoutRDY_ADC) // Прерывание от АЦП?
  { 
    ADC_write (DR+Read);          // да, дать команду на чтение
   
    ADC_DR_val.c[2] = ADC_read(); // получить данные
    ADC_DR_val.c[1] = ADC_read();
    ADC_DR_val.c[0] = ADC_read();
    ADC_DR_val.c[3] = 0;

  //  ADC_Status |= ADC_Data_Ready; 
    
    Pifg_DoutRDY_ADC &= ~DoutRDY_ADC;  // сброс прерывания от АЦП
    ADC_Dog_Count = 0;                  // сбросить "собаку" АЦП
 // if (ADC_Status &  ADC_HT_Mode)

  if (ADC_HT_Mode_Count)          //режим НТ активен?
     ADC_Automat ();                    // автомат АЦП для НТ
    else  
    (*TaskFunction[ADC_Func_Ptr])();    // автомат АЦП
  __bis_SR_register(GIE);              //  разрешить прерывания
  
/*  
// отладка
  Filter_P[FilterPtr++]= ADC_Func_Ptr; 
  if (FilterPtr == 800)
    FilterPtr = 0 ; // точка останова
// конец отладочной вставки  
  */
  
if  (( ADC_P_Count != 1) && ( ADC_P_Count != 3)&& ( ADC_P_Count != 5))
{
  
/* вставка для измерения на частоте 62HZ */  
  if ((Reg_ADC[0][3] == F62) && (( ADC_P_Count > 6)||(!(ADC_P_Count)))) // если все калибровки пройдены
   {
     P_int_Sum_62Hz += P_int.l;              //проводим  усреднение из 4 отсчетов
     if (++ADC_Sample_Count_62Hz >= 4)
       {
         P_int.l = P_int_Sum_62Hz >> 2;   // усредненное значение подставляем для расчета
         P_int_Sum_62Hz = 0;               // сбросить усреднение для 62 Гц
         ADC_Sample_Count_62Hz = 0;
       }
   }  
/* конец вставки для измерения на частоте 62HZ */ 
  
 if (!(ADC_Sample_Count_62Hz)) // если есть усредненное значение давления или значение давления после калибровок
 {                        // проводим расчет
       
  //** test

 P3OUT |= 0x40;   
  
 //**end test
    
  if (!(PT_Count))  
   {               // нет отладочной подмены Р и Т
    
    P_float.f = (float)P_int.l;
    T_float.f = (float)T_int.l;
    /* 
    P_float.f = 8300304;
    T_float.f = 9620420;
   */
   }

  #include <pTcompensation.c>             // термокомпенсация
  if (!(Actual.Filter_Limit))      // демпфер выключен?
     PpE=PpT;  

 //коррекция характеристики
switch (Actual.Math_Type & 0x03)   // выбор режима  по заводским уставкам
       { 
         case 0:           // без термокоррекции        
            __bic_SR_register(GIE);   //Запретить прерывания
            PpEt = (PpE - Actual.Zero_Shift0) * Actual.Span0 ; 
            __bis_SR_register(GIE);   // разрешить прерывания
           break;
         case 1:           // polinom..
             if (Pass.Span1) // если есть данные для этого способа термокоррекции
               {
                 __bic_SR_register(GIE);   //Запретить прерывания
                 PpEt = (PpE - Actual.Zero_Shift1) * Actual.Span1;   
                 __bis_SR_register(GIE);   // разрешить прерывания
               }
           break;
         case 2:// cubic_spl
             if (Pass.Span2)  // если есть данные для этого способа термокоррекции
               {
                __bic_SR_register(GIE);   //Запретить прерывания 
                PpEt = (PpE - Actual.Zero_Shift2) * Actual.Span2 ; 
                __bis_SR_register(GIE);   // разрешить прерывания
               }
           break;  
         case 3:  // zavod ?
             if (Pass.Span3)   // если есть данные для этого способа термокоррекции
               {
                 __bic_SR_register(GIE);   //Запретить прерывания
                 PpEt = (PpE - Actual.Zero_Shift3) * Actual.Span3;
                 __bis_SR_register(GIE);   // разрешить прерывания
                }
             break;  
         default:
             break; 
           }
  
   PpEE = (PpEt - Actual.Zero_Shift) * Actual.Span; // вставить коррекцию пользователя 

//загрубление нуля
   if  (Actual.Math_Type & 0x30)   // включен режим теста S0?
           #include <test_S0.c>

if (!(Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed))    // токовый выход не блокирован HARTом?   
 {
  if (!(DAC_Count))         // технология - счетчик блокирования ЦАПа 
   {
     #include <p2dac.c>          // пересчет давления в код ЦАП
    //#include <dacUshift.c>          //cдвиг ЦАПа
     #include <dacTcomp.c>          //термокомпенсация ЦАП
     #include <dacAlarm.c>          //ограничения аварийных сигналов ЦАП
   }
 }
              //записать значение Р в выходной буфер
 if (!(Hart_Meander_Test_Counter))
  {
 __bic_SR_register(GIE);   //Запретить прерывания
 P_Out = PpEE;     
 if (Flash_Device_Status_Byte.Primary_Master & FLoop_Current_Fixed)    //  выходной ток фиксирован?   
    DAC_Out = Flash_Device_Status_Byte.DAC_Code_Fixed;     //записать значение кода ЦАПа фиксированного тока в выходной буфер
  else
    DAC_Out = DAC_Code.w;      //записать значение кода ЦАПа в выходной буфер
 __bis_SR_register(GIE);   // разрешить прерывания
    if ((PpEE > Pass.Pmax_Lim) ||(PpEE < Pass.Pmin_Lim )) // или надо текущие?
      {
        Device_Status_Byte.Primary_Master |= FPrimary_Variable_Out_of_Limits;        // одновременно устанавливаются флаги Primary_Variable_Out_of_Limits для первого и второго мастера
        Device_Status_Byte.Secondary_Master |= FPrimary_Variable_Out_of_Limits;  
      }
     else
      {
        Device_Status_Byte.Primary_Master &= ~FPrimary_Variable_Out_of_Limits;        // одновременно сбрасываются флаги Primary_Variable_Out_of_Limits для первого и второго мастера
        Device_Status_Byte.Secondary_Master &= ~FPrimary_Variable_Out_of_Limits;       
      }

   }
  }
 }
 else
   {    // страховка от рассинхронизации
     P_int_Sum_62Hz = 0; 
     ADC_Sample_Count_62Hz = 0;
    }
  }
     
  //** test

 P3OUT &= ~0x40;   
  
 //**end test
    
}
