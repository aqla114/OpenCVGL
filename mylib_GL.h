void draw_axis()
{
  glPushMatrix();
  glBegin(GL_LINES);
    
  glColor3f ( 1.0f, 0.0f, 0.0f );
  glVertex3f( 50.0f, 0.0f, 0.0f );
  glVertex3f( 0.0f, 0.0f, 0.0f );
    
  glColor3f ( 0.0f, 1.0f, 0.0f );
  glVertex3f( 0.0f, 50.0f, 0.0f );
  glVertex3f( 0.0f, 0.0f, 0.0f );
    
  glColor3f ( 0.0f, 0.0f, 1.0f );
  glVertex3f( 0.0f, 0.0f, 50.0f );
  glVertex3f( 0.0f, 0.0f, 0.0f );
    
  glEnd();
  glPopMatrix();
}

//x,yに左下の点を与え、widthで横辺の長さ、heightで高さを与える。
void draw_square(double x, double y, double z, double width, double height)
{
	glPushMatrix();
	glBegin(GL_POLYGON);

	glColor3d(1.0, 0.0, 0.0);
	glVertex3d(x, y, z);
	glVertex3d(x + width, y, z);
	glVertex3d(x + width, y + height, z);
	glVertex3d(x, y + height, z);
	
	glEnd();
	glPopMatrix();
}

//x,y,zを中心としている。
void draw_cube(double x = 0.0, double y = 0.0, double z = 0.0, double x_size = 2.0, double y_size = 2.0, double z_size = 2.0)
{
  //GLdouble pointA[] = {1.5, -1.5, 1.5};
  //GLdouble pointB[] = {-1.5, -1.5, 1.5};
  //GLdouble pointC[] = {-1.5, -1.5, -1.5};
  //GLdouble pointD[] = {1.5, -1.5, -1.5};
  //GLdouble pointE[] = {1.5, 1.5, 1.5};
  //GLdouble pointF[] = {-1.5, 1.5, 1.5};
  //GLdouble pointG[] = {-1.5, 1.5, -1.5};
  //GLdouble pointH[] = {1.5, 1.5, -1.5};

	glPushMatrix();
	glTranslatef(x, y, z);
  
	GLdouble pointA[] = { x_size / 2.0, -y_size / 2.0, z_size / 2.0 };
	GLdouble pointB[] = { -x_size / 2.0, -y_size / 2.0, z_size / 2.0};
	GLdouble pointC[] = { -x_size / 2.0, -y_size / 2.0,- z_size / 2.0 };
	GLdouble pointD[] = { x_size / 2.0, -y_size / 2.0, -z_size / 2.0 };
	GLdouble pointE[] = { x_size / 2.0, y_size / 2.0, z_size / 2.0 };
	GLdouble pointF[] = { -x_size / 2.0, y_size / 2.0, z_size / 2.0 };
	GLdouble pointG[] = { -x_size / 2.0, y_size / 2.0, -z_size / 2.0 };
	GLdouble pointH[] = { x_size / 2.0, y_size / 2.0, -z_size / 2.0 };

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointB);
  glVertex3dv(pointC);
  glVertex3dv(pointD);
  glEnd();

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointB);
  glVertex3dv(pointF);
  glVertex3dv(pointE);
  glEnd();

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointB);
  glVertex3dv(pointF);
  glVertex3dv(pointG);
  glVertex3dv(pointC);
  glEnd();

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointC);
  glVertex3dv(pointD);
  glVertex3dv(pointH);
  glVertex3dv(pointG);
  glEnd();

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointD);
  glVertex3dv(pointH);
  glVertex3dv(pointE);
  glEnd();

  glColor3d(0.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointE);
  glVertex3dv(pointF);
  glVertex3dv(pointG);
  glVertex3dv(pointH);
  glEnd();

	glPopMatrix();
}

void draw_sphere(double r, double x, double y, double z)
{
  glPushMatrix();
	glColor3d(1.0, 0.0, 0.0);
  glTranslatef(x, y ,z);
  glutSolidSphere(r, 16, 16);
  glPopMatrix();
}