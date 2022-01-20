#ifndef __BOXFILE__
#define __BOXFILE__



//Número de vértices
const int pyramidNVertex = 13; 
const int pyramidNTriangleIndex = 6; 


const unsigned int pyramidTriangleIndex[] = {
	//Cara z = 1
	0,1,2,			1,3,2,
	//Cara z = -1
	4,6,5,			5,6,7,
	//Cara x = 1
	8,10,9,			9,10,11,
	//Cara x = -1
	12,13,14,		13,15,14,
	//Cara y = 1
	16,17,18,		17,19,18,
	//Cara y = -1
	20,22,21,		21,22,23,
};

//Posicíon de los vertices
const float pyramidVertexPos[] = { 
	//Cara z = 1
	-0.5f,	-0.5f,	 -0.5f, //0
	 0.5f,	-0.5f,	 -0.5f, //1
	 0.0f,	 0.5f,	  0.0f, //2

	//Cara z = -1		   
	-0.5f,	-0.5f,	  0.5f, //3
	 0.5f,	-0.5f,	  0.5f, //4
	 0.0f,	 0.5f,	  0.0f, //5

	//Cara x = 1		   
	-0.5f,	-0.5f,	 -0.5f, //6
	-0.5f,	-0.5f,	  0.5f, //7
	 0.0f,	 0.5f,	  0.0f, //8

	//Cara x = -1		   
	 0.5f,	-0.5f,	 -0.5f, //0
	 0.5f,	-0.5f,	  0.5f, //1
	 0.0f,	 0.5f,	  0.0f, //2

	//Cara y = 1		   
	-0.5f,	-0.5f,	  0.5f, //0
	-0.5f,	-0.5f,	 -0.5f, //1
	 0.0f,	 0.5f,	  0.0f, //2
 };


//Normal de los vertices
const float pyramidVertexNormal[] = { 
	//Cara z = 1
	0.0f,	0.0f,	-1.0f, 
	0.0f,	0.0f,   -1.0f, 
	0.0f,	0.0f,	-1.0f, 

	//Cara z = -1		   
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 

	//Cara x = 1		   
   -1.0f,	0.0f,	 0.0f, 
   -1.0f,	0.0f,	 0.0f, 
   -1.0f,	0.0f,	 0.0f, 

	//Cara x = -1		   
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 

	//Cara y = 1		   
	0.0f,  -1.0f,	0.0f, 
	0.0f,  -1.0f,	0.0f, 
	0.0f,  -1.0f,	0.0f, 

	//Cara y = -1		   
	0.0f,	1.0f,	0.0f, 
	0.0f,	1.0f,	0.0f, 
	0.0f,	1.0f,	0.0f, 
 };


//Color de los vertices
const float pyramidVertexColor[] = { 
	//Cara z = 1
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 
	0.0f,	0.0f,	 1.0f, 

	//Cara z = -1		   
	0.6f,	0.6f,	 1.0f, 
	0.6f,	0.6f,	 1.0f, 
	0.6f,	0.6f,	 1.0f, 
	0.6f,	0.6f,	 1.0f, 

	//Cara x = 1		   
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 
	1.0f,	0.0f,	 0.0f, 

	//Cara x = -1		   
	1.0f,	0.6f,	 0.6f, 
	1.0f,	0.6f,	 0.6f, 
	1.0f,	0.6f,	 0.6f, 
	1.0f,	0.6f,	 0.6f, 

	//Cara y = 1		   
	0.0f,	1.0f,	0.0f, 
	0.0f,	1.0f,	0.0f, 
	0.0f,	1.0f,	0.0f, 
	0.0f,	1.0f,	0.0f, 

	//Cara y = -1		   
	0.6f,	1.0f,	0.6f, 
	0.6f,	1.0f,	0.6f, 
	0.6f,	1.0f,	0.6f, 
	0.6f,	1.0f,	0.6f  
 };

//Cood. de textura de los vertices
const float pyramidVertexTexCoord[] = { 
	//Cara z = 1
	 0.0f, 0.0f,
	 1.0f, 0.0f,
	 0.0f, 1.0f,
	 1.0f, 1.0f,

	//Cara z = -1
	0.0f, 1.0f, 
	1.0f, 1.0f, 
	0.0f, 0.0f, 
	1.0f, 0.0f, 

	//Cara x = 1	
	0.0f,	1.0f,
	1.0f,	1.0f,
	0.0f,	0.0f,
	1.0f,	0.0f,

	//Cara x = -1
	0.0f,	0.0f,
	1.0f,	0.0f,
	0.0f,	1.0f,
	1.0f,	1.0f,

	//Cara y = 1	
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f,

	//Cara y = -1
	0.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
 };


const float pyramidVertexTangent[] = { 
	//Cara z = 1
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,

	//Cara z = -1				
	1.0f,	0.0f,	0.0f,		
	1.0f,	0.0f,	0.0f,		
	1.0f,	0.0f,	0.0f,		
	1.0f,	0.0f,	0.0f,		

	//Cara x = 1				
	0.0f,	0.0f,	-1.0f,		
	0.0f,	0.0f,	-1.0f,		
	0.0f,	0.0f,	-1.0f,		
	0.0f,	0.0f,	-1.0f,		

	//Cara x = -1				
	0.0f,	0.0f,	1.0f,		
	0.0f,	0.0f,	1.0f,		
	0.0f,	0.0f,	1.0f,		
	0.0f,	0.0f,	1.0f,		

	//Cara y = 1	   
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
					
	//Cara y = -1					   
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
	1.0f,	0.0f,	0.0f,
 };

#endif


