#include <GL/glut.h>
#include <vector>
#include "../include/controller.h"
#include "../include/model.h"
#include "../include/view.h"
#include "../include/trackball.h"

using namespace std;

Model * View:: model;
Controller * View:: controller;
Trackball View:: trackball;


void Controller:: init(int argc, char ** argv)
{
	View view;
	Model model;
	
	
	if(argc >= 2)
	{
		model.readPly(argv[2]);
		model.readPly(argv[1]);
		model.readPly(argv[1]);
		model.readPly(argv[3]);
		
		model.computeObjects();
	}
	else
	{
		cout << "Usage: ./assign2 <filenames> " << endl;
		exit(0);
	}

	if (argc >= 8) {
		model.readTex(argv[4], argv[5], argv[6], argv[7], argv[8]);
	}
	else
		model.readTex("../textures/wood.jpg", "../textures/boost.jpg", "../textures/lays.jpg", "../textures/earth.jpg", "../textures/earth.jpg" );
	
	this->model = &model;
	view.model = &model;
	
	View::controller = this;
	
	model.setTrackball (&view.trackball);
	
	view.init(argc, argv);
}

void Controller::MoveStopObjects() {
	model->MoveStopObjects();
}
void Controller::zoom(float scale_x) {
	model->zoomCam(model->getCamZoom() + scale_x);
}


void Controller::pickPoint(float nx, float ny, float nz) {
	model->pickPoint(nx, ny, nz);
}

void Controller::defaultTexToggle() {
	model->defaultTexToggle();
}

void Controller::increaseSpeed(int obj) { 
	model->increaseSpeed(obj);
}

void Controller::decreaseSpeed(int obj) {
	model->decreaseSpeed(obj);
}

void Controller::normalColor() {
	model->normalColor();
}

void Controller::filledPolygon() {
	model->filledPolygon();
}


void Controller::moveShark() {
	model->moveShark();
}

