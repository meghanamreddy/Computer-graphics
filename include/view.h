#include "model.h"
#include "controller.h"
# include "trackball.h"
# ifndef View_H
# define View_H


class View {
	public:
		static Model* model;
		static Controller* controller;
		static Trackball trackball;
		void init(int, char **);
		static void display();
		static void mouse(int, int, int, int);
		static void motion(int, int);
		static void keyboard(unsigned char, int, int);
		static void reshape(int, int);
		static void specialKeys(int, int , int);
		static void idle();

};





#endif
