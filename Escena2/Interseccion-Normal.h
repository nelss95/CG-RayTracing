/*******************************
		Intersecciones
*******************************/


UV createUVIntersection(Point point, Polygon poly) {
	UV uv;
	long double max,x,y,z;
	x = fabs(poly.Normal.x);
	y = fabs(poly.Normal.y);
	z = fabs(poly.Normal.z);
	max = x;
	if (y > max) {
		 max = y;
	}
	if (z > max) {
		max = z;
	}

	if (max == x) {

  	uv.U = point.y;
		uv.V = point.z;
  } else if (max == y) {

		uv.U = point.x;
		uv.V = point.z;

  } else if (max == z){

		uv.U = point.x;
		uv.V = point.y;
  }
	return uv;
}

int rayPoly(UV intersection, UV*flatPoints, int amounPoints){

	int i;
	UV * auxFlat = (UV*) calloc(amounPoints,sizeof(UV));
	for (i = 0; i < amounPoints; i++) {
		auxFlat[i].U = flatPoints[i].U - intersection.U;
		auxFlat[i].V = flatPoints[i].V - intersection.V;

	}

	int wallCounter = 0;
	int a;
	for ( a = 0; a < amounPoints; a++) {

		int b = (a+1)%amounPoints;
		if ((auxFlat[a].V > 0 && auxFlat[b].V < 0) || (auxFlat[a].V < 0 && auxFlat[b].V > 0)) {
			if (auxFlat[a].U > 0 && auxFlat[b].U > 0) {
				wallCounter++;
			} else if (auxFlat[a].U > 0 || auxFlat[b].U > 0){
					if ((auxFlat[a].U -auxFlat[a].V * (auxFlat[b].U - auxFlat[a].U)/(auxFlat[b].V-auxFlat[a].V)) >= 0) {
						wallCounter++;
					}
			}
		}
	}

	free (auxFlat);
	return (wallCounter%2);
}

Intersection calculateIntersection (Object pObject, Point pAnchor, Point pVector) {
	/*		Calcula la primera interseccion del objeto		*/
	long double (*functionPoint)(Point,Point,void*) = pObject.IntersectionFunction;
	long double t = functionPoint (pAnchor,pVector,pObject.details);
	int flagIntersection = 1;

	Intersection newIntersection;
	newIntersection.object = pObject;
	newIntersection.distance = t;
	newIntersection.vacio = 0;

	newIntersection.interPoint.x = pAnchor.x + t * pVector.x;
	newIntersection.interPoint.y = pAnchor.y + t * pVector.y;
	newIntersection.interPoint.z = pAnchor.z + t * pVector.z;


  	if (newIntersection.object.type == 2)
  	{

		Polygon poly = *(Polygon*)pObject.details;
		UV intersection = createUVIntersection(newIntersection.interPoint,poly);
		int flag = rayPoly(intersection,poly.flatPoints,poly.amounPoints);

		if (flag == 1)
		{

			if(poly.hasFretWork == 1 && newIntersection.distance != -1)
			{
				UV mapped = mapRectangle (pObject.details, newIntersection.interPoint);
		  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
				Color fretColor = poly.fretWork.RGB[llroundl(mapped.U * poly.fretWork.width)][llroundl(mapped.V * poly.fretWork.height)];

				if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
				{
					newIntersection.vacio = 1;
					newIntersection.distance = -1;
				}

			}
			return newIntersection;
		} else
		{
			newIntersection.vacio = 1;
			newIntersection.distance = -1;
			return newIntersection;
		}
	} else if (newIntersection.object.type == 5) {
		//printf("Veo el disco\n");
		Disc disc = *(Disc*)pObject.details;
		long double a = pow((newIntersection.interPoint.x - disc.center.x),2);
		long double b = pow((newIntersection.interPoint.y - disc.center.y),2);
		long double c = pow((newIntersection.interPoint.z - disc.center.z),2);
		long double result = a+b+c-pow(disc.radius,2);
		//printf("a %Lf\n",a);
		if (result <= EPSILON) {
			 //printf("disco\n");
			 return newIntersection;
		} else {
			newIntersection.vacio = 1;
			newIntersection.distance = -1;
			return newIntersection;
		}
	} else if (newIntersection.object.type == 6) {
		//printf("Veo el disco\n");
		Elip elip = *(Elip*)pObject.details;
		long double d1 = sqrt(pow(newIntersection.interPoint.x-elip.p1.x,2)+pow(newIntersection.interPoint.y-elip.p1.y,2)+pow(newIntersection.interPoint.z-elip.p1.z,2));
		long double d2 = sqrt(pow(newIntersection.interPoint.x-elip.p2.x,2)+pow(newIntersection.interPoint.y-elip.p2.y,2)+pow(newIntersection.interPoint.z-elip.p2.z,2));
		long double result = d1 + d2;
		//printf("a %Lf\n",a);
		if (result <= elip.k) {
			 //printf("disco\n");
			 return newIntersection;
		} else {
			newIntersection.vacio = 1;
			newIntersection.distance = -1;
			return newIntersection;
		}
	} else {
		return newIntersection;
	}

}

long double calcInterPoly(Point pAnchor, Point pVector, void* pDetails){
	Polygon polygonDetails = *(Polygon*)pDetails;
	long double t;
	long double denominador = (polygonDetails.Normal.x * pVector.x) + (polygonDetails.Normal.y * pVector.y) + (polygonDetails.Normal.z * pVector.z);
	if (denominador == EPSILON || denominador == 0) {
		return -1;
	} else {
		long double numerador = -((polygonDetails.Normal.x * pAnchor.x) + (polygonDetails.Normal.y * pAnchor.y) + (polygonDetails.Normal.z * pAnchor.z) + polygonDetails.D);
		t = numerador/denominador;
	}
	return t;
}


long double calcInterSphere(Point pAnchor, Point pVector, void* pDetails){

	Sphere *sphereDetails = (Sphere*) pDetails;
	Point center = sphereDetails->center;

	long double t;


	long double b = 2 * pVector.x * (pAnchor.x - center.x) + 2 * pVector.y * (pAnchor.y - center.y) + 2 * pVector.z * (pAnchor.z - center.z);
	long double c = pow(pAnchor.x - center.x, 2) + pow(pAnchor.y - center.y, 2) + pow(pAnchor.z - center.z, 2) - pow(sphereDetails[0].radius, 2);

	long double delta = pow(b, 2) - 4 * c;

	if(delta > EPSILON)
	{
		long double t1 = (-1 * b - sqrt(delta) ) / 2;
		long double t2 = (-1 * b + sqrt(delta) ) / 2;

		if (t1 > EPSILON && t2 > EPSILON){

			if(sphereDetails->hasFretWork == 1)
			{
				Point interseccion = {pAnchor.x + t1 * pVector.x, pAnchor.y + t1 * pVector.y, pAnchor.z + t1 * pVector.z};
				UV mapped = mapSphere (pDetails, interseccion);
		  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
				Color fretColor = sphereDetails->fretWork.RGB[llroundl(mapped.U * sphereDetails->fretWork.width)][llroundl(mapped.V * sphereDetails->fretWork.height)];

				if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
				{
				 	t = t2;
				}else
					return t1;

			}else
				return t1;
		}
		else if (t1 < EPSILON && t2 > EPSILON)
			t = t2;
		else
			return -1;

	}else if( delta < EPSILON && delta > (-1 * EPSILON) )
		t = -1 * b / 2 ;

	else
		return -1;


	if(sphereDetails->hasFretWork == 1 && t != -1)
	{
		Point interseccion = {pAnchor.x + t * pVector.x, pAnchor.y + t * pVector.y, pAnchor.z + t * pVector.z};

		UV mapped = mapSphere (pDetails, interseccion);
  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
		Color fretColor = sphereDetails->fretWork.RGB[llroundl(mapped.U * sphereDetails->fretWork.width)][llroundl(mapped.V * sphereDetails->fretWork.height)];

		if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
		 	t = -1;

	}

	return t;
}

int finiteCylinder (Point pAnchor, Point pVector, long double pT, Cylinder pCylinder)
{
	Point interseccion = {pAnchor.x + pT * pVector.x, pAnchor.y + pT * pVector.y, pAnchor.z + pT * pVector.z};

	Point vector = {interseccion.x - pCylinder.anchor.x, interseccion.y - pCylinder.anchor.y, interseccion.z - pCylinder.anchor.z};

	long double distance = calcDot(vector, pCylinder.direction);

	if (pCylinder.minSize < distance && distance < pCylinder.maxSize)
		return 1;
	else
		return 0;
}


long double calcInterCylinder(Point pAnchor, Point pVector, void* pDetails){

	Cylinder *cylinderDetails = (Cylinder*) pDetails;
	Point anchor = cylinderDetails->anchor;
	Point direction = cylinderDetails->direction;

	long double t;

	long double preCalcA = pVector.x * direction.x + pVector.y * direction.y + pVector.z * direction.z;
	long double preCalcB = (pAnchor.x - anchor.x) * direction.x + (pAnchor.y - anchor.y) * direction.y + (pAnchor.z - anchor.z) * direction.z;

	long double a = pow(preCalcA * direction.x - pVector.x, 2) + pow(preCalcA * direction.y - pVector.y, 2) + pow(preCalcA * direction.z - pVector.z, 2);
	long double b = 2.0 * ( ( preCalcA * direction.x - pVector.x) * ( preCalcB * direction.x - pAnchor.x + anchor.x) +
						( preCalcA * direction.y - pVector.y) * ( preCalcB * direction.y - pAnchor.y + anchor.y) +
						( preCalcA * direction.z - pVector.z) * ( preCalcB * direction.z - pAnchor.z + anchor.z) );
	long double c = pow(preCalcB * direction.x - pAnchor.x + anchor.x, 2) + pow(preCalcB * direction.y - pAnchor.y + anchor.y, 2) + pow(preCalcB * direction.z - pAnchor.z + anchor.z, 2)  - pow(cylinderDetails[0].radius, 2);

	long double delta = pow(b, 2) - 4.0 * a * c;

	if (delta > EPSILON)
	{
		long double t1 = (-1.0 * b - sqrt(delta) ) / (2.0 * a);
		long double t2 = (-1.0 * b + sqrt(delta) ) / (2.0 * a);

		if (t1 > EPSILON && t2 > EPSILON)
		{
			/* 	Verifica que se encuentre dentro del cilindro finito 	*/
			if (finiteCylinder(pAnchor, pVector, t1, cylinderDetails[0]) == 1){
				if(cylinderDetails->hasFretWork == 1)
				{
					Point interseccion = {pAnchor.x + t1 * pVector.x, pAnchor.y + t1 * pVector.y, pAnchor.z + t1 * pVector.z};
					UV mapped = mapCylinder (pDetails, interseccion);
			  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
					Color fretColor = cylinderDetails->fretWork.RGB[llroundl(mapped.U * cylinderDetails->fretWork.width)][llroundl(mapped.V * cylinderDetails->fretWork.height)];

					if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
					{
					 	t = t2;
					}else
						return t1;

				}else
					return t1;
			}else
				t = t2;

		}
		else if (t1 < EPSILON && t2 > EPSILON)
		{
			t = t2;
		}
		else
			return -1;

	}
	else if( delta < EPSILON && delta > (-1 * EPSILON) )
	{
		t = -1 * b / (2 * a);
	}
	else
		return -1;

	if (finiteCylinder(pAnchor, pVector, t, cylinderDetails[0]) == 1)
	{
		if(cylinderDetails->hasFretWork == 1 && t != -1)
		{
			Point interseccion = {pAnchor.x + t * pVector.x, pAnchor.y + t * pVector.y, pAnchor.z + t * pVector.z};

			UV mapped = mapCylinder (pDetails, interseccion);
	  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
			Color fretColor = cylinderDetails->fretWork.RGB[llroundl(mapped.U * cylinderDetails->fretWork.width)][llroundl(mapped.V * cylinderDetails->fretWork.height)];

			if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
			 	t = -1;

		}
		return t;
	}
	else
		return -1;
}

int finiteCone (Point pAnchor, Point pVector, long double pT, Cone pCone)
{
	Point interseccion = {pAnchor.x + pT * pVector.x, pAnchor.y + pT * pVector.y, pAnchor.z + pT * pVector.z};

	Point vector = {interseccion.x - pCone.anchor.x, interseccion.y - pCone.anchor.y, interseccion.z - pCone.anchor.z};

	long double distance = calcDot(vector, pCone.direction);

	if (pCone.minSize < distance && distance < pCone.maxSize)
		return 1;
	else
		return 0;
}

long double calcInterCone(Point pAnchor, Point pVector, void* pDetails){

	long double t;

	Cone *coneDetails = (Cone*) pDetails;
	Point anchor = coneDetails[0].anchor;
	Point direction = coneDetails[0].direction;
	long double k1 = coneDetails[0].k1;
	long double k2 = coneDetails[0].k2;

	long double preCalcA = pVector.x * direction.x + pVector.y * direction.y + pVector.z * direction.z;
	long double preCalcB = (pAnchor.x - anchor.x) * direction.x + (pAnchor.y - anchor.y) * direction.y + (pAnchor.z - anchor.z) * direction.z;

	long double a =pow(preCalcA * direction.x - pVector.x, 2) + pow(preCalcA * direction.y - pVector.y, 2) + pow(preCalcA * direction.z - pVector.z, 2) - pow((k2/k1) * preCalcA,2 );

	long double b = 2.0 * ( ( preCalcA * direction.x - pVector.x) * ( preCalcB * direction.x - pAnchor.x + anchor.x) +
						( preCalcA * direction.y - pVector.y) * ( preCalcB * direction.y - pAnchor.y + anchor.y) +
						( preCalcA * direction.z - pVector.z) * ( preCalcB * direction.z - pAnchor.z + anchor.z) -
						(pow(k2/k1,2) * preCalcA * preCalcB) );

	long double c =  pow(preCalcB * direction.x - pAnchor.x + anchor.x, 2) + pow(preCalcB * direction.y - pAnchor.y + anchor.y, 2) + pow(preCalcB * direction.z - pAnchor.z + anchor.z, 2)
					- pow((k2/k1) * preCalcB, 2);

	long double delta = pow(b, 2) - 4.0 * a * c;

	if (delta > EPSILON)
	{
		long double t1 = (-1.0 * b - sqrt(delta) ) / (2.0 * a);
		long double t2 = (-1.0 * b + sqrt(delta) ) / (2.0 * a);

		if (t1 > EPSILON && t2 > EPSILON)
		{
			/* 	Verifica que se encuentre dentro del cilindro finito 	*/
			if (finiteCone(pAnchor, pVector, t1, coneDetails[0]) == 1)
			{
				if(coneDetails->hasFretWork == 1)
				{
					Point interseccion = {pAnchor.x + t1 * pVector.x, pAnchor.y + t1 * pVector.y, pAnchor.z + t1 * pVector.z};

					UV mapped = mapCone (pDetails, interseccion);
			  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
					Color fretColor = coneDetails->fretWork.RGB[llroundl(mapped.U * coneDetails->fretWork.width)][llroundl(mapped.V * coneDetails->fretWork.height)];

					if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
					 	t = t2;
					else
						return t1;

				}else
					return t1;
			}
			else
				t = t2;

		}
		else if (t1 < EPSILON && t2 > EPSILON)
		{
			t = t2;
		}
		else
			return -1;

	}
	else if( delta < EPSILON && delta > (-1 * EPSILON) )
	{
		t = -1 * b / (2 * a);
	}
	else
		return -1;

	if (finiteCone(pAnchor, pVector, t, coneDetails[0]) == 1)
	{
		if(coneDetails->hasFretWork == 1 && t != -1)
		{
			Point interseccion = {pAnchor.x + t * pVector.x, pAnchor.y + t * pVector.y, pAnchor.z + t * pVector.z};

			UV mapped = mapCone (pDetails, interseccion);
	  		//printf("U %LF V %LF\n",mapped.U,mapped.V );
			Color fretColor = coneDetails->fretWork.RGB[llroundl(mapped.U * coneDetails->fretWork.width)][llroundl(mapped.V * coneDetails->fretWork.height)];

			if(fretColor.r == 0.0 && fretColor.g == 0.0  && fretColor.b == 0.0)
			 	t = -1;

		}
		return t;
	}
	else
		return -1;
}

long double calcInterQuadSuper(Point pAnchor, Point pVector, void* pDetails){
	QuadraticSuperface quadSuperDetails = *(QuadraticSuperface*) pDetails;

	long double a = quadSuperDetails.A * pow(pVector.x, 2) + quadSuperDetails.B * pow(pVector.y, 2) + quadSuperDetails.C * pow(pVector.z, 2) +
					2 * (quadSuperDetails.D * pVector.x * pVector.y + quadSuperDetails.E * pVector.y * pVector.z + quadSuperDetails.F * pVector.x * pVector.z);

	long double b = 2 * (quadSuperDetails.A * pAnchor.x * pVector.x + quadSuperDetails.B * pAnchor.y * pVector.y + quadSuperDetails.C * pAnchor.z * pVector.z + 
					quadSuperDetails.D * ( pAnchor.x * pVector.y + pAnchor.y * pVector.x ) + quadSuperDetails.E * ( pAnchor.y * pVector.z + pAnchor.z * pVector.y ) + 
					quadSuperDetails.F * ( pAnchor.z * pVector.x + pAnchor.x * pVector.z ) +
					quadSuperDetails.G * pVector.x + quadSuperDetails.H * pVector.y  + quadSuperDetails.J * pVector.z );

	long double c = quadSuperDetails.A * pow(pAnchor.x, 2) + quadSuperDetails.B * pow(pAnchor.y, 2) + quadSuperDetails.C * pow(pAnchor.z, 2) +
					2 * (quadSuperDetails.D * pAnchor.x * pAnchor.y + quadSuperDetails.E * pAnchor.y * pAnchor.z + quadSuperDetails.F * pAnchor.z * pAnchor.x +
					quadSuperDetails.G * pAnchor.x + quadSuperDetails.H * pAnchor.y + quadSuperDetails.J * pAnchor.z) + quadSuperDetails.K;


	long double delta = pow(b, 2) - 4 * a * c;

	if(delta > EPSILON)
	{
		long double t1 = (-1 * b - sqrt(delta) ) / (2 * a);
		long double t2 = (-1 * b + sqrt(delta) ) / (2 * a);

		if (t1 > EPSILON && t2 > EPSILON){
			return t1;
		}
		else if (t1 < EPSILON && t2 > EPSILON)
			return t2;
		else
			return -1;

	}else if( delta < EPSILON && delta > (-1 * EPSILON) )
		return -1 * b / (2 * a);

	else
		return -1;
}

long double calcInterElip(Point pAnchor, Point pVector, void* pDetails){
	//printf("Hola\n");
	Elip discDetails = *(Elip*)pDetails;
	long double t;
	long double denominador = (discDetails.Normal.x * pVector.x) + (discDetails.Normal.y * pVector.y) + (discDetails.Normal.z * pVector.z);
	if (denominador == EPSILON || denominador == 0) {
		return -1;
	} else {
		long double numerador = -((discDetails.Normal.x * pAnchor.x) + (discDetails.Normal.y * pAnchor.y) + (discDetails.Normal.z * pAnchor.z) + discDetails.D);
		t = numerador/denominador;
		//printf("%Lf\n",t);
	}
	return t;

}


long double calcInterDisc(Point pAnchor, Point pVector, void* pDetails){
	//printf("Hola\n");
	Disc discDetails = *(Disc*)pDetails;
	long double t;
	long double denominador = (discDetails.Normal.x * pVector.x) + (discDetails.Normal.y * pVector.y) + (discDetails.Normal.z * pVector.z);
	if (denominador == EPSILON || denominador == 0) {
		return -1;
	} else {
		long double numerador = -((discDetails.Normal.x * pAnchor.x) + (discDetails.Normal.y * pAnchor.y) + (discDetails.Normal.z * pAnchor.z) + discDetails.D);
		t = numerador/denominador;
		//printf("%Lf\n",t);
	}
	return t;
}

/**********************
		Normales
**********************/

Point calculateNormal(Object pObject, Point pInterPoint)
{
	Point (*functionPoint) (Point,void*) = pObject.NormalFunction;
	return functionPoint (pInterPoint, pObject.details);
}

Point calcNormalSphere(Point pInterPoint, void* pDetails){
	Sphere *sphereDetails = (Sphere*) pDetails;
	Point center = sphereDetails[0].center;
	long double radius = sphereDetails[0].radius;

	Point vectorN;
	vectorN.x = (pInterPoint.x - center.x) / radius;
	vectorN.y = (pInterPoint.y - center.y) / radius;
	vectorN.z = (pInterPoint.z - center.z) / radius;

	return vectorN;
}


Point calcNormalPoly(Point pInterPoint, void* pDetails){
	Polygon poly = *(Polygon*) pDetails;
	Point vectorN;
	vectorN.x = poly.Normal.x;
	vectorN.y = poly.Normal.y;
	vectorN.z = poly.Normal.z;
	return vectorN;

}

Point calcNormalCylinder(Point pInterPoint, void* pDetails){

	Cylinder *cylinderDetails = (Cylinder*) pDetails;
	Point anchor = cylinderDetails[0].anchor;
	Point direction = cylinderDetails[0].direction;

	long double preCalc = (pInterPoint.x - anchor.x)  * direction.x + (pInterPoint.y - anchor.y) * direction.y + (pInterPoint.z - anchor.z) * direction.z;

	long double preClacX = 2.0 * ((anchor.x + preCalc * direction.x) - pInterPoint.x);
	long double preClacY = 2.0 * ((anchor.y + preCalc * direction.y) - pInterPoint.y);
	long double preClacZ = 2.0 * ((anchor.z + preCalc * direction.z) - pInterPoint.z);

	Point vectorN;
	vectorN.x = preClacX * (pow(direction.x, 2) - 1) +
				preClacY * (direction.x * direction.y) +
				preClacZ * (direction.x * direction.z);

	vectorN.y = preClacX * (direction.y * direction.x) +
				preClacY * (pow(direction.y, 2) - 1) +
				preClacZ * (direction.y * direction.z);

	vectorN.z = preClacX * (direction.z * direction.x) +
				preClacY * (direction.z * direction.y) +
				preClacZ * (pow(direction.z, 2) - 1);

	vectorN = normalize(vectorN);

	return vectorN;
}

Point calcNormalCone(Point pInterPoint, void* pDetails){

	Cone *coneDetails = (Cone*) pDetails;
	Point anchor = coneDetails[0].anchor;
	Point direction = coneDetails[0].direction;
	long double k1 = coneDetails[0].k1;
	long double k2 = coneDetails[0].k2;

	long double preCalc = (pInterPoint.x - anchor.x)  * direction.x + (pInterPoint.y - anchor.y) * direction.y + (pInterPoint.z - anchor.z) * direction.z;

	long double preClacX = 2.0 * ((anchor.x + preCalc * direction.x) - pInterPoint.x);
	long double preClacY = 2.0 * ((anchor.y + preCalc * direction.y) - pInterPoint.y);
	long double preClacZ = 2.0 * ((anchor.z + preCalc * direction.z) - pInterPoint.z);

	Point vectorN;
	vectorN.x = preClacX * (pow(direction.x, 2) - 1.0) +
				preClacY * (direction.x * direction.y) +
				preClacZ * (direction.x * direction.z) -
				2.0* ((k2/k1) * preCalc) * (direction.x * (k2/k1));

	vectorN.y = preClacX * (direction.y * direction.x) +
				preClacY * (pow(direction.y, 2) - 1.0) +
				preClacZ * (direction.y * direction.z) -
				2.0* ((k2/k1) * preCalc) * (direction.y * (k2/k1));

	vectorN.z = preClacX * (direction.z * direction.x) +
				preClacY * (direction.z * direction.y) +
				preClacZ * (pow(direction.z, 2) - 1.0) -
				2.0* ((k2/k1) * preCalc) * (direction.z * (k2/k1));

	vectorN = normalize(vectorN);

	return vectorN;
}

Point calcNormalQuadSuper(Point pInterPoint, void* pDetails){

	QuadraticSuperface quadSuperDetails = *(QuadraticSuperface*) pDetails;

	Point vectorN;
	vectorN.x = quadSuperDetails.A * pInterPoint.x + quadSuperDetails.D * pInterPoint.y + quadSuperDetails.F * pInterPoint.y + quadSuperDetails.G;
	vectorN.y = quadSuperDetails.D * pInterPoint.x + quadSuperDetails.B * pInterPoint.y + quadSuperDetails.E * pInterPoint.y + quadSuperDetails.H;
	vectorN.z = quadSuperDetails.F * pInterPoint.x + quadSuperDetails.E * pInterPoint.y + quadSuperDetails.C * pInterPoint.y + quadSuperDetails.J;

	vectorN = normalize(vectorN);

	return vectorN;
}

Point calcNormalDisc(Point pInterPoint, void* pDetails){
	Disc disc = *(Disc*) pDetails;
	Point vectorN;
	vectorN.x = disc.Normal.x;
	vectorN.y = disc.Normal.y;
	vectorN.z = disc.Normal.z;
	return vectorN;

}

Point calcNormalElip(Point pInterPoint, void* pDetails){
	Elip disc = *(Elip*) pDetails;
	Point vectorN;
	vectorN.x = disc.Normal.x;
	vectorN.y = disc.Normal.y;
	vectorN.z = disc.Normal.z;
	return vectorN;

}
