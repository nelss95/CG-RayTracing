#define SWAP(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )
#define FIX(x) (*(unsigned *)&(x) = \
         SWAP(*(unsigned *)&(x)))



void writeAVS(Color** pBuffrer, char* pName, int pFileHeight, int pFileWidth){
   int row, column;
   int *width, *height;

   FILE *filePtr = fopen(pName, "w");




   if(filePtr != NULL) {

      /*    Hace el swap a big endian     */
      int fileWidth = SWAP(pFileWidth);
      int fileHeight = SWAP(pFileHeight);

      /*    Crea el header de la imagen      */
      fwrite(&fileWidth, sizeof(unsigned int),1,filePtr);
      fwrite(&fileHeight, sizeof(unsigned int),1,filePtr);

      /*    Escribe los colores     */

         for (row = pFileHeight - 1; row >=0; row--) {
            for (column = 0; column < pFileWidth; column++) {
               fputc(0, filePtr);
               fputc(pBuffrer[column][row].r * 255, filePtr);
               fputc(pBuffrer[column][row].g * 255, filePtr);
               fputc(pBuffrer[column][row].b * 255, filePtr);

            }
         }

   }else
      printf(" Fallo al abrir el archivo %s\n",pName );


   fclose(filePtr);
}

/*  *****************
        NUEVO
*****************  */

Texture initTexture(int pWidth, int pHeight){
   int row;
   Texture newTexture ;
   newTexture.width = pWidth;
   newTexture.height = pHeight;

   newTexture.RGB = (Color**)malloc((pWidth+1) * sizeof(Color*));

   for(row = 0; row <= pWidth; row++){
      newTexture.RGB[row] = (Color*)malloc( pHeight* sizeof(Color));
   }

   return newTexture;
}

Texture readAVS(char* pName){
   FILE *filePtr;
   int fileWidth, fileHeight, row, column, alpha, red, green, blue;
   Texture newTexture;
   char * newpName;

   newpName = strtok(pName,"\n");

   filePtr = fopen(newpName, "r");

   if(filePtr != NULL) {

      //Lee el header
      fread(&fileWidth, sizeof(int), 1, filePtr);
      fileWidth = FIX(fileWidth);
      fread(&fileHeight, sizeof(int), 1, filePtr);
      fileHeight = FIX(fileHeight);

      if(fileWidth > 0 && fileHeight > 0){
         newTexture = initTexture(fileWidth, fileHeight);

         for (row = fileHeight - 1; row >=0; row--) {
            for (column = 0; column < fileWidth; column++) {

               /* Read the current pixel */
               alpha = fgetc(filePtr);
               red = fgetc(filePtr);
               green = fgetc(filePtr);
               blue = fgetc(filePtr);

               if (alpha != EOF || green != EOF || red != EOF || blue != EOF) {

                  newTexture.RGB[column][row].r = (double) red / 256;
                  newTexture.RGB[column][row].g = (double) green / 256;
                  newTexture.RGB[column][row].b = (double) blue / 256;

                  //printf("%LF %LF %LF\n",  newTexture.RGB[column][row].r,  newTexture.RGB[column][row].g,  newTexture.RGB[column][row].b);

               }else
                  printf("EOF inesperado\n");

            }
         }


      }else
         printf("Header incorrecto\n");
   }else
      printf(" Fallo al abrir el archivo %s\n",pName );

   fclose(filePtr);

   return newTexture;
}
