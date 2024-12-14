//file Pup_Pdown_select

 /*!
 * \brief Pup_Pdown_select - выбор типа характеристики преобразования
 *
 *
 *  Stack:     не использует \par
 *  Return:    ничего не возвращает \par
 *  \param    None
 *  \return   None
 *  \sa       None
 *
 *****************************************************************************/


if  (Actual.Math_Mode & FTransfer_Lin_Drop  )  // обратная х-ка
 {
  Pup = Actual.Pmin;
  Pdown = Actual.Pmax;
 }
 else
 {                    // прямая или корнеизвлекающая х-ки
  Pup = Actual.Pmax;
  Pdown = Actual.Pmin;
 }