//***************************************************************************
// "slz.h"
// Header file containing the prototype for the SLZ decompression functions
// You should include this header when using these functions
//***************************************************************************

#ifndef SLZ_H
#define SLZ_H

// Function prototypes
void decompress_slz(unsigned char *, const unsigned char *);
void decompress_slz24(unsigned char *, const unsigned char *);

#endif
