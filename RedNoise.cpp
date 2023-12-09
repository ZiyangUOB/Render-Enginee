#include <CanvasTriangle.h>
#include <ModelTriangle.h>
#include <TextureMap.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include <RayTriangleIntersection.h>

#include <cstdlib> //used to create different
#include <ctime>   //random value .

#include <utility> // using swap()function.
#include <Utils.h>//using split function
#include <cmath> 
#define WIDTH 1240 // 320
#define HEIGHT 960// 240

	
//week 6 different operation choice
   int choice = 7;
//Week4 Task5 & Week5 global values
bool AnimationSwitch = true ;  // switch for animation 

// rotating angle define
 double angleInDegrees = 5.0; 
 double angleInRadians = angleInDegrees * M_PI / 180.0 ;
 glm::mat3 mX(1.0 , 0.0, 0.0, //first column
			            0.0 , cos(angleInRadians), sin(angleInRadians), //2nd column
                        0.0 , -sin(angleInRadians),cos(angleInRadians));//3rd column

 glm::mat3 mY(cos(angleInRadians) , 0.0, -sin(angleInRadians), //first column                      // rotation angle along x axis
			            0.0 ,1.0, 0.0, //2nd column
                        sin(angleInRadians) , 0.0,cos(angleInRadians));//3rd column                   // rotation angle along y axis

glm::mat3 orientation = glm::mat3(1.0, 0.0, 0.0,                                               // camera orientation points at world origin             
	              0.0,1.0,0.0,
				  0.0,0.0,1.0
	); 

glm::vec3 cameraPosition = glm::vec3(0.0, 0.0, 4.0) ; //4,0  
                             
float focalLength= 2.0 ;  
	               //2.0


//-----------------------------------------------------------------------------------------------------------------------------------------
//Helper Function Area



//helper function for returning new triangle class :
CanvasTriangle createTriangle()
{
	std::srand(std::time(nullptr)); // seed the random number generator with the current time,
									// ensuring that you get a different sequence of random
									// values each time you run the program ;
	CanvasPoint v1(std::rand() % WIDTH, std::rand() % HEIGHT);
	CanvasPoint v2(std::rand() % WIDTH, std::rand() % HEIGHT);
	CanvasPoint v3(std::rand() % WIDTH, std::rand() % HEIGHT);
	CanvasTriangle strokedTriangle(v1, v2, v3);

	return strokedTriangle;
}

//helper function for returning new colour class :
Colour createColour()
{
	std::srand(std::time(nullptr)); // seed the random number generator with the current time,
									// ensuring that you get a different sequence of random
									// values each time you run the program ;
	int red = std::rand() % 255;
	int green = std::rand() % 255;
	int blue = std::rand() % 255;
	Colour colourInput(red, green, blue);

	return colourInput;
}

//helper funtion for calculating all pixels between two vertixes.
std::vector<CanvasPoint> pixels(CanvasPoint from, CanvasPoint to)
{
	 std::vector<CanvasPoint> result ;

	float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	float steps = fmax(abs(xDiff), abs(yDiff));
	float xStepSize = xDiff / steps;
	float yStepSize = yDiff / steps;

    for (float i = 0.0; i <steps; i++)
	{
		float x = from.x + (i * xStepSize);
		float y = from.y + (i * yStepSize);
	    result.push_back(CanvasPoint(floor(x),floor(y))) ;
	}

	return result;
}

//self-drived sorting algorithm for 3 elements array
void sortVertixes (CanvasTriangle &triangle) {
	//  std::cout << triangle.vertices[0].y<<" " << triangle.vertices[1].y << " " <<triangle.vertices[2].y << std::endl ;
	 if (triangle.v0().y < triangle.v1().y) {
		std::swap(triangle.v0(),triangle.v1()) ;
		if (triangle.v1().y < triangle.v2().y){
		   std::swap(triangle.v1(),triangle.v2()) ;
		   if (triangle.v0().y < triangle.v1().y){
			std::swap(triangle.v0(), triangle.v1());
		   }
		}
	 }
	 else if (triangle.v1().y < triangle.v2().y){
	        std::swap(triangle.v1() , triangle.v2()) ;
			if (triangle.v0().y < triangle.v1().y) {
				  std::swap(triangle.v0() , triangle.v1()) ;
			}
	 }
	 std::swap(triangle.v0(), triangle.v2()) ;

	//  std::cout << triangle.vertices[0].y<<" " << triangle.vertices[1].y << " " <<triangle.vertices[2].y << std::endl ;
}

// Advanced verision for sorting (insertion sorting)
void sortVertixesAdvanced(CanvasTriangle &triangle){
	for (int i = 0; i < 2; i++){
		for (int j = 0; j < 2 - i; j++){
			if (triangle[j].y > triangle[j + 1].y){
				std::swap(triangle[j], triangle[j + 1]);
			}
		}
	}
}


//Interpolate two elements values(glm::vec2)
std::vector<glm::vec2> interpolateTwoElementValues(glm::vec2 from, glm::vec2 to, int numberOfValues)
{
	std::vector<glm::vec2> result;
	float spacedX = (to.x-from.x) / (numberOfValues+1) ;
	float spacedY = (to.y - from.y) / (numberOfValues+1);
	
	for (float i = 0.0; i < numberOfValues; i++)
	{   glm::vec2 value;
	    value.x = floor(from.x + (spacedX*i));
		value.y = floor(from.y + (spacedY*i));
		result.push_back(value);
	}

	return result;
}


//-----------------------------------------------------------------------------------------------------------------------------------------
//Week2

// 1. Week2 Task2 Implementation
std::vector<float> interpolateSingleFloats(float from, float to, int numberOfValues)
{

	   std::vector<float> result = {};
    
	if(numberOfValues ==1){
		result.push_back(from) ;
		return result ;
	}
    // Calculate the step size
    float stepSize = (to - from) / (numberOfValues - 1);
    
    // Generate the interpolated values and add them to the result vector
    for (int i = 0; i < numberOfValues; ++i) {
        result.push_back(from + i * stepSize);
    }
    
    return result;
}

// 2. Week2 Task3 implementation
void drawGrayGradient(DrawingWindow &window)
{
	float from = 255.0;
	float to = 0.0;
	std::vector<float> intensityList = interpolateSingleFloats(from, to, window.width);
	uint32_t intensity = 0;
	for (size_t y = 0; y < window.height; y++)
	{
		for (size_t x = 0; x < window.width; x++)
		{
			intensity = intensityList.at(x);
			float red = intensity;
			float green = intensity;
			float blue = intensity;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

// 3. Week2 Task4 implementation
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues)
{
	std::vector<glm::vec3> result;
	glm::vec3 spaced = (to - from) / static_cast<float>(numberOfValues - 1);
	glm::vec3 value;

	for (size_t i = 0; i < numberOfValues; i++)
	{
		value = from + (spaced * static_cast<float>(i));
		result.push_back(value);
	}

	return result;
}


//4.  Week2 Task5 implementation
void drawColorGradient(DrawingWindow &window)
{
	window.clearPixels();
	glm::vec3 topLeft(255, 0, 0);	   // red
	glm::vec3 topRight(0, 0, 255);	   // blue
	glm::vec3 bottomRight(0, 255, 0);  // green
	glm::vec3 bottomLeft(255, 255, 0); // yellow

	std::vector<glm::vec3> leftMostColumn = interpolateThreeElementValues(topLeft, bottomLeft, window.height);
	std::vector<glm::vec3> rightMostColumn = interpolateThreeElementValues(topRight, bottomRight, window.height);
	for (size_t y = 0; y < window.height; y++)
	{
		std::vector<glm::vec3> rowHandler = interpolateThreeElementValues(leftMostColumn[y], rightMostColumn[y], window.width);
		for (size_t x = 0; x < window.width; x++)
		{
			uint32_t colour = (255 << 24) + (int(rowHandler[x].x) << 16) + (int(rowHandler[x].y) << 8) + int(rowHandler[x].z);
			window.setPixelColour(x, y, colour);
		}
	}
}


// -------------------------------------------------------------------------------------------------------------------------------------------
//Week3


// 5. Week3 Task2 draw line implementation

void drawLine(DrawingWindow &window, CanvasPoint from, CanvasPoint to, Colour colorInput)
{
	uint32_t colour = (255 << 24) + (colorInput.red << 16) + (colorInput.green << 8) + colorInput.blue;
	std::vector<CanvasPoint> pixelsList = pixels(from, to) ;
    
	for(size_t i=0 ; i < pixelsList.size(); i++){
       CanvasPoint newPixel = pixelsList.at(i) ;
       window.setPixelColour(newPixel.x, newPixel.y , colour);
	}

	}


// 6. Week3 Task3 stroked triangle implementation
void drawStrokedTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour input)
{

	drawLine(window, triangle.v0(), triangle.v1(), input);
	drawLine(window, triangle.v0(), triangle.v2(), input);
	drawLine(window, triangle.v1(), triangle.v2(), input);
}


// 7. Week3 Task4 filled triangle implementation

void drawFilledTriangle(DrawingWindow &window, CanvasTriangle triangle, Colour colorInput)
{

	uint32_t colour = (255 << 24) + (colorInput.red << 16) + (colorInput.green << 8) + colorInput.blue;
	sortVertixes(triangle);

	// Defining postion
	CanvasPoint top = triangle.vertices[0];
	CanvasPoint middle = triangle.vertices[1];
	CanvasPoint bottom = triangle.vertices[2];

	CanvasPoint extraPoint;
	CanvasPoint left;
	CanvasPoint right;

	// Working out the position for extrapoint
	float topAndBottomGradient = (bottom.y - top.y) / (bottom.x - top.x);
	float deltaY = middle.y - top.y;
	float deltaX = deltaY / topAndBottomGradient;

	extraPoint.x = floor(top.x + deltaX);
	extraPoint.y = floor(middle.y);

	// Working out the right and left position for identifying left or right sides.
	if (extraPoint.x < middle.x)
	{
		left = extraPoint;
		right = middle;
	}
	else
	{
		left = middle;
		right = extraPoint;
	}

	float topLeftGradient = (top.y - left.y) / (top.x - left.x);
	float topRightGradient = (top.y - right.y) / (top.x - right.x);
	float LeftBottomGradient = (left.y - bottom.y) / (left.x - bottom.x);
	float rightBottomGradient = (right.y - bottom.y) / (right.x - bottom.x);

	// upper Triangle
	for (float y = top.y; y <= middle.y; y++)
	{
		float rowFrom;
		float rowTo;
		rowFrom = (top.x + ((y - top.y) / topLeftGradient)); 
		rowTo = (top.x + ((y - top.y) / topRightGradient));

		for (float x = rowFrom; x <= rowTo; x++)
		{
			window.setPixelColour(round(x), size_t(y), colour);
		}
	}

	// lower triangle
	for (float y = middle.y; y <= bottom.y; y++)
	{
		float rowFrom;
		float rowTo;
		rowFrom = left.x + ((y - middle.y) / LeftBottomGradient);
		rowTo = right.x + ((y - middle.y) / rightBottomGradient);

		for (float x = rowFrom; x <= rowTo; x++)
		{
			window.setPixelColour(round(x), size_t(y), colour);
		}
	}

	// Colour white(255, 255, 255);

	// drawStrokedTriangle(window, triangle, white);
} 



// 8. Week3 Task5 texture filled triangle implementation 
void drawTexturedTriangle(DrawingWindow &window, CanvasTriangle triangle, TextureMap textureMap){
     sortVertixes(triangle);


	// Defining postion
	CanvasPoint top = triangle.vertices[0];
	CanvasPoint middle = triangle.vertices[1];
	CanvasPoint bottom = triangle.vertices[2];

	CanvasPoint extraPoint;

	CanvasPoint left;
	CanvasPoint right;

	// Working out the position for extrapoint
	float topAndBottomGradient = (bottom.y - top.y) / (bottom.x - top.x);
	float deltaY = middle.y - top.y;
	float deltaX = deltaY / topAndBottomGradient;

	extraPoint.x = floor(top.x + deltaX);
	extraPoint.y = floor(middle.y);


	if (extraPoint.x < middle.x)  // case where the middle point is on the right
	{
		left = extraPoint;
		right = middle;


	float topLeftGradient = (top.y - left.y) / (top.x - left.x);
	float topRightGradient = (top.y - right.y) / (top.x - right.x);
	float LeftBottomGradient = (left.y - bottom.y) / (left.x - bottom.x);
	float rightBottomGradient = (right.y - bottom.y) / (right.x - bottom.x);


	// upper Triangle
	for (float y = top.y; y <= middle.y; y++)
	{   
		float rowFrom;
		float rowTo;
		rowFrom = floor((top.x + ((y - top.y) / topLeftGradient))); 
		rowTo = floor((top.x + ((y - top.y) / topRightGradient)));

        //The texture point can be regard as two elements value, i.e. vec2
		float textureFromY = floor(top.texturePoint.y + (bottom.texturePoint.y-top.texturePoint.y)/(bottom.y-top.y)*(y-top.y));
		float textureFromX =floor( top.texturePoint.x + (bottom.texturePoint.x-top.texturePoint.x)/(bottom.y-top.y)*(y-top.y));
        float textureToY = floor(top.texturePoint.y + (middle.texturePoint.y-top.texturePoint.y)/(middle.y-top.y)*(y-top.y));
        float textureToX = floor(top.texturePoint.x + (middle.texturePoint.x-top.texturePoint.x)/(middle.y-top.y)*(y-top.y));
		glm::vec2 textureFrom(textureFromX,textureFromY);
		glm::vec2 textureTo(textureToX,textureToY) ;
        std::vector<glm::vec2> texturePointList = interpolateTwoElementValues(textureFrom,textureTo,(rowTo-rowFrom+1)) ;
        
		//Filling every pixels on the specific row
		for (float x = rowFrom; x <= rowTo; x++)
		{ 

            float texturepointX = floor(texturePointList.at(x-rowFrom).x) ;
			float texturepointY = floor(texturePointList.at(x-rowFrom).y);

			uint32_t index = (floor(texturepointY) * textureMap.width) + floor(texturepointX)  ; 
			                                              
			window.setPixelColour(floor(x), floor(y), textureMap.pixels.at(index));	    
		             
		}

	}
	
	// lower triangle
	for (float y = middle.y+1 ; y <= bottom.y; y++)
	{
		float rowFrom;
		float rowTo;
	    rowFrom = floor(left.x + ((y - middle.y) / LeftBottomGradient));
		rowTo = floor(right.x + ((y - middle.y) / rightBottomGradient));

       //The texture point can be regard as two elements value, i.e. vec2
        float textureFromY = floor(top.texturePoint.y + ((bottom.texturePoint.y-top.texturePoint.y)/(bottom.y-top.y))*(y-top.y));
		float textureFromX = floor(top.texturePoint.x + ((bottom.texturePoint.x-top.texturePoint.x)/(bottom.y-top.y))*(y-top.y));
		float textureToY = floor(middle.texturePoint.y + ((bottom.texturePoint.y-middle.texturePoint.y)/(bottom.y-middle.y))*(y-middle.y));
	    float textureToX =floor(middle.texturePoint.x + ((bottom.texturePoint.x-middle.texturePoint.x)/(bottom.y-middle.y))*(y-middle.y));
		glm::vec2 textureFrom(textureFromX,textureFromY);
		glm::vec2 textureTo(textureToX,textureToY) ;
		std::vector<glm::vec2> texturePointList = interpolateTwoElementValues(textureFrom,textureTo,(rowTo-rowFrom+1)) ;
        
		//Filling every pixels on the specific row
		for (float x = rowFrom; x <= rowTo; x++)
		{ 
            float texturepointX = floor(texturePointList.at(x-rowFrom).x) ;
			float texturepointY = floor(texturePointList.at(x-rowFrom).y);

			uint32_t index = (floor(texturepointY) * textureMap.width) + (floor(texturepointX))  ;

			window.setPixelColour(floor(x), floor(y), textureMap.pixels.at(index));	  
			            
	    }
	  }
	}
	
	  else{  // case where the middle point is on the left
		  left = middle;
		  right = extraPoint;

	float topLeftGradient = (top.y - left.y) / (top.x - left.x);
	float topRightGradient = (top.y - right.y) / (top.x - right.x);
	float LeftBottomGradient = (left.y - bottom.y) / (left.x - bottom.x);
	float rightBottomGradient = (right.y - bottom.y) / (right.x - bottom.x);
     
	 // upper Triangle
	for (float y = top.y; y <= middle.y; y++)
	{   
		float rowFrom;
		float rowTo;
		rowFrom = floor(top.x + ((y - top.y) / topLeftGradient)); 
		rowTo = floor(top.x + ((y - top.y) / topRightGradient));
        
		//The texture point can be regard as two elements value, i.e. vec2
		float textureFromY = floor(top.texturePoint.y + ((middle.texturePoint.y-top.texturePoint.y)/(middle.y-top.y))*(y-top.y));
		float textureFromX = floor(top.texturePoint.x + ((middle.texturePoint.x-top.texturePoint.x)/(middle.y-top.y))*(y-top.y));
        float textureToY = floor(top.texturePoint.y + ((bottom.texturePoint.y-top.texturePoint.y)/(bottom.y-top.y))*(y-top.y));
        float textureToX = floor(top.texturePoint.x + ((bottom.texturePoint.x-top.texturePoint.x)/(bottom.y-top.y))*(y-top.y));
		glm::vec2 textureFrom(textureFromX,textureFromY);
		glm::vec2 textureTo(textureToX,textureToY) ;
		std::vector<glm::vec2> texturePointList = interpolateTwoElementValues(textureFrom,textureTo,(rowTo-rowFrom+1)) ;
        
		//Filling every pixels on the specific row
		for (float x = rowFrom; x <= rowTo; x++)
		{ 
            float texturepointX = floor(texturePointList.at(x-rowFrom).x );
			float texturepointY = floor(texturePointList.at(x-rowFrom).y );

			uint32_t index = (floor(texturepointY) * textureMap.width) + floor(texturepointX)  ; 
		                                      
			window.setPixelColour(floor(x), floor(y), textureMap.pixels.at(index));	
			   
		}

	}
	
	// lower triangle
	for (float y = middle.y +1; y <= bottom.y; y++)
	{
		float rowFrom;
		float rowTo;
	    rowFrom = floor(left.x + ((y - middle.y) / LeftBottomGradient));
		rowTo = floor(right.x + ((y - middle.y) / rightBottomGradient));
        
		//The texture point can be regard as two elements value, i.e. vec2
        float textureFromY =floor(middle.texturePoint.y + ((bottom.texturePoint.y-middle.texturePoint.y)/(bottom.y-middle.y))*(y-middle.y));
		float textureFromX =floor( middle.texturePoint.x + ((bottom.texturePoint.x-middle.texturePoint.x)/(bottom.y-middle.y))*(y-middle.y));
		float textureToY = floor(top.texturePoint.y + ((bottom.texturePoint.y-top.texturePoint.y)/(bottom.y-top.y))*(y-top.y));
	    float textureToX = floor(top.texturePoint.x + ((bottom.texturePoint.x-top.texturePoint.x)/(bottom.y-top.y))*(y-top.y));
        glm::vec2 textureFrom(textureFromX,textureFromY);
		glm::vec2 textureTo(textureToX,textureToY) ;
		std::vector<glm::vec2> texturePointList = interpolateTwoElementValues(textureFrom,textureTo,(rowTo-rowFrom+1)) ;
        
        
		//Filling every pixels on the specific row
		for (float x = rowFrom; x <= rowTo; x++)
		{ 
            float texturepointX = floor(texturePointList.at(x-rowFrom).x) ;
			float texturepointY = floor(texturePointList.at(x-rowFrom).y);

			uint32_t index = (floor(texturepointY) * textureMap.width) + (floor(texturepointX))  ;
            
			window.setPixelColour(round(x), round(y), textureMap.pixels.at(index));
			            
	    }
	   }
	}
	
	Colour white(255, 255, 255);

	drawStrokedTriangle(window, triangle, white);
}


// -------------------------------------------------------------------------------------------------------------------------------------------
//Week4 Wireframes and Rasterising 


//reading all vertices from the obj file
std::vector<glm::vec3> readOBJGeometryFileVetcies(const std::string& fileName, float scalingFactor) {

  std::vector<glm::vec3> vertices ;                  // vector for storing vetices for model triangle
  std::string textEachLine ;                         // string for each line on the file
 
  std::ifstream myfile(fileName);                    // opening file

     if (!myfile.is_open()) {                        // once fali to open file 
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return {};
    }


 while(std::getline(myfile,textEachLine)){ // iterates every line in the file

	if (textEachLine.empty()) {
                continue; // Ignore empty lines
            }

	std::vector<std::string> stringVector = split(textEachLine, ' ' ) ;   // extracting every vacabulary from the line
	

  
	if(stringVector[0] == "v") {
		glm::vec3 addedVertice;
		addedVertice.x = std::stof(stringVector[1]) * scalingFactor;
		addedVertice.y = std::stof(stringVector[2]) * scalingFactor; 
		addedVertice.z = std::stof(stringVector[3]) * scalingFactor;
		vertices.push_back(addedVertice) ;
   }
      
   
}
   return vertices ;

}

//Week 4 Task2: read OBJ geometry file and return ModelTriangles
std::vector<ModelTriangle> readOBJGeometryFile(const std::string& fileName, float scalingFactor, std::vector<Colour> ColourPalette) {
  std::vector<ModelTriangle> modelTriangleVector ;   // vector for storing model triangle
  std::vector<glm::vec3> vertices ;                  // vector for storing vetices for model triangle
  std::string textEachLine ;                         // string for each line on the file
  Colour relatedColour ;                             // auxiliary colour variable 
  std::ifstream myfile(fileName);                    // opening file

     if (!myfile.is_open()) {                        // once fali to open file 
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return {};
    }


 while(std::getline(myfile,textEachLine)){ // iterates every line in the file

	if (textEachLine.empty()) {
                continue; // Ignore empty lines
            }

	std::vector<std::string> stringVector = split(textEachLine, ' ' ) ;   // extracting every vacabulary from the line
	

    if(stringVector[0]== "usemtl"){
		for (size_t i = 0 ; i < ColourPalette.size() ; i++){
			if (ColourPalette[i].name == stringVector[1] ) {
                  relatedColour = ColourPalette[i] ;    
			}
		}
	}else if(stringVector[0] == "v") {
		glm::vec3 addedVertice;
		addedVertice.x = std::stof(stringVector[1]) * scalingFactor; //std::stof used for converting string to float
		addedVertice.y = std::stof(stringVector[2]) * scalingFactor; //std::stof used for converting string to float
		addedVertice.z = std::stof(stringVector[3]) * scalingFactor; //std::stof used for converting string to float
		vertices.push_back(addedVertice) ;
   }else if(stringVector[0] == "f") {
	    std::string string1 = stringVector[1] ;
		int v1  = std::stoi(string1)- 1 ; // the index of first vertice for the modelTriangle in the vertices vector


		std::string string2 = stringVector[2] ;
		int v2 = std::stoi(string2) - 1; // the index of second vertice for the modelTriangle in the vertices vector
     
		
		std::string string3 = stringVector[3] ;
		int v3  = std::stoi(string3)  - 1; // the index of third vertice for the modelTriangle in the vertices vector
  

		ModelTriangle newModelTriangle(vertices.at(v1), vertices.at(v2), vertices.at(v3), relatedColour) ;
		modelTriangleVector.push_back(newModelTriangle) ;
      
   }
   
}
   return modelTriangleVector ;

}


//Week 4 Task3: read MTL material file
std::vector<Colour> readMTLMaterialFile(const std::string& fileName) {

         std::vector<Colour> colourVector ;   // vector for storing model triangle
         std::string textEachLine ;                         // string for each line on the file

	     std::ifstream myfile(fileName); 

		if (!myfile.is_open()) {
         std::cerr << "Failed to open file: " << fileName << std::endl;
        return {};
    }

       std::string colourName; //auxiliary colour variable ;

	    while(std::getline(myfile,textEachLine)){ // iterates every line in the file
         if (textEachLine.empty()) {
                continue; // Ignore empty lines
            }

			 std::vector<std::string> stringVector = split(textEachLine, ' ') ;
          
		if(stringVector.at(0) == "newmtl" ) {
                colourName = stringVector.at(1) ;
				
		}else if(stringVector.at(0) == "Kd"){
            std::string redString = stringVector.at(1);
            float redFloat = std::stof(redString);  //convert redstring into float type ;

			std::string greenString = stringVector.at(2);
			float greenFloat = std::stof(greenString);  //convert greenstring into float type ;

            std::string blueString = stringVector.at(3);
			float blueFloat = std::stof(blueString);  //convert bluestring into float type ;
            
			Colour newColour ;
			newColour.name = colourName ;

			int redInt = floor(redFloat*255) ;
			newColour.red = redInt ;   

			int greenInt = floor(greenFloat*255) ;
			newColour.green = greenInt ;

			int blueInt = floor(blueFloat*255) ;
			newColour.blue = blueInt ;

            colourVector.push_back(newColour) ;
		}
	     
      }
	  return colourVector ;
}


//Week4 Task5  using  model coordinate system (i.e. origian: centre of the model), z is "out of the screen" (towards you), y towards up, x towards right
	CanvasPoint getCanvasIntersectionPoint(glm::vec3 cameraPosition, glm::vec3 vertexPosition,float focalLength){
	   
		glm::vec3 cameraToVertex =  cameraPosition - vertexPosition   ;

		glm::vec3 adjustedvertex =  cameraToVertex * orientation   ;   //results corrected vector giving direction of artefact(w.r.t to orientation of camera)

				// std::cout << "1(" << cameraToVertex.x << "," << cameraToVertex.y <<"," << cameraToVertex.z << ")" << std::endl ;
				// std::cout << "2(" << adjustedvertex.x << "," << adjustedvertex.y <<"," << adjustedvertex.z << ")" << std::endl ;
       
		  float dv = adjustedvertex.z ;    //adjacent disntance between camera and vertix in similar triangle in any diemnsion ;
		// Working out intersetction point y coordinate in image plane 
		    float hv = adjustedvertex.y;   //oppsite disntance between camera and vertix in similar triangle in y-z dimension 
		    float v = round((focalLength * (hv/dv)*(HEIGHT/2))) + (HEIGHT/2);

	    // Working out intersetction point x coordinate in image plane 
		    float hu = adjustedvertex.x;   //oppsite disntance between camera and vertix in similar triangle x-z dimension
		    float u = - round(focalLength * (hu/dv)* (HEIGHT/2)) + (WIDTH/2) ;
           
		// Working out the depth relative to camera
		   float depth = adjustedvertex.z ;
		   
      
		return CanvasPoint(u,v,depth) ;
	}



//Week4 Task6 
// Pointscloud render
void pointCloudRender(DrawingWindow &window, std::vector<glm::vec3> vertices, glm::vec3 cameraPosition, float focalLength){
	   Colour white(255, 255, 255);
	   uint32_t colour = (255 << 24) + (white.red << 16) + (white.green << 8) + white.blue;

       for(size_t i =0 ; i < vertices.size() ; i++) {
		  glm::vec3 vertice = vertices[i] ;        //iterate every vertex on the model , and then calculates its corresponding canvans points(by projection)
		  CanvasPoint newCanvasPoint = getCanvasIntersectionPoint(cameraPosition, vertice, focalLength) ;
			 float x = newCanvasPoint.x ;
			 float y = newCanvasPoint.y ;
			 window.setPixelColour(round(x), round(y),colour);
	   }
}


//Week4 Task7
//Wireframe render
void wireframeRender(DrawingWindow &window, std::vector<ModelTriangle> ModelTriangles , glm::vec3 cameraPosition, float focalLength){
	   Colour white(255, 255, 255);
	
  
	   for(size_t i=0 ; i < ModelTriangles.size(); i++){
          ModelTriangle newModelTriangle = ModelTriangles[i] ;
		  CanvasPoint v1 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[0],focalLength);
		  CanvasPoint v2 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[1],focalLength);
		  CanvasPoint v3 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[2],focalLength);
		  CanvasTriangle newCanvasTriangle(v1,v2,v3) ;
		  drawStrokedTriangle(window,newCanvasTriangle,white);
	   }
   
}


//Week4 Task8
//Rasterised  render (Occlusion Problem caused)
void rasterisedRender(DrawingWindow &window, std::vector<ModelTriangle> ModelTriangles , glm::vec3 cameraPosition, float focalLength){
	
	   for(size_t i=0 ; i < ModelTriangles.size(); i++){
          ModelTriangle newModelTriangle = ModelTriangles[i] ;
		  Colour colour = newModelTriangle.colour ;
		  CanvasPoint v1 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[0],focalLength);
		  CanvasPoint v2 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[1],focalLength);
		  CanvasPoint v3 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[2],focalLength);
		  CanvasTriangle newCanvasTriangle(v1,v2,v3) ;
		  drawFilledTriangle(window,newCanvasTriangle,colour);
	   }
   
}


//Week4 Task9
//dealing with occlusion problem
void rasterisedRenderAdvanced(DrawingWindow &window, std::vector<ModelTriangle> ModelTriangles , glm::vec3 cameraPosition, float focalLength){

float depthBuffer[WIDTH][HEIGHT] ; 

for(int i=0 ; i <WIDTH ; i++){
	for(int j = 0 ; j< HEIGHT ; j++){
		depthBuffer[i][j] = 0.0;
	}
}
	   // Use nested loops to set all elements to 0

	 for(size_t i=0 ; i < ModelTriangles.size(); i++){ // iterate all modelTriangles
          ModelTriangle newModelTriangle = ModelTriangles[i] ;
		  uint32_t colour = (255 << 24) + (newModelTriangle.colour.red << 16) + (newModelTriangle.colour.green << 8) +newModelTriangle.colour.blue;
		  CanvasPoint v1 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[0],focalLength);
		  CanvasPoint v2 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[1],focalLength);
		  CanvasPoint v3 = getCanvasIntersectionPoint(cameraPosition,newModelTriangle.vertices[2],focalLength);
		  CanvasTriangle newCanvasTriangle(v1,v2,v3) ;
          sortVertixes(newCanvasTriangle);
          
	// Defining postion
	CanvasPoint top = newCanvasTriangle.vertices[0];
	CanvasPoint middle = newCanvasTriangle.vertices[1];
	CanvasPoint bottom = newCanvasTriangle.vertices[2];
	CanvasPoint extraPoint;
	// Working out the position for extrapoint
	float topAndBottomGradient = (bottom.y - top.y) / (bottom.x - top.x);
	float deltaY = middle.y - top.y;
	float deltaX = deltaY / topAndBottomGradient;
	extraPoint.x = round(top.x + deltaX);
	extraPoint.y = middle.y;

    // important parameters
	int ydiff = bottom.y - top.y +1 ;
	int yStepsUpper = middle.y - top.y+1 ;
	int yStepLower = bottom.y - middle.y+1 ; 
	

	if(extraPoint.x < middle.x) { // middle point on right 
	 // depths
		std::vector<float> topBottomDepth = interpolateSingleFloats(top.depth, bottom.depth, ydiff) ;
		std::vector<float> topMiddleDepth = interpolateSingleFloats(top.depth, middle.depth, yStepsUpper) ;
        std::vector<float> MiddleBottomDepth = interpolateSingleFloats(middle.depth, bottom.depth,yStepLower ) ;
        //Left and right sides x coordinates for each row
		std::vector<float> LeftSide = interpolateSingleFloats(top.x, bottom.x, ydiff) ;
		std::vector<float> topMiddleSide = interpolateSingleFloats(top.x, middle.x, yStepsUpper) ;
		std::vector<float> middleBottomSide = interpolateSingleFloats(middle.x, bottom.x, yStepLower) ;

	    //upper triangle
		for(int y= top.y ; y <= middle.y ; y++){
			//Calculate end-points x coordinates
			int xFrom = (floor(LeftSide[y-top.y]));
			int xTo = (floor(topMiddleSide[y-top.y])) ;
			int xDiff = xTo - xFrom + 1 ;
            // Filling pixels based on comparation condition
            for(int x= xFrom ; x <= xTo ; x++){
                 std::vector<float> rowDepth = interpolateSingleFloats(topBottomDepth[y-top.y], topMiddleDepth[y-top.y], xDiff) ;
				 if(depthBuffer[x][y] < (1.0/rowDepth[x-xFrom])){
					depthBuffer[x][y] = (1.0/rowDepth[x-xFrom]) ;
					 window.setPixelColour(x,y,colour) ;
				 }
			}
		}
		//lower triangle
		for(int y= middle.y+1 ; y <= bottom.y ; y++){
            //Calculate end-points x coordinates
			int xFrom = (floor(LeftSide[y-top.y]));
			int xTo = (floor(middleBottomSide[y-middle.y])) ;
			int xDiff = xTo - xFrom + 1 ;
			 // Filling pixels based on comparation condition
			std::vector<float> rowDepth = interpolateSingleFloats(topBottomDepth[y-top.y], MiddleBottomDepth[y-middle.y], xDiff) ;
            for(int x= xFrom ; x <= xTo ; x++){
				 if(depthBuffer[x][y] < (1.0/rowDepth[x-xFrom])){
					depthBuffer[x][y] = (1.0/rowDepth[x-xFrom]) ;
					 window.setPixelColour(x,y,colour) ;
				 }
			}
		}
	

	 } else { //extra point on the right
		 // depths
		std::vector<float> topBottomDepth = interpolateSingleFloats(top.depth, bottom.depth, ydiff) ;
		std::vector<float> topMiddleDepth = interpolateSingleFloats(top.depth, middle.depth, yStepsUpper) ;
        std::vector<float> MiddleBottomDepth = interpolateSingleFloats(middle.depth, bottom.depth,yStepLower ) ;
        //Left and right sides x coordinates for each rowstd::vector<float> evenlySpalcedList;

		std::vector<float> rightSide = interpolateSingleFloats(top.x, bottom.x, ydiff) ;
		std::vector<float> topMiddleSide = interpolateSingleFloats(top.x, middle.x, yStepsUpper) ;
		std::vector<float> middleBottomSide = interpolateSingleFloats(middle.x, bottom.x, yStepLower) ;

	    //upper triangle
		for(int y= top.y ; y <= middle.y ; y++){
			//Calculate end-points x coordinates
			int xFrom = (round(topMiddleSide[y-top.y])) ;
			int xTo = (round(rightSide[y-top.y]));
			int xDiff = xTo - xFrom + 1 ;
			
            // Filling pixels based on comparation condition
			     std::vector<float> rowDepth = interpolateSingleFloats(topMiddleDepth[y-top.y], topBottomDepth[y-top.y], xDiff) ;
            for(int x= xFrom ; x <= xTo ; x++){ 
				 if(depthBuffer[x][y] < (1.0/rowDepth[x-xFrom])){
					depthBuffer[x][y] = (1.0/rowDepth[x-xFrom]) ;
					 window.setPixelColour(x,y,colour) ;
				 }
			}
		}
		//lower triangle
		for(int y= middle.y+1 ; y <= bottom.y ; y++){
            //Calculate end-points x coordinates
			int xFrom = (round(middleBottomSide[y-middle.y])) ;
			
			int xTo= (round(rightSide[y-top.y]));
			
			int xDiff = xTo - xFrom + 1 ;
		
			 // Filling pixels based on comparation condition
			  std::vector<float> rowDepth = interpolateSingleFloats( MiddleBottomDepth[y-middle.y], topBottomDepth[y-top.y],xDiff) ;
            for(int x= xFrom ; x <= xTo ; x++){
				 if(depthBuffer[x][y] < (1.0/rowDepth[x-xFrom])){
					depthBuffer[x][y] = (1.0/rowDepth[x-xFrom]) ;
					 window.setPixelColour(x,y,colour) ;
				 }
			}
		}

	}

 }
}



// -------------------------------------------------------------------------------------------------------------------------------------------
//Week5 Navigation and transformation
//Task 5 Look at function
void lookAt(glm::vec3 lookatPoint){
     glm::vec3 forward = glm::normalize(cameraPosition - lookatPoint) ;
	 glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0,1.0,0.0) , forward)) ;
	 glm::vec3 up = glm::normalize(glm::cross(forward, right)) ;
	 orientation = glm::mat3(right, up ,forward) ;
}


void lookAtRayTracing(glm::vec3 lookatPoint){
		glm::vec3 scalingCamera ;
				scalingCamera.x = cameraPosition.x * HEIGHT/2 + WIDTH/2 ;
				scalingCamera.y = cameraPosition.y * HEIGHT/2 + HEIGHT/2 ;
				scalingCamera.z = cameraPosition.z * HEIGHT/2 ;
     glm::vec3 forward = glm::normalize(scalingCamera - lookatPoint) ;
	 glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0,1.0,0.0) , forward)) ;
	 glm::vec3 up = glm::normalize(glm::cross(forward, right)) ;
	//  orientationRayTracing = glm::mat3(right, up ,forward) ;
}

// -------------------------------------------------------------------------------------------------------------------------------------------
//Week7 Ray Tracing and Shadows
RayTriangleIntersection getClosestIntersection(glm::vec3 startPosition , glm::vec3 rayDirection ,std::vector<ModelTriangle> ModelTriangles, size_t indexIgnored){
	
			float closetDistance = 0 ;     // reciprocal of farest distance ;
			 RayTriangleIntersection theOne ;
			 theOne.triangleIndex = -1;

			 
  for(size_t i=0 ; i < ModelTriangles.size(); i++){ // iterate all modelTriangles	
                 ModelTriangle triangle = ModelTriangles[i];

//shifting model vertixes 
for(float j =0 ; j <3 ; j++){
	triangle.vertices[j].x = triangle.vertices[j].x * HEIGHT/2 + WIDTH/2 ;
	triangle.vertices[j].y = -triangle.vertices[j].y * HEIGHT/2 + HEIGHT/2;
	triangle.vertices[j].z = triangle.vertices[j].z * HEIGHT/2 ;
}

           glm::vec3 e0 = triangle.vertices[1] - triangle.vertices[0];
           glm::vec3 e1 = triangle.vertices[2] - triangle.vertices[0];
           glm::vec3 SPVector = startPosition - triangle.vertices[0];  //
           glm::mat3 DEMatrix(-rayDirection, e0, e1);
           glm::vec3 possibleSolution = glm::inverse(DEMatrix) * SPVector;  //(t,u,v)
if(i!= indexIgnored) {
 if(possibleSolution.x > 0) {  // intersection validation check
    if((possibleSolution.y >= 0.0) && (possibleSolution.y<= 1.0)&&(possibleSolution.z >= 0.0) && (possibleSolution.z<= 1.0) && (possibleSolution.y+ possibleSolution.z) <= 1.0 ) {
      if(1/possibleSolution.x > closetDistance) {
	    	closetDistance =  1/possibleSolution.x  ;
           glm::vec3 interSectionPoint = startPosition + possibleSolution.x * rayDirection ;
        //    glm::vec3 interSectionversion2 = triangle.vertices[0] + (possibleSolution.y * e0) + (possibleSolution.z * e1) ;  //usde to check identity
            // std::cout << "1(" << interSectionPoint.x <<", " << interSectionPoint.y <<", " <<interSectionPoint.z << " )"<< std::endl; 
            // std::cout << "2(" << interSectionversion2.x <<", " << interSectionversion2.y <<", " <<interSectionversion2.z << " )"<< std::endl;        //used for debugging
            // std::cout << std::endl;                  
          RayTriangleIntersection newIntersection(interSectionPoint, possibleSolution.x , triangle, i) ;
		  theOne = newIntersection ;
	  }
	                                } 
	                             }
                               }
                           
	                   }    
						 return theOne ;
	    }


void drawWithRayTracing(DrawingWindow &window, std::vector<ModelTriangle> ModelTriangleList){
//identify light source
glm::vec3 v2(-0.64901096 ,2.7384973 ,-0.51796794) ;
glm::vec3 v4(0.650989,2.739334,0.532032) ;
glm::vec3 lightSource= v4 +v2;

lightSource.x = lightSource.x /2 * 0.35 * HEIGHT/2 + WIDTH/2;
lightSource.y =  -lightSource.y /2*  0.35 * HEIGHT/2 + HEIGHT/2;
lightSource.z = lightSource.z /2 * 0.35 * HEIGHT/2 ;	
//  std::cout << "1(" << lightSource.x <<", " << lightSource.y <<", " <<lightSource.z << " )"<< std::endl; 

          for(float y=0 ; y<HEIGHT ; y++)  {
			 for(float x=0; x <WIDTH ; x++){
				glm::vec3 fixedCameraPositionToImagePlane = glm::vec3(WIDTH/2, HEIGHT/2, cameraPosition.z *HEIGHT/2);  
                glm::vec3 rayDirection = glm::normalize(glm::vec3(x ,y ,focalLength*HEIGHT/2) - fixedCameraPositionToImagePlane)* orientation;
				glm::vec3 scalingCamera ;
				scalingCamera.x = cameraPosition.x * HEIGHT/2 + WIDTH/2 ;
				scalingCamera.y = cameraPosition.y * HEIGHT/2 + HEIGHT/2 ;
				scalingCamera.z = cameraPosition.z * HEIGHT/2 ;
RayTriangleIntersection ClosetValidIntersection = getClosestIntersection(scalingCamera,rayDirection,ModelTriangleList, -1);
	       glm::vec3 surfacePoint = ClosetValidIntersection.intersectionPoint ;
		   glm::vec3 ShadowRay = glm::normalize(lightSource - surfacePoint) ;
		    RayTriangleIntersection shadowIntersection = getClosestIntersection(surfacePoint, ShadowRay , ModelTriangleList, ClosetValidIntersection.triangleIndex) ;
// if( shadowIntersection.triangleIndex == 0|| shadowIntersection.triangleIndex ==1) {
if(shadowIntersection.triangleIndex == -1 || shadowIntersection.triangleIndex == 0|| shadowIntersection.triangleIndex ==1 ||shadowIntersection.triangleIndex == 2||shadowIntersection.triangleIndex == 3||
shadowIntersection.triangleIndex == 4|| shadowIntersection.triangleIndex == 5|| shadowIntersection.triangleIndex == 8 || shadowIntersection.triangleIndex == 9||
 shadowIntersection.triangleIndex == 11){
	uint32_t colour = (255 << 24) + (ClosetValidIntersection.intersectedTriangle.colour.red << 16) + (ClosetValidIntersection.intersectedTriangle.colour.green << 8) +ClosetValidIntersection.intersectedTriangle.colour.blue;
                       window.setPixelColour(x,y,colour) ;			
}else{
       	uint32_t colour = (255 << 24) + (ClosetValidIntersection.intersectedTriangle.colour.red/3 << 16) + (ClosetValidIntersection.intersectedTriangle.colour.green/3 << 8) +ClosetValidIntersection.intersectedTriangle.colour.blue/3;
                       window.setPixelColour(x,y,colour) ;			
}	 
		  }
}
}


// ------------------------------------------------------------------------------------------------------------------------------------------
// handle user event
void handleEvent(SDL_Event event, DrawingWindow &window)
{
	if (event.type == SDL_KEYDOWN)
	{
		if (event.key.keysym.sym == SDLK_LEFT){                             // moving cameara left
			window.clearPixels();
			std::cout << "Camera moving left" << std::endl;                        
			cameraPosition = cameraPosition - glm::vec3(0.1, 0.0, 0.0) ;
			lookAt(glm::vec3(0.0,0.0,0.0));

	    }
		else if (event.key.keysym.sym == SDLK_RIGHT){                     // moving cameara right
            window.clearPixels();
			std::cout << "Camera moving right" << std::endl;
			cameraPosition = cameraPosition + glm::vec3(0.1, 0.0, 0.0) ;  
			lookAt(glm::vec3(0.0,0.0,0.0));
		}
		else if (event.key.keysym.sym == SDLK_UP){                        // moving cameara up
		    window.clearPixels();
			std::cout << "Camera moving up" << std::endl;
			cameraPosition = cameraPosition + glm::vec3(0.0, 0.1, 0.0) ;
			lookAt(glm::vec3(0.0,0.0,0.0));
			}
		else if (event.key.keysym.sym == SDLK_DOWN){                      // moving cameara down
		    window.clearPixels();
			std::cout << "Camera moving down" << std::endl;
			cameraPosition = cameraPosition - glm::vec3(0.0, 0.1, 0.0) ;
			lookAt(glm::vec3(0.0,0.0,0.0));
	}
		else if (event.key.keysym.sym == SDLK_b)                           // moving cameara backward
		{
			window.clearPixels();
			std::cout << "Camera moving backward" << std::endl;
			cameraPosition = cameraPosition + glm::vec3(0.0, 0.0, 0.1) ;  
		}
			else if (event.key.keysym.sym == SDLK_v)                      //moving cameara forward  
		{
			window.clearPixels();
			std::cout << "Camera moving forward" << std::endl;
			cameraPosition = cameraPosition - glm::vec3(0.0, 0.0, 0.1) ;
		}
			else if (event.key.keysym.sym == SDLK_n)                     //rotate camera along x axis
		{
			window.clearPixels();
			std::cout << "Camera orientation rotating along  x axis" << std::endl;
			orientation = mX * orientation ;
			
		}
		    else if (event.key.keysym.sym == SDLK_m)                     //rotate camera orientation along y axis
		{
			window.clearPixels();
			std::cout << "Camera orientation rotating along y axis" << std::endl;
			orientation = mY * orientation ;
		}            
			else if (event.key.keysym.sym == SDLK_x)                     //rotate camera along x axis manually
		{
			window.clearPixels();
			std::cout << "Manually rotating camera along x axis" << std::endl;
			cameraPosition = mX * cameraPosition ;
			lookAt(glm::vec3(0.0,0.0,0.0)) ;
		}
			else if (event.key.keysym.sym == SDLK_y)                     //rotate camera along x axis manually
		{
			window.clearPixels();
			std::cout << "Manually rotating camera along y axis" << std::endl;
			cameraPosition = mY * cameraPosition ;	
		    lookAt(glm::vec3(0.0,0.0,0.0)) ;
		}
		else if (event.key.keysym.sym == SDLK_o)                     //switch ON/OFF for camera orbit animation along y axis
		{
		    window.clearPixels();
			AnimationSwitch = !AnimationSwitch ;
			
		}
		else if (event.key.keysym.sym == SDLK_u)                        //create new stroked triangle 
		{
			std::cout << "U : PRINTING NEW STROKED TRIANGLE" << std::endl;
			Colour newColour = createColour();
			CanvasTriangle newStrokedTriange = createTriangle();
			drawStrokedTriangle(window, newStrokedTriange, newColour);
		}
		else if (event.key.keysym.sym == SDLK_f)                       //create new filled triangle
		{
			std::cout << "F : PRINTING NEW FILLED TRIANGLE" << std::endl;
			Colour newColour = createColour();
			CanvasTriangle newFilledTriangle = createTriangle();
			drawFilledTriangle(window, newFilledTriangle, newColour);
		}
		else if (event.key.keysym.sym == SDLK_c)                      // clear pixcels
		{
			std::cout << "C : CLEARING THE PIXCELS" << std::endl;
			window.clearPixels();
		}
			else if (event.key.keysym.sym == SDLK_i)                      // Initialize camera ;
		{
			std::cout << "i : Initializing camera" << std::endl;
			orientation = glm::mat3(1.0, 0.0, 0.0,                                               // camera orientation points at world origin             
	              0.0,1.0,0.0,
				  0.0,0.0,1.0
	        );
          cameraPosition = glm::vec3(0.0, 0.0, 4.0) ; //4,0                                  
          focalLength= 2.0 ;  
	               //2.0
	
		
		}
		else if (event.key.keysym.sym == SDLK_1)                      // Texture Map drawing
		{
			std::cout << "Operation 1: Texture Map drawing" << std::endl;
			choice = 1 ;
		}
		}
		else if (event.key.keysym.sym == SDLK_2)                      // Stroked Triangle drawing
		{
			std::cout << "Operation 2: Stroked Triangle drawing" << std::endl;
			choice = 2 ;
		}
		else if (event.key.keysym.sym == SDLK_3)                      // Filled Triangle drawing
		{
			std::cout << "Operation 3: Filled Triangle drawing" << std::endl;
			choice = 3 ;
		}
		else if (event.key.keysym.sym == SDLK_4)                      // Point Cloud render
		{
			std::cout << "Operation 4: Point Cloud drawing" << std::endl;
			choice = 4 ;
		}
		else if (event.key.keysym.sym == SDLK_5)                      // Wireframe render
		{
			std::cout << "Operation 5: Wireframe drawing" << std::endl;
			choice = 5 ;
		}
		else if (event.key.keysym.sym == SDLK_6)                      // 3D Rasterising render
		{
			std::cout << "Operation 6: 3D Rastering drawing(Occlusion Problem Solved)" << std::endl;
			choice = 6 ;
		}
		else if (event.key.keysym.sym == SDLK_7)                      // 3D Ray Tracing & Shadowing render
		{
			AnimationSwitch = true ;
			std::cout << "Operation 7: 3D Ray tracing with shadow drawing" << std::endl;
			choice = 7 ;
		
		
	}

	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

// main
int main(int argc, char *argv[])
{
     Colour white(255,255,255) ;

	//---------------------------------------------------------------------------
	// Week5 Area 
    // every parameter for week5 are set to global values
	

    //---------------------------------------------------------------------------
	// Week4 Area 
	//Week4 Task3 read colour
       std::vector<Colour> ColourPalette = readMTLMaterialFile("cornell-box.mtl");
	    std::cout << "There are : " << ColourPalette.size() << " colours returned "<< std::endl; 
	    //  for (size_t i = 0; i < ColourPalette.size(); i++) {
	// 	std::cout << ColourPalette[i] << std::endl; 
    //    }
	//    std::cout << std::endl;
       


	//Week4 Task2 read geometry
		std::vector<ModelTriangle> ModelTriangleList = readOBJGeometryFile("cornell-box.obj", 0.35, ColourPalette);
	//     for (size_t i = 0; i < ModelTriangleList.size(); i++) {
	// 	std::cout << ModelTriangleList[i] <<ModelTriangleList[i].colour<< std::endl; 
	// 		std::cout << std::endl; 
    //    }
        std::cout << "There are : " << ModelTriangleList.size() << " modelTriangles returned "<< std::endl; 

	//Week4 Task6: obj vertices:
	std::vector<glm::vec3> vertices = readOBJGeometryFileVetcies("cornell-box.obj", 0.35) ;
	 std::cout << "There are : " << vertices.size() << " vertices returned "<< std::endl; 
	 std::cout <<  std::endl; 


	//---------------------------------------------------------------------------
	// Week6 Area (Task2 and 3)
    // glm::vec3 cameraShift = glm::vec3(cameraPosition.x + WIDTH/2, cameraPosition.y + HEIGHT/2, cameraPosition.z*HEIGHT/3); 
   // glm::vec3 rayDirection = glm::normalize(glm::vec3(WIDTH, HEIGHT, (cameraShift.z)/2) - cameraShift) ;
   // RayTriangleIntersection testing = getClosestIntersection(cameraShift , rayDirection , ModelTriangleList);

	//---------------------------------------------------------------------------
	// Week3 Area
	//parameters for week3 task5 TextureMapping
	CanvasPoint v1(160.0, 10.0) ;
	CanvasPoint v2(300.0, 230.0) ;
	CanvasPoint v3(10.0, 150.0) ;
	TexturePoint t1(195.0, 5.0);
	TexturePoint t2(395.0, 380.0) ; 
	TexturePoint t3(65.0 , 330.0) ;
	v1.texturePoint = t1 ;
    v2.texturePoint = t2 ;
	v3.texturePoint = t3 ;
	CanvasTriangle textureTriangle(v1,v2,v3);
	TextureMap textureMap1("texture.ppm") ;
	
	
	// parameters for week3
	CanvasTriangle Triangle = createTriangle();
	Colour colours = createColour();

	// parameters for week3 task2 to test line drawing
	CanvasPoint topLeft(0.0, 0.0);
	CanvasPoint centre((WIDTH - 1) / 2, (HEIGHT - 1) / 2);
	CanvasPoint topRight(WIDTH - 1, 0.0);
	CanvasPoint middleVerticalTop((WIDTH - 1) / 2, 0.0);
	CanvasPoint middleVerticalBottom((WIDTH - 1) / 2, HEIGHT - 1);
	CanvasPoint thirdHoriLeft((WIDTH / 3) - 1, (HEIGHT - 1) / 2);
	CanvasPoint thirdHoriRight((WIDTH / 3 * 2) - 1, (HEIGHT - 1) / 2);
	Colour colourWeek3Task2(255, 255, 255);


	//---------------------------------------------------------------------------
	// Week2 Area
	// code used for testing functionality of interpolateSingleFloats !
	std::vector<float> result1;
	result1 = interpolateSingleFloats(2.2, 8.5, 7);
	// for (size_t i = 0; i < result1.size(); i++)
	// 	std::cout << result1[i] << " ";
	// std::cout << std::endl;

	// code used for testing functionality of interpolateThreeElementValues !
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	std::vector<glm::vec3> result2;
	result2 = interpolateThreeElementValues(from, to, 4);
	// for (size_t i = 0; i < result2.size(); i++)
	// {
	// 	std::cout << "(" << result2[i].x << ", " << result2[i].y << ", " << result2[i].z << ")" << std::endl;
	// }
	// std::cout << std::endl;
//---------------------------------------------------------------------------------------------------------------------------------------------------

	// Draw SDL window
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;

	// infinite loop for drawing
	window.clearPixels();
	while (true)
	{    
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event))
		{
			handleEvent(event, window);
		}
		switch (choice)
		{
		case 1: 
		    window.clearPixels();
			drawTexturedTriangle(window,textureTriangle,textureMap1) ; //week3 Task6
			break;

		case 2: 
		       window.clearPixels();
			   // parameters for week3
	             Triangle = createTriangle();
                 colours = createColour();
			   	drawStrokedTriangle(window,Triangle,colours) ;   //week3 task3
			break;

		case 3: 
		       window.clearPixels();
			     Triangle = createTriangle();
                 colours = createColour();
			   	drawFilledTriangle(window, Triangle, colours );    //week3 task 4
			break;

		case 4: 
		     window.clearPixels();
		     if(AnimationSwitch){
				lookAt(glm::vec3(0.0,0.0,0.0));
			     pointCloudRender(window,vertices,cameraPosition,focalLength) ;     // week4 Task6
			 }  else{
					lookAt(glm::vec3(0.0,0.0,0.0));
		            window.clearPixels();
					    pointCloudRender(window,vertices,cameraPosition,focalLength) ;     // week4 Task6
			        cameraPosition = mY * cameraPosition ;
			 }                                                                                     
			break;
		case 5: 
		        window.clearPixels();
		        if(AnimationSwitch){
				lookAt(glm::vec3(0.0,0.0,0.0));
			      wireframeRender(window,ModelTriangleList,cameraPosition,focalLength)  ;  // week4 Task7
			 }  else{
					lookAt(glm::vec3(0.0,0.0,0.0));
		            window.clearPixels();
				   wireframeRender(window,ModelTriangleList,cameraPosition,focalLength)  ;  // week4 Task7
			        cameraPosition = mY * cameraPosition ;
			 }   
			break;

		case 6: 
		       window.clearPixels();
		       if(AnimationSwitch){
			    rasterisedRenderAdvanced(window,ModelTriangleList,cameraPosition,focalLength); //week4 Task9
			 } else{
		            window.clearPixels();
				    rasterisedRenderAdvanced(window,ModelTriangleList,cameraPosition,focalLength);
			        cameraPosition = mY * cameraPosition ;
					lookAt(glm::vec3(0.0,0.0,0.0));
			 }  
		
			break;


		case 7:                   //week6 Task6
		if(AnimationSwitch){
		lookAt(glm::vec3(0.0,0.0,0.0));
		drawWithRayTracing(window, ModelTriangleList) ;
		}
		else{
			
		    window.clearPixels();
			drawWithRayTracing(window, ModelTriangleList) ;
			cameraPosition = mY * cameraPosition ;
			lookAt(glm::vec3(0.0,0.0,0.0));
		}

			break;
		}
		
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}