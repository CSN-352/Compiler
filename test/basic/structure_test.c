// Passing

struct Point {
    int x;
    int y;
    // int (*operation)(int, int);
};
struct foo
{
    int a;
    int b;
    struct foo *next;
};

// typedef struct
// {
//     int id;
//     char name[20];
// } Student;

// struct Incomplete;

int main(){
    struct Point p1;
    // struct Point p2 = {x = 10, y = 20}; // Failing
    p1.x = 10;
    p1.y = 20;
    struct Point *ptr = &p1;
    ptr->x = 30;
    ptr->y = 40;
    // struct Incomplete *inc_ptr = 0;
    // breakpoint;
    // breakpoint_global;
}
