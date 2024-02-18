#ifndef LIMITS_H
#define LIMITS_H

#define PATH_MAX 200

#define CHAR_BIT 	(8) 	                // Defines the number of bits in a byte.
#define SCHAR_MIN 	(-128) 	                // Defines the minimum value for a signed char.
#define SCHAR_MAX 	(127)	                // Defines the maximum value for a signed char.
#define UCHAR_MAX 	(255) 	                // Defines the maximum value for an unsigned char.

#define CHAR_MIN 	(-128) 	                // Defines the minimum value for type char and its value will be equal to SCHAR_MIN if char represents negative values, 
                                            // otherwise zero.

#define CHAR_MAX 	(127) 	                // Defines the value for type char and its value will be equal to SCHAR_MAX if char represents negative values, 
                                            //  otherwise UCHAR_MAX. 

#define MB_LEN_MAX 	(16) 	                // Defines the maximum number of bytes in a multi-byte character.
#define SHRT_MIN 	(-32768) 	            // Defines the minimum value for a short int.
#define SHRT_MAX 	(32767) 	            // Defines the maximum value for a short int.
#define USHRT_MAX 	(65535) 	            // Defines the maximum value for an unsigned short int.
#define INT_MIN 	(-2147483648)	        // Defines the minimum value for an int.
#define INT_MAX 	(2147483647)	        // Defines the maximum value for an int.
#define UINT_MAX 	(4294967295)	        // Defines the maximum value for an unsigned int.
#define LONG_MIN 	(-9223372036854775808) 	// Defines the minimum value for a long int.
#define LONG_MAX 	(9223372036854775807) 	// Defines the maximum value for a long int.
#define ULONG_MAX 	(18446744073709551615) 	// Defines the maximum value for an unsigned long int.

#endif