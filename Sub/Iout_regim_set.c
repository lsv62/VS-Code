//file Iout_regim_set

 /*! 
 * \brief Iout_regim_set - выбор режима выходного тока 
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 * 
 *****************************************************************************/

if (Actual.Math_Mode & FSw_5mA_Off) 
 {       // 4-20 мА 
   Code_Up = Actual.DAC_Code_20_mA;
   Code_Down = Actual.DAC_Code_4_mA;
 }
 else
  {       // 0-5 мА
   Code_Up = Actual.DAC_Code_5_mA;
   Code_Down = Actual.DAC_Code_0_mA;
  }