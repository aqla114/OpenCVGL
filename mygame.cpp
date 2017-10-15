//#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "mylib.h"
#include <cmath>
#include <iostream>
#include <list>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960
#define PI 3.1415
#define ANGLE_OF_PERSPECTIVE 60.0
#define FREQUENCY_OF_ENEMY 300
#define WINDOW_NAME "sample"

//関数群
void init_GL(int argc, char **argv);
void init();
void set_callback_functions();
void glut_display();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_idle();

//グローバル変数
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;

class Camera
{
  public:
  //コンストラクタ
  Camera()
  {
    x = 0.0, y = 0.0, z = -20.0;
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

	//cameraの位置を設定（カメラの実設置）
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
  
	//カメラを初期位置に戻す
  void reset()
  {
    x = 0.0, y = 0.0, z = -20.0;
    view_x = 0.0, view_y = 0.0, view_z = 0.0;
    up_x = 0.0, up_y = 1.0, up_z = 0.0;
    prevx = 0.0, prevy = 0.0;
    g_angle1 = 0.0, g_angle2 = 0.0;
  }
  
  //カメラ位置の更新（実設置はしていない）
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
  Player()
  {
    x = 0.0;
    y = 0.0;
		z = -10.0;
		width = 5.0;
		height = 5.0;
    state = 0;
  };

  void move(int mouse_x, int mouse_y)
  {
    //スクリーン座標の中心をワールド座標(0,0)に移動
    double x_center_is_zero = 1.0 * mouse_x - WINDOW_WIDTH / 2.0;
    double y_center_is_zero = 1.0 * mouse_y - WINDOW_HEIGHT / 2.0;
    
    //Playerの位置を調整
    if (g_isLeftButtonOn == false && g_isRightButtonOn == false)
    {
			double angle = PI / 180.0 * ANGLE_OF_PERSPECTIVE / 2.0;
      x = x_center_is_zero / (WINDOW_WIDTH / 2.0) * (camera.z - this->z) * std::tan(angle) *  WINDOW_WIDTH / WINDOW_HEIGHT;
      y = y_center_is_zero / (WINDOW_HEIGHT / 2.0) * (camera.z - this->z) * std::tan(angle);
    }
    //printf("player.x = %f, player.y = %f\n", x, y);

    glutPostRedisplay();
  };

  void render()
  {
    draw_cube(x, y, z, width, height, 1.0);
  };

	//このx,y,zは中心の座標
  double x, y, z;
	double width, height;
  int state;
};
Player player;

class Enemy
{
public:
	Enemy(double init_x, double init_y, double init_z)
	{
		x = init_x;
		y = init_y;
		z = init_z;
	}

	void move()
	{
		//-z方向に速度speedで進む。
		z -= speed;
	}

	//描画
	void render()
	{
		draw_sphere(0.3, x, y, z);
	}

	//自らの座標
	double x;
	double y;
	double z;

private:
	double speed = 0.05;
};

class EnemyController
{
public:
	void generate_enemy()
	{
		//生成位置をgenerate_に入れて、Enemyオブジェクト生成
		double generate_x = -generate_range_x + 2.0 * generate_range_x * std::rand() / RAND_MAX;
		double generate_y = -generate_range_y + 2.0 * generate_range_y * std::rand() / RAND_MAX;
		Enemy *enemy = new Enemy(generate_x, generate_y, generate_z);
		
		//生成したオブジェクトをEnemyHolderに格納
		enemyHolder.push_back(*enemy);

		printf("Enemy generated! : %f, %f, %f\n", enemy->x, enemy->y, enemy->z);
	}

	std::list<Enemy> enemyHolder;
private:
	//敵の生成領域（x方向の幅の半分、y方向の幅の半分、z方向の奥行き）
	//-generate_range_x～generate_range_x、-generate_range_y～generate_range_y
	double generate_range_x = 5.0;
	double generate_range_y = 5.0;
	double generate_z = 20.0;
};

EnemyController enemyController;

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
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
	glutIdleFunc(glut_idle);
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
	//printf("x = %d, y = %d\n", x, y);
}

//描画のコールバック関数
void glut_display()
{
  //描画モードの設定
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(ANGLE_OF_PERSPECTIVE, 1.0 * WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 100);

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

	//確認用の正方形描画
	draw_square(-5.0, -5.0, 20.0, 10.0, 10.0);

	//enemyHolderに入っているenemyを全て描画
	std::list<Enemy>::iterator itr;
	for (itr = enemyController.enemyHolder.begin(); itr != enemyController.enemyHolder.end(); itr++)
	{
		(*itr).render();
	}

  glFlush();
  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

void glut_idle()
{
	static int counter = 0;

	if (counter == FREQUENCY_OF_ENEMY)
	{
		//敵を生成
		enemyController.generate_enemy();

		//counterを0に
		counter = 0;
	}

	counter++;

	//敵の移動および削除
	std::list<Enemy>::iterator itr;
	for (itr = enemyController.enemyHolder.begin(); itr != enemyController.enemyHolder.end(); )
	{
		//敵の移動
		(*itr).move();

		//Enemyが適当な位置に来たら削除。
		if ((*itr).z < -20.0)
		{
			//itr番目の要素をenemyHolderから削除
			itr = enemyController.enemyHolder.erase(itr);
			printf("miss!\n");
			//printf("Object enemy deleted! %d\n", enemyController.enemyHolder.size());
		}
		//PlayerにEnemyが当たったらEnemyを消す
		else if (player.x < (*itr).x && (*itr).x < player.x + player.width
				&& player.y < (*itr).y && (*itr).y < player.y + player.height
				&& (*itr).z < player.z)
		{
			itr = enemyController.enemyHolder.erase(itr);
			printf("hit!\n");
		}
		else
		{
			itr++;  //こうしないとerase(itr)で消してしまったitrに対してfor条件の参照が発生する。
		}
	}

	glutPostRedisplay();
}