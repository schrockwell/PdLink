#ifndef _tx_h_
#define _tx_h_

#include "pd_api.h"

int txInit(void);
int txStart(void);
int txEnd(void);
int txCallback(void* context, int16_t* left, int16_t* right, int len);
int txEnqueue(char byte);

#endif