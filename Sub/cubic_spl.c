
/*!
 * \brief cubic_spl - расчет термокомпенсации по сплайну
 *
 *  \par  расчет давления по кодам АЦП и калибровочным таблицам давления и температуры
 *
 *
 *  Stack:     не использует \par
 *  Return:    возвращает значение скомпенсированного давления \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/

#include  <sub_main_data.h>


#define ncols  Pass.T_Nmax
#define nrows  Pass.P_Nmax
#define T_code Axial.T_N
#define P_kPa Axial.P_N
#define T_code_calc T_float.f
#define Pv_code_calc P_float.f


unsigned char top_reg_eff , top_reg_eff2 ;
signed char bot_reg_eff2 , bot_reg_eff ;
unsigned char count , int_T , int_Ti , int_Pv , int_Pvi ;
float P_kPa_calc, P_kPa_calc1 , P_kPa_calc2 , hi , ts , hs , Ai , Bi ;
float dP_kPa1, dP_kPa2;
//---------------------------- Cubic_spl subroutine ------------------------------------//
float cubic_spl ()
{
unsigned char i;

top_reg_eff = 0;
top_reg_eff2 = 0;
bot_reg_eff2 = 0;
bot_reg_eff = 0;
count = 1;
i = 0;
int_T = 0;
int_Ti = 0;
int_Pv = 0;
int_Pvi = 0;


// == 1. Вычисление текущего интервала тарировочной таблицы и точки интерполяции == //

    for (int_T = 0; int_T < (ncols - 2); int_T++) // Нахождения интервала int_Ti попадания значения T_code_calc - кода АЦП по температуре в тарировочной таблице ИТ – T_code
	{
            if ( (T_code[int_T] <= T_code_calc)&&(T_code[int_T + 1] > T_code_calc) )
                   int_Ti = int_T;
	    	else if (T_code[0] > T_code_calc)
                   int_Ti = 0;			 // Учет 1-ого краевого эффекта по температуре
        	else if (T_code[int_T + 1] <= T_code_calc)
                   int_Ti = ncols - 2;		 // Учет 2-ого краевого эффекта по температуре



	}

    for (int_Pv = 0; int_Pv < (nrows - 2); int_Pv++)  // Нахождения интервала int_Pv попадания значения Pv_code_calc кода АЦП по давлению в тарировочной таблице ИД – Pv _code
        {
             if ( (Point[int_Pv*ncols + int_Ti].P_DAC >= Pv_code_calc)&&(Point[int_Pv*ncols + int_Ti + ncols].P_DAC < Pv_code_calc) )
                   int_Pvi = int_Pv;
                else if (Point[int_Ti].P_DAC < Pv_code_calc) // Учет 1-ого краевого эффекта по давлению 1-й этап
                   int_Pvi = 0;
                else if (Point[int_Pv*ncols + int_Ti + ncols].P_DAC >= Pv_code_calc)  // Учет 2-ого краевого эффекта по давлению 1-й этап
                   int_Pvi = nrows - 2;

		}

	      if (int_Pvi == 0)	  // Учет 1-ого краевого эффекта по давлению 2-й этап
		{
                  top_reg_eff = ncols;
		  top_reg_eff2 = 1;
		}
              if (int_Pvi >= nrows - 2)   // Учет 2-ого  краевого эффекта по давлению 2-й этап
		{
                  bot_reg_eff = - ncols;
	          bot_reg_eff2 = - 1;
		}

 i = (int_Pvi)*ncols + int_Ti;  // Вычисление номера ближайшего меньшего элемента в матрице ИД - Pv_code

// == 2. Вычисление значения ИД – P (кПа) методом  билинейной эрмитовой спайн-интерполяции ==//

	   while (count <= 2)
	   {

			  if (count == 2)
			  {
            	            i = i + 1;
			  }


                dP_kPa1 = ( P_kPa[int_Pvi + top_reg_eff2] - P_kPa[int_Pvi + top_reg_eff2 - 1])/(Point[i + top_reg_eff].P_DAC - Point[i + top_reg_eff - ncols].P_DAC ); 				// Вычисление производной dP_kPa1 в начале интервала int_Pv
        	dP_kPa2 = ( P_kPa[int_Pvi + bot_reg_eff2 + 2] - P_kPa[int_Pvi + bot_reg_eff2 + 1])/(Point[i + bot_reg_eff + 2*ncols].P_DAC - Point[i + bot_reg_eff + 1*ncols].P_DAC );   // Вычисление производной dP_kPa2 в конце интервала int_Pv


                                 hi = Point[i + ncols].P_DAC - Point[i].P_DAC; 								// Вычисление шага hi по коду АЦП для давления
				 ts = (Pv_code_calc - Point[i].P_DAC)/hi;								// а также промежуточных переменных ts, hs
				 hs = (P_kPa[int_Pvi + 1 /*+ bot_reg_eff2*/] - P_kPa[int_Pvi /*+ bot_reg_eff2*/])/hi;




					Ai = - 2 * hs + (dP_kPa1 + dP_kPa2);	// Вычисление коэффициентов кубического сплайна Ai и Bi
					Bi = - Ai + hs - dP_kPa1;


                        	  P_kPa_calc2 = P_kPa[int_Pvi] + (Pv_code_calc - Point[i].P_DAC)*(dP_kPa1 + ts*(Bi + ts*Ai)); // Вычисление значения давления P_kPa_calc1 и P_kPa_calc2 для двух кубических сплайнов по двум соседним столбцам матрицы ИД - Pv_code


			  if (count == 1)
			  {
                            P_kPa_calc1 = P_kPa_calc2;
			  }
			count++;
	    }

P_kPa_calc = ( ((P_kPa_calc2 - P_kPa_calc1)/(T_code[int_Ti + 1] - T_code[int_Ti]))*(T_code_calc - T_code[int_Ti]) ) + P_kPa_calc1; // Вычисление интерполяционного значения давления P_kPa_calc (кПа) по двум значениям P_kPa_calc1 и P_kPa_calc2

return P_kPa_calc;
}
