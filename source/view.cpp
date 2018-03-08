#include <GL/glut.h>
#include <GL/glui.h>
#include <vector>
#include <iostream>
#include "../include/view.h"
#include "../include/controller.h"
#include "../include/model.h"
# include "../include/trackball.h"


using namespace std; 
vector<Point> mouse_points;
vector<Point> delete_points;
double w, h;

GLdouble posX, posY=20, posZ ;
int mouse_x, mouse_y ;

/** These are the live variables passed into GLUI ***/
int   headlight = 1;
int spotlight1 = 1;
int spotlight2 = 1;
int axes = 1;
int boundingbox = 0;
int texture = 1;
int   main_window;
GLUI_RadioGroup *group1;

/* Everytime the display is updated, the model has to be rendered */
void View::display(void)
{
	int k,n=5,i, sz;
	float x,y,u;
	GLfloat x1, y1;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();
	
	model->render(headlight, spotlight1, spotlight2, axes, boundingbox, texture,  group1->get_int_val());
	
	glPopMatrix();
	
	glFlush();
	glutSwapBuffers();
}


/* Translates the xyz coordinates of window to frame */
void GetOGLPos(int x, int y, double & posX, double & posY, double & posZ)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
 
    glGetDoublev( GL_MODELVIEW_MATRIX, modelview );
    glGetDoublev( GL_PROJECTION_MATRIX, projection );
    glGetIntegerv( GL_VIEWPORT, viewport );
 
    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels( x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
 
    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
}



int old_x = 0;
int old_y = 0;
int valid = 0;


void View::idle( void )
{
  /* According to the GLUT specification, the current window is
     undefined during an idle callback.  So we need to explicitly change
     it if necessary */
  if ( glutGetWindow() != main_window )
    glutSetWindow(main_window);

  glutPostRedisplay();
}

/* Rotating the screen using mouse clicks */
void View::mouse (int button, int state, int x, int y) 
{

	double nx, ny, nz;	
	GetOGLPos(x, y, nx, ny, nz);

	old_x=x; 
	old_y=y;
	valid = state == GLUT_DOWN;
	if(valid)
		trackball.rotate( 0,0,0,0 ) ;
	switch(button) {
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN) {
			controller->pickPoint((float)nx, (float)ny, (float)nz);
		}
	}


}

/* Rotating the screen using mouse clicks */
void View::motion(int x, int y) 
{
	if (valid) 
	{	
		trackball.rotate((2.0 * old_x - w)/w,
						 (h - 2.0 * old_y)/h,
						 (2.0 * x - w)/w,
						 (h - 2.0 * y)/h
						);
	}
	old_x = x;
	old_y = y;
	
	glutPostRedisplay();
}

/* Actions on different keyboard inputs */
void View::keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		
      		case '+':
			cout << " vieeee " << endl;
			controller->zoom(-0.5);
			glutPostRedisplay();
	 		break;
		case '-':
			controller->zoom(0.5);
			glutPostRedisplay();
			break;
		
		case 'm':
			controller->MoveStopObjects();
			break;
		case 'q':
			controller->increaseSpeed(0);
			break;
		case 'w':
			controller->decreaseSpeed(0);
			break;
		case 'a':
			controller->increaseSpeed(1);
			break;
		case 's':
			controller->decreaseSpeed(1);
			break;
		case 'x':
			controller->moveShark();
			break;

		default:
        	break;
	}
	glutPostRedisplay();
}


void View::specialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_PAGE_DOWN)
		exit(0);
	glutPostRedisplay();
}

/* Initialisation of glut window and setting up the objects on screen */
void View::init (int argc, char ** argv)
{
	/* Set window specifications */
	glutInit(&argc, argv);
    	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    	glutInitWindowSize(700,700);
    	glutInitWindowPosition(50,50);
    	main_window = glutCreateWindow("Assignment 5");
    
    	/* Set perspective */
    	glMatrixMode( GL_PROJECTION );
    	glLoadIdentity();    
    	::w = glutGet( GLUT_WINDOW_WIDTH );
    	::h = glutGet( GLUT_WINDOW_HEIGHT );
    	gluPerspective( 60, w / h, 0.1, 20 );
	glMatrixMode( GL_MODELVIEW );
	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	/* Initialise the models */
	model->initlights() ;
	model->inittexture();
	model->inittree();

	glutDisplayFunc(View::display); 
	glutReshapeFunc(View::reshape); 
   	glutKeyboardFunc(View::keyboard);
	glutMouseFunc(View::mouse);
	glutSpecialFunc(View::specialKeys);

    	glutMotionFunc(motion);
    	glEnable( GL_DEPTH_TEST );

	/* Additional features on the screen */
	GLUI *glui = GLUI_Master.create_glui( "GLUI" );
  	glui->add_checkbox( "Headlights", &headlight );
	glui->add_checkbox( "Spotlight1", &spotlight1 );
	glui->add_checkbox( "Spotlight2", &spotlight2 );
	glui->add_checkbox( "Axes", &axes );
	glui->add_checkbox( "BoundingBox", &boundingbox );
	glui->add_checkbox( "Texture", &texture );
	glui->add_separator();
	group1 = glui->add_radiogroup();
	glui->add_radiobutton_to_group(group1,"Global");
	glui->add_radiobutton_to_group(group1,"Object A");
	glui->add_radiobutton_to_group(group1,"Object C");

  	glui->set_main_gfx_window( main_window );

  	/* We register the idle callback with GLUI, *not* with GLUT */
  	GLUI_Master.set_glutIdleFunc( View::idle );

    	glutMainLoop();       
}	

/* When the output window's size is changed */
void View::reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (h==0)
      		gluPerspective(60, (float) w, 0.1, 500.0);
   	else
      		gluPerspective(60, (float) w / (float) h, 0.1, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

