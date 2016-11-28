Point normalize(Point pVector);
long double calcDot(Point pVectorA, Point pVectorB);
Point calcNormalCylinder(Point pInterPoint, void* pDetails);

/*************************
		TEXTURAS
**************************/

Color getTexel(UV pPoint, Texture pTexture){
  return pTexture.RGB[llroundl(pPoint.U * pTexture.width)][llroundl(pPoint.V * pTexture.height)];
}

Color getTextureRGB(Object pObject, Point pIntersec){
	UV (*functionPoint) (void*, Point) = pObject.mapTexture;
	UV mapped = functionPoint (pObject.details, pIntersec);
  //printf("U %LF V %LF\n",mapped.U,mapped.V );
	return getTexel(mapped, pObject.texture);
}

UV mapRectangle (void *pDetails, Point pIntersec){

  Polygon* poly = (Polygon*) pDetails;
  RectangleWrap wrap = poly -> wrap;

  Point distance = {pIntersec.x - wrap.point0.x, pIntersec.y - wrap.point0.y, pIntersec.z - wrap.point0.z};

  UV newUV;
  newUV.U = calcDot(distance, wrap.vectorL) / wrap.lenght;
  newUV.V = calcDot(distance, wrap.vectorH) / wrap.height;

  return newUV;
}

UV mapCylinder (void *pDetails, Point pIntersec){
  Cylinder* cylinder = (Cylinder*) pDetails;

  Point distance = {pIntersec.x - cylinder->anchor.x, pIntersec.y - cylinder->anchor.y, pIntersec.z - cylinder->anchor.z};
  UV newUV;
 
  newUV.U = acos(calcDot(calcNormalCylinder(pIntersec, pDetails), cylinder->vectorG)) / (2 * PI);
  newUV.V = calcDot(distance, cylinder->direction) / cylinder->height;

  //Se calcula el plano para el lado oscuro
  Point Normal = crossProduct(cylinder->vectorG,cylinder->direction);
  long double coefD = getD(Normal,cylinder->anchor);

  if((Normal.x * pIntersec.x + Normal.y * pIntersec.y + Normal.z * pIntersec.z + coefD) < 0)
  	newUV.U = 1 - newUV.U;

  return newUV;
}


UV mapCone (void *pDetails, Point pIntersec){
  Cone* cone = (Cone*) pDetails;

  Point distance = {pIntersec.x - cone->anchor.x, pIntersec.y - cone->anchor.y, pIntersec.z - cone->anchor.z};
  UV newUV;

    Point distanceCenter = {pIntersec.x - cone->anchor.x, 
  						    pIntersec.y - cone->anchor.y, 
  						    pIntersec.z - cone->anchor.z};

  long double dotDQ = calcDot(distanceCenter, cone->direction);

  Point intersecPrime = {pIntersec.x + cone->direction.x * dotDQ,
  						 pIntersec.y + cone->direction.y * dotDQ,
						 pIntersec.z + cone->direction.z * dotDQ};
 
  newUV.U = acos(calcDot(calcNormalCylinder(intersecPrime, pDetails), cone->vectorG)) / (2 * PI);
  newUV.V = calcDot(distance, cone->direction) / cone->height;

  //Se calcula el plano para el lado oscuro
  Point Normal = crossProduct(cone->vectorG,cone->direction);
  long double coefD = getD(Normal,cone->anchor);

  if((Normal.x * pIntersec.x + Normal.y * pIntersec.y + Normal.z * pIntersec.z + coefD) < 0)
  	newUV.U = 1 - newUV.U;

  return newUV;
}

UV mapSphere (void *pDetails, Point pIntersec){
  Sphere* sphere = (Sphere*) pDetails;

  Point distanceCenter = {pIntersec.x - sphere->center.x, 
  						  pIntersec.y - sphere->center.y, 
  						  pIntersec.z - sphere->center.z};

  long double dotDN = calcDot(distanceCenter, sphere->vectorNorth);

  Point intersecPrime = {pIntersec.x - sphere->vectorNorth.x * dotDN,
  						 pIntersec.y - sphere->vectorNorth.y * dotDN,
  						 pIntersec.z - sphere->vectorNorth.z * dotDN};

  Point newNormal = {intersecPrime.x - sphere->center.x,
				  	 intersecPrime.y - sphere->center.y,
				  	 intersecPrime.z - sphere->center.z};
  newNormal = normalize(newNormal);

  Point distanceSouth = {pIntersec.x - sphere->pointSouth.x, 
  					     pIntersec.y - sphere->pointSouth.y, 
  					     pIntersec.z - sphere->pointSouth.z};
  UV newUV;
  newUV.U = acos(calcDot(newNormal, sphere->vectorG)) / (2 * PI);
  newUV.V = calcDot(distanceSouth, sphere->vectorNorth)/ (2 * sphere->radius);

  //Se calcula el plano para el lado oscuro
  Point Normal = crossProduct(sphere->vectorG,sphere->vectorNorth);
  long double coefD = getD(Normal,sphere->center);

  if((Normal.x * pIntersec.x + Normal.y * pIntersec.y + Normal.z * pIntersec.z + coefD) < 0)
  	newUV.U = 1 - newUV.U;

  return newUV;
}