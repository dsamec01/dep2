//prototypy funkci k RTM_monitor.c
void initPrepinacRTM(bool *Ptr_prepinac);
void runKomunikaceRTM(ZATEZOVATEL *Ptr_zat, int zatezovatel, bool *Ptr_prepinac, CAPTURE_RTM *Ptr_CaptureRTM, PRECH_CHAR *Ptr_PrechCharData, REGULATOR *Ptr_reg, bool MUX, bool tlacitkoS5MUX);