struct Point {
    int x;
    int y;
};
int main(){
    struct Point p1;
    struct Point p2 = {x = 10, y = 20};
    struct Point *ptr = &p2;
    p1.x = 10;
    p1.y = 20;
    ptr->x = 30;
    ptr->y = 40;
}
