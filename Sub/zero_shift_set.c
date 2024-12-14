
void zero_shift_set(void)
{
 float tmp_delta;
tmp_delta = Actual.P_Set_Zero_Shift-P_Out
if (tmp_delta<0)
  tmp_delta = -tmp_delta;
if (0.02*(Pass.Pmax_Lim - Pass.Pmin_Lim))<(tmp_delta))
  Actual.Zero_Shift = PpEt-Actual.P_Set_Zero_Shift;                //Да, установить ноль
 else
   Error_Code = Set_Zero_Error;
Actual_2_User();
}
