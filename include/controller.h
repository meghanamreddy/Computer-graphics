#include "model.h"
# ifndef Controller_H
# define Controller_H


class Controller {
	public:
	Model* model;
	void rotate(float, int);
	void translate(float);
	void zoom(float);
	void delete_point(float, float);
	void defaultTexToggle();
	void normalColor();
	void filledPolygon();
	void init(int, char **);
	void pickPoint(float nx, float ny, float nz);
	void decreaseSpeed(int a);
	void increaseSpeed(int a);
	void MoveStopObjects();
	void moveShark();
	

};


#endif
