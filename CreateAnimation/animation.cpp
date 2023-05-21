#include <iostream>
#include <fstream>
#include <cstring>
#include <windows.h>

using namespace std;

struct Image {
	unsigned char descriptor [10];
	unsigned char colourTable [768];
	unsigned char data [1000000];
	int sizeData;
};

struct AnimFrame {
	Image * image;
	int duration;
	AnimFrame * next;
};

struct Animation {
	AnimFrame * first;
	AnimFrame * last;
};


Image * readImage (char filename[]);
void saveImage (Image * image, char filename[]);
Image * copyImage (Image * image);
Image * applyEffectImage (Image * image, string effect);

Animation * initAnimation ();
void addFrame (Animation * animation, Image * image, int duration);
void saveAnimation (Animation * animation, char filename[]);


Image * readImage (char filename[]) 
{
	ifstream inputGif;
	
	unsigned char gif;
	
	Image * image;
	image = new Image;
	
	inputGif.open(filename, ios::binary | ios::in);
	if(!inputGif.is_open())  
	{
		cout << "Error  opening " << filename << "  Aborting...";
	}
	
	inputGif.seekg(13, ios::beg);  		// places pointer back to beginning, then skip 13 bytes
	
	for (int x = 0; x < 10; x++)
	{
		inputGif.read((char*)&gif, sizeof(char));
		image->descriptor[x] = gif;
	}
	
	for (int i = 0; i < 768; i++)
	{
		inputGif.read((char*)&gif, sizeof(char));
		image->colourTable[i] = gif;
	}
	
	int q = 0;
	inputGif.read((char*)&gif, sizeof(char));
	
	while(!inputGif.eof())
	{
		image->data[q] = gif;
		q++;
		
		inputGif.read((char*)&gif, sizeof(char));
	}
	
	q = q-1; 			// to "delete" the last byte
	
	image->sizeData = q;
	
	return image;
}


void saveImage (Image * image, char filename[]) 
{
	ofstream output;
	
	char five9 = {59};
	char header[6] = {71, 73, 70, 56, 57, 97};
	char LSD[7] = {88, 2, 144, 1, 112, 0, 0};
	
	LSD[0] = image->descriptor[5];  // set width of canvas to match image     
	LSD[1] = image->descriptor[6];      
	LSD[2] = image->descriptor[7];  // set height of canvas to match image     
	LSD[3] = image->descriptor[8]; 
	
	output.open(filename, ios::binary | ios::out);
	if (!output.is_open())  
	{
		cout << "Error  opening " << filename << " Aborting..."; 
	}
	
	for (int x = 0; x < 6; x++)
	{
		output.write((char*)&header[x], sizeof(char));
	} 
	
	for (int x = 0; x < 7; x++)
	{
		output.write((char*)&LSD[x], sizeof(char));
	}
	
	for(int x = 0; x < 10; x++)
	{
		output.write((char*)&image->descriptor[x], sizeof(char));
	}
	
	for(int x = 0; x < 768; x++)
	{
		output.write((char*)&image->colourTable[x], sizeof(char));
	}
	
	for(int x = 0; x < image->sizeData; x++)
	{
		output.write((char*)&image->data[x], sizeof(char));
	}
	
	output.write((char*)&five9, sizeof(char));	
}


void toGrayScale (int * red, int * green, int * blue) {

	int avg;
	
	avg = (*red + *green + *blue) / 3;
	
	*red = *green = *blue = avg;
}


void toBlueTint (int * red, int * green, int * blue) {

	int avg;
	
	avg = (*red + *green + *blue) / 3;
	
	*red = *green = avg;
}


void toRedTint (int * red, int * green, int * blue) {

	int avg;
	
	avg = (*red + *green + *blue) / 3;
	
	*green = *blue = avg;
}


void toSepia (int * red, int * green, int * blue) {

	int newRed = (int) (*red * 0.393 + * green * 0.769 + *blue * 0.189);
	int newGreen = (int) (*red * 0.349 + *green * 0.686 + *blue * 0.168);
	int newBlue = (int) (*red * 0.272 + *green * 0.534 + *blue * 0.131);
	
	if (newRed > 255)
		newRed = 255;
		
	if (newGreen > 255)
		newGreen = 255;

	if (newBlue > 255)
		newBlue = 255;
		
	*red = newRed;
	*green = newGreen;
	*blue = newBlue;
}


void brighten (int * red, int * green, int * blue, int percent) {

	*red = (int) (*red * (100 + percent) / 100.0);
	*green = (int) (*green * (100 + percent) / 100.0);
	*blue = (int) (*blue * (100 + percent) / 100.0);
	
	if (*red > 255)
		*red = 255;
		
	if (*green > 255)
		*green = 255;

	if (*blue > 255)
		*blue = 255;
}


Image * copyImage (Image * image) 
{
	Image * image1;
	image1 = new Image;
	
	for (int x = 0; x < 10; x++)
	{
		image1->descriptor[x] = image->descriptor[x];
	}
	
	for (int i = 0; i < 768; i++)
	{
		
		image1->colourTable[i] = image->colourTable[i];
	}
	
	for(int q = 0; q < image->sizeData ; q++)
	{
		image1->data[q] = image->data[q];
	}
	
	image1->sizeData = image->sizeData;
	
	return image1;
}


Image * applyEffectImage (Image * image, string effect) 
{
	Image * copyImage1;
	copyImage1 = new Image;
	copyImage1 = copyImage(image);
	
	int red, green, blue, x = 0;
	
	unsigned char num;
	
	if (effect == "sepia")
	{
		while(x < 768)
		{
			red = (int) copyImage1->colourTable[x];
			
			x++;
			green = (int) copyImage1->colourTable[x];
			
			x++;
			blue = (int) copyImage1->colourTable[x];
			
			toSepia(&red, &green, &blue);
			
			num = (char) red;
			copyImage1->colourTable[x - 2] = num;
			
			num = (char) green;
			copyImage1->colourTable[x - 1] = num;
			
			num = (char) blue;
			copyImage1->colourTable[x] = num;
			
			x++;
		}
	}
	else if (effect == "blueTint")
	{
		while(x < 768)
		{
			red = (int) copyImage1->colourTable[x];
			
			x++;
			green = (int) copyImage1->colourTable[x];
			
			x++;
			blue = (int) copyImage1->colourTable[x];
			
			toBlueTint(&red, &green, &blue);
			
			num = (char) red;
			copyImage1->colourTable[x - 2] = num;
			
			num = (char) green;
			copyImage1->colourTable[x - 1] = num;
			
			num = (char) blue;
			copyImage1->colourTable[x] = num;
			
			x++;
		}
	}
	else if (effect == "redTint")
	{
		while(x < 768)
		{
			red = (int) copyImage1->colourTable[x];
			
			x++;
			green = (int) copyImage1->colourTable[x];
			
			x++;
			blue = (int) copyImage1->colourTable[x];
			
			toRedTint(&red, &green, &blue);
			
			num = (char) red;
			copyImage1->colourTable[x - 2] = num;
			
			num = (char) green;
			copyImage1->colourTable[x - 1] = num;
			
			num = (char) blue;
			copyImage1->colourTable[x] = num;
			
			x++;
		}
	}
	else if (effect == "grayScale")
	{
		while(x < 768)
		{
			red = (int) copyImage1->colourTable[x];
			
			x++;
			green = (int) copyImage1->colourTable[x];
			
			x++;
			blue = (int) copyImage1->colourTable[x];
			
			toGrayScale(&red, &green, &blue);
			
			num = (char) red;
			copyImage1->colourTable[x - 2] = num;
			
			num = (char) green;
			copyImage1->colourTable[x - 1] = num;
			
			num = (char) blue;
			copyImage1->colourTable[x] = num;
			
			x++;
		}
	}
	else if (effect == "brighten")
	{
		while(x < 768)
		{
			red = (int) copyImage1->colourTable[x];
			
			x++;
			green = (int) copyImage1->colourTable[x];
			
			x++;
			blue = (int) copyImage1->colourTable[x];
			
			brighten(&red, &green, &blue, 200);
			
			num = (char) red;
			copyImage1->colourTable[x - 2] = num;
			
			num = (char) green;
			copyImage1->colourTable[x - 1] = num;
			
			num = (char) blue;
			copyImage1->colourTable[x] = num;
			
			x++;
		}
	}
	return copyImage1;
	
}


Animation * initAnimation () 
{
	Animation * newAnimation;
	newAnimation = new Animation;

	newAnimation->first = NULL;
	newAnimation->last = NULL;
	
	return newAnimation;
}


void addFrame (Animation * animation, Image * image, int duration) 
{
	AnimFrame * animFrame;
	animFrame = new AnimFrame;
	
	animFrame->image = image;
	animFrame->duration = duration;
	animFrame->next = NULL;
	
	if (animation->first == NULL && animation->last == NULL)
	{
		animation->first = animFrame;
		animation->last = animFrame;
	}
	else
	{
		animation->last->next = animFrame;
		animation->last = animFrame;
	}
	
}
	

void saveAnimation (Animation * animation, char filename[]) 
{
	AnimFrame * top;
	top = new AnimFrame;
	
	top = animation->first;
	
	Image * image;
	image = new Image;
	
	ofstream output;
	
	char five9 = {59};
	char header[6] = {71, 73, 70, 56, 57, 97};
	char LSD[7] = {88, 2, 144, 1, 112, 0, 0};
	char appEx[19] = {33, 255, 11, 78, 69, 84, 83, 67, 65, 80, 69, 50, 46, 48, 3, 1, 0, 0, 0};
	char GCE[8] = {33, 249, 4, 8, 100, 0, 0, 0};
	
	unsigned char num;
	
	LSD[0] = animation->first->image->descriptor[5];  // set width of canvas to match image     
	LSD[1] = animation->first->image->descriptor[6];      
	LSD[2] = animation->first->image->descriptor[7];  // set height of canvas to match image     
	LSD[3] = animation->first->image->descriptor[8]; 
	
	output.open(filename, ios::binary | ios::out);
	if (!output.is_open())  
	{
		cout << "Error  opening " << filename << " Aborting..."; 
	}
	
	for (int x = 0; x < 6; x++)
	{
		output.write((char*)&header[x], sizeof(char));
	} 
	
	for (int x = 0; x < 7; x++)
	{
		output.write((char*)&LSD[x], sizeof(char));
	}
	
	for (int x = 0; x < 19; x++)
	{
		output.write((char*)&appEx[x], sizeof(char));
	}

	while(top != NULL)
	{
		image = top->image;
		
		num = (char) top->duration;
		GCE[4] = num;
		
		for (int x = 0; x < 8; x++)
		{
			output.write((char*)&GCE[x], sizeof(char));
		}
		
		for(int x = 0; x < 10; x++)
		{
			output.write((char*)&image->descriptor[x], sizeof(char));
		}
		
		for(int x = 0; x < 768; x++)
		{
			output.write((char*)&image->colourTable[x], sizeof(char));
		}
		
		for(int x = 0; x < image->sizeData; x++)
		{
			output.write((char*)&image->data[x], sizeof(char));
		}
		
		top = top->next;
	}
	
	output.write((char*)&five9, sizeof(char));
} 


// This function displays a Web page on the default browser (Windows only)

void showWebPage (string URL) {
	char cURL [50];
	int i;

	for (i = 0; i < URL.length(); i=i+1){
    	cURL[i] = URL[i];
	}

	cURL[URL.length()] = '\0';
    
    ShellExecute(NULL, "open", cURL, NULL, NULL, SW_SHOWNORMAL);
}


int main() 
{
	ifstream input;
	
	Image * image1;
	image1 = new Image;
	
	Image * image2;
	image2 = new Image;
	
	Image * image3;
	image3 = new Image;
	
	Image * image4;
	image4 = new Image;
	
	Image * image5;
	image5 = new Image;
	
	Image * image1Effect;
	image1Effect = new Image;
	
	Image * image2Effect;
	image2Effect = new Image;
	
	Image * image3Effect;
	image3Effect = new Image;
	
	Image * image4Effect;
	image4Effect = new Image;
	
	Image * image5Effect;
	image5Effect = new Image;
	
	char gifName[25];

	char webPage[50];	
	strcpy (webPage, "TwoDayAssessment.html");
	
	char gif1 [25];
	strcpy (gif1, "Maracas.gif");
	char newGif1[25];
	strcpy(newGif1, "Maracas-S.gif");
	
	char gif2 [25];
	strcpy (gif2, "MoraineLake.gif");
	char newGif2[25];
	strcpy(newGif2, "MoraineLake-BT.gif");	
	
	char gif3 [25];
	strcpy (gif3, "PigeonPoint.gif");
	char newGif3[25];
	strcpy(newGif3, "PigeonPoint-RT.gif");	

	char gif4 [25];
	strcpy (gif4, "Saskatoon.gif");	
	char newGif4[25];
	strcpy(newGif4, "Saskatoon-GS.gif");	
	
	char gif5 [25];
	strcpy (gif5, "ScarletIbis.gif");
	char newGif5[25];
	strcpy(newGif5, "ScarletIbis-B.gif");
	
	char animScene [25];
	strcpy(animScene, "Scenery-Animated.gif");		

	// applies effects to each gif
	
	image1 = readImage(gif1);
	image1Effect = applyEffectImage(image1, "sepia");
	saveImage(image1Effect, newGif1);
	
	image2 = readImage(gif2);
	image2Effect = applyEffectImage(image2, "blueTint");
	saveImage(image2Effect, newGif2);
	
	image3 = readImage(gif3);
	image3Effect = applyEffectImage(image3, "redTint");
	saveImage(image3Effect, newGif3);
	
	image4 = readImage(gif4);
	image4Effect = applyEffectImage(image4, "grayScale");
	saveImage(image4Effect, newGif4);
	
	image5 = readImage(gif5);
	image5Effect = applyEffectImage(image5, "brighten");
	saveImage(image5Effect, newGif5);
	
	Animation * Scenary;
	Scenary = initAnimation();
	
	addFrame(Scenary, image1, 250);
	addFrame(Scenary, image2Effect, 250);
	addFrame(Scenary, image3, 250);
	addFrame(Scenary, image1Effect, 250);
	addFrame(Scenary, image4, 250);
	
	saveAnimation(Scenary, animScene);
	
	
	// creating deer animation
	
	Image * deer[12];
	for (int x = 0; x < 12; x++)
		deer[x] = new Image;
	
	Animation * RunningDeer;
	RunningDeer = initAnimation();
	
	char deer0[25];
	strcpy(deer0, "Deer0.gif");
	deer[0] = readImage(deer0);
	
	char deer1[25];
	strcpy(deer1, "Deer1.gif");
	deer[1] = readImage(deer1);
	
	char deer2[25];
	strcpy(deer2, "Deer2.gif");
	deer[2] = readImage(deer2);

	char deer3[25];
	strcpy(deer3, "Deer3.gif");
	deer[3] = readImage(deer3);
	
	char deer4[25];
	strcpy(deer4, "Deer4.gif");
	deer[4] = readImage(deer4);

	char deer5[25];
	strcpy(deer5, "Deer5.gif");
	deer[5] = readImage(deer5);
	
	char deer6[25];
	strcpy(deer6, "Deer6.gif");
	deer[6] = readImage(deer6);
	
	char deer7[25];
	strcpy(deer7, "Deer7.gif");
	deer[7] = readImage(deer7);
	
	char deer8[25];
	strcpy(deer8, "Deer8.gif");
	deer[8] = readImage(deer8);
	
	char deer9[25];
	strcpy(deer9, "Deer9.gif");
	deer[9] = readImage(deer9);
	
	char deer10[25];
	strcpy(deer10, "Deer10.gif");
	deer[10] = readImage(deer10);
	
	char deer11[25];
	strcpy(deer11, "Deer11.gif");
	deer[11] = readImage(deer11);
	
	for(int i = 0; i < 12; i++)
	{
		addFrame(RunningDeer,deer[i], 4);
	}
	
	char animDeer[25];
	strcpy(animDeer, "RunningDeer.gif");
	
	saveAnimation(RunningDeer, animDeer);
	
	// display images created on a browser
	
	showWebPage(webPage);

	cout << endl;
	cout << "End of program.";
	
	return 0;
}

