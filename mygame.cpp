#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "mylib.h"

#define WINDOW_X 640
#define WINDOW_Y 640
#define PI 3.1415
#define WINDOW_NAME "sample"

//関数群
void init_GL(int argc, char **argv);
void init();
void set_callback_functions();
void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);

//グローバル変数
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;

class Camera
{
  public:
  //コンストラクタ
  Camera()
  {
    x = 0.0, y = 0.0, z = 20.0;
    view_x = 0.0, view_y = 0.0, view_z = 0.0;
    up_x = 0.0, up_y = 1.0, up_z = 0.0;
    prevx = 0.0, prevy = 0.0;
  };
  Camera(double initx, double inity, double initz)
  {
    x = initx, y = inity, z = initz;
    view_x = 0.0, view_y = 0.0, view_z = 0.0;
    up_x = 0.0, up_y = 1.0, up_z = 0.0;
    prevx = initx, inity = 0.0;
  }

  void set()
  {
    if (cos(g_angle2)>0){
      gluLookAt(z * cos(g_angle2) * sin(g_angle1), 
          z * sin(g_angle2), 
          z * cos(g_angle2) * cos(g_angle1), 
          0.0, 0.0, 0.0, 0.0, 1.0, 0.0);}
    else{
      gluLookAt(z * cos(g_angle2) * sin(g_angle1),
          z * sin(g_angle2),
          z * cos(g_angle2) * cos(g_angle1),
          0.0, 0.0, 0.0, 0.0, -1.0, 0.0);}
  };
  
  void reset()
  {
    x = 0.0, y = 0.0, z = 20.0;
    view_x = 0.0, view_y = 0.0, view_z = 0.0;
    up_x = 0.0, up_y = 1.0, up_z = 0.0;
    prevx = 0.0, prevy = 0.0;
    g_angle1 = 0.0, g_angle2 = 0.0;
  }
  
  //カメラ位置の更新（描画自体はしていない）
  void move(int mouse_x, int mouse_y)
  {
    if(g_isLeftButtonOn == true){
      if(prevx >= 0 && prevy >= 0){
        g_angle1 += (double)-(mouse_x - prevx)/20;
        g_angle2 += (double)(mouse_y - prevy)/20;
      }
      prevx = mouse_x;
      prevy = mouse_y;
    }else if(g_isRightButtonOn == true){
      if(prevx >= 0 && prevy >= 0){
        z += (double)(mouse_y - prevy)/20;
      }
      prevx = mouse_x;
      prevy = mouse_y;
    }else{
      prevx = -1;
      prevy = -1;
    }
  }

  double x, y, z;
  double view_x, view_y, view_z;
  double up_x, up_y, up_z;

private:
  double prevx, prevy;
  double g_angle1, g_angle2;
};
Camera camera;

class Player
{
  public:
  //コンストラクタ
  Player(int initx, int inity)
  {
    x = initx;
    y = inity;
    state = 0;
  };

  void move(int mouse_x, int mouse_y)
  {
    //スクリーン座標の中心をワールド座標(0,0)に移動
    double x_center_is_zero = 1.0 * mouse_x - WINDOW_X / 2.0;
    double y_center_is_zero = 1.0 * mouse_y - WINDOW_Y / 2.0;
    
    //Playerの位置を調整
    if (g_isLeftButtonOn == false && g_isRightButtonOn == false)
    {
      x = x_center_is_zero / (WINDOW_X / 2.0) * camera.z * std::tan(PI * 15 / 180);
      y = -y_center_is_zero / (WINDOW_Y / 2.0) * camera.z * std::tan(PI * 15 / 180);
    }
    printf("player.x = %f, player.y = %f\n", x, y);

    glutPostRedisplay();
  };

  void render()
  {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    draw_cube();
    glPopMatrix();
  };

  double x, y;
  int state;
};
Player player(0.0, 0.0);

class Enemy
{

};

int main(int argc, char **argv)
{
  //GLの初期化
  init_GL(argc, argv);

  //各種初期化
  init();

  //コールバック関数の登録
  set_callback_functions();

  //メインループ
  glutMainLoop();

  return 0;
}

//GLの初期化
void init_GL(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_X, WINDOW_Y);
  glutCreateWindow(WINDOW_NAME);
}

//初期化
void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0); 
}

//コールバック関数の登録
void set_callback_functions()
{
  glutDisplayFunc(glut_display);
  glutKeyboardFunc(glut_keyboard);
  glutMouseFunc(glut_mouse);
  glutMotionFunc(glut_motion);
  glutPassiveMotionFunc(glut_motion);
}

void glut_keyboard(unsigned char key, int x , int y)
{
  switch(key)
  {
    case 'q':
    case '\033':
      exit(0);
      break;
    case 'r':
      camera.reset();  //カメラの位置をリセット
      break;
    default:
      break;
  }

  glutPostRedisplay();
}

//マウスのボタン押し時のコールバック関数
void glut_mouse(int button, int state, int x, int y)
{
  if(button == GLUT_LEFT_BUTTON){
    if(state == GLUT_UP){
      g_isLeftButtonOn = false;
    }else if(state == GLUT_DOWN){
      g_isLeftButtonOn = true;
    }
  }

  if(button == GLUT_RIGHT_BUTTON){
    if(state == GLUT_UP){
      g_isRightButtonOn = false;
    }else if(state == GLUT_DOWN){
      g_isRightButtonOn = true;
    }
  }
}

//マウスの動きのコールバック関数
void glut_motion(int x, int y)
{
  //Playerの移動
  player.move(x, y);
  camera.move(x, y);
  printf("x = %d, y = %d\n", x, y);
}

//描画のコールバック関数
void glut_display()
{
  //描画モードの設定
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0, 1.0, 0.1, 100);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //カメラの設定
  camera.set();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  //軸描画
  draw_axis();
  
  //playerを描画
  player.render();

  //球描画
  draw_sphere(0.1, 0.0, 0.0, 0.0);

  glFlush();
  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}