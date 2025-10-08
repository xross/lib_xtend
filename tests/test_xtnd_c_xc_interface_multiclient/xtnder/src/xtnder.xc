
#include <stddef.h>
#include <print.h>

/* Test uses an interface with multiple clients since this uses the libc_lock
 */

interface math_if
{
    int add(int, int);
    int sub(int, int);
    void exit();
};

[[distributable]]
void mathTask(server interface math_if i_math[n], size_t n)
{
    while(1)
    {
        select
        {
            case (size_t i = 0; i < n; i++) i_math[i].add(int a, int b) -> int result:
                result = a + b;
                break;
            case (size_t i = 0; i < n; i++) i_math[i].sub(int a, int b) -> int result:
                result = a - b;
                break;
            case (size_t i = 0; i < n; i++) i_math[i].exit():
                return;
        }
    }
}

/* Note, this pattern generates non-PIC */
int adderTask(client interface math_if i_math, int a, int b)
{
    int result = i_math.add(a, b);
    i_math.exit();
    return result;
}

int xtnd_add(unsigned a, unsigned b)
{
    interface math_if i_math[1];
    int result;

   par
    {
        mathTask(i_math, 1);
        result = adderTask(i_math[0], a, b);
    }

    return result;
}

int xtnd_sub(unsigned a, unsigned b)
{
    interface math_if i_math[1];
    int result;

    par
    {
        mathTask(i_math, 1);
        {
            result = i_math[0].sub(a, b);
            i_math[0].exit();
        };
    }

    return result;
}

