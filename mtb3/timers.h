
void T1_Init(unsigned int match_value);
void T2_Init(unsigned int match_value);
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void);
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);
