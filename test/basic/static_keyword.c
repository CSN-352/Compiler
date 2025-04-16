static int global_counter = 100;

// static int static_array[3] = {1, 2, 3};

static int static_add(int x)
{
    return x + 1;
}

void test_static_local()
{
    static int count = 0;
    count++;
}
enum Status
{
    OK,
    FAIL
};
static enum Status current_status = OK;
union U
{
    int a;
    float b;
};
static union U static_union_instance;

struct Data
{
    int x;
    // static int y; // Should fail
};

// static struct Data static_data = {.x = 10};

void set_static_var(int val)
{
    static int shared = 0;
    shared = val;
}
void get_static_var()
{
    // this 'shared' is separate, demonstrates static is local to function
    static int shared = -1;
}

int main()
{
    test_static_local(); // Should print 1
    test_static_local(); // Should print 2
    set_static_var(55);
    get_static_var(); // Should show separate scope
}
