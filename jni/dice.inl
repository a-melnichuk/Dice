
float dicePositions[]= {
	-1.0f, -1.0f, -1.0f ,//bottom left
	-1.0f,  1.0f, -1.0f , //top left
	1.0f,  1.0f, -1.0f , //top right
	1.0f, -1.0f, -1.0f ,//bottom right

	// Fill in the back face vertex data.
	-1.0f, -1.0f, 1.0f , 
	 1.0f, -1.0f, 1.0f, 
	 1.0f,  1.0f, 1.0f , 
	-1.0f,  1.0f, 1.0f , 

	// Fill in the top face vertex data.
	-1.0f, 1.0f, -1.0f ,
	-1.0f, 1.0f,  1.0f , 
	 1.0f, 1.0f,  1.0f , 
	 1.0f, 1.0f, -1.0f ,

	// Fill in the bottom face vertex data.
	-1.0f, -1.0f, -1.0f , 
	 1.0f, -1.0f, -1.0f ,
	 1.0f, -1.0f,  1.0f ,
	-1.0f, -1.0f,  1.0f ,

	// Fill in the left face vertex data.
	-1.0f, -1.0f,  1.0f , 
	-1.0f,  1.0f,  1.0f ,
	-1.0f,  1.0f, -1.0f , 
	-1.0f, -1.0f, -1.0f , 

	// Fill in the right face vertex data.
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f, 
};

float  diceNormals[] = {
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, -1.0f,
 0.0f, 0.0f, -1.0f, 
  0.0f, 0.0f, -1.0f,
  
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 
  0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 
  
  0.0f, 1.0f, 0.0f, 
  0.0f, 1.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 
  0.0f, 1.0f, 0.0f,
  
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f, 
	0.0f, -1.0f, 0.0f,
   
   -1.0f, 0.0f, 0.0f, 
   -1.0f, 0.0f, 0.0f, 
   -1.0f, 0.0f, 0.0f,
   -1.0f, 0.0f, 0.0f, 
   
   1.0f, 0.0f, 0.0f, 
   1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f,
   1.0f, 0.0f, 0.0f   
};

uint16_t diceIndices[] = {
	0, 1, 2,
	0, 2, 3,
               
	4, 5, 6,
	4 ,6 ,7,
               
	8 ,9, 10,
	8, 10, 11,
               
	12, 13 ,14,
	12, 14, 15,
               
	16, 17, 18,
	16, 18, 19,
               
	20, 21, 22,
	20, 22, 23
};

/*
float  diceTextures[] = {
	
	0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
	0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
		0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
		0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
		0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
		0.0f,0.0f,
	0.0f,0.1f,
	1.0f,1.0f,
	1.0f,0.0f,
	
	
};*/
