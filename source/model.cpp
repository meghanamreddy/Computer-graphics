#include <GL/glut.h>
#include <iterator>
#include <iostream>
# include <string.h>
#include <fstream>
#include <sstream>
# include "SOIL/SOIL.h"
#include <vector>
#include <string>
#include <cmath>
#include "../include/model.h"
# include "../include/trackball.h"

using namespace std;

Vertex translatePoint(float x, float y, float z, float alphax, float alphay, float alphaz) {
	Vertex temp;
	temp.x = x + alphax;
	temp.y = y + alphay;
	temp.z = z + alphaz;
	return temp;
}

Vertex addVectors(Vertex v1, Vertex v2) {
	Vertex v3;
	v3.x = v1.x + v2.x;
	v3.y = v1.y + v2.y;
	v3.z = v1.z + v2.z;

	return v3;
}

Vertex subtractVectors(Vertex v1, Vertex v2) {
	Vertex v3;
	v3.x = v1.x - v2.x;
	v3.y = v1.y - v2.y;
	v3.z = v1.z - v2.z;

	return v3;
}

float mod(Vertex v1) {
	float m = pow((v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z), 0.5);
	return m;
}

Vertex normalize(Vertex v1, int dir) {
	float m = mod(v1);
	Vertex v2;
	v2.x = (v1.x / m)*dir;
	v2.y = (v1.y / m)*dir;
	v2.z = (v1.z / m)*dir;
	return v2;
}


float dotProduct(Vertex v1, Vertex v2) {
	float d = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return d;
}

Vertex crossProduct(Vertex v1, Vertex v2) {
	Vertex v3;
	v3.x = v1.y*v2.z - v1.z*v2.y;
	v3.y = v1.z*v2.x - v1.x*v2.z;
	v3.z = v1.x*v2.y - v1.y*v2.x;
	
	return v3;
}

float AngleBetweenVectors(Vertex v1, Vertex v2, int dir) {
	Vertex v3, v4;
	float val1, val2;
	if (dir == 0) { //X
		v3.x = v1.x;
		v4.x = v2.x;
		v3.y = v3.z = v4.y = v4.z = 0;
	}
	else if (dir == 1) { // Y
		v3.y = v1.y;
		v4.y = v2.y;
		v3.x = v3.z = v4.x = v4.z = 0;
	}
	else if (dir == 2) { //Z
		v3.z = v1.z;
		v4.z = v2.z;
		v3.y = v3.x = v4.y = v4.x = 0;
	}
	float dot = dotProduct(v3, v4);
	val1 = mod(v3);
	val2 = mod(v4);
	float cosval = dot / (val1*val2);
	float angle = acos(cosval) * 180 / 3.1415926f;
}



void Object::computePolygonNormalsAndArea() {
	int i, dir = 1;
	float area;
	Vertex v1, v2, v3, v4, v5, v6, v7;
	for (i=0; i<numFaces; i++) {
		v1 = vertices[polygons[i].poly_vertices[0]];
		v2 = vertices[polygons[i].poly_vertices[1]];
		v3 = vertices[polygons[i].poly_vertices[2]];

		v4 = subtractVectors(v2, v1);
		v5 = subtractVectors(v1, v3);

		v6 = crossProduct(v4, v5);
		
		if (dotProduct(v6, subtractVectors(v1, centroid)) > 0)
			dir = 1;
		else
			dir = -1;
		v7 = normalize(v6, dir);
		polygons[i].normalX = v7.x;
		polygons[i].normalY = v7.y;
		polygons[i].normalZ = v7.z;

		//Area
		area = mod(v6);
		polygons[i].area = area/2;
	}
}

void Object::computeVertexNormals() {
	int i, j;
	Vertex *v1, *v2, *v3;
	for (i=0; i<numFaces; i++) {
		v1 = &vertices[polygons[i].poly_vertices[0]];
		v2 = &vertices[polygons[i].poly_vertices[1]];
		v3 = &vertices[polygons[i].poly_vertices[2]];

		// normals
		v1->normalX = v1->normalX + polygons[i].normalX;
		v1->normalY = v1->normalY + polygons[i].normalY;
		v1->normalZ = v1->normalZ + polygons[i].normalZ;
		v1->incidentPoly += 1;

		v2->normalX = v2->normalX + polygons[i].normalX;
		v2->normalY = v2->normalY + polygons[i].normalY;
		v2->normalZ = v2->normalZ + polygons[i].normalZ;
		v2->incidentPoly += 1;

		v3->normalX = v3->normalX + polygons[i].normalX;
		v3->normalY = v3->normalY + polygons[i].normalY;
		v3->normalZ = v3->normalZ + polygons[i].normalZ;
		v3->incidentPoly += 1;
	}
	for (i=0; i<numVertices; i++) {
		vertices[i].normalX = vertices[i].normalX / vertices[i].incidentPoly;
		vertices[i].normalY = vertices[i].normalY / vertices[i].incidentPoly;
		vertices[i].normalZ = vertices[i].normalZ / vertices[i].incidentPoly;
	}
}

void Object::computeTexture() {
	int i,j, fx, fy, fz, f;
	unsigned long long int sides;
	float x,y,z,theta, texX, texY;
	if (texture_option != 1) {
		for (i=0; i<numVertices; i++) {
			x = vertices[i].x;
			y = vertices[i].y;
			z = vertices[i].z;

			if (texture_option == 2) { //cylindrical
				theta = atan2((x-centroid.x),(y-centroid.y));
				vertices[i].texX = ((theta-1.57)/3.14);
				vertices[i].texY =  1 - (z - minz)/(maxz-minz);
			}
			else if (texture_option == 3) { //default
				vertices[i].texX = x; 
				vertices[i].texY = y;
			}
			else if (texture_option == 4) { //sphere
				vertices[i].texX = 0.5 + (x - centroid.x)/(maxx-minx) * 1/(z-centroid.z + 1); 
				vertices[i].texY = 0.5 + (y - centroid.y)/(maxy-miny) * 1/(z-centroid.z + 1);
			}
		}
	}

}

void Object::updateMinMax() {

	Vertex t = translatePoint(minx,0,0, translate_x, 0,0);
	cur_minx = t.x;
	t = translatePoint(maxx,0,0, translate_x, 0,0);
	cur_maxx = t.x;
	t = translatePoint(0,miny,0, 0, translate_y,0);
	cur_miny = t.y;
	t = translatePoint(0,maxy,0, 0, translate_y, 0);
	cur_maxy = t.y;
	t = translatePoint(0,0,minz, 0,0,translate_z);
	cur_minz = t.z;
	t = translatePoint(0,0,maxz, 0,0, translate_z);
	cur_maxz = t.z;

}

int Object::isPointInside(float nx, float ny, float nz){
	int temp = 0;
	updateMinMax();
	if (nx <= cur_maxx && nx >= cur_minx) {
		temp += 1;
	}
	if (ny <= cur_maxy && ny >= cur_miny) {
		temp += 1;
	}
	if (nz <= cur_maxz && nz >= cur_minz) {
		temp += 1;
	}
	if (temp > 1)
		return 1;
	return 0;
}


Vertex Model::mult (float mat[4][4], Vertex v)
{
	Vertex v2;
	
	v2.x = mat[0][0]*v.x + mat[0][1]*v.y + mat[0][2]*v.z;
	v2.y = mat[1][0]*v.x + mat[1][1]*v.y + mat[1][2]*v.z;
	v2.z = mat[2][0]*v.x + mat[2][1]*v.y + mat[2][2]*v.z;
		
	return v2;
}

void Model::pickPoint(float nx, float ny, float nz) {
	int obj;
	obj = model_object.isPointInside(nx,ny,nz);
	// found object to move to. change chamelon
	if ((obj == 1) && (shark_obj == 1)) {
			moveShark();
	}
	if ((obj == 2) && (shark_obj == 0)) {
			moveShark();
	}
	cout << "pick point object " << obj << endl;
}

void Model::initlights() {

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
	glEnable(GL_COLOR_MATERIAL);
   	glColorMaterial(GL_FRONT, GL_SPECULAR);
    
    	float black[4]={0,0,0,0};
    	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    	glMaterialfv(GL_FRONT,GL_SPECULAR, black);
    	glMaterialfv(GL_FRONT,GL_EMISSION, black);

	//headlight

	light_t light0 = {{-3.0, 3.0, -2.0, 0.0}, {0.6, 0.6, 0.6, 1.0}, {1.0, 1.0, 1.0, 1.0}, {0.4, 0.4, 0.4, 1}, {50.0} };
	//pos, diffuse, specular, ambient, {1, 0.2, 0.2, 1.0}


	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0.ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0.diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0.specular);
	glLightfv(GL_LIGHT0, GL_SHININESS, light0.shininess);
	lights.push_back(light0);
	
	//Spotlight 1
	light_t light1 = {{3.0, 3.0, -3.0, 1.0}, {1.0,0.0, 0.0, 1.0}, {1.0,0.0, 0.0, 1.0}, {0.1, 0.1, 0.1, 1} , {50.0}};

	glEnable(GL_LIGHT1);
	

	glLightfv(GL_LIGHT1, GL_AMBIENT, light1.ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1.diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1.specular);
	glLightfv(GL_LIGHT1, GL_SHININESS, light1.shininess);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 50.0);
    	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5.0);
    	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION, dir1);

	lights.push_back(light1);
	
	//Spotlight 2
	light_t light2 = {{0.0, 6.0, 0.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.0, 0.0, 1.0, 1.0}, {0.1, 0.1, 0.1, 1}, {50.0} };

	glEnable(GL_LIGHT2);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light2.ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2.diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2.specular);
	glLightfv(GL_LIGHT2, GL_SHININESS, light2.shininess);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 50.0);
    	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 50.0);
    	glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION, dir2);
	lights.push_back(light2);	
	
	
}

void Model::render(int headlight, int spotlight1, int spotlight2, int axes, int boundingbox, int texture, int cam) {
	int sz,i;
	if (texture == 0) {
		model_object.objects[0].disableTex();
		model_object.objects[1].disableTex();
	}
	else {
		model_object.objects[0].enableTex();
		model_object.objects[1].enableTex();
	}
		
	
	static float rotation_transform[4][4] ;
	trackball->rotationMatrix( rotation_transform ) ;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	if (cam == 1) {
	// Camera at object A (0)
		camera_pos.x = model_object.objects[0].translate_x + model_object.objects[0].centroid.x;
	    	camera_pos.y = model_object.objects[0].translate_y + model_object.objects[0].centroid.y;
	    	camera_pos.z = model_object.objects[0].translate_z + model_object.objects[0].minz+0.1;
	    	look_at.x = model_object.objects[0].translate_x ;
	    	look_at.y = model_object.objects[0].translate_y;
	    	look_at.z = model_object.objects[0].translate_z + 3*(model_object.objects[0].minz+0.1 - model_object.objects[0].centroid.z);

		up_vec.x = model_object.objects[0].translate_x + model_object.objects[0].centroid.x;
		up_vec.y = model_object.objects[0].translate_y + model_object.objects[0].centroid.y+3;
		up_vec.z = model_object.objects[0].translate_z + model_object.objects[0].minz+0.1;

		
	
		gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z, (look_at.x), (look_at.y), (look_at.z), (up_vec.x), (up_vec.y), (up_vec.z));
	}	
	if (cam == 0) { //global camera
		camera_pos.x = 0.5;
	    	camera_pos.y = 10 + getCamZoom();
	    	camera_pos.z = 10 + getCamZoom();
	    
	    	up_vec.x = 0;
	    	up_vec.y = 1;
	    	up_vec.z = 0;

		camera_pos = mult(rotation_transform, camera_pos);
		up_vec = mult(rotation_transform, up_vec);

		gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z, 0, 0, 0, up_vec.x, up_vec.y, up_vec.z);
	}


	if (cam == 2) {
	// Camera at object C (2)

		camera_pos.x = model_object.objects[1].translate_x + model_object.objects[1].centroid.x;
	    	camera_pos.y = model_object.objects[1].translate_y + model_object.objects[1].centroid.y;
	    	camera_pos.z = model_object.objects[1].translate_z + model_object.objects[1].maxz+0.1;
	    	look_at.x = model_object.objects[1].translate_x ;
	    	look_at.y = model_object.objects[1].translate_y;
	    	look_at.z = model_object.objects[1].translate_z + 3*(model_object.objects[1].maxz+0.1 - model_object.objects[1].centroid.z);

		up_vec.x = model_object.objects[1].translate_x + model_object.objects[1].centroid.x;
		up_vec.y = model_object.objects[1].translate_y + model_object.objects[1].centroid.y+3;
		up_vec.z = model_object.objects[1].translate_z + model_object.objects[1].maxz+0.1;
	
		gluLookAt(camera_pos.x, camera_pos.y, camera_pos.z, (look_at.x), (look_at.y), (look_at.z), (up_vec.x), (up_vec.y), (up_vec.z));
	}	

	
		

	// Light
	//Headlight	
	float light_pos[] = {camera_pos.x, camera_pos.y, camera_pos.z, 0};
	if (headlight != 1)
		glDisable(GL_LIGHT0);
	else {
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	}

	// SPotlights
	if (spotlight1 != 1)
		glDisable(GL_LIGHT1);
	else {
		
		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_POSITION, lights[1].pos); // position of light
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir1); // direction of light
	}

	if (spotlight2 != 1)
		glDisable(GL_LIGHT2);
	else {
		//update direction
		dir2[0] = model_object.getSpotlightDirX();
		dir2[2] = model_object.getSpotlightDirZ();
		
		glEnable(GL_LIGHT2);
		glLightfv(GL_LIGHT2, GL_POSITION, lights[2].pos); // position of light
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, dir2); // direction of light
	}

	glEnable(GL_LIGHTING);
	
	if (axes != 0)
		drawAxes();
	drawFloor();

	
	renderObjects(boundingbox);
	updateShark();
	
	
}

void ModelObject::renderObjects(int boundingbox) {
	objects[0].render(boundingbox);
	objects[1].render(boundingbox);
	manageSpeeds();
	
}

void ModelObject::manageSpeeds() {

	if (stationary)
		return;

	objects[0].translate_x = 5*sin(objects[0].theta *(3.14/180));
	objects[0].translate_z = 5*cos(objects[0].theta *(3.14/180));
	objects[0].theta += objects[0].speed;
	if (objects[0].theta > 360)
		objects[0].theta = objects[0].theta - 360;

	objects[0].spin = (objects[0].spin + 0.5);
	if (objects[0].spin > 360)
		objects[0].spin = objects[0].spin - 360;

	objects[1].translate_x = 9*sin(objects[1].theta *(3.14/180));
	objects[1].translate_z = 9*cos(objects[1].theta *(3.14/180));
	
	objects[1].theta += objects[1].speed;
	if (objects[1].theta > 360)
		objects[1].theta = objects[1].theta - 360;
	

	objects[1].spin = (objects[1].spin + 0.5);
	if (objects[1].spin > 360)
		objects[1].spin = objects[1].spin - 360;

	objects[0].children[0].revolve = (objects[0].children[0].revolve + 3);
	if (objects[0].children[0].revolve > 360)
		objects[0].children[0].revolve = objects[0].children[0].revolve - 360;

	objects[0].children[0].spin = (objects[0].children[0].spin + 1);
	if (objects[0].children[0].spin > 360)
		objects[0].children[0].spin = objects[0].children[0].spin - 360;

}


void Model::renderObjects(int boundingbox) {
	model_object.renderObjects(boundingbox);
}

void Object::render(int boundingbox) {
	glPushMatrix();

	// Do necessary tranformations; render children
	glRotatef ((GLfloat)revolve, 0.0, 1.0, 0.0);
	glTranslatef(translate_x,translate_y,translate_z);
	int c = 0;
	// if the child is shark, spin before rendering
	if (isChildShark) {
		if (children.size() > 1) {
			children[c].render(boundingbox);
			c += 1;
		}
		glRotatef ((GLfloat)spin, 0.0, 0.0, 1.0);
		children[c].render(boundingbox);
	}
	
	else if (children.size() > 0) {
		children[0].render(boundingbox);
		glRotatef ((GLfloat)spin, 0.0, 0.0, 1.0);
	}	
	
	else
		glRotatef ((GLfloat)spin, 0.0, 0.0, 1.0);

	

	//setup texture
	static GLfloat xequalzero[] = {1.0, 0.0, 0.0, 0.0};
	static GLfloat *currentCoeff;
	if (texture_enable == 1) {
		
		if (texture_option == 3) { //default  opengl texture map
			currentCoeff = xequalzero;
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
			glTexGenfv(GL_S, GL_OBJECT_PLANE, currentCoeff);
			glEnable(GL_TEXTURE_GEN_S);
		}
		
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, object_texture);
	}

	int i,j, fx, fy, fz, f;
	unsigned long long int sides;
	glColor3fv(color);
	float x,y,z,theta, texX, texY;
	glLineWidth(2) ;
	
	for (i=0; i<numFaces; i++) {
		sides = polygons[i].sides;
		glBegin (GL_POLYGON);

		for (j=0; j<sides; j++) {
			glNormal3f(vertices[polygons[i].poly_vertices[j]].normalX, vertices[polygons[i].poly_vertices[j]].normalY,
										 vertices[polygons[i].poly_vertices[j]].normalZ );

			x = vertices[polygons[i].poly_vertices[j]].x;
			y = vertices[polygons[i].poly_vertices[j]].y;
			z = vertices[polygons[i].poly_vertices[j]].z;
			texX = vertices[polygons[i].poly_vertices[j]].texX;
			texY = vertices[polygons[i].poly_vertices[j]].texY;
			
			if (texture_option != 1 && texture_enable == 1) {
				glTexCoord2f(texX, texY);
			}
			else if (texture_option == 1 && texture_enable==1)  { //rectangular
				if (j == 0)
					glTexCoord2f(0,0);
				else if (j==1)
					glTexCoord2f(1,0);
				else if (j==2)
					glTexCoord2f(1,1);
				else if (j==3)
					glTexCoord2f(0,1);
			}
			glVertex3f(vertices[polygons[i].poly_vertices[j]].x, vertices[polygons[i].poly_vertices[j]].y, 
										vertices[polygons[i].poly_vertices[j]].z);
		}
		glEnd();
	}

	if (boundingbox == 1) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawBoundingBox();
		glEnable(GL_LIGHTING);	
		if (texture_enable == 1) 
			glEnable(GL_TEXTURE_2D);
	}
	if (isSaturn) {
		
		GLUquadricObj *quadratic; 
		quadratic=gluNewQuadric();  
		gluDisk(quadratic,1.2f,1.5f,32,32);
	}

	glPopMatrix();

	if (texture_enable == 1) {
		glDisable(GL_TEXTURE_2D);
		if (texture_option == 3)
			glDisable(GL_TEXTURE_GEN_S);
	}
}



void ModelObject::increaseSpeed(int obj) {
	objects[obj].speed = objects[obj].speed + 0.1;
}

void ModelObject::decreaseSpeed(int obj) {
	objects[obj].speed = objects[obj].speed - 0.1;
	if (objects[obj].speed < 0.1)
		objects[obj].speed = 0.1;
}

void Model::inittree() {
	model_object.objects[0].children.push_back(model_object.objects[1]);
	model_object.objects.erase(model_object.objects.begin()+1);

	model_object.objects[0].children.push_back(model_object.objects[2]);
	model_object.objects[0].isChildShark = 1;
	model_object.objects[0].children[1].chamelon_move(0, 
		(model_object.objects[0].maxy - model_object.objects[0].miny)/2, 0);
	model_object.objects[0].children[1].parent_x = model_object.objects[0].translate_x;
	model_object.objects[0].children[1].parent_y = model_object.objects[0].translate_y;
	model_object.objects[0].children[1].parent_z = model_object.objects[0].translate_z;

	model_object.objects.erase(model_object.objects.begin()+2);
}


void Model::moveShark() {
	if (shark_obj == 0) {
		model_object.objects[0].isChildShark = 0; // detach shark
		shark = model_object.objects[0].children[1];
		model_object.objects[0].children.erase(model_object.objects[0].children.begin()+1);
		shark_stationary = false;
		//update destination
		shark.dest_x = model_object.objects[1].translate_x - model_object.objects[0].translate_x;
		shark.dest_y = model_object.objects[1].translate_y - model_object.objects[0].translate_y;
		shark.dest_z = model_object.objects[1].translate_z - model_object.objects[0].translate_z;
		// update current position
		shark.translate_x = model_object.objects[0].translate_x;
		shark.translate_y = model_object.objects[0].translate_y;
		shark.translate_z = model_object.objects[0].translate_z;
		updateShark();
	}
	if (shark_obj == 1) {
		model_object.objects[1].isChildShark = 0; // detach shark
		shark = model_object.objects[1].children[0];
		model_object.objects[1].children.erase(model_object.objects[1].children.begin());
		shark_stationary = false;
		//update destination
		shark.dest_x = model_object.objects[0].translate_x - model_object.objects[1].translate_x;
		shark.dest_y = model_object.objects[0].translate_y - model_object.objects[1].translate_y;
		shark.dest_z = model_object.objects[0].translate_z - model_object.objects[1].translate_z;
		// update current position
		shark.translate_x = model_object.objects[1].translate_x;
		shark.translate_y = model_object.objects[1].translate_y;
		shark.translate_z = model_object.objects[1].translate_z;
		updateShark();
	}
}

void Model::updateShark() {
	if (shark_stationary )
		return;
	if (shark_obj == 0) {
		shark.dest_x = model_object.objects[1].translate_x - shark.translate_x;
		shark.dest_y = model_object.objects[1].translate_y - shark.translate_y;
		shark.dest_z = model_object.objects[1].translate_z - shark.translate_z;
	
		glPushMatrix();
		shark.render(0);
		shark.translate_x += 0.01*shark.dest_x;
		shark.translate_y += shark.speed;
		shark.translate_z += 0.01*shark.dest_z;
		
		if (shark.translate_y > 2)
			shark.speed = -0.05;
		else if (shark.translate_y < 0.1)
			shark.speed = 0.05;

		if (objectCloseToDest(model_object.objects[1].translate_x, model_object.objects[1].translate_y, model_object.objects[1].translate_z,
			shark.translate_x, shark.translate_y, shark.translate_z)) {
			model_object.objects[1].children.push_back(shark);
			
			model_object.objects[1].isChildShark = 1;
			shark_stationary = !shark_stationary;
			model_object.objects[1].children[0].translate_x = 0;
			model_object.objects[1].children[0].translate_y = (model_object.objects[1].maxy - model_object.objects[1].miny)/2;
			model_object.objects[1].children[0].translate_z = 0;
			shark_obj = 1;
		}
	
		glPopMatrix();
	}
	if (shark_obj == 1) {
		shark.dest_x = model_object.objects[0].translate_x - shark.translate_x;
		shark.dest_y = model_object.objects[0].translate_y - shark.translate_y;
		shark.dest_z = model_object.objects[0].translate_z - shark.translate_z;
	
		glPushMatrix();
		shark.render(0);
		shark.translate_x += 0.01*shark.dest_x;
		shark.translate_y += shark.speed;
		shark.translate_z += 0.01*shark.dest_z;
		
		if (shark.translate_y > 2)
			shark.speed = -0.05;
		else if (shark.translate_y < 0.1)
			shark.speed = 0.05;

		if (objectCloseToDest(model_object.objects[0].translate_x, model_object.objects[0].translate_y, model_object.objects[0].translate_z,
			shark.translate_x, shark.translate_y, shark.translate_z)) {
			model_object.objects[0].children.push_back(shark);
			
			model_object.objects[0].isChildShark = 1;
			shark_stationary = !shark_stationary;
			model_object.objects[0].children[1].translate_x = 0;
			model_object.objects[0].children[1].translate_y = (model_object.objects[0].maxy - model_object.objects[0].miny)/2;
			model_object.objects[0].children[1].translate_z = 0;
			shark_obj = 0;
		}
	
		glPopMatrix();
	}
}

bool Model::objectCloseToDest(float a, float b, float c, float x, float y, float z) {
	float d1 = (a-x)*(a-x);
	float d2 = (b-y)*(b-y);
	float d3 = (c-z)*(c-z);
	if (d1 +d2 + d3 < 4)
		return true;
	return false;
}


void Model::drawFloor()
{

	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, floor_texture);

	glBegin(GL_QUADS);    
	
	glColor3f(0.6, 0.6, 0.6);
		
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0, 0.0);	
	glVertex3f (-15, -0.1, 15);

	glTexCoord2f(1.0, 0.0);
	glVertex3f (-15, -0.1, -15);
	
	glTexCoord2f(1.0, 1.0);
	glVertex3f (15, -0.1, -15);

	glTexCoord2f(0.0, 1.0);
	glVertex3f (15, -0.1, 15);
	glEnd(); 

	glDisable(GL_TEXTURE_2D);

}

void Model::readTex(char* filename1, char* filename2, char* filename3, char* filename4, char* filename5) {
	strcpy(floor_tex, filename1);
	strcpy(obj_tex1, filename3);
	strcpy(obj_tex2, filename4);
	strcpy(obj_tex3, filename2);
	strcpy(chamelon_tex, filename5);
}

void Object::inittexture(char * texture) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &object_texture);
	glBindTexture(GL_TEXTURE_2D, object_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
				   GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				   GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(texture, &width, &height, 0, SOIL_LOAD_RGB);
	
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
              GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);   
}


void Model::inittexture() {
	//make this a for loop if you want to generalise it
	model_object.objects[0].inittexture(obj_tex1);
	model_object.objects[1].inittexture(obj_tex2);
	model_object.objects[2].inittexture(obj_tex3);
	model_object.objects[3].inittexture(chamelon_tex);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &floor_texture);
	glBindTexture(GL_TEXTURE_2D, floor_texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
				   GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
				   GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(floor_tex, &width, &height, 0, SOIL_LOAD_RGB);
	
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
              GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);   
}


void Model::readPly(char* filename) {
	ifstream infile;
	infile.open(filename);

	string line, temp;
	int num,i, j, k;
	unsigned long long s;
	float x, y, z;	
	int numVertices, numFaces;
	Vertex *vertices;
	Polygon *polygons;

	while (line.compare(string("end_header")))
	{
		getline(infile, line);
    		istringstream iss(line);
		iss >> temp; 

		if (temp.compare(string("end_header")) == 0) {	
			break;
		}
		if (temp.compare(string("element")) == 0)
		{
			iss >> temp;
			iss >> num;
			if (temp.compare(string("vertex")) == 0) {
				numVertices = num;
			}
			if (temp.compare(string("face")) == 0) {
				numFaces = num;
			}
		}
	}

	polygons = new Polygon[numFaces];
	vertices = new Vertex[numVertices];

	for (i=0; i<numVertices; i++) {
		getline(infile, line);
    		istringstream iss(line);
		iss >> vertices[i].x;
		iss >> vertices[i].y;
		iss >> vertices[i].z;
	}
	for (i=0; i<numFaces; i++) {
		getline(infile, line);
    		istringstream iss(line);
		iss >> s;
		vector<Vertex> temp;
		for (j=0; j < s; j++) {
			iss >> polygons[i].poly_vertices[j];
		}
		polygons[i].sides = s;
	}
	Object obj;
	obj.polygons = polygons;
	obj.vertices = vertices;
	obj.numVertices = numVertices;
	obj.numFaces = numFaces;
	
	model_object.addObject(obj);
}

void Model::computeObjects() {
	model_object.objects[0].texture_option = 2;
	model_object.objects[1].texture_option = 4;
	model_object.objects[2].texture_option = 4;
	model_object.objects[3].texture_option = 3;
	model_object.computeObjects();
}



void Model::defaultTexToggle() {
	if (texdefault == 0) {
		texdefault = 1;
		model_object.objects[0].texture_option = 3;
		model_object.objects[1].texture_option = 3;
		model_object.objects[2].texture_option = 3;
	}
	else if (texdefault == 1) {
		texdefault = 0;
		model_object.objects[0].texture_option = 2;
		model_object.objects[1].texture_option = 4;
		model_object.objects[2].texture_option = 4;
	}
}

void Object::getCentroid() {
	int i;
	centroid.x = 0;
	centroid.y = 0;
	centroid.z = 0;
	for (i=0; i<numVertices; i++) {
		if (minx > vertices[i].x)
			minx = vertices[i].x;
		if (maxx < vertices[i].x)
			maxx = vertices[i].x;
		if (miny > vertices[i].y)
			miny = vertices[i].y;
		if (maxy < vertices[i].y)
			maxy = vertices[i].y;
		if (minz > vertices[i].z)
			minz = vertices[i].z;
		if (maxz < vertices[i].z)
			maxz = vertices[i].z;

		centroid.x += vertices[i].x / numVertices;
		centroid.y += vertices[i].y / numVertices;
		centroid.z += vertices[i].z / numVertices;
	}
	
}

void Object::shiftOriginToCentroidAndNormalise(int s) {
	int i, j, temp;
	for (i=0; i<numVertices; i++) {
		vertices[i].x -= centroid.x;
		vertices[i].y -= centroid.y;
		vertices[i].z -= centroid.z;
	}
	minx -= centroid.x;
	maxx -=centroid.x;
	miny -= centroid.y;
	maxy -=centroid.y;
	minz -= centroid.z;
	maxz -=centroid.z;
	centroid.x = 0;
	centroid.y = 0;
	centroid.z = 0;
	
	float aspect_ratio;
	if (s == 1) {
		temp = maxx - minx;
		if (maxy - miny > temp)
			temp = maxy - miny;
		if (maxz - minz > temp)
			temp = maxz - minz;
		aspect_ratio = temp;
	}
	else if (s == 2)
		aspect_ratio = (maxx - minx);
	else
		aspect_ratio = (maxx - minx)/2;

	if (aspect_ratio < 0) 
		aspect_ratio = aspect_ratio*-1.0;

	for (i=0; i<numVertices; i++) {
		vertices[i].x /= aspect_ratio;
		vertices[i].y /= aspect_ratio;
		vertices[i].z /= aspect_ratio;
	}
	minx /= aspect_ratio;
	maxx /= aspect_ratio;
	miny /= aspect_ratio;
	maxy /= aspect_ratio;
	minz /= aspect_ratio;
	maxz /= aspect_ratio;
}


void Model::drawBoundingBox() {
	glDisable(GL_LIGHTING);
	model_object.drawBoundingBox();
	glEnable(GL_LIGHTING);	
}

void Object::drawBoundingBox() {
	
	glColor3f(1, 1, 1);
	glLineWidth(2) ;

	glBegin(GL_LINES);
	//1
	glVertex3f(minx, miny, minz);
	glVertex3f(maxx, miny, minz);
	//2
	glVertex3f(minx, miny, minz);
	glVertex3f(minx, maxy, minz);
	//3
	glVertex3f(minx, miny, minz);
	glVertex3f(minx, miny, maxz);	
	//4
	glVertex3f(minx, miny, maxz);
	glVertex3f(maxx, miny, maxz);
	//5
	glVertex3f(maxx, miny, maxz);
	glVertex3f(maxx, miny, minz);
	//6
	glVertex3f(minx, maxy, minz);
	glVertex3f(maxx, maxy, minz);
	//7
	glVertex3f(maxx, maxy, minz);
	glVertex3f(maxx, miny, minz);
	//8
	glVertex3f(minx, maxy, minz);
	glVertex3f(minx, maxy, maxz);
	//9
	glVertex3f(maxx, miny, maxz);
	glVertex3f(maxx, maxy, maxz);
	//10
	glVertex3f(maxx, maxy, maxz);
	glVertex3f(maxx, maxy, minz);
	//11
	glVertex3f(maxx, maxy, maxz);
	glVertex3f(minx, maxy, maxz);
	//12
	glVertex3f(minx, miny, maxz);
	glVertex3f(minx, maxy, maxz);

	glEnd();
	glPointSize(8) ;
	glBegin(GL_POINTS) ; 
	glVertex3f(centroid.x, centroid.y, centroid.z);
	glEnd();

}


void Model::set_spin(float x, int i) {
	if (i==0) { // spinX
		spinX += x;
	}	
	else if (i==1) { // spinY
		spinY += x;
	}		
	else if (i==2) { // spinZ
		spinZ += x;
	}	
}

void Model::drawAxes() {
	glLineWidth(1) ;
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);    
	//~ X - red
	glColor3f(1, 0, 0);
	glVertex3f (-6, 0, 0);
	glVertex3f (6, 0, 0);
	//~ Y - green
	glColor3f(0, 1, 0);
	glVertex3f (0, -6, 0);
	glVertex3f (0, 6, 0);
	//~ Z - blue
	glColor3f(0, 0, 1);
	glVertex3f (0, 0, -6);
	glVertex3f (0, 0, 6);
	glEnd();
	
	//~ Origin
	glPointSize(5.0);
	glColor3f(0, 1, 1);
	glBegin(GL_POINTS);
	glVertex3f(0, 0, 0);
	glEnd();

	glEnable(GL_LIGHTING);
}
