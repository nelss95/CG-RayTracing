/*    Librerias         */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*    Estructuras         */
typedef struct Point Point;
struct Point{
    long double x;
    long double y;
    long double z;
};

typedef struct UV UV;
struct UV {
  long double U;
  long double V;
};


typedef struct Window Window;
struct Window{
    long double Xmin;
    long double Ymin;
    long double Xmax;
    long double Ymax;
};

typedef struct Color Color;
struct Color{
  long double r;
  long double g;
  long double b;
};


typedef struct Texture Texture;
struct Texture{
   int width;
   int height;
   Color** RGB;
};

typedef struct Plane Plane;
struct Plane{
  Point point0;
  Point point1;
  Point point2;
  Point Normal;
  long double D;
};

typedef struct Object Object;
struct Object{
  char  type;
  Color color;
  long double kd; /* coeficiente de reflexión difusa*/
  long double ka; /* coeficiente de iluminación ambiente*/
  long double ks; /* coeficiente de reflexion especular */
  long double kn;
  long double o1; /*Los o de la reflexion*/
  long double o2;
  long double o3; /* El o de la transparenicia*/
  int amountCutPlanes; /* Numero de planos de corte por objeto*/
  Plane * cutPlanesList;
  long double (*IntersectionFunction) (Point,Point,void*);
  Point (*NormalFunction) (Point,void*);
  void *details;

  int hasTexture;
  UV (*mapTexture) (void*, Point);
  Texture texture;

};

typedef struct Sphere Sphere;
struct Sphere{
  long double radius;
  Point center;

  Point vectorG;
  Point vectorNorth;
  Point pointSouth;

  int hasFretWork;
  Texture fretWork;
};


typedef struct RectangleWrap RectangleWrap;
struct RectangleWrap
{
  Point point0;
  Point point1;
  Point point2;
  Point point3;

  long double height;
  long double lenght;

  Point vectorH;
  Point vectorL;
};

typedef struct Polygon Polygon;
struct Polygon{
  int amounPoints;
  Point * pointList;
  Point Normal;
  long double D;
  UV * flatPoints;
  RectangleWrap wrap;

  int hasFretWork;
  Texture fretWork;
};


typedef struct Cylinder Cylinder;
struct Cylinder{
  Point anchor;
  Point direction;
  long double radius;
  long double minSize;
  long double maxSize;
  long double height;
  Point vectorG;

  int hasFretWork;
  Texture fretWork;
};

typedef struct Cone Cone;
struct Cone{
  Point anchor;
  Point direction;
  long double k1;
  long double k2;
  long double minSize;
  long double maxSize;
  long double height;
  Point vectorG;

  int hasFretWork;
  Texture fretWork;
};

typedef struct Light Light;
struct Light{
  Point position;
  long double intensity;
  long double c1;
  long double c2;
  long double c3;
};

typedef struct Intersection Intersection;
struct Intersection
{
  Object object;
  long double distance;
  Point interPoint;
  int vacio;
};

typedef struct Scene Scene;
struct Scene
{
  Point eye;
  long double Ia; /* Iluminación ambiente*/
  Object* objectList;
  Light* lightList;
  int amountObjects;
  int amountLights;
  int levelOfReflex;
  int levelOfTrans;
  int levelOfQuality;
};

typedef struct Disc Disc;
struct Disc{
  long double radius;
  Point center;
  Point point1;
  Point point2;
  Point Normal;
  long double D;

  RectangleWrap wrap;

  int hasFretWork;
  Texture fretWork;
};

typedef struct Elip Elip;
struct Elip{
  long double k;
  Point p1;
  Point p2;
  Point pA;
  Point Normal;
  long double D;
};

typedef struct QuadraticSuperface QuadraticSuperface;
struct QuadraticSuperface
{
  long double A;
  long double B;
  long double C;
  long double D;
  long double E;
  long double F;
  long double G;
  long double H;
  long double J;
  long double K;
  long double pTx;
  long double pTy;
  long double pTz;
};


/*     Constante          */
#define HORIZONTALRES 1920
#define VERTICALRES 1080
#define EPSILON 0.0005
#define PI (3.141592653589793)
#define MAXLANTIALIASING 1
