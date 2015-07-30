#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <errno.h>

int tolower( int c )
{
    if ( ( c >= 'A' ) && ( c <= 'Z' ) )
    {
        c += ( 'a' - 'A' );
    }
    return c;
}

int toupper( int c )
{
    if ( ( c >= 'a' ) && ( c <= 'z' ) )
    {
        c -= ( 'a' - 'A' );
    }
    return c;
}

void * memchr( const void * s, int c, size_t n )
{
    const unsigned char * p = (const unsigned char *) s;
    while ( n-- )
    {
        if ( *p == (unsigned char) c )
        {
            return (void *) p;
        }
        ++p;
    }
    return NULL;
}

extern char _PDCLIB_digits[];

const char * _PDCLIB_strtox_prelim( const char * p, char * sign, int * base )
{
    /* skipping leading whitespace */
    while ( isspace( *p ) ) ++p;
    /* determining / skipping sign */
    if ( *p != '+' && *p != '-' ) *sign = '+';
    else *sign = *(p++);
    /* determining base */
    if ( *p == '0' )
    {
        ++p;
        if ( ( *base == 0 || *base == 16 ) && ( *p == 'x' || *p == 'X' ) )
        {
            *base = 16;
            ++p;
            /* catching a border case here: "0x" followed by a non-digit should
               be parsed as the unprefixed zero.
               We have to "rewind" the parsing; having the base set to 16 if it
               was zero previously does not hurt, as the result is zero anyway.
            */
            if ( memchr( _PDCLIB_digits, tolower(*p), *base ) == NULL )
            {
                p -= 2;
            }
        }
        else if ( *base == 0 )
        {
            *base = 8;
        }
        else
        {
            --p;
        }
    }
    else if ( ! *base )
    {
        *base = 10;
    }
    return ( ( *base >= 2 ) && ( *base <= 36 ) ) ? p : NULL;
}

void *memcpy (void *__restrict s1, const void *__restrict s2,
		     size_t n)
{
    char * dest = (char *) s1;
    const char * src = (const char *) s2;
    while ( n-- )
    {
        *dest++ = *src++;
    }
    return s1;
}

void *memset(void *p, int val, size_t sz)
{
    char *dst = p;
    for (size_t i =0; i<sz; i++) {
        dst[i] = val;
    }
    return p;
}

char * strcpy( char * s1, const char * s2 )
{
    char * rc = s1;
    while ( ( *s1++ = *s2++ ) );
    return rc;
}

struct _PDCLIB_status_t
{
    int      base;   /* base to which the value shall be converted   */
    int      flags; /* flags and length modifiers                */
    size_t   n;      /* print: maximum characters to be written      */
                             /* scan:  number matched conversion specifiers  */
    size_t   i;      /* number of characters read/written            */
    size_t   current;/* chars read/written in the CURRENT conversion */
    char *           s;      /* *sprintf(): target buffer                    */
                             /* *sscanf():  source string                    */
    size_t   width;  /* specified field width                        */
    size_t   prec;   /* specified field precision                    */
    struct _PDCLIB_file_t * stream; /* *fprintf() / *fscanf() stream         */
    va_list  arg;    /* argument stack                               */
};

const char * _PDCLIB_print( const char * spec, struct _PDCLIB_status_t * status );

int vsnprintf( char * s, size_t n, const char * format, va_list arg )
{
    /* TODO: This function should interpret format as multibyte characters.  */
    struct _PDCLIB_status_t status;
    status.base = 0;
    status.flags = 0;
    status.n = n;
    status.i = 0;
    status.current = 0;
    status.s = s;
    status.width = 0;
    status.prec = 0;
    status.stream = NULL;
    va_copy( status.arg, arg );

    while ( *format != '\0' )
    {
        const char * rc;
        if ( ( *format != '%' ) || ( ( rc = _PDCLIB_print( format, &status ) ) == format ) )
        {
            /* No conversion specifier, print verbatim */
            s[ status.i++ ] = *(format++);
        }
        else
        {
            /* Continue parsing after conversion specifier */
            format = rc;
        }
    }
    s[ status.i ] = '\0';
    va_end( status.arg );
    return status.i;
}

char * strncpy( char * s1, const char * s2, size_t n )
{
    char * rc = s1;
    while ( ( n > 0 ) && ( *s1++ = *s2++ ) )
    {
        /* Cannot do "n--" in the conditional as size_t is unsigned and we have
           to check it again for >0 in the next loop below, so we must not risk
           underflow.
        */
        --n;
    }
    /* Checking against 1 as we missed the last --n in the loop above. */
    while ( n-- > 1 )
    {
        *s1++ = '\0';
    }
    return rc;
}

size_t strlen( const char * s )
{
    size_t rc = 0;
    while ( s[rc] )
    {
        ++rc;
    }
    return rc;
}

char * strcat( char * s1, const char * s2 )
{
    char * rc = s1;
    if ( *s1 )
    {
        while ( *++s1 );
    }
    while ( (*s1++ = *s2++) );
    return rc;
}

int strcmp( const char * s1, const char * s2 )
{
    while ( ( *s1 ) && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
    }
    return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

typedef unsigned long long int uintmax_t;

int errno;

uintmax_t _PDCLIB_strtox_main( const char ** p, unsigned int base, uintmax_t error, uintmax_t limval, int limdigit, char * sign )
{
    uintmax_t rc = 0;
    int digit = -1;
    const char * x;
    while ( ( x = (const char *)memchr( _PDCLIB_digits, tolower(**p), base ) ) != NULL )
    {
        digit = x - _PDCLIB_digits;
        if ( ( rc < limval ) || ( ( rc == limval ) && ( digit <= limdigit ) ) )
        {
            rc = rc * base + (unsigned)digit;
            ++(*p);
        }
        else
        {
            errno = ERANGE;
            /* TODO: Only if endptr != NULL - but do we really want *another* parameter? */
            /* TODO: Earlier version was missing tolower() here but was not caught by tests */
            while ( memchr( _PDCLIB_digits, tolower(**p), base ) != NULL ) ++(*p);
            /* TODO: This is ugly, but keeps caller from negating the error value */
            *sign = '+';
            return error;
        }
    }
    if ( digit == -1 )
    {
        *p = NULL;
        return 0;
    }
    return rc;
}

long int strtol( const char * s, char ** endptr, int base )
{
    long int rc;
    char sign = '+';
    const char * p = _PDCLIB_strtox_prelim( s, &sign, &base );
    if ( base < 2 || base > 36 ) return 0;
    if ( sign == '+' )
    {
        rc = (long int)_PDCLIB_strtox_main( &p, (unsigned)base, (uintmax_t)LONG_MAX, (uintmax_t)( LONG_MAX / base ), (int)( LONG_MAX % base ), &sign );
    }
    else
    {
        rc = (long int)_PDCLIB_strtox_main( &p, (unsigned)base, (uintmax_t)LONG_MIN, (uintmax_t)( LONG_MIN / -base ), (int)( -( LONG_MIN % base ) ), &sign );
    }
    if ( endptr != NULL ) *endptr = ( p != NULL ) ? (char *) p : (char *) s;
    return ( sign == '+' ) ? rc : -rc;
}

int memcmp( const void * s1, const void * s2, size_t n )
{
    const unsigned char * p1 = (const unsigned char *) s1;
    const unsigned char * p2 = (const unsigned char *) s2;
    while ( n-- )
    {
        if ( *p1 != *p2 )
        {
            return *p1 - *p2;
        }
        ++p1;
        ++p2;
    }
    return 0;
}

int strncmp( const char * s1, const char * s2, size_t n )
{
    while ( *s1 && n && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( ( n == 0 ) )
    {
        return 0;
    }
    else
    {
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
    }
}

unsigned long long int strtoull( const char * s, char ** endptr, int base )
{
    unsigned long long int rc;
    char sign = '+';
    const char * p = _PDCLIB_strtox_prelim( s, &sign, &base );
    if ( base < 2 || base > 36 ) return 0;
    rc = _PDCLIB_strtox_main( &p, (unsigned)base, (uintmax_t)ULLONG_MAX, (uintmax_t)( ULLONG_MAX / base ), (int)( ULLONG_MAX % base ), &sign );
    if ( endptr != NULL ) *endptr = ( p != NULL ) ? (char *) p : (char *) s;
    return ( sign == '+' ) ? rc : -rc;
}

unsigned long int strtoul( const char * s, char ** endptr, int base )
{
    unsigned long int rc;
    char sign = '+';
    const char * p = _PDCLIB_strtox_prelim( s, &sign, &base );
    if ( base < 2 || base > 36 ) return 0;
    rc = (unsigned long int)_PDCLIB_strtox_main( &p, (unsigned)base, (uintmax_t)ULONG_MAX, (uintmax_t)( ULONG_MAX / base ), (int)( ULONG_MAX % base ), &sign );
    if ( endptr != NULL ) *endptr = ( p != NULL ) ? (char *) p : (char *) s;
    return ( sign == '+' ) ? rc : -rc;
}

#undef isspace
int isspace(int c)
{
    switch ( c )
    {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            return 1;
        default:
            return 0;
    }
}

int *__errno_location(void)
{
    static int _static_errno;
    return &_static_errno;
}
