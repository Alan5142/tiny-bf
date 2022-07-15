#include <stddef.h>
#  include <asm/unistd.h>

#if __x86_64__
inline int syscall_incomp(int sys, size_t a, size_t b, size_t c)
{
    int ret;
    asm volatile
    (
        "syscall"
        : "=a"(ret)
        : "0"(sys), "D"(a), "S"(b), "d"(c)
        : "rcx", "r11", "memory"
    );
    return ret;
}

#endif

#if __i386__
inline int syscall_incomp(int sys, size_t a, size_t b, size_t c)
{
    int ret;
    asm volatile
    (
        "int $0x80"
        : "=a"(ret)
        : "a"(sys), "b"(a), "c"(b), "d"(c)
        : "memory"
    );
    return ret;
}
#endif


inline int write(int fd, const char * restrict buf, size_t size)
{
    return syscall_incomp(__NR_write, fd, (size_t)buf, size);
}

inline int read(int fd, char *restrict buf, size_t count)
{
    return syscall_incomp(__NR_read, fd, (size_t)buf, count);
}

inline int exit_sys(int exit_code)
{
    return syscall_incomp(__NR_exit, exit_code, 0, 0);
}

inline int string_length(const char * restrict str)
{
    int count = 0;
    while (*str++)
        count++;
    return count;
}

inline static void memset_zero(char * restrict ptr, unsigned long size)
{
    while (--size)
        *ptr++ = 0;
}


// Entry point
void _start()
{
    char program_mem[30000];
    char data_mem[30000];
    memset_zero(data_mem, sizeof(data_mem));
    int r = read(0, program_mem, sizeof(program_mem));

    char *pc = program_mem;
    char *p = data_mem;

    char level = 0;

    while (*pc)
    {
        switch(*pc)
        {
            case '>': ++p; break;
            case '<': --p; break;
            case '+': ++*p; break;
            case '-': --*p; break;
            case '.': write(1, p, 1); break;
            case ',': 
            {
                char buf[2];
                read(0, buf, 2);
                *p = buf[0];
                break;
            }
            case '[':
                if (*p == 0)
                {
                    while (*(++pc))
                    {
                        if (*pc == ']' && level == 0)
                        {
                            break;
                        }
                        if (*pc == '[') ++level;
                        else if (*pc == ']') --level;
                    }
                    break;
                }
                break;
            case ']':
                if (*p != 0)
                {
                    while (*(--pc))
                    {
                        if (*pc == '[' && level == 0)
                        {
                            break;
                        }
                        if (*pc == ']') ++level;
                        else if (*pc == '[') --level;
                    }
                    break;
                }
                break;
            default:
                break;
        }
        pc++;
    }

    exit_sys(0);
}