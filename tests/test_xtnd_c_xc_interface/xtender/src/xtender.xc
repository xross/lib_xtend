interface math_if
{
    int add(int, int);
    int sub(int, int);
    void exit();
};

[[distributable]]
void mathTask(server interface math_if i_math)
{
    while(1)
    {
        select
        {
            case i_math.add(int a, int b) -> int result:
                result = a + b;
                break;
            case i_math.sub(int a, int b) -> int result:
                result = a - b;
                break;
            case i_math.exit():
                return;
        }
    }
}

/* Note, this pattern generares non-PIC */
int adderTask(client interface math_if i_math, int a, int b)
{
    int result = i_math.add(a, b);
    i_math.exit();
    return result;
}

int xtend_add(unsigned a, unsigned b)
{
    interface math_if i_math;
    int result;

   par
    {
        mathTask(i_math);
        result = adderTask(i_math, a, b);
    }

    return result;
}

int xtend_sub(unsigned a, unsigned b)
{
    interface math_if i_math;
    int result;

    par
    {
        mathTask(i_math);
        {
            result = i_math.sub(a, b);
            i_math.exit();
        };
    }

    return result;
}

