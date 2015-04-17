/******************************************************************************
File    :  printf.c

This file is part of the ETOS distribution
Copyright (c) 2014, ETOS Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
(version 2) as published by the Free Software Foundation. See
the LICENSE file in the top-level directory for more details.

Description:
		printf functions implement for ETOS

History:

Date           Author       Notes
----------     -------      -------------------------
2014-10-6     deeve        Create

*******************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "etos_includes.h"

/******************************************************************************
 *                                 Defines                                    *
 ******************************************************************************/


/******************************************************************************
 *                                 Global Variables                           *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Variables                            *
 ******************************************************************************/

/******************************************************************************
 *                                 Local Functions                            *
 ******************************************************************************/
static inline s32 _skip_atoi(const s8 **s)
{
    register s32 ret = 0;
    while (isdigit(**s)) {
        ret = ret * 10 + *((*s)++) - '0';
    }

    return ret;
}

static s8 *_format_number(s8 *buf, s8 *buf_end, s32 number, u32 base, s32 field_width, s32 precision, u32 prefix_mask)
{
    s8 c, sign;
    s8 num_str[16];
    register s32 i;

    const s8 *digits;
    static const s8 small_digits[] = "0123456789abcdef";
    static const s8 large_digits[] = "0123456789ABCDEF";

    /*do not support base > 16 for speed*/
    if (base > 16) {
        base = 16;
    }

    digits = (prefix_mask & LARGE) ? large_digits : small_digits;
    if (prefix_mask & LEFT) {
        prefix_mask &= ~ZEROPAD;
    }

    c = (prefix_mask & ZEROPAD) ? '0' : ' ';

    /* get unsigned/signed */
    sign = 0;
    if (prefix_mask & SIGN)	{
        if (number < 0) {
            sign = '-';
            number = -number;
            field_width--;
        } else if (prefix_mask & PLUS) {
            sign = '+';
            field_width--;
        } else if (prefix_mask & SPACE) {
            sign = ' ';
            field_width--;
        }
    }

    /* adjust field width */
    if (prefix_mask & SPECIAL) {
        if (base == 16) {
            field_width -= 2;
        } else if (base == 8) {
            field_width--;
        }
    }

    /* format number string*/
    i = 0;
    if (number == 0) {
        num_str[i++] = '0';
    } else {
        while (number != 0) {
#if 1
            num_str[i++] = digits[DIVIDE(number, base)];
#else
            num_str[i++] = digits[DIVIDE((u32 *)&number, base)];
#endif
        }
    }

    if (i > precision) {
        precision = i;
    }
    field_width -= precision;

    /* start to format string for buf*/

    /* align to the right by default if field width is specified */
    if ((prefix_mask & (ZEROPAD | LEFT)) == 0)	{
        while (field_width-- > 0) {
            if (buf <= buf_end) {
                *buf++ = ' ';
            }
        }
    }

    /* process unsigned/signed */
    if (sign) {
        if (buf <= buf_end) {
            *buf++ = sign;
            field_width--;
        }
    }

    /* process 0x/0X */
    if (prefix_mask & SPECIAL) {
        if (base == 8) {
            if (buf <= buf_end) {
                *buf++ = '0';
            }
        } else if (base == 16) {
            if (buf <= buf_end) {
                *buf++ = '0';
            }
            if (buf <= buf_end) {
                *buf++ = (prefix_mask & LARGE) ? 'X' : 'x';
            }
        }
    }

    /* no align to the left */
    if ((prefix_mask & LEFT) == 0) {
        while (field_width-- > 0) {
            if (buf <= buf_end) {
                *buf++ = c;
            }
        }
    }

    /* pad zero for precision */
    while (i < precision)	{
        precision--;
        if (buf <= buf_end) {
            *buf++ = '0';
        }
    }


    /* put num_str into the buf */
    while (i-- > 0)	{
        if (buf <= buf_end) {
            *buf++ = num_str[i];
        }
    }

    /* pad space for field_width */
    while (field_width-- > 0) {
        if (buf <= buf_end) {
            *buf++ = ' ';
        }
    }

    return buf;
}

/******************************************************************************
 *                                 Global Functions                           *
 ******************************************************************************/


/*  regular expression of fmt(one argument) = .*%[-0+ #]*[0-9]*\*?[lh]?[csp%oxXdiu].*  */
u32 vsnprintf(s8 *buf, u32 buf_size, const s8 *fmt, va_list args)
{
    u32 prefix_mask;
    s32 field_width;	/* width of output field */
    s32 precision;		/* bits for fractional part */
    s32 qualifier;		/* 'h', 'l', or 'q' for integer fields */

    const char *s;
    u32 len, i, base;
    s32 num;

    s8 *str;
    s8 *str_end = buf + buf_size - 1;

    for (str = buf; ((*fmt != '\0') && (str < str_end)); ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        /* process prefix */
        prefix_mask = 0;
        while (1) {
            ++fmt;		/* also skips first '%' */
            if (*fmt == '0') {
                prefix_mask |= ZEROPAD;
            } else if (*fmt == '-') {
                prefix_mask |= LEFT;
            } else if (*fmt == '+') {
                prefix_mask |= PLUS;
            } else if (*fmt == ' ') {
                prefix_mask |= SPACE;
            } else if (*fmt == '#') {
                prefix_mask |= SPECIAL;
            } else {
                break;
            }
        }

        /* get field width */
        field_width = -1;
        if (isdigit(*fmt)) {
            field_width = _skip_atoi(&fmt);
        } else if (*fmt == '*') {
            ++fmt;
            /* use next argument for width*/
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                prefix_mask |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (isdigit(*fmt)) {
                precision = _skip_atoi(&fmt);
            } else if (*fmt == '*') {
                ++fmt;
                /* use next argument for width*/
                precision = va_arg(args, int);
            }

            if (precision < 0) {
                precision = 0;
            }
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if ((*fmt == 'l') || (*fmt == 'h')) {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
            case 'c':
                if ((prefix_mask & LEFT) == 0) {
                    while (--field_width > 0) {
                        if (str < str_end) {
                            *str++ = ' ';
                        }
                    }
                }

                if (str < str_end) {
                    *str++ = (s8) va_arg(args, int);
                }

                while (--field_width > 0) {
                    if (str < str_end) {
                        *str++ = ' ';
                    }
                }

                continue;  /*process next character in fmt, goto next loop in for*/

            case 's':
                s = va_arg(args, char *);
                if (!s) {
                    s = "<NULL>";
                }

                len = strlen(s);

                if ((prefix_mask & LEFT) == 0) {
                    while ((s32)len < field_width) {
                        field_width--;
                        if (str < str_end) {
                            *str++ = ' ';
                        }
                    }
                }

                for (i = 0; (i < len) && (str < str_end); i++) {
                    *str++ = *s++;
                }

                while ((s32)len < field_width) {
                    field_width--;
                    if (str < str_end) {
                        *str++ = ' ';
                    }
                }

                continue;  /*process next character in fmt, goto next loop in for*/

            case 'p':
                if (field_width == -1) {
                    field_width = sizeof(void *) << 1;
                    prefix_mask |= ZEROPAD;
                }
                str = _format_number(str, str_end, (s32) va_arg(args, void *),
                                     16, field_width, precision, prefix_mask);
                continue;  /*process next character in fmt, goto next loop in for*/

            case '%':
                if (str < str_end) {
                    *str++ = '%';
                }
                continue;  /*process next character in fmt, goto next loop in for*/

                /* integer number formats - set up the prefix_mask and "break" */
            case 'o':
                base = 8;
                break;

            case 'X':
                prefix_mask |= LARGE;
            case 'x':
                base = 16;
                break;

            case 'd':
            case 'i':
                prefix_mask |= SIGN;
            case 'u':
                break;

            default:
                if (str < str_end) {
                    *str++ = '%';
                }
                if (*fmt) {
                    if (str < str_end) {
                        *str++ = *fmt;
                    }
                } else {
                    fmt--;
                }
                continue;  /*process next character in fmt, goto next loop in for*/
        }

        if (qualifier == 'l') {
            num = va_arg(args, unsigned long);
        } else if (qualifier == 'h') {
            num = va_arg(args, int);
            if (prefix_mask & SIGN) {
                num = (short) num;
            }
        } else if (prefix_mask & SIGN) {
            num = va_arg(args, int);
        } else {
            num = va_arg(args, unsigned int);
        }

        str = _format_number(str, str_end, num, base, field_width, precision, prefix_mask);
    }

    *str = '\0';

    return (u32)(str - buf);
}



u32 snprintf(s8 *buf, u32 buf_size, const s8 *fmt, ...)
{
    va_list args;
    u32 ret = 0;

    if ((buf == NULL) || (fmt == NULL)) {
        return ret;
    }

    va_start(args, fmt);
    ret = vsnprintf(buf, buf_size, fmt, args);
    va_end(args);

    return ret;
}



void printf(u32 output_handle, const s8 *fmt, ...)
{
    u32 num;
    va_list args;
    s8 print_buf[MAX_PRT_BUF_SIZE];

    va_start(args, fmt);
    num = vsnprintf(print_buf, MAX_PRT_BUF_SIZE, fmt, args);
    va_end(args);

    /* output the string */
    etos_gioi_put_bytes(output_handle, FALSE, (u8 *)print_buf, num);
}



/*print string will be sent when this API return*/
void printf_directly(u32 output_handle, const s8 *fmt, ...)
{
    u32 num;
    va_list args;
    s8 print_buf[MAX_PRT_BUF_SIZE];

    va_start(args, fmt);
    num = vsnprintf(print_buf, MAX_PRT_BUF_SIZE, fmt, args);
    va_end(args);

    /* output the string until it is sent*/
    etos_gioi_put_bytes(output_handle, TRUE, (u8 *)print_buf, num);
}



void panic(u32 output_handle, const s8 *fmt, ...)
{
    u32 num;
    va_list args;
    s8 print_buf[MAX_PRT_BUF_SIZE];

    va_start(args, fmt);
    num = vsnprintf(print_buf, MAX_PRT_BUF_SIZE, fmt, args);
    va_end(args);

    /* output the string until it is sent*/
    etos_gioi_put_bytes(output_handle, TRUE, (u8 *)print_buf, num);

#if (ETOS_DISABLE_INTERRUPT_AFTER_PANIC)
    etos_disable_cpu_interrupt();
#endif

    while (1);
}


/* EOF */

