/*******************************************************************************************************************//**
 * @file    utilities.c
 * @author  G.Zini
 * @version 1.0
 * @date    2018 July, 24
 * @brief   General purpose utility functions
 **********************************************************************************************************************/

// Includes ------------------------------------------------------------------------------------------------------------
#include "utilities.h"
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// Private macro -------------------------------------------------------------------------------------------------------

// Test and convertion of an ASCII char to an hex nibble
#define _tohex(ch) (((uint8_t)((ch)-='0') <= 9u) \
                || (((uint8_t)((ch)-=('A'-'0'-10u)) >= 10u) && (ch <= 15u)) \
                || (((uint8_t)((ch)-=('a'-'A'))     >= 10u) && (ch <= 15u)))

// Test and convertion of an ASCII char to a decimal nibble
#define _todec(ch) ((uint8_t)(ch -= '0') < 10u)

// Truncate and round local functions
#define trunc(x)    ((double)(long)(x))
#define round(x)    (trunc((double)(x)+0.5))


// Private variables ---------------------------------------------------------------------------------------------------


// Exported functions --------------------------------------------------------------------------------------------------

/*******************************************************************************************************************//**
 * @brief   Convertion of an unsigned integer to an ASCII string representing the decimal value of the integer
 * @param   szBuf   Pointer to the destination memory buffer. The buffer must have a minimum size of 11 bytes. At the
 *                  end of the function, the buffer[10] location will contain the string terminator ('\0'). The
 *                  buffer[9] will contain the units, the buffer[8] the tens digit and so on up to the most significant
 *                  digit of the number. The function zero-fills the buffer at the left of the most significant digit,
 *                  but returns a pointer to the first non-zero digit, if present, or to the units.
 *          uNbr    Number to be converted (0..4294967296)
 * @retval  char *  Pointer to the most significant non-zero digit of the ASCII string or to the units.
 */
char *ultoa(char *szBuf, uint32_t uNbr)
{
    if (szBuf)
    {
        int i ;
        for (i = 0 ; i < 10 ; i++) *(szBuf++) = '0' ;
        *szBuf = '\0' ;
        do
        {
            *(--szBuf) = (char)(uNbr % 10) + '0' ;
            uNbr /= 10 ;
        } while (uNbr) ;
    }
    return szBuf ;
}


/*******************************************************************************************************************//**
 * @brief   Convertion of an unsigned long integer to an ASCII string representing the decimal value of the integer
 * @param   szBuf   Pointer to the destination memory buffer. The buffer must have a minimum size of 21 bytes. At the
 *                  end of the function, the buffer[20] location will contain the string terminator ('\0'). The
 *                  buffer[19] will contain the units, the buffer[18] the tens digit and so on up to the most
 *                  significant digit. The function zero-fills the buffer at the left of the most significant digit,
 *                  but returns a pointer to the first non-zero digit, if present, or to the units.
 *          uNbr    Number to be converted (0..18446744073709551615)
 * @retval  char *  Pointer to the most significant non-zero digit of the ASCII string or to the units.
 */
char *ulltoa(char *szBuf, uint64_t uNbr)
{
    if (szBuf)
    {
        unsigned i ;
        for (i = 0 ; i < 20 ; i++) *(szBuf++) = '0' ;
        *szBuf = '\0' ;
        do
        {
            *(--szBuf) = (char)(uNbr % 10LL) + '0' ;
            uNbr /= 10LL ;
        } while (uNbr) ;
    }
    return szBuf ;
}


/*******************************************************************************************************************//**
 * @brief   Convertion of an unsigned integer to an ASCII string representing the hexadecimal value of the integer
 * @param   szBuf   Pointer to the destination memory buffer. The buffer must have a minimum size of 9 bytes. At the
 *                  end of the function, the buffer[8] location will contain the string terminator ('\0'). The
 *                  buffer[7] will contain the least significant hex digit, the buffer[6] the next digit and so on up to
 *                  the most significant hex digit of the number. The function zero-fills the buffer at the left of the
 *                  most significant digit, but returns a pointer to the first non-zero digit, if present, or to the
 *                  least significant digit.
 *          uNbr    Number to be converted (0..0xFFFFFFFF)
 * @retval  char *  Pointer to the most significant non-zero hex digit of the ASCII string or to the least significant
 *                  digit
 */
char *ultox(char *szBuf, uint32_t uNbr)
{
    static const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'} ;
    if (szBuf)
    {
        int i ;
        for (i = 0 ; i < 8 ; i++) *(szBuf++) = '0' ;
        *szBuf = '\0' ;
        do
        {
            *(--szBuf) = hex[uNbr & 0x0F] ;
            uNbr >>= 4 ;
        } while (uNbr) ;
    }
    return szBuf ;
}


/*******************************************************************************************************************//**
 * @brief   Convertion of a signed integer to an ASCII string representing the decimal value of the integer
 * @param   szBuf   Pointer to the destination memory buffer. The buffer must have a minimum size of 12 bytes. At the
 *                  end of the function, the szBuf[11] location will contain the string terminator ('\0'). The
 *                  szBuf[10] will contain the units, the szBuf[8] the tens digit and so on up to the most significant
 *                  digit of the number in szBuf[1]. The sign character is in szBuf[0]. The function zero-fills the
 *                  buffer at the left of the most significant digit, but returns a pointer to the first non-zero digit,
 *                  if present, or to the units.
 *          iNbr    Number to be converted (-2147483648..2147483647)
 * @retval  char *  Pointer to the most significant non-zero digit of the ASCII string or to the units.
 */
char *sltoa(char *szBuf, int32_t iNbr)
{
    if (szBuf)
    {
        // Aggiusta il segno
        if (iNbr >= 0)
        {
            *(szBuf++) = '+' ;
        }
        else
        {
            *(szBuf++) = '-' ;
            iNbr = -iNbr ;
        }
        // Converte e restituisce il numero
        return ultoa(szBuf, (uint32_t)iNbr) ;
    }
    return szBuf ;
}


/*******************************************************************************************************************//**
 * @brief   Convertion of a signed long integer to an ASCII string representing the decimal value of the integer
 * @param   szBuf   Pointer to the destination memory buffer. The buffer must have a minimum size of 21 bytes. After the
 *                  termination of the function, the buffer[20] location will contain the string terminator ('\0'). The
 *                  buffer[19] will contain the units, the buffer[18] the tens digit and so on up to the most
 *                  significant digit in szBuf[1]. The sign character is in szBuf[0]. The function zero-fills the
 *                  buffer at the left of the most significant digit, but returns a pointer to the first non-zero digit,
 *                  if present, or to the units.
 *          uNbr    Number to be converted (-9223372036854775808..9223372036854775807)
 * @retval  char *  Pointer to the most significant non-zero digit of the ASCII string or to the units.
 */
char *slltoa(char *szBuf, int64_t iNbr)
{
    char *str ;
    if (NULL != (str = szBuf))
    {
        // Aggiusta il segno
        if (iNbr >= 0LL)
        {
            // Converte
            str = ulltoa(szBuf, (uint64_t)iNbr) ;
            szBuf[0] = '+' ;
        }
        else
        {
            str = ulltoa(szBuf, (uint64_t)(-iNbr)) ;
            szBuf[0] = '-' ;
        }
    }
    return str ;
}

/*******************************************************************************************************************//**
 * @brief Calculates the square root of a 32 bit unsigned integer. The result is such that the following holds:
 *          Radicand = Root^2 + Remainder
 * @param Rad   Radicand value. Any uint32_t value is valid
 * @param pRem  Pointer to the uint32_t variable that will store the Remainder at the end of the function. If the
 *              pointer value is NULL, the remainder is discarded.
 * @return      The Root value
 */
uint16_t SqrtU32(uint32_t Rad, uint32_t *pRem)
{
    unsigned i ;
    uint32_t Rem, Tmp ;
    uint16_t Roo ;
    for (i = 0, Rem = 0, Roo = 0 ; i < 16 ; i++)
    {
        Rem = (Rem << 2) | (Rad >> 30) ;
        Rad <<= 2 ;
        Tmp = (Roo << 2) | 0x01 ;
        Roo <<= 1 ;
        if (Rem >= Tmp) { Rem -= Tmp ; Roo |= 1 ; }
    }
    if (pRem) *pRem = Rem ;
    return Roo ;
}


/*******************************************************************************************************************//**
 * @brief  Floating point division and remainder
 * @param  *fQuot  Pointer to the floating pointer dividend. At the end the functions stores the quotient of the
 *                 division
 * @param  fDiv    Divisor
 * @retval         Remainder of the division. The sign is equal to the sign of the quotient
 */
double frem( double *fQuot, double fDiv )
{
    double fTmp ;
    fTmp = *fQuot ;
    return fTmp - (fDiv * (*fQuot = trunc(fTmp / fDiv))) ;
}


/******************************************************************************************************************//**
 * @brief  Convert a floating point value to an ASCII string
 * @param  szBuf    Pointer to a buffer where the converted string will be placed. The size of the buffer must be (at
 *                  least) 35 bytes. At the end of the function szBuf[0] will contain the sign, szBuf[1] will contain
 *                  the most significand digit of the mantissa, and so on.
 * @param  fNum     Floating point variable to be converted
 * @param  iNFrac   Number of fractional digits to print. The value 0 forces the print of the decimal point with no
 *                  fractional digits. The value -1 suppresses the print of the decimal point.
 * @retval char *   Pointer to the converted string. The first character is the sign ('+' or '-')
 */
char *ftoa(char *szBuf, double fNum, int iNFrac)
{
    static const double _10[] =
    {1e0,1e1,1e2,1e3,1e4,1e5,1e6,1e7,1e8,1e9,1e10,1e11,1e12,1e13,1e14,1e15,1e16 } ;

    // Checks the argument
    if (szBuf != NULL)
    {
        char cSign ;
        // Go to the end of the string
        szBuf = &(szBuf[35]) ;
        *(--szBuf) = '\0' ;
        // Check the sign of the argument
        if (fNum < 0.0)
        {
            cSign = '-' ;
            fNum = -fNum ;
        }
        else cSign = '+' ;
        // Check the maximum printable number
        if (fNum < 1e17)
        {
            // Must print the fractional part
            if (iNFrac >= 0)
            {
                // Not more than 16 digits
                if (iNFrac > 16) iNFrac = 16 ;
                fNum = round(fNum * _10[iNFrac]) ;
                // Prints each digit
                for ( ; iNFrac > 0 ; --iNFrac )
                {
                    *(--szBuf) = '0' + (uint8_t)frem(&fNum, 10.0) ;
                }
                *(--szBuf) = '.' ;
            }
            // Must print only the integer part
            else fNum = round(fNum) ;
            // Prints the mantissa
            do
            {
                *(--szBuf) = '0' + (uint8_t)frem(&fNum, 10.0) ;
            } while (fNum >= 1.0) ;
            *(--szBuf) = cSign ;
        }
    }
    // Returns the pointer
    return szBuf ;
}


/*******************************************************************************************************************//**
 * @brief   String edit utility through a generic communication device
 * @param   *pfTx       Pointer to the transmit function used to echo the received characters. If NULL the echo is
 *                      suppressed. The function must be in the following form:
 *                          void (*pfTx)(char ch)
 *                      where ch is the character to be printed
 * @param   *pfRx       Pointer to the receive function. It cannot be NULL. The function must be in the following form:
 *                          char (*pfRx)(void)
 *                      where ch is the received character
 * @param   *str        Pointer to the destination buffer of the string. The value NULL is not accepted
 * @param   size        Size of the destination buffer. It must be greater than or equal to 1. The destination buffer
 *                      should store the string and the termination character, so this argument must be set to one
 *                      character more than the maximum string length that the function should handle. If size is
 *                      exactly equal to 1 then the function does not accept characters other than 'new line' and
 *                      generates an empty string
 * @retval  char *      Value of the argument 'str' or NULL in case of errors
 */
char *EditString(void (*pfTx)(char), char (*pfRx)(void), char *str, size_t size)
{
    char ch ;
    uint32_t curs = 0 ;
    // Check the arguments
    if ((NULL == pfRx) || (NULL == str) || (0 == (size--))) return NULL ;
    // Edit loop
    while (1)
    {
        // Wait for a new char
        ch = pfRx() ;
        // Is it printable?
        if (isprint(ch))
        {
            // Is there room in the string?
            if (curs < size)
            {
                // Echoes the received character
                if (NULL != pfTx) (*pfTx)(ch) ;
                // Stores the received character
                str[curs++] = ch ;
            }
        }
        // Is it "backspace" or DEL?
        else if (('\b' == ch) || ('\177' == ch))
        {
            // At least one char to delete
            if (curs > 0)
            {
                // Echo required
                if (NULL != pfTx)
                {
                    // Echoes the backspace
                    (*pfTx)('\b') ;
                    (*pfTx)(' ') ;
                    (*pfTx)('\b') ;
                }
                // Decrements the cursor
                --curs ;
            }
        }
        // Is it "carriage return"?
        else if ('\r' == ch) break ;
    }
    // String termination
    str[curs] = '\0' ;
    return str ;
}


/*******************************************************************************************************************//**
 * @brief   Search of the first non-BLANK character in a string. The non-BLANK characters are the ones that return FALSE
 *          in the stantdard isspace() function.
 * @param   pCur:   Pointer to the current string pointer. The function starts scanning the string from the position
 *                  indicated by the pointer at the beginning, then it leaves the pointer at the first non-blank
 *                  character or at the string terminator.
 * @retval  First non-blank character or '\0' if the string terminates before finding any non-blank character
 */
char skipblank(const char **pCur)
{
    const char *str ;
    char ch  = '\0' ;
    // Checks the pointers
    if ( (pCur != NULL) && ((str = *pCur) != NULL) )
    {
        // Skips the white spages
        while (isspace(ch = *str)) ++str ;
        // Updates the cursor
        *pCur = str ;
    }
    // First not blank char
    return ch ;
}


/*******************************************************************************************************************//**
 * @brief   Compares the first non-blank character in a string with a sample character.
 * @param   ch:     Sample character to be compared. The comparision is case-sensitive
 * @param   pCur:   Pointer to the current string pointer. The function search the first non blank character in the
 *                  string, then it compares it with the sample character. If the characters are equal then the string
 *                  updates the pointer the first character after the searched one.
 * @retval  - true: The characters are equal. The pointer is updated
 *          - false: The characters are different. The pointer is unchanged
 */
bool charcmp(char ch, const char **pCur)
{
    if (skipblank(pCur) == ch)
    {
        ++(*pCur) ;
        return true ;
    }
    return false ;
}


/*******************************************************************************************************************//**
 * @brief   Compares a token in a string with a sample token. The comparision is case-insensitive
 * @param   tok:    Pointer to the sample token. It must be a LOWERCASE NOT-BLANK string.
 * @param   pCur:   Pointer to the current string pointer. The function search the first token in the string, then it
 *                  compares it with the sample token. If the tokens are equal then the string updates the pointer to
 *                  the first character after the token.
 * @retval  - true: the token are identical. The pointer has been updated
 *          - false: the tokens are different. The pointer is unchanged
 */
bool tokencmp(const char *tok, const char **pCur)
{
    const char *ptr ;
    char ch1, ch2 ;
    ch1 = *tok ;
    ch2 = skipblank(pCur) ;
    ptr = *pCur ;
    while (ch1 != '\0')
    {
        if (tolower(ch1) != tolower(ch2)) return false ;
        ch1 = *(++tok) ;
        ch2 = *(++ptr) ;
    }
    if (isalnum(ch2)) return false ;
    *pCur = ptr ;
    return true ;
}


/*******************************************************************************************************************//**
 * @brief   Converts an ASCII string, representing an unsigned decimal number, to a 32 bit unsigned integer. The
 *          function skips all the non-blank characters before the number, then converts all the consecutive numerical
 *          characters. The function leaves the pointer to the first non-numerical character after the converted number.
 *          The functions signals an error if it does not find any numerical character
 * @param   pCur:   Pointer to the current string pointer
 * @param   pNbr:   Pointer to the unsigned 32 bit variable used to store the converted number
 * @retval  - true:     Conversion competed without errors
 *          - false:    Errors detected.
 */
bool atoul(const char **pCur, uint32_t *pNbr)
{
    const char *str ;
    // Checks the pointers
    if ( pCur && ((str = *pCur) != NULL) )
    {
        uint8_t ch ;
        uint32_t ul ;
        // Skips the blank spaces
        ch = skipblank( &str ) ;
        // At least one digit
        if (_todec(ch))
        {
            // Converts the first digit
            ul = (uint32_t)ch ; ch = *(++str) ;
            // Converts all the other digits
            for ( ; _todec(ch) ; ch = *(++str))
            {
                // Check and convertion of a new digit
                if (ul > 429496729u) goto __error ;
                ul *= 10u ;
                ul += (uint32_t)ch ;
                if (ul < (uint32_t)ch) goto __error ;
            }
            // Update pointer and result
            *pCur = str ; if (pNbr) *pNbr = ul ;
            // All done
            return true ;
        }
    }
    // Errors detected
    __error: return false ;
}


/*******************************************************************************************************************//**
 * @brief   Converts an ASCII string, representing a signed decimal number, to a 32 bit signed integer. The function
 *          skips all the non-blank characters before the number, then converts all the consecutive numerical
 *          characters. The function leaves the pointer to the first non-numerical character after the converted number.
 *          The functions signals an error if it does not find any numerical character
 * @param   pCur:   Pointer to the current string pointer
 * @param   pNbr:   Pointer to the unsigned 32 bit variable used to store the converted number
 * @retval  - true:     Conversion competed without errors
 *          - false:    Errors detected.
  */
bool atosl(const char **pCur, int32_t *pNbr)
{
    const char *str ;
    // Checks the pointers
    if ( pCur && ((str = *pCur) != NULL) )
    {
        uint8_t ch ;
        uint8_t sign ;
        uint32_t ul ;
        // Skips the blank spaces
        ch = skipblank( &str ) ;
        // Detect the sign of the number
        if ((ch == '+') || (ch == '-'))
        {
            sign = ch ;
            ch = *(++str) ;
        }
        // If sign is absent, assumes the number is positive
        else sign = '+' ;
        // At least one digit
        if (_todec(ch))
        {
            // Converts the first digit
            ul = (uint32_t)ch ; ch = *(++str) ;
            // Converts all the other digits
            for ( ; _todec(ch) ; ch = *(++str))
            {
                // Check and convert a new digit
                if (ul > 214748364u) goto __error ;
                ul *= 10u ;
                ul += (uint32_t)ch ;
                if (ul < (uint32_t)ch) goto __error ;
            }
            // Should be negative
            if (sign == '-')
            {
                // Check the most negative number
                if (ul > 0x80000000) goto __error ;
                // Adjust sign
                ul = ~ul + 1 ;
            }
            // Check the most positive number
            else if (ul >= 0x80000000) goto __error ;
            // Update pointer and number
            *pCur = str ; if (pNbr) *pNbr = (int32_t)ul ;
            // Done
            return true ;
        }
    }
    // Errore
    __error: return false ;
}


/*******************************************************************************************************************//**
 * @brief   Converts an ASCII string, representing an unsigned hexadecimal number, to a 32 bit unsigned integer. The
 *          function skips all the non-blank characters before the number, then converts all the consecutive hex
 *          characters. The function accepts the following characters: '0'..'9', 'A'..'F', 'a'..'f'. The function leaves
 *          the pointer to the first non-hex character after the converted number. The function signals an error if it
 *          does not find any hex character.
 * @param   pCur:   Pointer to the current string pointer
 * @param   pNbr:   Pointer to the unsigned 32 bit variable used to store the converted number
 * @retval  - true:     Conversion competed without errors
 *          - false:    Errors detected.
  */
bool xtoul(const char **pCur, uint32_t *pNbr)
{
    const char *str ;
    // Verifica i puntatori
    if ( (pCur != NULL) && ((str = *pCur) != NULL) )
    {
        uint8_t ch ;
        uint32_t ul ;
        // Salta gli spazi iniziali
        ch = skipblank( &str ) ;
        // Ci vuole almeno un digit
        if ( _tohex(ch) )
        {
            // Converte il primo digit
            ul = (uint32_t)ch ; ch = *(++str) ;
            // Conversione dei digit successivi
            for ( ; _tohex(ch) ; ch = *(++str) )
            {
                // Primo check dell'overflow
                if (ul > 0x0FFFFFFF) goto __error ;
                // Check e conversione del digit
                ul <<= 4 ;
                ul += (uint32_t)ch ;
                // Secondo check dell'overflow
                if (ul < (uint32_t)ch) goto __error ;
            }
            // Update pointer and result
            *pCur = str ; if (pNbr) *pNbr = ul ;
            // Done
            return true ;
        }
    }
    // Errors
    __error: return false ;
}


/*******************************************************************************************************************//**
 * @brief  Tiny printf-type string parser.
 * @param  *fp      Pointer to the function to be used to print the character. The function must be in the following
 *                  form:
 *                      void (*cfp)(char ch)
 *                  where ch is the character to be printed
 * @param  *fmt     Format string. The format is like the standard printf format, with the following limitations:
 *                  - Only the %d, %u, %x, %X, %f, %s, %S formats are accepted. The h and l flags are not accepted
 * @param  ...      Variable arguments
 * @retval int      Numbers of arguments accepted and printed
 */
enum
{
	_dubl_ = 0x10,
	_sign_ = 0x08,
	_left_ = 0x04,
	_prec_ = 0x02,
	_zero_ = 0x01
} ;
static void __print( void (*fp)(char), char *str, int width, int prec, int flags, int size )
{
    char ch ;
	// Pad with leading spaces
	if (!(flags & _left_)) {while (width > size++) (*fp)(' ');}
	// Print the number
	while ((ch = *(str++)) != '\0') (*fp)(ch) ;
	// Pad with trailing spaces
	if (flags & _left_) {while (width > size++) (*fp)(' ');}
}
int tiny_vprintf( void (*fp)(char), const char *fmt, va_list ap)
{
    char buf[35] ;
    char ch ;
    char *str ;
    int width, prec, flags, size ;
    int acnt ;
    // Scans the format string
    for (acnt = 0 ; (ch = *fmt) != '\0' ; fmt++)
    {
        // Detects a format field
        if (ch == '%')
        {
            // Clears all the field options
            flags = prec = width = 0 ;
            // Parse the flags
            while (1)
            {
                // Get the new char
                ch = *(++fmt) ;
                // Forces the print of the sign
                if (ch == '+') flags |= _sign_ ;
                // Forces the left-justification
                else if (ch == '-') flags |= _left_ ;
                else break ;
            }
            // Starts the 'width' field
            if (isdigit(ch))
            {
                // Forces the print of leading zeros
                if (ch == '0') flags |= _zero_ ;
                do
                {
                    // Get the 'width' field
                    width = 10u*width + (ch -'0') ;
                } while (isdigit(ch = *(++fmt))) ;
            }
            // Starts the 'prec' field
            if (ch == '.')
            {
                flags |= _prec_ ;
                // Get the prec field
                while (isdigit(ch = *(++fmt))) prec = 10u*prec + (ch -'0') ;
            }
            // Get the 'long' prefix
            if ((ch == 'l')||(ch == 'L'))
            {
                flags |= _dubl_ ;
                ch = *(++fmt) ;
            }
            // Decode the type field
            switch (ch)
            {
                case 'c':
                case 'C':
                    // Prints the character
                    (*fp)((uint32_t)va_arg(ap, uint32_t)) ;
                    ++acnt ;
                    break ;
                case 'd':
                case 'i':
                    {
                        if (flags&_dubl_)
                        {
                            int64_t ld ;
                            // Get the argument
                            ld = va_arg(ap, int64_t) ;
                            // Convert to string
                            str = slltoa(buf, ld) ;
                            // Number of digits
                            size = &(buf[21]) - str ;
                            // Adjust the precision
                            if (!(flags&_prec_) && (flags&_zero_)) prec = width ;
                            if (prec > 20) prec = 20 ;
                            if (size < prec) size = prec ;
                            str = &(buf[21-size]) ;
                        }
                        else
                        {
                            int32_t d ;
                            // Get the argument
                            d = (int32_t)va_arg(ap, int32_t) ;
                            // Convert to string
                            str = sltoa(buf, d) ;
                            // Number of digits
                            size = &(buf[11]) - str ;
                            // Adjust the precision
                            if (!(flags&_prec_) && (flags&_zero_)) prec = width ;
                            if (prec > 10) prec = 10 ;
                            if (size < prec) size = prec ;
                            str = &(buf[11-size]) ;
                        }
                        // Add the sign if required
                        if ((buf[0] == '-') || (flags & _sign_))
                        {
                            *(--str)=buf[0] ;
                            ++size ;
                        }
                    }
                    __print(fp, str, width, prec, flags, size); ++acnt;
                    break ;
                case 'f':
                    {
                        // Adjust the default precision
                        if (!(flags & _prec_)) prec = -1 ;
                        else if (prec > 16) prec = 16 ;
                        // Get the argument and convert
                        str = ftoa(buf, (double)va_arg(ap, double), prec) ;
                        // Overflow: cannot print            12345678901234567890123456789012345
                        if (str[0]=='\0') strcpy(str = buf, "#################.################") ;
                        // Shouldn't print the screen if positive
                        if (!(flags & _sign_) && (*str == '+')) ++str ;
                        // Calculates the size of the string
                        size = &buf[sizeof(buf)-1] - str ;
                    }
                    __print(fp, str, width, prec, flags, size); ++acnt;
                    break ;
                case 's':
                    {
                        // Get the argument
                        str = (char *)va_arg(ap, char *) ;
                        // Calculate the size of the string
                        size = strlen(str) ;
                    }
                    __print(fp, str, width, prec, flags, size); ++acnt;
                    break ;

#ifdef WCHAR_ENABLE
                case 'S':
                    {
                        WCHAR *sp ;
                        WCHAR sc ;
                        unsigned size ;
                        // Get the argument
                        sp = (WCHAR *)va_arg(ap, WCHAR *) ;
                        // Calculate the size of the string
                        size = wsize(sp) ;
                        // Pad with leading spaces
                        if (!(flags & _left_))
                        {
                            while (width > size++) (*fp)(' ') ;
                        }
                        // Print the string up to the terminator
                        while ((sc = *(sp++)) != 0) (*fp)((uint32_t)sc) ;
                        // Pad with trailing spaces
                        if (flags & _left_)
                        {
                            while (width > size++) (*fp)(' ') ;
                        }
                        ++acnt ;
                    }
                    break ;
#endif

                case 'u':
                    {
                        if (flags&_dubl_)
                        {
                            int64_t lu ;
                            // Get the argument
                            lu = va_arg(ap, uint64_t) ;
                            // Convert to string
                            str = ulltoa(buf, lu) ;
                            // Number of digits
                            size = &(buf[21]) - str ;
                            // Adjust the precision
                            if (!(flags&_prec_) && (flags&_zero_)) prec = width ;
                            if (prec > 21) prec = 21 ;
                            if (size < prec) size = prec ;
                            str = &(buf[21-size]) ;
                        }
                        else
                        {
                            uint32_t u ;
                            // Get the argument
                            u = va_arg(ap, uint32_t) ;
                            // Convert to string
                            str = ultoa(buf, u) ;
                            // Number of digits
                            size = &(buf[10]) - str ;
                            // Adjust the precision
                            if (!(flags&_prec_) && (flags&_zero_)) prec = width ;
                            if (prec > 10) prec = 10 ;
                            if (size < prec) size = prec ;
                            str = &(buf[10-size]) ;
                        }
                    }
                    __print(fp, str, width, prec, flags, size); ++acnt;
                    break ;
                case 'x':
                case 'X':
                    {
                        uint32_t x ;
                        // Get the argument
                        x = (uint32_t)va_arg(ap, uint32_t) ;
                        // Convert to string
                        str = ultox(buf, x) ;
                        // Number of digits
                        size = &(buf[8]) - str ;
                        // Adjust the precision
                        if (!(flags&_prec_) && (flags&_zero_)) prec = width ;
                        if (prec > 8) prec = 8 ;
                        if (size < prec) size = prec ;
                        str = &(buf[8-size]) ;
                    }
                    __print(fp, str, width, prec, flags, size); ++acnt;
                    break ;
                default:
                    // Unknown field
                    (*fp)(ch) ;
                    break ;
            }
        }
        // Not a format field
        else (*fp)(ch) ;
    }
    return acnt ;
}

#ifdef WCHAR_ENABLE
/*******************************************************************************************************************//**
 * @brief   Returns the lenght of a wide-char string
 * @param   *pSrc       Pointer to the wide-char string source
 * @retval  unsigned    String lenght (the terminating NUL character is not included)
 */
unsigned wsize(const WCHAR *pSrc)
{
    unsigned i = 0 ;
    if (pSrc != NULL) for ( ; *(pSrc++) != 0 ; i++) ;
    return i ;
}
#endif

#ifdef WCHAR_ENABLE
/*******************************************************************************************************************//**
 * @brief   wide-char string copy
 * @param   *pDst   Pointer to the wide-char string destination
 * @param   *pSrc   Pointer to the wide-char string source
 * @retval  *pDst   The same pointer to the wide-char string destination
 */
WCHAR *wcpy(WCHAR *pDst, const WCHAR *pSrc)
{
    if (pDst && pSrc)
    {
        WCHAR *pd = pDst ;
        while ((*(pd++) = *(pSrc++)) != 0) ;
    }
    return pDst ;
}
#endif

#ifdef WCHAR_ENABLE
/*******************************************************************************************************************//**
 * @brief   wide-char string concatenation
 * @param   *pDst   Pointer to the wide-char string destination. The buffer must be large enough to hold the resulting
 *                  concatenated string
 * @param   *pSrc   Pointer to the wide-char string source
 * @retval  *pDst   The same pointer to the wide-char string destination
 */
WCHAR *wcat(WCHAR *pDst, const WCHAR *pSrc)
{
    if (pDst)
    {
        WCHAR *pd = pDst ;
        for ( ; (*(pd)) != 0 ; pd++) ;
        wcpy(pd, pSrc) ;
    }
    return pDst ;
}
#endif

/* END OF FILE ********************************************************************************************************/
