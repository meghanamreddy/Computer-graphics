#include <iostream>
#include <vector>
# include "trackball.h"
#include <GL/glut.h>
# ifndef Model_H
# define Model_H

using namespace std;


typedef struct
{
	float pos[4];
	float diffuse[4];
	float specular[4];
	float ambient[4];
	float shininess[1];
}light_t;

class Point {
	public:
		GLfloat x;
		GLfloat y;
		GLfloat z;
		Point() {};
		Point(GLfloat a, GLfloat b, GLfloat c) {
			x = a;
			y = b;
			z = c;
		}
		GLfloat get_x(){
			return x;
		}
		GLfloat get_y(){
			return y;
		}
		GLfloat get_z(){
			return z;
		}
		
};

class Vertex{
	public:
	float x,y,z;
	float normalX, normalY, normalZ;
	float texX, texY, texZ;
	float incidentPoly;
	void render() {glVertex3f(x,y,z);}
};

class Polygon{
	public:
	int sides;
	float normalX, normalY, normalZ;
	unsigned long long poly_vertices[10];
	//assuming triangle
	float perimeter;
	float a, b, c; //variables for incircle.
	Vertex incentre;
	float incircleRadius;
	float area;
};


class Object{
	public:
		vector<Object> children;
		bool isChildShark = 0;
		int numVertices = 0;
		int numFaces = 0;
		float color[3] = {0.8, 0.8, 0.8};
		Vertex* vertices;
		Polygon* polygons;
		bool stationary = 0;
		bool isSaturn = 0;
		
		float revolve = 0, spin = 0;
		int shark_position = 0;
		float dest_x =0, dest_y=0, dest_z=0;
		float parent_x =0, parent_y=0, parent_z=0;
		float speed = 0.1;
		float theta = 0;
		float minx=0, maxx=0, miny=0, maxy=0, minz=0, maxz=0, translate_x=0, translate_y=0, translate_z=0, scale_x=1;
		float cur_minx=0, cur_maxx=0, cur_miny=0, cur_maxy=0, cur_minz=0, cur_maxz=0;
		float midx=0, midy=0, midz=0;
		Vertex centroid;
		void render(int boundingbox);
		void drawBoundingBox();
		void getCentroid();
		void shiftOriginToCentroidAndNormalise(int s);
		void computePolygonNormalsAndArea();
		void computeVertexNormals();
		void computeTexture();
		void move(float i, float j) {
			translate_x = i;
			translate_y = -1*miny;
			translate_z = j;
			updateMinMax(); // call whenever the object is transformed
		}
		void chamelon_move(float tx, float ty, float tz) {
			translate_x = tx;
			translate_y = ty + (-1*miny);
			translate_z = tz;
			updateMinMax(); // call whenever the object is transformed
		}

		void updateMinMax();
		GLuint object_texture;
		void inittexture(char * texture);

		int texture_option = 1; // 1 - rectangular, 2 - cylindrical, 3 - default, 4 - sphere
		int texture_enable = 1; // 0 - disable, 1 - enable

		void enableTex() { if (texture_enable == 0) texture_enable = 1; 
			int i;
			for (i=0; i<children.size(); i++) children[i].enableTex();}
		void disableTex() { if (texture_enable == 1) texture_enable = 0; 
			int i;
			for (i=0; i<children.size(); i++) children[i].disableTex();}
		int isPointInside(float x, float y, float z);
		void changeTex(GLuint texx) { object_texture = texx; }
};

class ModelObject{
	public:
	vector<Object> objects;	
	bool stationary = 1;

	void getCentroid(Object* obj);
	void shiftOriginToCentroidAndNormalise(Object* obj);
	void computeVertexNormals(Object* obj);
	
	void computePolygonNormalsAndArea(Object* obj);
	void addObject(Object obj) {objects.push_back(obj);}
	void decreaseSpeed(int obj);
	void increaseSpeed(int obj);
	void manageSpeeds();
	float getSpotlightDirZ() { return objects[1].translate_z; }
	float getSpotlightDirX() { return objects[1].translate_x; }
	void renderObjects(int boundingbox);
	
	void drawBoundingBox() {
		int i;
		for (i=0; i<objects.size() ; i++) 
			objects[i].drawBoundingBox();
	}
	void computeObjects() {
		int i;
		for (i=0; i<objects.size(); i++) {
			objects[i].getCentroid();
			if (i == objects.size()-1 )
				objects[i].shiftOriginToCentroidAndNormalise(1);
			else if (i == objects.size()-3 )
				objects[i].shiftOriginToCentroidAndNormalise(2);
			else
				objects[i].shiftOriginToCentroidAndNormalise(0);
			objects[i].computePolygonNormalsAndArea();
			objects[i].computeVertexNormals();
			objects[i].computeTexture();
		}
		objects[0].move(5, 0);
		objects[1].move(-2, 0);
		objects[2].move(0, -9);
		objects[0].theta = 90;
		objects[2].theta = 180;
		
		objects[3].chamelon_move(0, (objects[0].maxy - objects[0].miny)/2, 0);
		objects[3].speed = 0.05;
		objects[2].isSaturn = true;
	}
	int isPointInside(float nx, float ny, float nz) {
		int i, t, temp;
		for (i=0; i<objects.size() ; i++) {
			
			t = objects[i].isPointInside(nx,ny,nz);
			if (t==1)
				return (i+1);

		}
		return 0;
	}
};

class Model {
	public:
	//GLdouble pos3D_x, pos3D_y=20, pos3D_z ;
	float spinX = 0.0, spinY = 0.0, spinZ = 0.0;
	int min_max_set=0;
	int mesh_splat = 0;
	int normalCol = 0;
	int filledpoly = 1;
	float alpha = 0.5; // for splat rendering
	int splat = 0;
	Trackball * trackball;

	GLfloat dir1[4] = {0.0, -1, 2.0, 0.0};
	GLfloat dir2[4] = {3.0, -5, 1.0, 0.0};

	int shark_obj = 0; // 0 - Object A, 1 - Object C
	bool shark_stationary = 1;
	Vertex *vertices;
	Polygon *polygons;
	ModelObject model_object;
	
	Object shark;
	char floor_tex[100];
	char obj_tex1[100];
	char obj_tex2[100];
	char obj_tex3[100];
	char chamelon_tex[100];

	GLuint floor_texture;

	vector<light_t> lights;

	void drawAxes();
	void drawSplats();
	void readPly(char*);
	void drawPolygons();
	void perimeterIncentreOfTriangles();
	void defaultTexToggle();
	int texdefault = 0;
	void normalColor() { normalCol = (normalCol + 1)%2; }
	void filledPolygon() {filledpoly = (filledpoly + 1) % 2; }
	void initlights();
	void inittexture();
	void inittree();
	void readTex(char* filename1, char* filename2, char* filename3, char* filename4, char* filename5);
	void setTrackball(Trackball * t) {trackball = t;}
	void render(int headlight, int spotlight1, int spotlight2, int axes, int boundingbox, int texture, int cam);
	void set_spin(float x, int i) ;
	float get_spin() { return spinX; };
	
	Vertex camera_pos;
	float camera_zoom =0;
	Vertex up_vec;
	Vertex look_at;
	Vertex mult(float mat[4][4], Vertex);

	void renderObjects(int boundingbox);
	void drawFloor();
	void drawBoundingBox();
	void computeObjects();

	void pickPoint(float nx, float ny, float nz);
	void zoomCam(float t) {camera_zoom = t; }
	float getCamZoom() {return camera_zoom;}

	void decreaseSpeed(int obj) {model_object.decreaseSpeed(obj); }
	void increaseSpeed(int obj) {model_object.increaseSpeed(obj); }
	void MoveStopObjects() {model_object.stationary = !model_object.stationary; }
	bool objectCloseToDest(float a, float b, float c, float x, float y, float z) ;
	void moveShark();
	void updateShark();



};




#endif
