enum Color
{
    RED,   // = 0
    GREEN, // = 1
    BLUE   // = 2
};

enum Status
{
    OK = 200,
    NOT_FOUND = 404,
    INTERNAL_ERROR = 500
};

union Data
{
    int intValue;
    float floatValue;
    char strValue[50];
};

typedef enum
{
    MONDAY = 1,
    TUESDAY,
    WEDNESDAY
} Day;

int main()
{
    // Enums
    enum Color color = RED;
    enum Status s = OK;
    if (s == 200){}
    Day today = TUESDAY;

    // Unions
    union Data data;
    data.intValue = 2006;
    data.floatValue = 2.2;
}
