//gcc proyecto2.c -lm -o pro3


#include "proyecto2.h"
#include "image.h"
#include "lectura.h"
#include "texturas.h"
#include "Interseccion-Normal.h"




/*		Variables globales		*/
Color **FrameBuffer;
Color BackColor;	//Color del fondo
Window UniWindow;	//Ventana universal
Scene UniScene; 	//Escena

/*		Prototipos		*/
void rayTracer();
QuadraticSuperface moveQuadSuper(QuadraticSuperface pQuadSuper, long double pTx, long double pTy, long double pTz);


void iniGlobalVar(FILE* file, FILE* fileScene) {

	//Fondo
	BackColor.r = 0.3;
	BackColor.g = 0.3;
	BackColor.b = 0.3;


	//Ventana universal
	UniWindow.Xmax =  800;
	UniWindow.Ymax =  600;
	UniWindow.Xmin =  0;
	UniWindow.Ymin =  100;


while (feof(fileScene) == 0) {
	char str[10] = {0};
	fgets(str,100,fileScene);
	if (strcmp(str,"sce{\n") == 0) {
		UniScene= createScene(fileScene);

	}
}

rewind(fileScene);
Light * liList;
liList = (Light*) calloc(UniScene.amountLights,sizeof(Light));
int count = 0;
while (feof(fileScene) == 0) {
	char str[10] = {0};
	fgets(str,100,fileScene);
	if (strcmp(str,"lig{\n") == 0) {
		Light light;
		light = createLigh(fileScene);
		liList[count] = light;
		count ++;

	}
}
UniScene.lightList = liList;

Object * list;
list = (Object *) calloc(UniScene.amountObjects,sizeof(Object));

count = 0;
while (feof(file) == 0) {
	char str[10] = {0};
	fgets(str,100,file);
	if (strcmp(str,"sph{\n")==0) {
		//printf("Intento crear una esfera\n");
		Object objSphere;
		objSphere = createObject(file);
		objSphere.type = 1;
		Sphere * sphere;
		sphere = createSphere(file);


		objSphere.details = sphere;
		objSphere.IntersectionFunction = &calcInterSphere;
		objSphere.NormalFunction = &calcNormalSphere;
		objSphere.mapTexture = &mapSphere;

		list[count] = objSphere;
		count++;
	} else if (strcmp(str,"poly{\n")==0) {
		//printf("Intento crear un poligono\n");
		Object objPolygon;
		objPolygon = createObject(file);
		objPolygon.type = 2;
		Polygon * polygon;
		polygon = createPolygon(file);


		objPolygon.details = polygon;
		objPolygon.IntersectionFunction = &calcInterPoly;
		objPolygon.NormalFunction = &calcNormalPoly;
		objPolygon.mapTexture = &mapRectangle;

		list[count] = objPolygon;
		count++;
	}else if (strcmp(str,"cylin{\n")==0) {
		Object objCyl;
		objCyl = createObject(file);
		objCyl.type = 3;
		Cylinder * cylinder;
		cylinder = createCylinder(file);

		objCyl.details = cylinder;
		objCyl.IntersectionFunction = &calcInterCylinder;
		objCyl.NormalFunction = &calcNormalCylinder;
		objCyl.mapTexture = &mapCylinder;

		list[count] = objCyl;
		count++;
	} else if (strcmp(str,"cone{\n")==0) {
			//printf("Intento crear un poligono\n");
			Object objCone;
			objCone = createObject(file);
			objCone.type = 4;
			Cone * cone;
			cone = createCone(file);

			objCone.details = cone;
			objCone.IntersectionFunction = &calcInterCone;
			objCone.NormalFunction = &calcNormalCone;
			objCone.mapTexture = &mapCone;

			list[count] = objCone;
			count++;
	} else if (strcmp(str,"disc{\n")==0) {
			printf("Intento crear un disco\n");
			Object objDisc;
			objDisc = createObject(file);
			objDisc.type = 5;
			Disc * disc;
			disc = createDisc(file);
			objDisc.details = disc;
			objDisc.IntersectionFunction = &calcInterDisc;
			objDisc.NormalFunction = &calcNormalDisc;
			objDisc.mapTexture = &mapRectangle;
			list[count] = objDisc;
			count++;
 } else if (strcmp(str,"elip{\n")==0) {
		 //printf("Intento crear un disco\n");
		 Object objElip;
		 objElip = createObject(file);
		 objElip.type = 6;
		 Elip * elip;
		 elip = createElip(file);
		 objElip.details = elip;
		 objElip.IntersectionFunction = &calcInterElip;
		 objElip.NormalFunction = &calcNormalElip;
		 list[count] = objElip;
		 count++;
 } else if (strcmp(str,"quad{\n")==0) {
		 //printf("Intento crear un disco\n");
		 Object objQuad;
		 objQuad = createObject(file);
		 objQuad.type = 7;
		 QuadraticSuperface *quad = createQuad(file);
		 quad[0] = moveQuadSuper(quad[0], quad->pTx, quad->pTy, quad->pTz);
		 objQuad.details = quad;

		 objQuad.IntersectionFunction = &calcInterQuadSuper;
		 objQuad.NormalFunction = &calcNormalQuadSuper;

		 
		 list[count] = objQuad;
		 count++;
 }
}


UniScene.objectList = list;

	//Incializa el buffer
	int column;

  	FrameBuffer = (Color **)malloc((HORIZONTALRES+1) * sizeof(Color*));
 	for (column = 0; column < HORIZONTALRES; column++)
    {
        FrameBuffer[column] = (Color *)malloc((VERTICALRES+1) * sizeof(Color));
    }


}

int main(int argc, char *argv[])
{

	FILE * file;
	FILE * fileScene;

	file = fopen(argv[1],"r");
	fileScene = fopen(argv[2],"r");

  if (file == NULL){
    printf("No se puedo abrir el archivo de los objetos correctamente\n");
    return -1;
  }
  if (fileScene == NULL){
    printf("No se puedo abrir el archivo de la escena correctamente\n");
    return -1;
  }
	iniGlobalVar(file,fileScene);

	rayTracer();


	free(UniScene.lightList);
	free(UniScene.objectList);
}

/*  *****************
        NUEVO
*****************  */
Point mapBufferWindow(long double pX, long double pY)
{
	Point newPoint;
	newPoint.x = ( pX * (UniWindow.Xmax - UniWindow.Xmin) / HORIZONTALRES ) + UniWindow.Xmin;
	newPoint.y = ( pY * (UniWindow.Ymax - UniWindow.Ymin) / VERTICALRES ) + UniWindow.Ymin;
	newPoint.z = 0.0;

	return newPoint;
}

Point normalize(Point pVector)
{
	long double norm = sqrt( pow(pVector.x, 2) + pow(pVector.y, 2) + pow(pVector.z,2) );
	pVector.x = pVector.x / norm;
	pVector.y = pVector.y / norm;
	pVector.z = pVector.z / norm;

	return pVector;
}

long double calcDot(Point pVectorA, Point pVectorB)
{
	return pVectorA.x * pVectorB.x + pVectorA.y * pVectorB.y + pVectorA.z * pVectorB.z;
}

Point calcReflexVecor(Point pVector, Point vectorN) {
		Point vectorR;
		pVector.x = -1 * pVector.x;
		pVector.y = -1 * pVector.y;
		pVector.z = -1 * pVector.z;
		long double dotPoint = calcDot (vectorN,pVector);
		vectorR.x = 2 * vectorN.x * dotPoint - pVector.x;
		vectorR.y = 2 * vectorN.y * dotPoint - pVector.y;
		vectorR.z = 2 * vectorN.z * dotPoint - pVector.z;
		return vectorR;

}
Intersection firstIntersection(Point pAnchor, Point pVector)
{
	Intersection newIntersection, lastIntersection;
	long double tMin;
	int index, firstIntersectionn = 1;

	newIntersection.vacio = 1;
	lastIntersection.vacio = 1;

	for (index = 0; index < UniScene.amountObjects; ++index)
	{

		//printf(" Color: %LF\n",UniScene.objectList[index].color.g );
		lastIntersection = calculateIntersection(UniScene.objectList[index], pAnchor, pVector);

		if(lastIntersection.distance > EPSILON)
		{
			if(firstIntersectionn == 1)
			{
				newIntersection = lastIntersection;
				tMin = newIntersection.distance;
				firstIntersectionn = 0;
			}else if(lastIntersection.distance < tMin)
			{
				newIntersection = lastIntersection;
				tMin = newIntersection.distance;
			}
		}

	}

	if (newIntersection.vacio == 0) {
			int i;
			int flag = 1;
			for (i = 0; i < newIntersection.object.amountCutPlanes; i++) {
				long double a = newIntersection.object.cutPlanesList[i].Normal.x;
				long double b = newIntersection.object.cutPlanesList[i].Normal.y;
				long double c = newIntersection.object.cutPlanesList[i].Normal.z;
				long double d = newIntersection.object.cutPlanesList[i].D;
				long double xi = newIntersection.interPoint.x;
				long double yi = newIntersection.interPoint.y;
				long double zi = newIntersection.interPoint.z;
				long double test = (a*xi)+(b*yi)+(c*zi)+d;
				if (test > 0) {
					//printf("Existe\n");
					flag = 0;
				}
			}
			if (flag == 0) {
				newIntersection = firstIntersection(newIntersection.interPoint,pVector);
			}
	}

	return newIntersection;
}

long double calcLightDistance(Point pInterPoint, Point pPosition)
{
	return sqrt( pow(pPosition.x - pInterPoint.x, 2) + pow(pPosition.y - pInterPoint.y, 2) + pow(pPosition.z - pInterPoint.z, 2));
}

Point calcVectorL(Point pInterPoint, Point pPosition)
{
	long double dL = calcLightDistance(pInterPoint, pPosition);
	Point vectorL;
	vectorL.x = (pPosition.x - pInterPoint.x) / dL;
	vectorL.y = (pPosition.y - pInterPoint.y) / dL;
	vectorL.z = (pPosition.z - pInterPoint.z) / dL;

	return vectorL;
}



long double calcFatt(Point pInterPoint, Light pLight)
{
	long double dL = calcLightDistance(pInterPoint, pLight.position);
	long double Fatt = 1 / (pLight.c1 + pLight.c2 * dL + pLight.c3 * pow(dL, 2));

	if(Fatt > 1.0)
		return 1.0;
	else
		return Fatt;
}

Point calcVectorR(Point pVectorN, Point pVectorL, long double pNLDot)
{
	Point vectorR;
	vectorR.x = 2 * (pVectorN.x * pNLDot) - pVectorL.x;
	vectorR.y = 2 * (pVectorN.y * pNLDot) - pVectorL.y;
	vectorR.z = 2 * (pVectorN.z * pNLDot) - pVectorL.z;

	return vectorR;
}

Color getColor(Point pAnchor, Point pVector, int levelOfReflex, int levelOfTrans)
{
	Color newColor;
	Color reflexColor;
	Color tranparencyColor;
	Intersection newIntersection;
	newIntersection.vacio = 1;

	newIntersection = firstIntersection(pAnchor, pVector);

	if(newIntersection.vacio == 1)
		newColor = BackColor;
	else
	{
		Intersection qIntersec = newIntersection;

		Point vectorN = calculateNormal(qIntersec.object, qIntersec.interPoint);

		if(calcDot(vectorN,pVector) > EPSILON)
		{
			vectorN.x = -1 * vectorN.x;
			vectorN.y = -1 * vectorN.y;
			vectorN.z = -1 * vectorN.z;
		}

		Point vectorV = {(-1 * pVector.x), (-1 * pVector.y), (-1 * pVector.z)};

		long double intensity = 0.0;
		long double specular = 0.0;


		int index;

		for (index = 0; index < UniScene.amountLights; ++index)
		{
			Point vectorL = calcVectorL(qIntersec.interPoint, UniScene.lightList[index].position);

			long double NLdot = calcDot(vectorN, vectorL);

			if(NLdot > EPSILON)
			{

				Intersection obstacle = firstIntersection(qIntersec.interPoint, vectorL);

				if (obstacle.vacio == 1 || obstacle.distance > calcLightDistance(qIntersec.interPoint, UniScene.lightList[index].position) )
				{

					long double Fatt = calcFatt(qIntersec.interPoint, UniScene.lightList[index]);

					intensity = intensity + (NLdot * qIntersec.object.kd * Fatt * UniScene.lightList[index].intensity);


					Point vectorR = calcVectorR(vectorN, vectorL, NLdot);

					long double VRDot = calcDot(vectorV, vectorR);

					if(pow(VRDot, qIntersec.object.kn) > EPSILON)
					{
						specular = specular + (pow(VRDot, qIntersec.object.kn) * qIntersec.object.ks * Fatt * UniScene.lightList[index].intensity);
					}

				}

			}
		}

		intensity = intensity + (UniScene.Ia * qIntersec.object.ka);

		if(intensity > 1.0)
			intensity = 1.0;
		if( specular > 1.0)
			specular = 1.0;

		if (qIntersec.object.hasTexture == 1)
		{
			Color textureColor = getTextureRGB(qIntersec.object, qIntersec.interPoint);

			newColor.r = textureColor.r * intensity;
			newColor.b = textureColor.b * intensity;
			newColor.g = textureColor.g * intensity;

		}else
		{
			newColor.r = qIntersec.object.color.r * intensity;
			newColor.b = qIntersec.object.color.b * intensity;
			newColor.g = qIntersec.object.color.g * intensity;

		}

		if (levelOfReflex > 0 ) {
			Point refVector = calcReflexVecor(pVector, vectorN);
			reflexColor = getColor(qIntersec.interPoint,refVector,levelOfReflex-1,0);
			newColor.r = newColor.r * qIntersec.object.o1 + reflexColor.r * qIntersec.object.o2;
			newColor.g = newColor.g * qIntersec.object.o1 + reflexColor.g * qIntersec.object.o2;
			newColor.b = newColor.b * qIntersec.object.o1 + reflexColor.b * qIntersec.object.o2;

    }

		if (levelOfTrans > 0 && qIntersec.object.o3 != 0) {
			//printf("Intento tranparencyColor\n");
			tranparencyColor = getColor(qIntersec.interPoint,pVector,0,levelOfTrans-1);
			newColor.r = newColor.r + tranparencyColor.r * qIntersec.object.o3;
			newColor.g = newColor.g + tranparencyColor.g * qIntersec.object.o3;
			newColor.g = newColor.b + tranparencyColor.b * qIntersec.object.o3;
		}


		newColor.r = newColor.r + specular * (1.0 - newColor.r);
		newColor.g = newColor.g + specular * (1.0 - newColor.g);
		newColor.b = newColor.b + specular * (1.0 - newColor.b);


	}

	return newColor;

}

Color throwRay(long double pX, long double pY)
{
	Point windowPoint = mapBufferWindow(pX, pY);

	Point dVector;
	dVector.x = windowPoint.x - UniScene.eye.x;
	dVector.y = windowPoint.y - UniScene.eye.y;
	dVector.z = windowPoint.z - UniScene.eye.z;

	dVector = normalize(dVector);

	return getColor(UniScene.eye, dVector,UniScene.levelOfReflex,UniScene.levelOfTrans);
}

long double getDitanceDif(Color pColorA, Color pColorB)
{
	return sqrt(pow(pColorA.r - pColorB.r, 2.0) + pow(pColorA.g - pColorB.g, 2.0) + pow(pColorA.b - pColorB.b, 2.0) );
}

Color antiAlaising(int pX, int pY, int pLevels)
{
	long double move = pLevels / MAXLANTIALIASING;

	Color rayNoth = throwRay(pX + move / 2.0, pY + move);
	Color raySouth = throwRay(pX + move / 2.0, pY);
	Color rayEast = throwRay(pX + move, pY + move / 2.0);
	Color rayWest = throwRay(pX, pY + move / 2.0);

	long double difNS = getDitanceDif(rayNoth, raySouth);
	long double difNE = getDitanceDif(rayNoth, rayEast);
	long double difNW = getDitanceDif(rayNoth, rayWest);
	long double difSE = getDitanceDif(raySouth, rayEast);
	long double difSW = getDitanceDif(raySouth, rayWest);
	long double difEW = getDitanceDif(rayEast, rayWest);

	if ((difNS + difNE + difNW + difSE + difSW + difEW) / 6.0 < 0.866025 && pLevels > 1)
	{
		pLevels--;
		move = pLevels / MAXLANTIALIASING;

		rayNoth = antiAlaising(pX + move / 2.0, pY + move / 2.0, pLevels);
		raySouth = antiAlaising(pX, pY, pLevels);
		rayEast = antiAlaising(pX + move / 2.0, pY, pLevels);
		rayWest = antiAlaising(pX, pY + move / 2.0, pLevels);
	}

	Color newColor = {(rayNoth.r + raySouth.r + rayEast.r + rayWest.r) / 4.0,
					  (rayNoth.g + raySouth.g + rayEast.g + rayWest.g) / 4.0,
					  (rayNoth.b + raySouth.b + rayEast.b + rayWest.b) / 4.0};
	return newColor;

}


void rayTracer()
{
	int row, column;

	for (row = 0; row < HORIZONTALRES; ++row)
	{
		for (column = 0; column < VERTICALRES; ++column)
		{

			Color newColor = antiAlaising(row, column, MAXLANTIALIASING);

			FrameBuffer[row][column] = newColor;

		}
	}

	writeAVS(FrameBuffer, "Imagen.avs", VERTICALRES, HORIZONTALRES);

}


/***************************************
		Superficies cuadraticas
***************************************/

QuadraticSuperface moveQuadSuper(QuadraticSuperface pQuadSuper, long double pTx, long double pTy, long double pTz)
{

	QuadraticSuperface newQuadSuper =
	{
		pQuadSuper.A, pQuadSuper.B, pQuadSuper.C, pQuadSuper.D, pQuadSuper.E, pQuadSuper.F,
		pQuadSuper.G - pQuadSuper.A * pTx - pQuadSuper.D * pTy - pQuadSuper.F * pTz,
		pQuadSuper.H - pQuadSuper.D * pTx - pQuadSuper.B * pTy - pQuadSuper.E * pTz,
		pQuadSuper.J - pQuadSuper.F * pTx - pQuadSuper.E * pTy - pQuadSuper.C * pTz,
		pQuadSuper.K - 2 * (pQuadSuper.G * pTx + pQuadSuper.H * pTy + pQuadSuper.J * pTz) +
					pTx * (pQuadSuper.A * pTx + pQuadSuper.D * pTy + pQuadSuper.F * pTz) +
					pTy * (pQuadSuper.D * pTx + pQuadSuper.B * pTy + pQuadSuper.E * pTz) +
					pTz * (pQuadSuper.F * pTx + pQuadSuper.E * pTy + pQuadSuper.C * pTz)
	};

	return newQuadSuper;
}
