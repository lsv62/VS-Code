
// file pTcompensation.c

 /*! 
 * \brief pTcompensation - выбор режима термокоррекции давления 
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

switch (Actual.Math_Type & 0x03)   // выбор режима - пока по заводским уставкам
       { 
         case 0:           // без термокоррекции             
                PpT = P_float.f /* * K_P_normalize*/;     
           break;
         case 1:           // polinom..
             if (Pass.Span1) // если есть данные для этого способа термокоррекции
               {
                 #include "polinom.c" 
                 PpT=Pp;           
               }
           break;
         case 2:// cubic_spl
             if (Pass.Span2)  // если есть данные для этого способа термокоррекции
               {
                 PpT = cubic_spl ();   
               }
           break;  
         case 3:  // zavod ?
             if (Pass.Span3)   // если есть данные для этого способа термокоррекции
               {
                 //  #include "zavod.c"    
                  PpT = P_float.f;
                }
             break;  
         default:
             break; 
           }
