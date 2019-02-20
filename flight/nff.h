#ifndef __NFF_H__
#define __NFF_H__
#include "cl.h"

#define NFF_MAXBUFSIZE (200)
#define NFF_NUMDATAFIELDS (21)
#define NFF_MAXFIELDSIZE (20)
#define NFF_CHECKPOSITION (3)
#define ERROR (-1)

long nff_getData(DATA* d);
//long parse_nff_data(char* buffer);

#endif
