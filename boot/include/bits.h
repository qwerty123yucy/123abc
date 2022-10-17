#ifndef __BITS_H
#define __BITS_H

#ifdef __ASSEMBLY__
#define U(x)	x
#else
#define U(x)	x##U
#endif


#define BITS(msb, lsb)	\
	((~U(0) >> (31 - msb)) & (~U(0) << lsb))

#define BIT(b) BITS(b, b)

#endif
