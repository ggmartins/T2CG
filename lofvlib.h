typedef struct{ float x,y,z; } vertex;
typedef struct{ vertex v1, v2; } line;
typedef struct
{
  int n;
  line *lines;
} lofv;

void lofv_load(char *fname, lofv **lofv1);

