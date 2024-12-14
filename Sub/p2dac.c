
#include <math.h> 
//файл p2dac.c

 /*! 
 * \brief p2dac - преобразование давления в код ЦАПа
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

       if (Actual.Math_Mode & FTransfer_SQRT)             // тип выходной х-ки
         {                                             // корнеизвлекающая х-ка
           if (PpEE < Pp1)
             {
                DAC_Code_float =Span * (PpEE-Pdown) + Code_Down;
              }
              else
                {
                  if (PpEE > PpC)
                   {
                     DAC_Code_float = Sqrt_A * sqrt(PpEE-Pdown) + Code_Down;
                    }
                    else
                     {
                       DAC_Code_float =Sqrt_K*(PpEE-Pdown)- Sqrt_C  + Code_Down;
                      }
                 }
         }
         else
         {                                                     // линейная х-ка
                      DAC_Code_float = (PpEE-Pdown)* Span + Code_Down;  
         }


   