#ifndef _rx_h_
#define _rx_h_

void rxInit(void);
int rxMicCallback(void* context, int16_t* data, int len);

#endif