



//файл  dacTcomp.c

 /*! 
 * \brief dacTcomp - термокомпенсация ЦАПа  
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

/* 

if ((DAC_Polinom.NAt[0]) || (DAC_Polinom.NAt[1]) || (DAC_Polinom.NAt[2]) || (DAC_Polinom.NAt[3])) // если хотя бы 1 из описателей <> 0, считаемм коррекцию.
        {*/
DAC_Temperature_Float = (float)DAC_Temperature.w;
     /*   */  
if (Actual.Math_Mode & FSw_5mA_Off)
 {                       // режим 4..20 ма
   if  (Flash_DAC_Tcomp_Reg & FDAC_Tcomp_4_20_Enable)
    {
      DAC_Code_Tmp=0;
      for (i=0;i<16;)
       { 
          
         c_DAC_Tcomp = DAC_T_4_20[i++];
         for (j=0;j<3;j++)
          {
            c_DAC_Tcomp = c_DAC_Tcomp * DAC_Temperature_Float + DAC_T_4_20[i++]; 
          }
         DAC_Code_Tmp = DAC_Code_Tmp * DAC_Code_float + c_DAC_Tcomp ;      
       }

/*
                     i=0; DAC_Code_Tmp=0; j=0;   // Обнуление переменных
                       do  
                        { 
                          if (i)
                            DAC_Code_Tmp *= DAC_Code_float;
                          a_p=0;
                             lim=i+4-1;   //  извлечь порядок очередного полинома температуры
                          if  (DAC_Polinom.NAt[j])
                          {
                           do                       // расчет очередного коэффициента полинома давления "a_p"
                             {
                              a_p+=DAC_Polinom.A_T_4_20[i];
                              a_p*= T_float.f;
                              i++;
                             }
                            while (i<lim);
        
                            a_p+=DAC_Polinom.A_T_4_20[i];  // расчет давления с использованием вычисленных коэффициентов
                           }
                          else
                            i+=3;
                           DAC_Code_Tmp += a_p;
                          i++;
                          j++;
                        }  
                       while (j<4) ;     
                     }
                   }*/
    }
   else
     DAC_Code_Tmp = DAC_Code_float;
 }
else
  {                      // режим 0..5 ма
   if  (Flash_DAC_Tcomp_Reg & FDAC_Tcomp_0_5_Enable)
    {      
      DAC_Code_Tmp=0;
      for (i=0;i<16;)
       {   
         c_DAC_Tcomp = DAC_T_0_5[i++];
         for (j=0;j<3;j++)
          {
            c_DAC_Tcomp = c_DAC_Tcomp * DAC_Temperature_Float + DAC_T_0_5[i++]; 
          }
         DAC_Code_Tmp = DAC_Code_Tmp * DAC_Code_float + c_DAC_Tcomp ;      
       }               
         
 /*        
                       i=0; DAC_Code_Tmp=0; j=0;   // Обнуление переменных
                       do  
                        { 
                          if (i)
                            DAC_Code_Tmp *= DAC_Code_float;
                          a_p=0;
                             lim=i+4-1;   //  извлечь порядок очередного полинома температуры
                          if  (DAC_Polinom.NAt[j])
                          {
                           do                       // расчет очередного коэффициента полинома давления "a_p"
                             {
                              a_p+=DAC_Polinom.A_T_0_5[i];
                              a_p*= T_float.f;
                              i++;
                             }
                            while (i<lim);
        
                            a_p+=DAC_Polinom.A_T_0_5[i];  // расчет давления с использованием вычисленных коэффициентов
                           }
                          else
                            i+=3;
                           DAC_Code_Tmp += a_p;
                          i++;
                          j++;
                        }  
                       while (j<4) ; 
                   }*/
       //    DAC_Code.w = (int)(DAC_Code_Tmp+.5) + Actual.DAC_Code_Shift;
     }
    else     
     DAC_Code_Tmp = DAC_Code_float;
  }

