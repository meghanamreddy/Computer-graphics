30 April 2017
Meghana M Reddy
IMT2013025
Computer Graphics - Assignment 5

--------------------
FOLDER STRUCTURE:
The source files are in folder named "source".
The .h files are in folder named "include".
To compile, run the make command (Makefile is included in the main folder).
After compiling, the compiled executable appears in the folder named "bin".
The screenshots of output images are included in the folder "images".
The texture images are included in the folder "textures".
The images are named accordingly to indicate Default texture/ manual, Headlight enabled/disabled, Spotlight enabled/disabled.

-------------------
TO COMPILE :
-> Run "make" to compile.
-> Run "make clean" to delete the compiled file.


-------------------
TO RUN (in the "bin" folder, type - )
./assign5 <filename1> <filename2> <filename3> <floortexture file> <texture1 file> <texture2 file> <texture3 file> <alien texture file>

./assign5 ../input/apple.ply ../input/trashcan.ply ../input/shark.ply ../textures/galaxy.jpg ../textures/planet2.png ../textures/earth3.jpg ../textures/venus.jpg ../textures/saturn.jpg

The value of the 2nd to 4th arguments is a string which is the path to the ply file -  'tennis_shoe.ply' or 'airplane.ply' - to indicate which ply file to be used for the three objects.
Available ply files in input folder -
1. shark.ply
2. airplane.ply
3. tennis_shoe.ply
4. pickup_big.ply
5. trashcan.ply
6. cube.ply
7. apple.ply


-------------------
GLUI / check boxes:
A number of check boxes are provided for added functionality. The available check boxes are -
1. Headlights - turn off or on.
2. Spotlight1 - turn off or on the first spotlight.
3. Spotlight1 - turn off or on the second spotlight.
4. texture - enable/disable texture on all object.
7. Axes - enable/disable the rendering of the axes.
8. Bounding box - enable/disable the rendering of the bounding box.
9. Camera - global / object A / object C - allows the different positions of the camera.


-------------------
INPUT ACTIONS (translation/rotation/zoom/rendering):
-> To zoom in or out, use the keys +/-
	'+' to zoom in, '-' to zoom out.
-> Use mouse drag to rotate around the screen when the camera is in global position.
-> Use the button 'm' to stop/start the motion of all the objects. 
-> Use the buttons 'q' and 'w' to increase/decrease the speed of the green planet.
-> Use the buttons 'a' and 's' to increase/decrease the speed of the blue planet / earth.
-> Use the button 'x' to move the alien/shark from one object to another. An alternate method to this is to choose the object by right clicking on it with mouse.


RENDERING OPTIONS:
-> Use RIGHT CLICK of the mouse to select the object on which we want to move the lizard/frog/chameleon.
Additionally, use the checkboxes available on the screen.


--------------------
OUTPUT:
-> The axes is drawn using red, green and blue colors for x, y and z axis respectively.
-> The bounding box of the objects are rendered in white.
-> The spotlights are shown using different colors. One of the spotlights follows the green planet.
-> Three objects (two spherical, and one cylindrical) are rendered.
-> A moving object (a shark) is "attached" to one of the objects and can be moved from one object to another.


-------------------
SOURCES:
The ply files were taken from - https://people.sc.fsu.edu/~jburkardt/data/ply/ply.html
Though the ply reader was implemented from scratch, I referred to the following websites for the format of ply files -
http://paulbourke.net/dataformats/ply/
https://people.sc.fsu.edu/~jburkardt/data/ply/ply.html

Fast Track ball code was the code that was provided to seniors (Computer Graphics 2016 students).

Used SOIL library to load texture images.

Texture files were taken from Google images.
GLUI was used to implement the functionality of checkboxes - http://www-h.eng.cam.ac.uk/help/tpl/graphics/using_glui.html






