#pragma once

/* FILE structure */
struct _PDCLIB_file_t
{
    int            handle;   /* OS file handle */
    char *                  buffer;   /* Pointer to buffer memory */
    size_t          bufsize;  /* Size of buffer */
    size_t          bufidx;   /* Index of current position in buffer */
    size_t          bufend;   /* Index of last pre-read character in buffer */
    //fpos_t          pos;      /* Offset and multibyte parsing state */
    size_t          ungetidx; /* Number of ungetc()'ed characters */
    unsigned char *         ungetbuf; /* ungetc() buffer */
    unsigned int            status;   /* Status flags; see above */
    /* multibyte parsing status to be added later */
    char *                  filename; /* Name the current stream has been opened with */
    struct _PDCLIB_file_t * next;     /* Pointer to next struct (internal) */
};

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
