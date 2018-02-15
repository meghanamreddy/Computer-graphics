#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "../include/view.h"
#include "../include/controller.h"
#include "../include/model.h"

using namespace std;



int main(int argc, char* argv[]) {
	cout << endl;
	cout << "Press key 'm' to start the motion." << endl;
	cout << "Use keys q,w,a,s to increase/decrease speeds." << endl;
	cout << "Press key 'x' to move the alien to a different planet." << endl;
	cout << endl;
	Controller controller;
	controller.init(argc, argv);
	

	return 0;   
}



