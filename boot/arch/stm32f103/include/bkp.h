#ifndef	__BKP_H
#define __BKP_H

#define BKP_DR(x)	(0x00 + (U(x) << 2) + (U(x)>10?0x14:0x00))  // x = 1 .. 42

#endif
