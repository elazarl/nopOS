
int *__errno_location(void)
{
    static int _static_errno;
    return &_static_errno;
}
