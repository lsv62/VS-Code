/*!
 * \brief StartDataInit - инициализация рабочих переменных датчика
 *
 *  \par  вызывается при старте программного обеспечения
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/

#include  "msp430x54xA.h"
//#include  <types_define.h>
#include  <sub_main_data.h>
void StartDataInit(void){
 //  Strob_Not_Active = 15;

Time_Stamp_MAx_Limit = (unsigned long) 24*3600*32000;
 T_int.l = ADC_Flash.T_Code_Default;  // Значение кода АЦП температуры для старта
 P_int.l = ADC_Flash.P_Code_Default;  // Значение кода АЦП давления для старта
  Master_Control_Flags=0;
  Bus_Control_Flags = 0;

 Number_T_Points = ADC_Flash.Number_T_Points + 2; // кол-во измерений + 2 на калибровку
 Number_P_Points = ADC_Flash.Number_P_Points + 2; // кол-во измерений + 2 на калибровку

 ADC_T_Count = Number_T_Points-1;    // запуск автомата АЦП
 ADC_P_Count = 0;                    // запуск автомата АЦП

 ADC_Func_Ptr = 0;         // запуск автомата команд АЦП
 ADC_HT_Mode_Count = 0;
 if (Reg_ADC[0][3] == F62)
  {
   P_int_Sum_62Hz = 2*ADC_Flash.P_Code_Default;
   ADC_Sample_Count_62Hz = 2;
  }
 else
  {
   P_int_Sum_62Hz = 0;
   ADC_Sample_Count_62Hz = 0;
  }
  AddrHTp = ADC_Flash.AddrHTp ;
 /*
 Key_Z_Status = Get_Key_Z;
 if(Key_Z_Status)
  AddrHTp = ADC_Flash.AddrHTp ;
 else
  AddrHTp = 3;
 */
Actual.P_Unit = User.P_Unit;             // единица измерения давления
Actual.Math_Mode = User.Math_Mode;       // режим математики по умолчанию
Actual.Math_Type = Pass.Math_Type;       // тип математики по умолчанию
Actual.Pmin=User.Pmin;                   // минимальное давление
Actual.Pmax= User.Pmax;                 // проба случая если ноль05052018

/*if (User.Pmax)
  Actual.Pmax= User.Pmax;                // максимальное давление
 else
   Actual.Pmax= .001; //пробы вариантов и ошибок при разрежении 05052018 */


//Actual.Pmax= User.Pmax;                 // проба случая если ноль05052018
/*if (User.Pmax)
  Actual.Pmax= User.Pmax;                // максимальное давление
 else
   Actual.Pmax=0.000001; //пробы вариантов и ошибок при разрежении 05052018  */
   //Actual.Pmax=40;
// коды заменить на User
Actual.DAC_Code_4_mA=User.DAC_Code_4_mA;           // Код 4-мА
Actual.DAC_Code_20_mA=User.DAC_Code_20_mA;          // Код 20-мА
Actual.DAC_Code_0_mA=User.DAC_Code_0_mA;           // Код 0-мА
Actual.DAC_Code_5_mA=User.DAC_Code_5_mA;          // Код 5-мА
//***************************

Actual.Zero_Shift0=Pass.Zero_Shift0;                // cмещение нуля 0 режима
//Actual.Span0=1 ; //Pass.Span0;                   // коэффициент усиления
Actual.Span0=Pass.Span0;                   // коэффициент усиления

Actual.Zero_Shift1=Pass.Zero_Shift1;                // cмещение нуля 1 режима
//Actual.Span1=1 ; //Pass.Span1;                   // коэффициент усиления
Actual.Span1=Pass.Span1;                   // коэффициент усиления

Actual.Zero_Shift2=Pass.Zero_Shift2;                // cмещение нуля 2 режима
//Actual.Span2=1 ; //Pass.Span2;                   // коэффициент усиления
Actual.Span2=Pass.Span2;                   // коэффициент усиления

Actual.Zero_Shift3=Pass.Zero_Shift3;                // cмещение нуля 3 режима
//Actual.Span3=1 ; //Pass.Span3;                   // коэффициент усиления
Actual.Span3=Pass.Span3;                   // коэффициент усиления

Actual.Zero_Shift= User.Zero_Shift; //пользовательская коррекция нуля
Actual.Span= User.Span;       //пользовательская коррекция диапазона


Actual.DAC_Code_Shift = User.DAC_Code_Shift;   //сдвиг тока
Actual.P_Set_Zero_Shift= User.P_Set_Zero_Shift;  // давление нуля

Actual.Filter_Limit= User.Filter_Limit;
#include "Pup_Pdown_select.c"
/*
if (Actual.Math_Mode & FTransfer_Lin_Drop)
 {       // обратная х-ка
   Pup = Actual.Pmin;
   Pdown = Actual.Pmax;
 }
 else
 {       // прямая или корнеизвлекающая характеристика
   Pup = Actual.Pmax;
   Pdown = Actual.Pmin;
 }*/

if (Actual.Math_Mode & FSw_5mA_Off)
 {       // 4-20 мА
   Pre_Buf.DAC = ADC_Flash.DAC_Code_Default_4_20;   // записать значение кода ЦАПа для старта 4-20 в SPI буфер
   Code_Up = Actual.DAC_Code_20_mA;
   Code_Down = Actual.DAC_Code_4_mA;
 }
 else
 {       // 0-5 мА
   Pre_Buf.DAC = ADC_Flash. DAC_Code_Default_0_5;   // записать значение кода ЦАПа для старта 0-5 в SPI буфер
   Code_Up = Actual.DAC_Code_5_mA;
   Code_Down = Actual.DAC_Code_0_mA;
 }



Pre_Buf.P = ADC_Flash.P_Default;            //записать значения Р в SPI буфер

Calculate_Span();             //рассчитать константы преобразования для ЦАПа
Calculate_S0() ;      //рассчитать константы преобразования S0

//K_P_normalize = 0.0000789;
//SPI_error_count=0.001;


//расчет параметров для журнала
Delta_P_Lim_Journal = .05 * (Pass.Pmax_Lim-Pass.Pmin_Lim);
Pmax_Lim_Journal = Pass.Pmax_Lim + Delta_P_Lim_Journal;
Pmin_Lim_Journal = Pass.Pmin_Lim - Delta_P_Lim_Journal;

Delta_T_Lim_Journal = .05 * (Axial.T_N[Pass.T_Nmax]-Axial.T_N[0]);
Tmax_Lim_Journal = Axial.T_N[Pass.T_Nmax] + Delta_T_Lim_Journal;
Tmin_Lim_Journal = Axial.T_N[0] - Delta_T_Lim_Journal;

   DAC_Code_Low_Limit = Actual.DAC_Code_4_mA - (int)(0.1*(Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA)/16.0);    //  3.9 ma
   DAC_Code_High_Limit = Actual.DAC_Code_20_mA + (int)(1.5*(Actual.DAC_Code_20_mA - Actual.DAC_Code_4_mA)/16.0); //  21.5ma

//DAC_Code_Alarm=0x76;

Kout_P=1;

Filter_Limit_Max=600;

FilterPtr=0;
FilterCount=0;

Device_Status_Byte.Primary_Master |= FCold_Start;               // одновременно устанавливается флаги Cold Start для первого и второго мастера
Device_Status_Byte.Secondary_Master |= FCold_Start;

HT_Debugiing_Column_Row=255;

Device_Status_Reg |= FBurst_Data_Copy_Active;

}