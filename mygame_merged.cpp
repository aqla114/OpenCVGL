#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <cmath>
#include <string.h>
#include <string>
#include <opencv2/opencv.hpp>
//#include <opencv2/opencv_lib.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "mylib_CV.h"
#include <GL/glut.h>
#include "mylib_GL.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define PI 3.1415
#define MAX_HP 5
#define ANGLE_OF_PERSPECTIVE 60.0
#define FREQUENCY_OF_ENEMY 20
#define SPEED_OF_ENEMY 0.8
#define WINDOW_NAME "sample"
#define TEST_MODE 1 //0:カメラを使わない 1:カメラを使う


//関数群
void init_GL(int argc, char **argv);
void init();
void set_callback_functions();
void glut_display();
void glut_display2();
void glut_keyboard(unsigned char key, int x, int y);
void glut_mouse(int button, int state, int x, int y);
void glut_motion(int x, int y);
void glut_idle();

//グローバル変数
bool g_isLeftButtonOn = false;
bool g_isRightButtonOn = false;
int windowID[2];
#if TEST_MODE
cv::Point gcenter(0, 0);
cv::Point prev_gcenter(0, 0);
cv::Point prev_prev_gcenter(0, 0);
cv::Mat dst_img;
#endif TEST_MODE

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
		thickness = 1.0;
    state = 0;
  };

  void moveWithMouse(int mouse_x, int mouse_y)
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

  void moveWithHand(int hand_x, int hand_y)
  {
    //スクリーン座標の中心をワールド座標(0,0)に移動
    double x_center_is_zero = 1.0 * hand_x - FRAME_WIDTH / 2.0;
    double y_center_is_zero = 1.0 * hand_y - FRAME_HEIGHT / 2.0;
    
    //Playerの位置を調整
    if (g_isLeftButtonOn == false && g_isRightButtonOn == false)
    {
			double angle = PI / 180.0 * ANGLE_OF_PERSPECTIVE / 2.0;
      x = -x_center_is_zero / (FRAME_WIDTH / 2.0) * (camera.z - this->z) * std::tan(angle) *  FRAME_WIDTH / FRAME_HEIGHT;
      y = y_center_is_zero / (FRAME_HEIGHT / 2.0) * (camera.z - this->z) * std::tan(angle);
    }
    //printf("player.x = %f, player.y = %f\n", x, y);

    glutPostRedisplay();
  };

	void reset()
	{
		x = 0.0;
		y = 0.0;
	}

  void render()
  {
    draw_cube(x, y, z, width, height, thickness);
  };

	//このx,y,zは中心の座標
  double x, y, z;
	double width, height, thickness;
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
			speed = SPEED_OF_ENEMY;
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
		//Enemyの動く速度
		double speed;
};

class EnemyController
{
	public:
		EnemyController()
		{
			generate_range_x = 4.0;
			generate_range_y = 4.0;
			generate_z = 20.0;
		}
		void generate_enemy()
		{
			//生成位置をgenerate_に入れて、Enemyオブジェクト生成
			double generate_x = -generate_range_x + 2.0 * generate_range_x * rand() / RAND_MAX;
			double generate_y = -generate_range_y + 2.0 * generate_range_y * rand() / RAND_MAX;
			Enemy *enemy = new Enemy(generate_x, generate_y, generate_z);
		
			//生成したオブジェクトをEnemyHolderに格納
			enemyHolder.push_back(*enemy);

			//printf("Enemy generated! : %f, %f, %f\n", enemy->x, enemy->y, enemy->z);
		}

		std::list<Enemy> enemyHolder;
	private:
		//敵の生成領域（x方向の幅の半分、y方向の幅の半分、z方向の奥行き）
		//-generate_range_x～generate_range_x、-generate_range_y～generate_range_y
		double generate_range_x;
		double generate_range_y;
		double generate_z;
};
EnemyController enemyController;

class GameController
{
	public:
		GameController()
		{
			hp = MAX_HP;
			state = 0;
			score = 0;
			hp_offset_x = 0.1;
			hp_offset_y = 0.1;
			hp_offset_z = -15.0;
			hp_offset = 0.1;
			hp_size = 0.5;
		}
		//hp減少
		int decrease_hp()
		{
			hp--;
			return hp;
		}
		//hp減少
		int increase_hp()
		{
			hp++;
			return hp;
		}
		//hpを表示
		void render_hp()
		{
			//スクリーン座標の中心をワールド座標(0,0)に移動
			double x_center_is_zero = -WINDOW_WIDTH / 2.0;
			double y_center_is_zero = -WINDOW_HEIGHT / 2.0;

			double angle = PI / 180.0 * ANGLE_OF_PERSPECTIVE / 2.0;
			double x = x_center_is_zero / (WINDOW_WIDTH / 2.0) * (camera.z - hp_offset_z) * std::tan(angle) - hp_offset_x - hp_size;
			double y = y_center_is_zero / (WINDOW_HEIGHT / 2.0) * (camera.z - hp_offset_z) * std::tan(angle) - hp_offset_x - hp_size;

			for (int i = 0; i < hp; i++)
				draw_square(x - (hp_size + hp_offset) * i, y, hp_offset_z, hp_size, hp_size);
		}
		void render_score()
		{
			std::string strScore;
			strScore = "Score:" + to_string(score);
			glPushMatrix();
			glColor3d(1.0, 1.0, 1.0);
			draw_string(strScore, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH - 100, 20);
			glPopMatrix();
		}
		void restart()
		{
			//stateをstart前に戻す
			state = 0;

			//hp回復
			hp = MAX_HP;

			//scoreを0に
			score = 0;

			//enemy全消し
			enemyController.enemyHolder.clear();

			//camera,playerの位置を初期位置に戻す
			camera.reset();
			player.reset();

			glutPostRedisplay();
		}

		int hp;  //プレイヤーの体力
		int state; //ゲームの状態、0:スタート前 1:プレイ中 2:ゲームオーバー中
		int score;

	private:
		//hp表示位置のオフセット
		double hp_offset_x;
		double hp_offset_y;
		double hp_offset_z;
		//hp間のオフセット
		double hp_offset;
		//hp画像の一辺サイズ
		double hp_size;
};
GameController gameController;

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
  glutInitDisplayMode(GLUT_RGBA);
  glutInitWindowSize(640, 480);
  windowID[1] = glutCreateWindow("mask");
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glutDisplayFunc(glut_display2);

  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  windowID[0] = glutCreateWindow(WINDOW_NAME);

  glutSetWindow(windowID[0]);
}

//初期化
void init()
{
  glClearColor(0.0, 0.0, 0.0, 0.0);

#if TEST_MODE
  //captureを開く
  open_capture();
  //matの初期化
  cv::Mat frame;
  cap >> frame;
  dst_img.create(frame.size(), CV_8UC1);
#endif

printf("Initialized completed!\n");
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
  //一番最初にキーが押されたときに、ゲーム開始
  if (gameController.state == 0)
  {
		gameController.state = 1;  //make state "now-playing"
  }
	////Gameover状態でキーが押されたときに、スタート前の状態に戻す。
	if (gameController.state == 2)
	{
		gameController.restart();
	}

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
	if (gameController.state ==  1)
		player.moveWithMouse(x, y);

  //Cameraの移動
  camera.move(x, y);
	//printf("x = %d, y = %d\n", x, y);

	glutPostRedisplay();
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
  //draw_axis();

	//確認用の正方形描画
	//draw_square(-5.0, -5.0, 20.0, 10.0, 10.0);
  //draw_square(-5.0, -5.0, -20.0, 10.0, 10.0);
  
  //playerを描画
  player.render();

	//HPを描画
	gameController.render_hp();

	//enemyHolderに入っているenemyを全て描画
	std::list<Enemy>::iterator itr;
	for (itr = enemyController.enemyHolder.begin(); itr != enemyController.enemyHolder.end(); itr++)
	{
		(*itr).render();
	}

	//before start, print "press_any_key"
	if (gameController.state == 0)
	{
		//printf("press_any_key");
		glPushMatrix();
		glColor3d(1.0, 0.0, 1.0);
		draw_string("press any key", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH / 2.0 - 50, WINDOW_HEIGHT / 2.0);
		glPopMatrix();
	}

	//スコア表示
	gameController.render_score();

	//when GAMEOVER, print "game_over and restart for press any key"
	if (gameController.state == 2)
	{
		glPushMatrix();
		glColor3d(1.0, 0.0, 0.0);
		draw_string("GAMEOVER", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH / 2.0 - 50, WINDOW_HEIGHT / 2.0);
		draw_string("to restart, press any key", WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH / 2.0 - 120, WINDOW_HEIGHT / 2.0 + 20);
		glPopMatrix();
	}

  glFlush();
  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

void glut_display2()
{
  if (dst_img.empty())
    return;

  //printf("glut_display2 loaded!\n");
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 640, 480, 0.0);
  glViewport(0, 0, 640, 480);
 
  cv::flip(dst_img, dst_img, 0);

  cv::Mat tmp;
  tmp.create(dst_img.size(), CV_8UC3);
  cv::cvtColor(dst_img, tmp, CV_GRAY2RGB);

  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(tmp.cols, tmp.rows, GL_RGB, GL_UNSIGNED_BYTE, tmp.data);
  glFlush();
}

void glut_idle()
{
  //ゲーム開始前とGAMEOVER後は各種処理をすっ飛ばす
  if (gameController.state != 1)
  {
    glutPostRedisplay();
    return;
  }
	
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

		//Enemyが適当な位置に来たらダメージ判定と削除。
		if ((*itr).z < -15.0)
		{
			//itr番目の要素をenemyHolderから削除
			itr = enemyController.enemyHolder.erase(itr);

			//hpを減らし、hpが0ならstate=2(:GameOver)にする。
			 if (gameController.decrease_hp() == 0)
			 {
			 	gameController.state = 2;
			 	break;
			 }
			printf("HP = %d\n", gameController.hp);

			printf("miss!\n");
			//printf("Object enemy deleted! %d\n", enemyController.enemyHolder.size());
		}
		//PlayerにEnemyが当たったらEnemyを消す
		else if (player.x - player.width / 2.0 < (*itr).x && (*itr).x < player.x + player.width / 2.0
				&& player.y - player.height / 2.0 < (*itr).y && (*itr).y < player.y + player.height / 2.0
				&& player.z - player.thickness / 2.0 < (*itr).z && (*itr).z < player.z + player.thickness / 2.0)
		{
			itr = enemyController.enemyHolder.erase(itr);

			//scoreを増やす
			gameController.score += 1;
			printf("hit!\n");
		}
		else
		{
			itr++;  //こうしないとerase(itr)で消してしまったitrに対してfor条件の参照が発生する。
		}
  }

#if TEST_MODE

  //手の検出により重心座標を得る
  cv::Mat frame;
  cap >> frame;

  //手の検出
  if (!frame.empty())
  {  
    detect_hand(frame, dst_img, gcenter);
    glutSetWindow(windowID[1]);
    // cv::imshow("Mask", dst_img);
    printf("get the Gravity Center Point! : %d, %d\n", gcenter.x, gcenter.y);
    glutPostRedisplay();
    glutSetWindow(windowID[0]);
  }

  //直近
  prev_prev_gcenter.x = prev_gcenter.x;
  prev_prev_gcenter.y = prev_gcenter.y;
  prev_gcenter.x = gcenter.x;
  prev_gcenter.y = gcenter.y;

  cv::Point averagePt((prev_prev_gcenter.x + prev_gcenter.x + gcenter.x) / 3.0,
                      (prev_prev_gcenter.y + prev_gcenter.y + gcenter.y) / 3.0);

  if(gcenter.x != 0 && gcenter.y != 0)
  {
    //playerの移動
    player.moveWithHand(averagePt.x, averagePt.y);
  }

#endif

	glutPostRedisplay();
}