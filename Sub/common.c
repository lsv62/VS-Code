#include  "msp430x54xA.h"

#include  <sub_main_data.h>
#include  <TypeDefSPI.h>
#include  <math.h> 
#include  <SPI_Bus.c> 
#include  <port_defines.h> 



//      0     1        2        3          4            5           6                          
enum {Null, Cmd_Up, Cmd_ESC, Cmd_Down, Cmd_Enter, Cmd_Up_Long, Cmd_ESC_Long, 
//     7             8            9
Cmd_Down_Long, Cmd_Enter_Long, Menu_Stop};


/*! 
 * \brief ports_init - фиксация незадействованных выводов портов
 *
 *  \par  вызывается при старте контроллера 
 *  
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void ports_init(void)
{
  P1OUT |= P1_NC;
  P1DIR |= P1_NC;
  P2OUT |= P2_NC;
  P2DIR |= P2_NC;
  P3OUT |= P3_NC;
  P3DIR |= P3_NC;
  P4OUT |= P4_NC;
  P4DIR |= P4_NC;
  P5OUT |= P5_NC;
  P5DIR |= P5_NC;
  P6OUT |= P6_NC;
  P6DIR |= P6_NC;
  P7OUT |= P7_NC;
  P7DIR |= P7_NC;
  P8OUT |= P8_NC;
  P8DIR |= P8_NC; 
  
  
  //*** Vref
  P5OUT &= ~0x02;  
  P5DIR |= 0x02;
  
  //** test
  P3OUT &= ~0x40;  
  P3DIR |= 0x40; 
  
  //**end test
};



/*! 
 * \brief Calculate_Span - Расчет коэффициентов преобразования для ЦАПа в различных режимах
 *
 *  \par  вызывается при изменении параметров, влияющих на характеристику преобразования давление-код ЦАПа 
 *  
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void Calculate_Span(void)  // Расчет коэффициентов преобразования для ЦАПа
{
   Span   = (Code_Up - Code_Down) / (Pup - Pdown); // коэффициент наклона линейной характеристики
   Sqrt_K = 41.42 * Span;      // коэффициент наклона линейной части корнеизвлекающей характеристики на участке от Р1 до РС
   Sqrt_A = (Code_Up - Code_Down) / sqrt(Pup - Pdown);// коэффициент масштабирования корнеизвлекающей характеристики на участке от РС до максимума 
   Sqrt_C = 0.006 * 40.42 * (Code_Up - Code_Down); // смещение линейной части корнеизвлекающей характеристики на участке от Р1 до РС
   Pp1    = 0.006 * (Pup - Pdown) + Pdown; // точка излома линейной части корнеизвлекающей характеристики (точка Р1) 
   PpC    = 0.0080168 * (Pup - Pdown) + Pdown;  // точка стыковки линейной части корнеизвлекающей характеристики с корнеизвлекающим участком (точка РС) 
   
  // расчет токов насыщения 
   DAC_Code_Low_Limit = Actual.DAC_Code_4_mA - (int)(0.1*(Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA)/16.0);    //  3.9 ma
   DAC_Code_High_Limit = Actual.DAC_Code_20_mA + (int)(1.5*(Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA)/16.0); //  21.5ma   
//закомментировано для отладки - убрать потом
//   if (FHART_Module_Set & (DAC_Status<<8)) //подготовить константы для Burst
   {
     K_PerCent = 100/ (Actual.Pmax - Actual.Pmin);
     if (Actual.Math_Mode & FSw_5mA_Off) 
      {       // 4-20 мА 
        K_Loop_Current = 16.0;
        Base_Loop_Current = 4.0;
      }
      else
      {       // 0-5 мА
        K_Loop_Current = 5.0;
        Base_Loop_Current = 0.0;
      } 
     K_Loop_Current /=(Code_Up-Code_Down); 
   }
}
  
      

/*! 
 * \brief Calculate_S0 - Расчет констант для  теста загрубления нуля
 *
 *  \par  вызывается при изменении параметра Pmax или изменении режима загрубления нуля 
 *  
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

 void Calculate_S0(void)   //расчет для теста S0 (загрубления нуля)
{
  if  (Actual.Math_Type & 0x30)   // включено загрубление нуля ?
  {
    switch ((Actual.Math_Type & 0x30)>>4)   // выбор режима 
       { 
         case 0:           // без загрубления нуля    
              PpZ1= 0;  //точка завершения загрубления 
              PpZ2 =1;  //точка восстановления нормальной характеристики
           break;
         case 1:           // 0.15/0.15 Pmax_Lim
             PpZ1= 0.0015 * Pass.Pmax_Lim;    //точка завершения загрубления 
             PpZ2 = 2 * PpZ1;        
               
           break;
         case 2:// // 0.15/0.15 Pmax
             PpZ1= 0.0015 * Actual.Pmax;    //точка завершения загрубления 
             PpZ2 = 2 * PpZ1;              //точка восстановления нормальной характеристики
       
           break;  
         case 3:  // // 0.2/0.1 Pmax_Lim
             PpZ1= 0.002 * Pass.Pmax_Lim;    //точка завершения загрубления 
             PpZ2 = 0.003 * Pass.Pmax_Lim;   //точка восстановления нормальной характеристики
             break;  
         default:
             break; 
       }
    KP_Z = (PpZ2 - .5 * PpZ1)/(PpZ2 - PpZ1); //коэффициент наклона участка восстановления нормальной характеристики
    AP_Z = PpZ2 - KP_Z*PpZ2 ;   //смещение участка восстановления нормальной характеристики
  }             
}

void dampfer_zero(void) // обнулить все переменные демпфера
{
  
 FilterPtr   = 0;  
 FilterSum   = 0; 
 FilterCount = 0; 
}

/*! 
 * \brief zero_shift_set - коррекция нуля
 *
 *  \par  вызывается при изменении параметра Pmax или изменении режима загрубления нуля 
 *  
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

void zero_shift_set(void)
{
 float tmp_delta;
tmp_delta = Actual.P_Set_Zero_Shift-P_Out;
if (tmp_delta<0)
  tmp_delta = -tmp_delta;
if (0.02*(Pass.Pmax_Lim - Pass.Pmin_Lim)>(tmp_delta))
 {
   Actual.Zero_Shift = PpEt-Actual.P_Set_Zero_Shift;                //Да, установить ноль
   Actual_2_User();
   SPI_Command = 5; // команда - передать статус датчика  
   Error_Code &= ~Set_Zero_Error;
 }
 else
   Error_Code = Set_Zero_Error;

}


/*! 
 * \brief write_record_2_journal - Запись в журнал аварийной ситуации
 *
 *  \par  вызывается при обнаружении аварийной ситуации
 *   при вызове в структуре Jurnal_Record должны быть заполнены поля:
 *   Jurnal_Record.Parametr      значение аварийного параметра
 *   Jurnal_Record.Param_Code    код параметра
 *   Jurnal_Record.Units_Code    код единиц параметра
 *   п/п заполняет поля времени-даты и при наличии свободного места производит запись в журнал
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void write_record_2_journal(void)// Запись в журнал
{
 char i; 
 for (i=0;i<32;i++)
 if (Journal[i].Record_Status) // есть свободное место для записи?
 {
   Jurnal_Record.Record_Status=0;   // статус - записано
   Jurnal_Record.Math_Mode = Actual.Math_Mode;  // байт режима
 /*   раскомментировать, когда запустим RTC
   Jurnal_Record.rtcYEARL = RTCYEARL;      // год - младшая часть
   Jurnal_Record.rtcYEARH = RTCYEARH;      // год - старшая часть
   Jurnal_Record.rtcMON = RTCMON;          // месяц
   Jurnal_Record.rtcDAY = RTCDAY;          // день
   Jurnal_Record.rtcHOUR = RTCHOUR;        // часы
   Jurnal_Record.rtcMIN = RTCMIN;          // минуты
 */ 
   i = write_data2flash((unsigned long int)((char *) &Journal[i].Record_Status),((char *) &Jurnal_Record), 16) ;//запись
   break;
 } 
}

/*! 
 * \brief temperature - расчет температуры по коэффициентам апроксимирующего полинома
 *
 *  \par  вызывается для расчета значения температуры измерительного блока
 *   использует значение кода АЦП во float представлении  T_float.f
 *   
 *
 *  Stack:     не использует \par
 *  Return:   возвращает значение температуры в град. цельсия во float представлении \par
 *  \param    None
 *  \return   возвращает значение температуры в град. цельсия во float представлении 
 *  \sa       None
 * 
 *****************************************************************************/
/*
float temperature(void)  // расчет температуры по коэффициентам полинома 
{
  char n;
  float Tp;
  
      Tp = 0;
     for(n=0; n < Polinom.NAt[7]; n++)
        Tp = Tp * T_float.f + Polinom.A_T[n];
     return Tp;
}
*/

/*! 
 * \brief Plant_2_User - возврат к заводским настройкам
 *
 *  \par  записывает в пользовательские настройки заводские значения 
 *        стирается номер подключенного ЦАП и дается ресет,  
 *        что вызывает обновление копии даных ЦАПа
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void Plant_2_User(void)
{
Actual.P_Unit = Pass.P_Unit;  
Actual.Filter_Limit=0;
Actual.Pmin = Pass.Pmin_Lim;          // минимальное давление
Actual.Pmax = Pass.Pmax_Lim;          // максимальное давление
Actual.Zero_Shift=0;                  //пользовательская коррекция нуля
Actual.Span=1;                        //пользовательская коррекция диапазона
Actual.DAC_Code_Shift=0;                 // cмещение тока
Actual.P_Set_Zero_Shift = 0.0;
if (F0_5_Module_Set & (DAC_Status<<8))
   Actual.Math_Mode = 0;    
 else
   Actual.Math_Mode = FSw_5mA_Off;  
 /*
Actual.DAC_Code_4_mA=DAC_Copy.DAC_Code_4_mA;            // Код 4-мА
Actual.DAC_Code_20_mA=DAC_Copy.DAC_Code_20_mA;          // Код 20-мА
Actual.DAC_Code_0_mA=DAC_Copy.DAC_Code_0_mA;            // Код 0-мА
Actual.DAC_Code_5_mA=DAC_Copy.DAC_Code_5_mA;            // Код 5-мА
 */
Actual_2_User();

// уничтожение пользовательской единицы

  if ( write_data2flash( (unsigned long int)((char *) &User_P_unit), (char *) &Plant_User_P_unit, 18) )
   Error_Code = Write2Flash_Error; 
               
// здесь обнулить код ЦАПа в АЦП и дать ресет

Float_Buf_Tmp.c[0]=0; 
Float_Buf_Tmp.c[1]=0;
Float_Buf_Tmp.c[2]=0;
Float_Buf_Tmp.c[3]=0;
if ( write_data2flash( (unsigned long int)((char *) &Attached_DAC_ID[0]), (char *) &Float_Buf_Tmp.c[0], 4) )
    Error_Code = Write2Flash_Error; 
SPI_Command = 1; // команда - сброс 
}








/*! 
 * \brief Cnfg_Counter_Increment - модификация счетчика изменения конфигурации
 *
 *  \par  увеличивает значение счетчика на единицу
 *   
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void Cnfg_Counter_Increment(void)
{
 unsigned int Cnfg_Counter;
 Cnfg_Counter = HART_Cmd_0_Data[14]<<8;
 Cnfg_Counter += HART_Cmd_0_Data[15];
 Cnfg_Counter++;
 Int_Buf_Tmp.c[0] =  Cnfg_Counter>>8;  
 Int_Buf_Tmp.c[1] =  Cnfg_Counter; 
 if (write_data2flash( (unsigned long int)((char *) &HART_Cmd_0_Data[14]),((char *) &Int_Buf_Tmp.c[0]), 2))
   Error_Code = Write2Flash_Error;   
 if ((!(Flash_Device_Status_Byte.Primary_Master & FConfiguration_Changed))||(!(Flash_Device_Status_Byte.Secondary_Master & FConfiguration_Changed)))
  {
    Int_Buf_Tmp.c[0]=Flash_Device_Status_Byte.Primary_Master;
    Int_Buf_Tmp.c[1]=Flash_Device_Status_Byte.Secondary_Master;
    Int_Buf_Tmp.c[0] |= FConfiguration_Changed; 
    Int_Buf_Tmp.c[1] |= FConfiguration_Changed;     
    if (write_data2flash( (unsigned long int)((char *) &Flash_Device_Status_Byte.Primary_Master),((char *) &Int_Buf_Tmp.c[0]), 2))
      Error_Code = Write2Flash_Error; 
  }  
}


/*! 
 * \brief Actual_2_User - сохранение текущего значения
 *
 *  \par  взаписывает в пользовательские настройки заводские значения 
 *   
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void Actual_2_User(void)
{
 if (write_data2flash( (unsigned long int)((char *) &User),((char *) &Actual), 34))
   Error_Code = Write2Flash_Error; 
 Cnfg_Counter_Increment(); 
}



/*! 
 * \brief Set_Pup_Pdown - установка максимального и минимального давления 
 *
 *  \par  устанавливает максимальное и минимальное давление и выбирает 
 *   соответствующий режим выходного преоборазователя
 *   
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void Set_Pup_Pdown(void)
{

 if (Pdown < Pup)  // заданные пределы по давлению соответствуют прямой харктеристике?
  {                      // да,
    Actual.Pmax      =  Pup;
    Actual.Pmin      =  Pdown;
    Actual.Math_Mode &= ~FTransfer_Lin_Drop; // установить режим прямой х-ки..
   }
  else
    {
      Actual.Pmax      =  Pdown;
      Actual.Pmin      =  Pup;
      Actual.Math_Mode |= FTransfer_Lin_Drop;   //  установить режим обратной х-ки
     }  
 }



/*! 
 * \brief Start_CRC - запуск системы контроля CRC
 *  
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void Start_CRC (void)  //запуск системы контроля CRC
{
   CRC_test_Record_pointer=1;
   CRC_test_Start.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[0];
   CRC_test_Start.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[1];
   CRC_test_Start.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Start_Adress[2];
   CRC_test_pointer=CRC_test_Start.dw;                       // подготовка к работе 
   CRC_test_Finish.c[0]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[0];
   CRC_test_Finish.c[1]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[1];
   CRC_test_Finish.c[2]=Flash_CRC_Record[CRC_test_Record_pointer].Finish_Adress[2];
   CRCINIRES = 0xffff;                           // инициализация аппаратуры CRC
   CRCDIRB_L = __data20_read_char (CRC_test_pointer);  // подсчет CRC
}




/*! 
 * \brief supervisor - модуль фонового исполнения
 *
 *  \par  контролирует наличие требований фоновой обработки 
 *   информации и запускает соответствующие модули 
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/
void supervisor(void)
{
  //убрать после отладки
// extern unsigned long int  WDTdelayStart;
 // unsigned long int il, WDTdelay;
  // :)
 unsigned char i,j, k,  count;
 #pragma pack(1)
 char* ptr;
 char* ptrMessage;
 const float* ptrTemperature_Polinom;
 
  Start_CRC (); //запуск системы контроля CRC

 while(1)
 {
  if(flagHTechR & RHTpReady)  // есть флаг новой технологич. команды?
   {
    #include "HT_command.c"
   }
   else
    
    if (Bus_Control_Flags & New_Buf_In) // есть принятые по SPI данные?
     {
       #include "bus_in_service.c"     
       Bus_Control_Flags &= ~New_Buf_In; 
     }
//    else    
    
   if ((!(Buf_Out.buf[1]))&&(!(Bus_Control_Flags & FHART_CMD_Active)))///&& ((SPI_Command)||(Device_Status_Reg & FNew_Temperature)))    // буфер выхода свободен?
    {
     if ((!(SPI_Command))&&(Device_Status_Reg & FNew_Temperature))
      {
        SPI_Command = 8;
        Device_Status_Reg &= ~FNew_Temperature;
      }
       #include "bus_out_service.c"
    }

 // if ((Mode_Setting_Reg<<8) & FLogon_Enable)
  //  #include "journal.c"  // работа с журналом
 /*   */  
     
   if (Device_Status_Reg & FTemperature_Calculate)
     {
       #include "tempraturaBURST.c" 
     }
    else
      if ((Mode_Setting_Reg<<8) & FCRC_Test_Enable )  // нужна CRC?
        {
          #include "crc_test.c"
        }
  
 // for (il=0;il< WDTdelay;il++);

  // WDTCTL = WDT_ARST_1000;    // WatchDog reset -  Watchdog mode ->  WDT is clocked by ACLK (assumed 32KHz), 1000ms interval
  
 }
}


