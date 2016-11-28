#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

Point loadAPoint(char str[50]);


Point loadAPoint(char str[50]) {
  //printf("%s\n",str);
  Point point;
  char * tok;
  tok = strtok(str,",");
  long double pivot = atof(tok);
  point.x = pivot;
  int flag = 0;
  int j =0;
  while( j < 2 ) {
    //printf("Hola\n");
      tok = strtok(NULL, ",");
      long double pivot = atof(tok);
      if (flag == 0) {
        point.y = pivot;

        flag++;
      }
      if (flag == 1) {
        point.z = pivot;

      }
      j++;
   }
   return point;
}

Scene createScene(FILE* file){
  char str[10] = {0};
  int count = 0;
  Scene scene;
  while (count < 10) {
    fgets(str,100,file);
    long double pivot = atof(str);
    //printf("%Lf\n",pivot);
    if (count == 0) {
      scene.eye.x = pivot;
    } else if (count == 1) {
      scene.eye.y = pivot;
    } else if (count == 2) {
      scene.eye.z = pivot;
    } else if (count == 3) {
      scene.Ia = pivot;
    }else if (count == 4) {
      scene.amountObjects = pivot;
    } else if (count == 5) {
      scene.amountLights = pivot;
    } else if (count == 6) {
      scene.levelOfReflex = pivot;
    } else if (count == 7){
      scene.levelOfTrans = pivot;
    } else if (count == 8) {
      scene.levelOfQuality = pivot;
    }
    count++;
  }

  return (scene);
}

Light  createLigh(FILE * file) {
  char str[10] = {0};
  int count = 0;
  Light light;
  while (count < 7) {
    fgets(str,10,file);
    long double pivot = atof(str);
    if (count == 0) {
      light.position.x = pivot;
    } else if (count == 1) {
      light.position.y = pivot;
    } else if (count == 2) {
      light.position.z = pivot;
    }else if (count == 3) {
      light.intensity = pivot;
    }else if (count == 4) {
      light.c1 = pivot;
    }else if (count == 5) {
      light.c2 = pivot;
    }else if (count == 6) {
      light.c3 = pivot;
    }
    count++;
 }
 return (light);
}

Sphere* createSphere(FILE * file){
  char str[10] = {0};
  int count = 0;
  Sphere* sph;
  sph = (Sphere*) calloc(1,sizeof(Sphere));
  while (count < 7) {
    fgets(str,10,file);
    long double pivot = atof(str);

    if (count == 0) {
      //printf("%Lf\n",pivot);
      sph->radius = pivot;
    } else if (count == 1) {
      sph->center.x = pivot;
    } else if (count == 2) {
      sph->center.y = pivot;
    }else if (count == 3) {
      sph->center.z = pivot;
    } else if (count == 4) {
      sph->vectorG = loadAPoint(str);
    } else if (count == 5) {
      sph->vectorNorth = loadAPoint(str);
    } else if (count == 6) {
      sph->hasFretWork = pivot;
      if (pivot == 1) {
        fgets(str,15,file);
        sph->fretWork = readAVS(str);
      }
    }
    //printf("%d\n",count);
    Point south = {sph->center.x - sph->radius * sph->vectorNorth.x,
					   sph->center.y - sph->radius * sph->vectorNorth.y,
					   sph->center.z - sph->radius * sph->vectorNorth.z};
    sph->pointSouth = south;

    count++;
 }

 return (sph);
}

Point * loadPoints(int number,FILE * file) {
  Point * list = (Point*) calloc(number,sizeof(Point));
  int i = 0;
  while (i < number ) {
    Point point;
    char * tok;
    char str[20] = {0};
    fgets(str,20,file);
    tok = strtok(str,",");
    long double pivot = atof(tok);
    point.x = pivot;

    //printf("Hice el primer x\n");

    int flag = 0;
    int j =0;
    while( j < 2 )
     {
        //printf( " %s\n", tok );
        tok = strtok(NULL, ",");

        long double pivot = atof(tok);
        if (flag == 0) {
          point.y = pivot;
          flag++;
        }
        if (flag == 1) {
          point.z = pivot;;
        }
        //printf("Termine el punto %d\n",i);
        j++;
     }

     list[i] = point;
     i++;

  }

  return (list);
}

Point createVector(Point destiny,Point origin){
        Point vector;
        vector.x = destiny.x - origin.x;
        vector.y = destiny.y - origin.y;
        vector.z = destiny.z - origin.z;
        return (vector);

}

Point crossProduct(Point vectorA, Point vectorB){
  Point vector;
  vector.x = (vectorA.y * vectorB.z) - (vectorA.z * vectorA.y);
  vector.y = (vectorA.z * vectorB.x) - (vectorA.x * vectorB.z);
  vector.z = (vectorA.x * vectorB.y) - (vectorA.y * vectorA.x);
  return(vector);
}

long double normaPolygon(Point pVector)
{
	long double norm = sqrt( pow(pVector.x, 2) + pow(pVector.y, 2) + pow(pVector.z,2) );
	return (norm);
}

long double getD(Point AuxNormal,Point dot) {
  long double D;
  D = -(AuxNormal.x * dot.x) - (AuxNormal.y * dot.y) - (AuxNormal.z * dot.z);
  return (D);
}

int i;
UV * loadFlatPoints (int number,Point * list, Point vector) {
  UV * flatPoints = (UV*) calloc(number,sizeof(UV));
  long double x,y,z,max;
  x = fabs(vector.x);
  y = fabs(vector.y);
  z = fabs(vector.z);
  max = x;
  if (y > max)
    max = y;
  if (z > max)
    max = z;

  if (max == x) {
    for (i = 0; i < number; i++) {
      UV auxFlat;
      auxFlat.U = list[i].y;
      auxFlat.V = list[i].z;
      flatPoints[i] = auxFlat;
    }
  } else if (max == y) {
    for (i = 0; i < number; i++) {
      UV auxFlat;
      auxFlat.U = list[i].x;
      auxFlat.V = list[i].z;
      flatPoints[i] = auxFlat;
    }
  } else if (max == z){
    for (i = 0; i < number; i++) {
      UV auxFlat;
      auxFlat.U = list[i].x;
      auxFlat.V = list[i].y;
      flatPoints[i] = auxFlat;
    }
  }

  return (flatPoints);
}






Cylinder * createCylinder(FILE * file){
  char str[20] = {0};
  int count = 0;
  Cylinder * cylin = (Cylinder*) calloc(1,sizeof(Cylinder));

  while (count < 7) {
    fgets(str,20,file);
    if (count == 0) {
      cylin->anchor = loadAPoint(str);
    //  printf("Hago el ancla\n");
    } else if (count == 1) {
      cylin->direction= loadAPoint(str);
      long double norm = sqrt( pow(cylin->direction.x, 2) + pow(cylin->direction.y, 2) + pow(cylin->direction.z,2) );
      cylin->direction.x = cylin->direction.x/norm;
      cylin->direction.y = cylin->direction.y/norm;
      cylin->direction.z = cylin->direction.z/norm;

    //  printf("Hago la direcion\n");
    } else if (count == 2) {

      long double pivot = atof(str);
      cylin->radius = pivot;
    } else if (count == 3) {
      long double pivot = atof(str);
      cylin->minSize = pivot;
    } else if (count == 4){
      long double pivot = atof(str);
      cylin->maxSize = pivot;
    } else if (count == 5) {
      cylin->vectorG = loadAPoint(str);
    } else if (count == 6) {
      long double pivot = atof(str);

      cylin->hasFretWork = pivot;
      if (pivot == 1) {
        fgets(str,15,file);
        cylin->fretWork = readAVS(str);
      }
    }
    count++;
  }
  cylin->height = cylin->maxSize - cylin->minSize;

  return cylin;
}

Cone * createCone(FILE * file){
  char str[20] = {0};
  int count = 0;
  Cone * cone = (Cone*) calloc(1,sizeof(Cone));

  while (count < 6) {
    fgets(str,20,file);
    if (count == 0) {
      cone->anchor = loadAPoint(str);
    } else if (count == 1) {
      cone->direction = loadAPoint(str);

      long double norm = sqrt( pow(cone->direction.x, 2) + pow(cone->direction.y, 2) + pow(cone->direction.z,2) );
      cone->direction.x = cone->direction.x/norm;
      cone->direction.y = cone->direction.y/norm;
      cone->direction.z = cone->direction.z/norm;

    } else if (count == 2) {
      long double pivot = atof(str);
      cone->k1 = pivot;
    } else if (count == 3) {
      long double pivot = atof(str);
      cone->k2 = pivot;
    } else if (count == 4) {
      long double pivot = atof(str);
      cone->minSize = pivot;
    } else if (count == 5) {
      long double pivot = atof(str);
      cone->maxSize = pivot;
    } else if (count == 6) {
      cone->vectorG = loadAPoint(str);
    } else if (count == 7) {
      long double pivot = atof(str);
      cone->hasFretWork = pivot;
      if (pivot == 1) {
        fgets(str,15,file);
        cone->fretWork = readAVS(str);
      }
    }
    count++;
  }
  cone->height = cone->maxSize - cone->minSize;
  return cone;
}


RectangleWrap calcRectangleSize(RectangleWrap pRectangle){

  pRectangle.lenght = sqrt(pow(pRectangle.point1.x - pRectangle.point0.x, 2) + pow(pRectangle.point1.y - pRectangle.point0.y, 2) + pow(pRectangle.point1.z - pRectangle.point0.z, 2));
  pRectangle.height = sqrt(pow(pRectangle.point3.x - pRectangle.point0.x, 2) + pow(pRectangle.point3.y - pRectangle.point0.y, 2) + pow(pRectangle.point3.z - pRectangle.point0.z, 2));

  pRectangle.vectorL.x = (pRectangle.point1.x - pRectangle.point0.x) / pRectangle.lenght;
  pRectangle.vectorL.y = (pRectangle.point1.y - pRectangle.point0.y) / pRectangle.lenght;
  pRectangle.vectorL.z = (pRectangle.point1.z - pRectangle.point0.z) / pRectangle.lenght;

  pRectangle.vectorH.x = (pRectangle.point3.x - pRectangle.point0.x) / pRectangle.height;
  pRectangle.vectorH.y = (pRectangle.point3.y - pRectangle.point0.y) / pRectangle.height;
  pRectangle.vectorH.z = (pRectangle.point3.z - pRectangle.point0.z) / pRectangle.height;

  return pRectangle;

}

Point calcPoint(long double pX, long double pY, Point pNormal, long double pD){
  Point newPoint = {pX, pY, -1 * (pNormal.x * pX + pNormal.y * pY + pD) / pNormal.z};
  return newPoint;
}




RectangleWrap wrapPolygon (void* pDetails){
  Polygon *poly = (Polygon*) pDetails;
  RectangleWrap rectangle;

  if(poly->amounPoints >= 3)
  {
    long double Xmin, Ymin, Xmax, Ymax;
    int index;

    if(poly->pointList[0].x > poly->pointList[1].x)
    {
      Xmax = poly->pointList[0].x;
      Xmin = poly->pointList[1].x;
    }else
    {
      Xmax = poly->pointList[1].x;
      Xmin = poly->pointList[0].x;
    }

    if(poly->pointList[0].y > poly->pointList[1].y)
    {
      Ymax = poly->pointList[0].y;
      Ymin = poly->pointList[1].y;
    }else
    {
      Ymax = poly->pointList[1].y;
      Ymin = poly->pointList[0].y;
    }


    for(index = 2; index < (poly->amounPoints - 1); index++)
    {
      if(poly->pointList[index].x > Xmax)
        Xmax = poly->pointList[index].x;
      else if (poly->pointList[index].x < Xmin)
        Xmin = poly->pointList[index].x;

      if(poly->pointList[index].y > Ymax)
        Ymax = poly->pointList[index].y;
      else if (poly->pointList[index].y < Ymin)
        Ymin = poly->pointList[index].y;

    }

    rectangle.point0 = calcPoint(Xmin, Ymin, poly -> Normal, poly -> D);
    rectangle.point1 = calcPoint(Xmax, Ymin, poly -> Normal, poly -> D);
    rectangle.point2 = calcPoint(Xmax, Ymax, poly -> Normal, poly -> D);
    rectangle.point3 = calcPoint(Xmin, Ymax, poly -> Normal, poly -> D);

    rectangle = calcRectangleSize(rectangle);

  }else{
    rectangle.point0.x = 0; rectangle.point0.y = 0; rectangle.point0.z = 0;
    rectangle.point1.x = 0; rectangle.point1.y = 0; rectangle.point1.z = 0;
    rectangle.point2.x = 0; rectangle.point2.y = 0; rectangle.point2.z = 0;
    rectangle.point3.x = 0; rectangle.point3.y = 0; rectangle.point3.z = 0;
  }


  return  rectangle;
}



RectangleWrap wrapDisc (void* pDetails)
{
  Disc *disk = (Disc*) pDetails;
  RectangleWrap rectangle;
  long double Xmin, Ymin, Xmax, Ymax;

  Ymin = disk->center.y - (disk -> radius + 1);
  Xmin = disk->center.x - (disk -> radius + 1);
  Ymax = disk->center.y + (disk -> radius + 1);
  Xmax = disk->center.x - (disk -> radius + 1);

  rectangle.point0 = calcPoint(Xmin, Ymin, disk -> Normal, disk -> D);
  rectangle.point1 = calcPoint(Xmax, Ymin, disk -> Normal, disk -> D);
  rectangle.point2 = calcPoint(Xmax, Ymax, disk -> Normal, disk -> D);
  rectangle.point3 = calcPoint(Xmin, Ymax, disk -> Normal, disk -> D);

  rectangle = calcRectangleSize(rectangle);

  return rectangle;
}




Plane * loadPlanes(FILE* file, int amountCutPlanes){
  Plane * list = (Plane*) calloc(amountCutPlanes,sizeof(Plane));
  printf("%d\n",amountCutPlanes);
  Point a;
  if (amountCutPlanes == 0) {
    printf("encontre un caso 0\n");
    return NULL;
  }
  int i = 0;
  while (i < amountCutPlanes ) {
    Plane plane;
    char * tok;
    char str[50] = {0};
    fgets(str,50,file);
    char * ba = str;
    tok = strtok_r(str,"/",&ba);
    //printf("%s\n",tok);
    a = loadAPoint(tok);
    plane.point0=a;
    //printf("%Lf\n",plane.point0.z);

    int flag = 0;
    int j =0;
    while( j < 2 ) {
        //printf("%d\n",j);
        if (flag == 0) {
          tok = strtok_r(ba,"/",&ba);
          a = loadAPoint(tok);
          plane.point1 = a;
          flag++;
        }
        if (flag == 1) {
          tok = strtok_r(ba,"/",&ba);
          //printf("Hago el segundo punto\n");
          a = loadAPoint(tok);
          plane.point2 = a;
          flag++;
        }
        j++;
     }
     Point vectorA = createVector(plane.point0,plane.point1);
     Point vectorB = createVector(plane.point2,plane.point1);
     Point AuxNormal = crossProduct(vectorA,vectorB);
     long double auxD = getD(AuxNormal,plane.point0);
     long double norm = normaPolygon(AuxNormal);
     plane.Normal.x = AuxNormal.x/norm;
     plane.Normal.y = AuxNormal.y/norm;
     plane.Normal.z = AuxNormal.z/norm;
     plane.D = auxD/norm;
     list[i] = plane;
     i++;

  }

  return (list);
}

Object createObject(FILE * file){
  //printf("aqui\n");
  char str[10] = {0};
  int count = 0;
  Object obj;
  while (count < 12) {
    fgets(str,10,file);
    //printf("%d\n",count);
    long double pivot = atof(str);
    if (count == 0) {
      obj.color.r = pivot;
    } else if (count == 1) {
      obj.color.g = pivot;
    } else if (count == 2) {
      obj.color.b = pivot;
    } else if (count == 3) {
      obj.kd = pivot;
    } else if (count == 4) {
      obj.ka = pivot;
    } else if (count == 5) {
      obj.kn = pivot;
    } else if (count == 6) {
      obj.ks = pivot;
    } else if (count == 7) {
      obj.o1 = pivot;
    } else if (count == 8) {
      obj.o2 = pivot;
    } else if (count == 9) {
      obj.o3 = pivot;
    } else if (count == 10) {
      //printf("Hola\n");
      obj.amountCutPlanes = pivot;
      obj.cutPlanesList = loadPlanes(file,obj.amountCutPlanes);
    } else if (count == 11) {
      obj.hasTexture = pivot;
      printf("pivot es %Lf\n",pivot);
      if (pivot == 1) {
        fgets(str,15,file);
        obj.texture = readAVS(str);
      }
    }

    count++;
 }
 return (obj);
}



Polygon* createPolygon(FILE * file){
  char str[20] = {0};
  int count = 0;
  Polygon * poly;
  poly = (Polygon*) calloc(1,sizeof(Polygon));
  //Point * list ;
  while (count < 3) {
    if (count == 0) {
      fgets(str,20,file);
      long double pivot = atof(str);
      poly->amounPoints = pivot;
      printf("son %Lf\n",pivot);
    } else if (count == 1) {
      int number = (int) poly->amounPoints;
      //printf(" numero de vertices %d\n",number);
      poly->pointList = loadPoints(number,file);
    }else if (count == 2) {
      fgets(str,20,file);
      long double pivot = atof(str);
      poly->hasFretWork = pivot;
      if (pivot == 1) {
        fgets(str,15,file);
        poly->fretWork = readAVS(str);
      }
    }
    count++;
  }

  //poly->pointList = list;
  Point pointZero = poly->pointList[0];
  Point pointOne = poly->pointList[1];
  Point pointTwo = poly->pointList[2];
  /* sacar A,B,C.D */
  Point vectorA = createVector(pointZero,pointOne);
  Point vectorB = createVector(pointOne,pointTwo);
  Point AuxNormal = crossProduct(vectorA,vectorB);
  long double auxD = getD(AuxNormal,pointZero);
  long double norm = normaPolygon(AuxNormal);
  poly->Normal.x = AuxNormal.x/norm;
  poly->Normal.y = AuxNormal.y/norm;
  poly->Normal.z = AuxNormal.z/norm;
  poly->D = auxD/norm;
  /*Sacar los flatPoints*/
  UV * listFPoints = loadFlatPoints(poly->amounPoints,poly->pointList,poly->Normal);
  poly->flatPoints = listFPoints;
  poly->wrap  = wrapPolygon(poly);


  return (poly);
}

Elip * createElip(FILE * file){
  char str[50] = {0};
  int count = 0;
  Elip * elip = (Elip*) calloc(1,sizeof(Elip));
  while (count < 4) {
    fgets(str,50,file);
    if (count == 0) {
      long double pivot = atof(str);
      elip->k = pivot;
    } else if (count == 1) {
      printf("%s\n",str);
      elip->p1 = loadAPoint(str);
    } else if (count == 2){
      elip->p2 = loadAPoint(str);
    } else if (count == 3) {
      elip->pA = loadAPoint(str);
    }
    count++;
  }
  Point vectorA = createVector(elip->p2,elip->p1);
  Point vectorB = createVector(elip->pA,elip->p1);
  Point AuxNormal = crossProduct(vectorA,vectorB);
  long double auxD = getD(AuxNormal,elip->p1);
  long double norm = normaPolygon(AuxNormal);
  elip->Normal.x = AuxNormal.x/norm;
  printf("%Lf\n",norm);
  elip->Normal.y = AuxNormal.y/norm;
  elip->Normal.z = AuxNormal.z/norm;
  elip->D = auxD/norm;

  return elip;
}


Disc * createDisc(FILE * file){
  char str[50] = {0};
  int count = 0;
  Disc * disc = (Disc*) calloc(1,sizeof(Disc));
  while (count < 4) {
    fgets(str,50,file);
    if (count == 0) {
      long double pivot = atof(str);
      disc->radius = pivot;
    } else if (count == 1) {
      printf("%s\n",str);
      disc->center = loadAPoint(str);
    } else if (count == 2){
      disc->point1 = loadAPoint(str);
    } else if (count == 3) {
      disc->point2 = loadAPoint(str);
    }
    count++;
  }
  Point vectorA = createVector(disc->point1,disc->center);
  Point vectorB = createVector(disc->point2,disc->center);
  Point AuxNormal = crossProduct(vectorA,vectorB);
  long double auxD = getD(AuxNormal,disc->center);
  long double norm = normaPolygon(AuxNormal);
  disc->Normal.x = AuxNormal.x/norm;
  printf("%Lf\n",norm);
  disc->Normal.y = AuxNormal.y/norm;
  disc->Normal.z = AuxNormal.z/norm;
  disc->D = auxD/norm;

  //disc->wrap  = wrapDisc(disc);

  return disc;
}

QuadraticSuperface* createQuad(FILE * file) {
  
  char str[10] = {0};
  int count = 0;
  QuadraticSuperface * quad;
  quad = (QuadraticSuperface*) calloc(2,sizeof(QuadraticSuperface));

  while (count < 13) {
    fgets(str,50,file);
    
    long double pivot = atof(str);
    if (count == 0) {
      quad->A = pivot;
    } else if (count == 1) {
      quad->B = pivot;
    } else if (count == 2) {
      quad->C = pivot;
    } else if (count == 3) {
      quad->D = pivot;
    } else if (count == 4) {
      quad->E = pivot;
    } else if (count == 5) {
      quad->F = pivot;
    } else if (count == 6) {
      quad->G = pivot;
    } else if (count == 7) {
      quad->H = pivot;
    } else if (count == 8) {
      quad->J = pivot;
    } else if (count == 9) {
      quad->K = pivot;
    } else if (count == 10) {
      quad->pTx = pivot;
    } else if (count == 11) {
      quad->pTy = pivot;
    } else if (count == 12) {
      quad->pTz = pivot;
    }
      count++;
  }
  return (quad);
}
