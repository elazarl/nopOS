int toupper( int c )
{
    if ( ( c >= 'a' ) && ( c <= 'z' ) )
    {
        c -= ( 'a' - 'A' );
    }
    return c;
}

