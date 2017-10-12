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

void draw_cube()
{
  GLdouble pointA[] = {1.5, -1.5, 1.5};
  GLdouble pointB[] = {-1.5, -1.5, 1.5};
  GLdouble pointC[] = {-1.5, -1.5, -1.5};
  GLdouble pointD[] = {1.5, -1.5, -1.5};
  GLdouble pointE[] = {1.5, 1.5, 1.5};
  GLdouble pointF[] = {-1.5, 1.5, 1.5};
  GLdouble pointG[] = {-1.5, 1.5, -1.5};
  GLdouble pointH[] = {1.5, 1.5, -1.5};
  
  glColor3d(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointB);
  glVertex3dv(pointC);
  glVertex3dv(pointD);
  glEnd();

  glColor3d(1.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointB);
  glVertex3dv(pointF);
  glVertex3dv(pointE);
  glEnd();

  glColor3d(1.0, 1.0, 0.0);
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

  glColor3d(1.0, 0.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointA);
  glVertex3dv(pointD);
  glVertex3dv(pointH);
  glVertex3dv(pointE);
  glEnd();

  glColor3d(0.0, 0.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3dv(pointE);
  glVertex3dv(pointF);
  glVertex3dv(pointG);
  glVertex3dv(pointH);
  glEnd();
}

void draw_sphere(double r, double x, double y, double z)
{
  glPushMatrix();
  glTranslatef(x, y ,z);
  glutSolidSphere(r, 16, 16);
  glPopMatrix();
  
}