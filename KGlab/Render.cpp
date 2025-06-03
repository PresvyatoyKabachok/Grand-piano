#include "Render.h"
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "GUItextRectangle.h"
#include "MyShaders.h"
#include "Texture.h"

#include "ObjLoader.h"

#include "debout.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define PI 3.14159265358979323846


//внутренняя логика "движка"
#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;

bool isCapOp = false;

//переключение режимов освещения, текстурирования, альфаналожения
void switchModes(OpenGL *sender, KeyEventArg arg)
{
	//конвертируем код клавиши в букву
	auto key = LOWORD(MapVirtualKeyA(arg.key, MAPVK_VK_TO_CHAR));

	if (key == 'H')
	{
		isCapOp = !isCapOp;
	}

	if (isCapOp)
	{
		switch (key)
		{
		case '1':
			PlaySoundA("sounds/2meow_C_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '2':
			PlaySoundA("sounds/2meow_Db_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '3':
			PlaySoundA("sounds/2meow_D_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '4':
			PlaySoundA("sounds/2meow_Eb_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '5':
			PlaySoundA("sounds/2meow_E_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '6':
			PlaySoundA("sounds/2meow_F_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '7':
			PlaySoundA("sounds/2meow_Gb_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '8':
			PlaySoundA("sounds/2meow_G_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '9':
			PlaySoundA("sounds/2meow_Ab_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '0':
			PlaySoundA("sounds/2meow_A_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '-':
			PlaySoundA("sounds/2meow_Bb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '=':
			PlaySoundA("sounds/2meow_B_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'Q':
			PlaySoundA("sounds/1meow_C_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'W':
			PlaySoundA("sounds/1meow_Db_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'E':
			PlaySoundA("sounds/1meow_D_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'R':
			PlaySoundA("sounds/1meow_Eb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'T':
			PlaySoundA("sounds/1meow_E_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'Y':
			PlaySoundA("sounds/1meow_F_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'U':
			PlaySoundA("sounds/1meow_Gb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'I':
			PlaySoundA("sounds/1meow_G_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'O':
			PlaySoundA("sounds/1meow_Ab_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'P':
			PlaySoundA("sounds/1meow_A_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '[':
			PlaySoundA("sounds/1meow_Bb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case ']':
			PlaySoundA("sounds/1meow_B_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'Z':
			PlaySoundA("sounds/0meow_C_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'X':
			PlaySoundA("sounds/0meow_Db_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'C':
			PlaySoundA("sounds/0meow_D_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'V':
			PlaySoundA("sounds/0meow_Eb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'B':
			PlaySoundA("sounds/0meow_E_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'N':
			PlaySoundA("sounds/0meow_F_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'M':
			PlaySoundA("sounds/0meow_Gb_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case ',':
			PlaySoundA("sounds/0meow_G_major.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '.':
			PlaySoundA("sounds/0meow_Ab_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case '/':
			PlaySoundA("sounds/0meow_A_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'A':
			PlaySoundA("sounds/0meow_Bb_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		case 'S':
			PlaySoundA("sounds/0meow_B_minor.wav", NULL, SND_FILENAME | SND_ASYNC);
			break;
		}
	}
}

//умножение матриц c[M1][N1] = a[M1][N1] * b[M2][N2]
template<typename T, int M1, int N1, int M2, int N2>
void MatrixMultiply(const T* a, const T* b, T* c)
{
	for (int i = 0; i < M1; ++i)
	{
		for (int j = 0; j < N2; ++j)
		{
			c[i * N2 + j] = T(0);
			for (int k = 0; k < N1; ++k)
			{
				c[i * N2 + j] += a[i * N1 + k] * b[k * N2 + j];
			}
		}
	}
}

//Текстовый прямоугольничек в верхнем правом углу.
//OGL не предоставляет возможности для хранения текста
//внутри этого класса создается картинка с текстом (через виндовый GDI),
//в виде текстуры накладывается на прямоугольник и рисуется на экране.
//Это самый простой способ что то написать на экране
//но ооооочень не оптимальный
GuiTextRectangle text;

//айдишник для текстуры
GLuint texId;
//выполняется один раз перед первым рендером

ObjModel f;
Shader phong_sh;

Texture iooi_tex;
Shader simple_texture_sh;

void initRender()
{
	phong_sh.VshaderFileName = "shaders/v.vert";
	phong_sh.FshaderFileName = "shaders/light.frag";
	phong_sh.LoadShaderFromFile();
	phong_sh.Compile();

	simple_texture_sh.VshaderFileName = "shaders/v.vert";
	simple_texture_sh.FshaderFileName = "shaders/textureShader.frag";
	simple_texture_sh.LoadShaderFromFile();
	simple_texture_sh.Compile();

	iooi_tex.LoadTexture("textures/iooi.png");
	f.LoadModel("models//oiio.obj");
	//==============НАСТРОЙКА ТЕКСТУР================
	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//================НАСТРОЙКА КАМЕРЫ======================
	camera.caclulateCameraPos();

	//привязываем камеру к событиям "движка"
	gl.WheelEvent.reaction(&camera, &Camera::Zoom);
	gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
	gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
	gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
	gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);
	//==============НАСТРОЙКА СВЕТА===========================
	//привязываем свет к событиям "движка"
	gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
	gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
	gl.KeyUpEvent.reaction(&light, &Light::StopDrug);
	//========================================================
	//====================Прочее==============================
	gl.KeyDownEvent.reaction(switchModes);
	text.setSize(512, 180);
	//========================================================

	camera.setPosition(2, 1.5, 1.5);
}

struct Point 
{
	double x = 0;
	double y = 0;
	double z = 0;

	const double* p() const
	{
		return &x;
	}

	Point upW() const
	{
		return { x, y, z + 0.2 };
	}

	Point upB() const
	{
		return { x, y, z + 0.22 };
	}

	Point operator-(const Point& other) const 
	{
		return { x - other.x, y - other.y, z - other.z };
	}
};

Point CalculateQuadNormal(const Point& A, const Point& B, const Point& C, const Point& D, bool invertZ = false) 
{
	Point AB = B - A;
	Point AD = D - A;

	// Векторное произведение AB x AD
	Point N
	{
		AB.y * AD.z - AB.z * AD.y,
		-AB.x * AD.z + AB.z * AD.x,
		AB.x * AD.y - AB.y * AD.x
	};

	// Нормализация
	double length = sqrt(N.x * N.x + N.y * N.y + N.z * N.z);
	if (length > 0) 
	{
		N.x /= length;
		N.y /= length;
		N.z /= length;
	}

	// Инверсия по Z 
	if (invertZ) 
	{
		N.z = -N.z;
	}

	return N;
}
float view_matrix[16];
double full_time = 0;
int location = 0;

double CapAngle = 0.0;
const double maxCapAngle = 25.0, speed = 40.0;

void updateCapAnim(double delta_time)
{
	if (isCapOp and CapAngle < maxCapAngle)
	{
		CapAngle += speed * delta_time;
		if (CapAngle > maxCapAngle) 
			CapAngle = maxCapAngle;
	}
	else if (!isCapOp and CapAngle > 0) 
	{
		CapAngle -= speed * delta_time;
		if (CapAngle < 0)
			CapAngle = 0;
	}
}

void White(float light_pos_v[4])
{
	location = glGetUniformLocationARB(phong_sh.program, "Ia");
	glUniform3fARB(location, 1, 1, 1);
	location = glGetUniformLocationARB(phong_sh.program, "Id");
	glUniform3fARB(location, 1, 1, 1);
	location = glGetUniformLocationARB(phong_sh.program, "Is");
	glUniform3fARB(location, 1, 1, 1);

	location = glGetUniformLocationARB(phong_sh.program, "ma");
	glUniform3fARB(location, 0.1, 0.1, 0.1);
	location = glGetUniformLocationARB(phong_sh.program, "md");
	glUniform3fARB(location, 1, 1, 1);
	location = glGetUniformLocationARB(phong_sh.program, "ms");
	glUniform4fARB(location, 1, 1, 1, 128);

	location = glGetUniformLocationARB(phong_sh.program, "light_pos_v");
	glUniform3fvARB(location, 1, light_pos_v);
}

void Black(float light_pos_v[4])
{
	location = glGetUniformLocationARB(phong_sh.program, "Ia");
	glUniform3fARB(location, 0.3, 0.3, 0.3);
	location = glGetUniformLocationARB(phong_sh.program, "Id");
	glUniform3fARB(location, 0.8, 0.8, 0.8);
	location = glGetUniformLocationARB(phong_sh.program, "Is");
	glUniform3fARB(location, 1.0, 1.0, 1.0);

	location = glGetUniformLocationARB(phong_sh.program, "ma");
	glUniform3fARB(location, 0.05, 0.05, 0.05);
	location = glGetUniformLocationARB(phong_sh.program, "md");
	glUniform3fARB(location, 0.1, 0.1, 0.1);
	location = glGetUniformLocationARB(phong_sh.program, "ms");
	glUniform4fARB(location, 1.0, 1.0, 1.0, 128);

	location = glGetUniformLocationARB(phong_sh.program, "light_pos_v");
	glUniform3fvARB(location, 1, light_pos_v);
}

void DrawWKey(Point A, Point B,Point C,Point D,Point E,Point F,Point G, Point H, int fl,float light_pos_v[4])
{
	White(light_pos_v);
	glPushMatrix();
	Point A1 = A.upW();
	Point B1 = B.upW();
	Point C1 = C.upW();
	Point D1 = D.upW();
	Point E1 = E.upW();
	Point F1 = F.upW();
	Point G1 = G.upW();
	Point H1 = H.upW();

	Point N = CalculateQuadNormal( E1,F1, F,E, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(E.p());
	glVertex3dv(F.p());
	glVertex3dv(F1.p());
	glVertex3dv(E1.p());
	glEnd();

	N = CalculateQuadNormal(B, C, C1, B1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(C1.p());
	glVertex3dv(B1.p());
	glEnd();

	N = CalculateQuadNormal(A1, B1, C1, D1, false);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A1.p());
	glVertex3dv(B1.p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glEnd();

	if (!fl)
	{
		N = CalculateQuadNormal(C, G, G1, C1, true);
		glBegin(GL_QUADS);
		glNormal3dv(N.p());
		glVertex3dv(G.p());
		glVertex3dv(C.p());
		glVertex3dv(C1.p());
		glVertex3dv(G1.p());
		glEnd();
	}
	N = CalculateQuadNormal(G, F, F1, G1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(G.p());
	glVertex3dv(F.p());
	glVertex3dv(F1.p());
	glVertex3dv(G1.p());
	glEnd();

	N = CalculateQuadNormal(D, A, A1, D1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(D.p());
	glVertex3dv(A.p());
	glVertex3dv(A1.p());
	glVertex3dv(D1.p());
	glEnd();

	N = CalculateQuadNormal(E, H, H1, E1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(E.p());
	glVertex3dv(H.p());
	glVertex3dv(H1.p());
	glVertex3dv(E1.p());
	glEnd();


	N = CalculateQuadNormal(E1, F1, G1, H1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(E1.p());
	glVertex3dv(F1.p());
	glVertex3dv(G1.p());
	glVertex3dv(H1.p());
	glEnd();

	if (fl)
	{
		N = CalculateQuadNormal(H, D, D1, H1, true);
		glBegin(GL_QUADS);
		glNormal3dv(N.p());
		glVertex3dv(H.p());
		glVertex3dv(D.p());
		glVertex3dv(D1.p());
		glVertex3dv(H1.p());
		glEnd();
	}

	glPopMatrix();
}

void DrawBKey(Point A, Point B, Point C, Point D, Point E, Point F, Point G, Point H, float light_pos_v[4])
{
	Black(light_pos_v);
	glPushMatrix();
	Point A1 = A.upB();
	Point B1 = B.upB();
	Point C1 = C.upB();
	Point D1 = D.upB();

	Point N = CalculateQuadNormal(D, A, A1, D1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A.p());
	glVertex3dv(D.p());
	glVertex3dv(D1.p());
	glVertex3dv(A1.p());
	glEnd();

	N = CalculateQuadNormal(C, D, D1, C1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(D.p());
	glVertex3dv(C.p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glEnd();
	
	N = CalculateQuadNormal(B, C, C1, B1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(C.p());
	glVertex3dv(B.p());
	glVertex3dv(B1.p());
	glVertex3dv(C1.p());
	glEnd();

	N = CalculateQuadNormal(C1, D1, H, G, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(C1.p());
	glVertex3dv(G.p());
	glVertex3dv(H.p());
	glVertex3dv(D1.p());
	glEnd();

	N = CalculateQuadNormal(B1, C1, G, F, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(C1.p());
	glVertex3dv(G.p());
	glVertex3dv(F.p());
	glVertex3dv(B1.p());
	glEnd();

	N = CalculateQuadNormal(D1, A1, E, H, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(D1.p());
	glVertex3dv(H.p());
	glVertex3dv(E.p());
	glVertex3dv(A1.p());
	glEnd();

	N = CalculateQuadNormal(A1, B1, F, E, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(B1.p());
	glVertex3dv(F.p());
	glVertex3dv(E.p());
	glVertex3dv(A1.p());
	glEnd();

	N = CalculateQuadNormal(H, G, F, E, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(G.p());
	glVertex3dv(F.p());
	glVertex3dv(E.p());
	glVertex3dv(H.p());
	glEnd();

	glPopMatrix();
}

void DrawOctav(double x, float light_pos_v[4])
{
	Point A { x+0,0,0 };
	Point B { x+0.13,0,0 }; 
	Point C { x+0.13,1.01,0 };
	Point D { x+0,1.01,0 };
	Point E { x+0,1.5,0 }; 
	Point F { x+0.23,1.5,0 };
	Point G { x+0.23,1.01,0 };
	Point H { x+0,1.01,0 };
	DrawWKey(A,B,C,D,E,F,G,H,0, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.12;
	C = { B.x,1,0 };
	D = { A.x,1,0 };
	E = { A.x + 0.008,0.02,0.3 };
	F = { B.x - 0.008,0.02,0.3 };
	G = { F.x,0.9,0.3 };
	H = { E.x,0.9,0.3 };
	DrawBKey(A, B, C, D, E, F, G, H, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.15;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x - 0.012,1.5,0 };
	F = { E.x+0.23 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H,1, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.12;
	C = { B.x,1,0 };
	D = { A.x,1,0 };
	E = { A.x + 0.008,0.02,0.3 };
	F = { B.x - 0.008,0.02,0.3 };
	G = { F.x,0.9,0.3 };
	H = { E.x,0.9,0.3 };
	DrawBKey(A, B, C, D, E, F, G, H, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.15;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x - 0.054,1.5,0 };
	F = { E.x + 0.23 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H, 1, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.12;
	C = { B.x,1,0 };
	D = { A.x,1,0 };
	E = { A.x + 0.008,0.02,0.3 };
	F = { B.x - 0.008,0.02,0.3 };
	G = { F.x,0.9,0.3 };
	H = { E.x,0.9,0.3 };
	DrawBKey(A, B, C, D, E, F, G, H, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.13;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x - 0.096,1.5,0 };
	F = { E.x + 0.236 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H, 0, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.13;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x +0.002,1.5,0 };
	F = { E.x + 0.22 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H, 1, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.12;
	C = { B.x,1,0 };
	D = { A.x,1,0 };
	E = { A.x + 0.008,0.02,0.3 };
	F = { B.x - 0.008,0.02,0.3 };
	G = { F.x,0.9,0.3 };
	H = { E.x,0.9,0.3 };
	DrawBKey(A, B, C, D, E, F, G, H, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.15;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x - 0.022,1.5,0 };
	F = { E.x + 0.22 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H, 1, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.12;
	C = { B.x,1,0 };
	D = { A.x,1,0 };
	E = { A.x + 0.008,0.02,0.3 };
	F = { B.x - 0.008,0.02,0.3 };
	G = { F.x,0.9,0.3 };
	H = { E.x,0.9,0.3 };
	DrawBKey(A, B, C, D, E, F, G, H, light_pos_v);

	A.x = B.x + 0.002;
	B.x = A.x + 0.13;
	C = { B.x,1.01,0 };
	D = { A.x,1.01,0 };
	E = { F.x - 0.074,1.5,0 };
	F = { E.x + 0.214 ,1.5,0 };
	G = { F.x,1.01,0 };
	H = { E.x,1.01,0 };
	DrawWKey(A, B, C, D, E, F, G, H, 0, light_pos_v);
}

void DrawKeyboard()
{
	glPushMatrix();
	Point A { -0.01,0,0.5 };
	Point B { -0.01,1.52,0.1 };
	Point C { -0.01,1.52,-0.2 };
	Point D { -0.01,0,-0.2 };
	Point E{ -0.2,-0.2,0.53 };
	Point F{ -0.2,1.61,0.1 };
	Point G{ -0.2,1.61,-0.2 };
	Point H{ -0.2,-0.2,-0.2 };
	Point A1{ 4.783,0,0.5 };
	Point B1{ 4.783,1.52,0.1 };
	Point C1{ 4.783,1.52,-0.2 };
	Point D1{ 4.783,0,-0.2 };
	Point E1{ 4.983,-0.2,0.53 };
	Point F1{ 4.983,1.61,0.1 };
	Point G1{ 4.983,1.61,-0.2 };
	Point H1{ 4.983,-0.2,-0.2 };

	Point N = CalculateQuadNormal(A, D, C, B, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(D.p());
	glEnd();

	N = CalculateQuadNormal(G,H, E, F, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(H.p());
	glVertex3dv(G.p());
	glVertex3dv(F.p());
	glVertex3dv(E.p());
	glEnd();

	N = CalculateQuadNormal(B, A, E, F, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(B.p());
	glVertex3dv(A.p());
	glVertex3dv(E.p());
	glVertex3dv(F.p());
	glEnd();

	N = CalculateQuadNormal(D1, A1, B1, C1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A1.p());
	glVertex3dv(B1.p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glEnd();

	N = CalculateQuadNormal(H1, G1, F1, E1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(H1.p());
	glVertex3dv(G1.p());
	glVertex3dv(F1.p());
	glVertex3dv(E1.p());
	glEnd();

	N = CalculateQuadNormal(A1, B1, F1, E1, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(B1.p());
	glVertex3dv(A1.p());
	glVertex3dv(E1.p());
	glVertex3dv(F1.p());
	glEnd();

	N = CalculateQuadNormal(A, A1, D1, D, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A1.p());
	glVertex3dv(A.p());
	glVertex3dv(D.p());
	glVertex3dv(D1.p());
	glEnd();

	N = CalculateQuadNormal(A, A1, E1, E, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(A1.p());
	glVertex3dv(A.p());
	glVertex3dv(E.p());
	glVertex3dv(E1.p());
	glEnd();

	N = CalculateQuadNormal(H, H1, E1, E, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(E1.p());
	glVertex3dv(E.p());
	glVertex3dv(H.p());
	glVertex3dv(H1.p());
	glEnd();

	N = CalculateQuadNormal(C, C1, B1, B, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(B1.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(C1.p());
	glEnd();

	N = CalculateQuadNormal(F, F1, G1, G, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(F1.p());
	glVertex3dv(F.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	N = CalculateQuadNormal(F, F1, B1, B, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(F1.p());
	glVertex3dv(F.p());
	glVertex3dv(B.p());
	glVertex3dv(B1.p());
	glEnd();

	N = CalculateQuadNormal(H, H1, G1, G, true);
	glBegin(GL_QUADS);
	glNormal3dv(N.p());
	glVertex3dv(H1.p());
	glVertex3dv(H.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	glPopMatrix();
}

void DrawLegs(double r,double Ox,double Oy)
{
	double a1, a2;
	int n = 180;
	
	for (int i = 0; i < n; i++)
	{
		a1 = 2*PI / n * i;
		a2 = 2*PI / n * (i + 1);
		Point A = { Ox + r * cos(a1), Oy + r * sin(a1), -0.2 };
		Point B = { Ox + r * cos(a2), Oy + r * sin(a2), -0.2 };
		Point C = { Ox + r * cos(a2), Oy + r * sin(a2), -2 };
		Point D = { Ox + r * cos(a1), Oy + r * sin(a1), -2 };
		Point N = CalculateQuadNormal(B, A, D, C);
		
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();
	}
}

void DrawPodium(double r, double Ox, double Oy)
{
	double a1, a2;
	int n = 180;

	for (int i = 0; i < n; i++)
	{
		a1 = 2*PI / n * i;
		a2 = 2* PI / n * (i + 1);
		Point A = { Ox + r * cos(a1), Oy + r * sin(a1), -2 };
		Point B = { Ox + r * cos(a2), Oy + r * sin(a2), -2 };
		Point C = { Ox + r * cos(a2), Oy + r * sin(a2), -3 };
		Point D = { Ox + r * cos(a1), Oy + r * sin(a1), -3 };
		Point M = { Ox, Oy, -2 };
		
		Point N = CalculateQuadNormal(A, B, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();

		N = CalculateQuadNormal(B, A, D, C);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();

	}

	r += 1;
	for (int i = 0; i < n; i++)
	{
		a1 = 2 * PI / n * i;
		a2 = 2 * PI / n * (i + 1);
		Point A = { Ox + r * cos(a1), Oy + r * sin(a1), -4 };
		Point B = { Ox + r * cos(a2), Oy + r * sin(a2), -4 };
		Point C = { Ox + r * cos(a2), Oy + r * sin(a2), -3 };
		Point D = { Ox + r * cos(a1), Oy + r * sin(a1), -3 };
		Point M = { Ox, Oy, -4 };

		Point N = CalculateQuadNormal(B, A, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();

		M = { Ox, Oy, -3 };
		N = CalculateQuadNormal(D, C, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glVertex3dv(M.p());
		glEnd();

		N = CalculateQuadNormal(A, B, C, D);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();
	}
}

void DrawS(double Ox,double Oy)
{
	double r = 1.5, a1, a2;
	int n = 180;
	double start_angle = PI;      
	double end_angle = 2 * PI;
	for (int i = 0; i < n; i++)
	{
		a1 = start_angle + (end_angle - start_angle) * i / n;
		a2 = start_angle + (end_angle - start_angle) * (i + 1) / n;
		Point A = { Ox + r * cos(a1), Oy + r * sin(a1), -0.2 };
		Point B = { Ox + r * cos(a2), Oy + r * sin(a2), -0.2 };
		Point C = { Ox + r * cos(a2), Oy + r * sin(a2), 1 };
		Point D = { Ox + r * cos(a1), Oy + r * sin(a1), 1 };
		Point M = { Ox, Oy, -0.2 };

		Point N = CalculateQuadNormal(B, A, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();

		N = CalculateQuadNormal(A, B, C, D);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();
		
		A = { Ox + (r - 0.03) * cos(a1), Oy + (r - 0.03) * sin(a1), 1 };
		B = { Ox + (r - 0.03) * cos(a2), Oy + (r - 0.03) * sin(a2), 1 };
		C = { Ox + (r - 0.03) * cos(a2), Oy + (r - 0.03) * sin(a2), -0.15 };
		D = { Ox + (r - 0.03) * cos(a1), Oy + (r - 0.03) * sin(a1), -0.15 };
		M = { Ox, Oy, -0.15 };
		N = CalculateQuadNormal(D, C, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glVertex3dv(M.p());
		glEnd();

		N = CalculateQuadNormal(A, B, C, D);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();

		C = { Ox + r * cos(a2), Oy + r * sin(a2), 1 };
		D = { Ox + r * cos(a1), Oy + r * sin(a1), 1 };
		N = CalculateQuadNormal(B, A, D, C);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();
	}

	Point A{4.983,-0.2,1};
	Point B{ 4.983,-2,1 };
	Point C{ 4.983,-2,-0.2 };
	Point D{ 4.983,-0.2,-0.2 };
	Point A1{ 4.953,-0.21,1 };
	Point B1{ 4.953,-2,1 };
	Point C1{ 4.953,-2,-0.2 };
	Point D1{ 4.953,-0.2,-0.2 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(A, B, C, D, true).p());
	glVertex3dv(A.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(D.p());
	glEnd();
	
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(B1,A1,D1, C1, true).p());
	glVertex3dv(A1.p());
	glVertex3dv(B1.p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(B1, A1, A, B, true).p());
	glVertex3dv(A1.p());
	glVertex3dv(B1.p());
	glVertex3dv(B.p());
	glVertex3dv(A.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(D1, C1, C, D, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(D.p());
	glVertex3dv(C.p());
	glEnd();

	Point E{ -0.2,-0.2,1 };
	Point F{ -0.2,-1,1 };
	Point G{ -0.2,-1,-0.2 };
	Point H{ -0.2,-0.2,-0.2 };
	Point E1{ -0.17,-0.21,1 };
	Point F1{ -0.17,-0.97,1 };
	Point G1{ -0.17,-0.97,-0.2 };
	Point H1{ -0.17,-0.2,-0.2 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(F, E, H, G, true).p());
	glVertex3dv(E.p());
	glVertex3dv(F.p());
	glVertex3dv(G.p());
	glVertex3dv(H.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(E1, F1, G1, H1, true).p());
	glVertex3dv(E1.p());
	glVertex3dv(F1.p());
	glVertex3dv(G1.p());
	glVertex3dv(H1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(E1, F1, F, E, true).p());
	glVertex3dv(E1.p());
	glVertex3dv(F1.p());
	glVertex3dv(F.p());
	glVertex3dv(E.p());
	glEnd();

	H.z = 0.53;
	D.z = H.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H, E, A, D, true).p());
	glVertex3dv(E.p());
	glVertex3dv(H.p());
	glVertex3dv(D.p());
	glVertex3dv(A.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(A1, E1,E, A, true).p());
	glVertex3dv(E.p());
	glVertex3dv(E1.p());
	glVertex3dv(A1.p());
	glVertex3dv(A.p());
	glEnd();

	H1.z = -0.15;
	D1.z = H1.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(E1, H1, D1, A1, true).p());
	glVertex3dv(E1.p());
	glVertex3dv(H1.p());
	glVertex3dv(D1.p());
	glVertex3dv(A1.p());
	glEnd();

	Point J{ 0.9915 ,-0.979,-0.2 };
	Point J1{ 0.9915 ,-0.979,1};
	Point K{ 0.9915 ,-1.009,-0.2 };
	Point K1{ 0.9915 ,-1.009,1 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(G1, F1, J1, J, true).p());
	glVertex3dv(F1.p());
	glVertex3dv(G1.p());
	glVertex3dv(J.p());
	glVertex3dv(J1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(F, G, K, K1, true).p());
	glVertex3dv(F.p());
	glVertex3dv(G.p());
	glVertex3dv(K.p());
	glVertex3dv(K1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(J1, K1, F, F1, true).p());
	glVertex3dv(J1.p());
	glVertex3dv(K1.p());
	glVertex3dv(F.p());
	glVertex3dv(F1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(K, J, G1, G, true).p());
	glVertex3dv(J.p());
	glVertex3dv(K.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	H.z = -0.2;
	H1.z = H.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H1, H, G, G1, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(H.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	double Mx = 0.9915, My = -2;
	r = 0.9915;

	for (int i = 0; i < n; i++)
	{
		a1 = 0.5 * PI / n * i;
		a2 = 0.5 * PI / n * (i + 1);
		Point A = { Mx + r * cos(a1), My + r * sin(a1), -0.2 };
		Point B = { Mx + r * cos(a2), My + r * sin(a2), -0.2 };
		Point C = { Mx + r * cos(a2), My + r * sin(a2), 1 };
		Point D = { Mx + r * cos(a1), My + r * sin(a1), 1 };
		Point M = { 1.983, -0.2, -0.2 };

		Point N = CalculateQuadNormal(A, B, M, M);
		glNormal3dv(N.p());

		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();
		
		N = CalculateQuadNormal(B, A, D, C); 
		glNormal3dv(N.p());

		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();

		A = { Mx + (r + 0.03) * cos(a1), My + (r + 0.03) * sin(a1), -0.15 };
		B = { Mx + (r + 0.03) * cos(a2), My + (r + 0.03) * sin(a2), -0.15 };
		C = { Mx + (r + 0.03) * cos(a1), My + (r + 0.03) * sin(a1), -0.2 };
		D = { Mx + (r + 0.03) * cos(a2), My + (r + 0.03) * sin(a2), -0.2 };
		M = { 1.983, -0.2, -0.15 };

		N = CalculateQuadNormal(B, A, M, M);
		glNormal3dv(N.p());

		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();

		A = { Mx + (r + 0.03) * cos(a1), My + (r + 0.03) * sin(a1), 1 };
		B = { Mx + (r + 0.03) * cos(a2), My + (r + 0.03) * sin(a2), 1 };
		C = { Mx + (r + 0.03) * cos(a1), My + (r + 0.03) * sin(a1), -0.15 };
		D = { Mx + (r + 0.03) * cos(a2), My + (r + 0.03) * sin(a2), -0.15 };
		N = CalculateQuadNormal(B, A, D, C);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(D.p());
		glVertex3dv(C.p());
		
		glEnd();
	
		C = { Mx + r * cos(a2), My + r * sin(a2), 1 };
		D = { Mx + r * cos(a1), My + r * sin(a1), 1 };
		N = CalculateQuadNormal(A, B, C, D);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();
		
	}

	Point u{ 1.983, -0.2, -0.15 };
	Point u1{ 1.983, -0.2, -0.2 };

	glBegin(GL_TRIANGLES);
	glNormal3dv(CalculateQuadNormal(J, K , u1, u1, true).p());
	glVertex3dv(J.p());
	glVertex3dv(K.p());
	glVertex3dv(u1.p());

	glEnd();
	
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H1, G1, J, u1, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(G1.p());
	glVertex3dv(J.p());
	glVertex3dv(u1.p());
	glEnd();

	H1.z = -0.15;
	G1.z = H1.z;
	J.z = H1.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(G1, H1, u, J, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(G1.p());
	glVertex3dv(J.p());
	glVertex3dv(u.p());
	glEnd();

	D1.z = -0.2;
	Point o1{ 1.983,-2, -0.2};
	Point o{ 1.983,-2, -0.15 };
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(C1, D1, u1, o1, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(u1.p());
	glVertex3dv(o1.p());
	glEnd();

	D1.z = -0.15;
	C1.z = D1.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(D1, C1, o, u, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(u.p());
	glVertex3dv(o.p());
	glEnd();
}

void DrawCap(double Ox, double Oy)
{
	double r = 1.5, a1, a2, start_angle = PI, end_angle = 2 * PI;
	int n = 180;
	for (int i = 0; i < n; i++)
	{
		a1 = start_angle + (end_angle - start_angle) * i / n;
		a2 = start_angle + (end_angle - start_angle) * (i + 1) / n;
		Point A = { Ox + r * cos(a1), Oy + r * sin(a1), 1 };
		Point B = { Ox + r * cos(a2), Oy + r * sin(a2), 1 };
		Point C = { Ox + r * cos(a2), Oy + r * sin(a2), 1.05 };
		Point D = { Ox + r * cos(a1), Oy + r * sin(a1), 1.05 };
		Point M = { Ox, Oy, 1 };

		Point N = CalculateQuadNormal(B, A, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();
		
		N = CalculateQuadNormal(A, B, C, D);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();

		M = { Ox, Oy, 1.05 };
		N = CalculateQuadNormal(D, C, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glVertex3dv(M.p());
		glEnd();
	}

	Point A{ 4.983,-0.2,1.05 };
	Point B{ 4.983,-2,1.05 };
	Point C{ 4.983,-2,1 };
	Point D{ 4.983,-0.2,1 };
	Point A1{ 4.953,-0.21,1.05 };
	Point B1{ 4.953,-2,1.05 };
	Point C1{ 4.953,-2,1 };
	Point D1{ 4.953,-0.2,1 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(A, B, C, D, true).p());
	glVertex3dv(A.p());
	glVertex3dv(B.p());
	glVertex3dv(C.p());
	glVertex3dv(D.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(B1, A1, A, B, true).p());
	glVertex3dv(A1.p());
	glVertex3dv(B1.p());
	glVertex3dv(B.p());
	glVertex3dv(A.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(D1, C1, C, D, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(D.p());
	glVertex3dv(C.p());
	glEnd();

	Point E{ -0.2,-0.2,1.05 };
	Point F{ -0.2,-1,1.05 };
	Point G{ -0.2,-1,1 };
	Point H{ -0.2,-0.2,1 };
	Point E1{ -0.17,-0.21,1.05 };
	Point F1{ -0.17,-0.97,1.05 };
	Point G1{ -0.17,-0.97,1 };
	Point H1{ -0.17,-0.2,1 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(F, E, H, G, true).p());
	glVertex3dv(E.p());
	glVertex3dv(F.p());
	glVertex3dv(G.p());
	glVertex3dv(H.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(E1, F1, F, E, true).p());
	glVertex3dv(E1.p());
	glVertex3dv(F1.p());
	glVertex3dv(F.p());
	glVertex3dv(E.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H, E, A, D, true).p());
	glVertex3dv(E.p());
	glVertex3dv(H.p());
	glVertex3dv(D.p());
	glVertex3dv(A.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(A1, E1, E, A, true).p());
	glVertex3dv(E.p());
	glVertex3dv(E1.p());
	glVertex3dv(A1.p());
	glVertex3dv(A.p());
	glEnd();

	Point J{ 0.9915 ,-0.979,1 };
	Point J1{ 0.9915 ,-0.979,1.05 };
	Point K{ 0.9915 ,-1.009,1 };
	Point K1{ 0.9915 ,-1.009,1.05 };

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(F, G, K, K1, true).p());
	glVertex3dv(F.p());
	glVertex3dv(G.p());
	glVertex3dv(K.p());
	glVertex3dv(K1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(J1, K1, F, F1, true).p());
	glVertex3dv(J1.p());
	glVertex3dv(K1.p());
	glVertex3dv(F.p());
	glVertex3dv(F1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(K, J, G1, G, true).p());
	glVertex3dv(J.p());
	glVertex3dv(K.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H1, H, G, G1, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(H.p());
	glVertex3dv(G.p());
	glVertex3dv(G1.p());
	glEnd();

	double Mx = 0.9915, My = -2;
	r = 0.9915;

	for (int i = 0; i < n; i++)
	{
		a1 = 0.5 * PI / n * i;
		a2 = 0.5 * PI / n * (i + 1);
		Point A = { Mx + r * cos(a1), My + r * sin(a1), 1 };
		Point B = { Mx + r * cos(a2), My + r * sin(a2), 1 };
		Point C = { Mx + r * cos(a2), My + r * sin(a2), 1.05 };
		Point D = { Mx + r * cos(a1), My + r * sin(a1), 1.05 };
		Point M = { 1.983, -0.2, 1.05 };

		Point N = CalculateQuadNormal(C, D, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glVertex3dv(M.p());
		glEnd();

		N = CalculateQuadNormal(B, A, D, C);
		glNormal3dv(N.p());
		glBegin(GL_QUADS);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(C.p());
		glVertex3dv(D.p());
		glEnd();

		M = { 1.983, -0.2, 1 };
		N = CalculateQuadNormal(A, B, M, M);
		glNormal3dv(N.p());
		glBegin(GL_TRIANGLES);
		glVertex3dv(A.p());
		glVertex3dv(B.p());
		glVertex3dv(M.p());
		glEnd();
	}

	Point u{ 1.983, -0.2, 1.05 };
	Point u1{ 1.983, -0.2, 1 };

	glBegin(GL_TRIANGLES);
	glNormal3dv(CalculateQuadNormal(J, K, u1, u1, true).p());
	glVertex3dv(J.p());
	glVertex3dv(K.p());
	glVertex3dv(u1.p());
	glEnd();

	glBegin(GL_TRIANGLES);
	glNormal3dv(CalculateQuadNormal(K1, J1, u, u, true).p());
	glVertex3dv(J1.p());
	glVertex3dv(K1.p());
	glVertex3dv(u.p());
	glEnd();

	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(H1, G1, J, u1, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(G1.p());
	glVertex3dv(J.p());
	glVertex3dv(u1.p());
	glEnd();

	H1.z = 1.05;
	G1.z = H1.z;
	J.z = H1.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(G1, H1, u, J, true).p());
	glVertex3dv(H1.p());
	glVertex3dv(G1.p());
	glVertex3dv(J.p());
	glVertex3dv(u.p());
	glEnd();

	Point o1{ 1.983,-2, 1 };
	Point o{ 1.983,-2, 1.05 };
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(C1, D1, u1, o1, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(u1.p());
	glVertex3dv(o1.p());
	glEnd();

	D1.z = 1.05;
	C1.z = D1.z;
	glBegin(GL_QUADS);
	glNormal3dv(CalculateQuadNormal(D1, C1, o, u, true).p());
	glVertex3dv(C1.p());
	glVertex3dv(D1.p());
	glVertex3dv(u.p());
	glVertex3dv(o.p());
	glEnd();
}

void DrawBody(double delta_time)
{
	
	DrawKeyboard();
	DrawS(3.483, -2);
	DrawLegs(0.15,0.4, 0.5);
	DrawLegs(0.15,4.583, 0.5);
	DrawLegs(0.15, 3.483, -2);
	
	glPushMatrix();
	updateCapAnim(delta_time);
	glPushMatrix();
	glTranslated(4.983, -0.2, 1);
	glRotatef(CapAngle, 0, 1, 0);
	glTranslated(-4.983, 0.2, -1);
	DrawCap(3.483, -2);
	glPopMatrix();
}

void Render(double delta_time)
{    
	
	full_time += delta_time;
	
	//натройка камеры и света
	//в этих функциях находятся OGLные функции
	//которые устанавливают параметры источника света
	//и моделвью матрицу, связанные с камерой.

	if (gl.isKeyPressed('F')) //если нажата F - свет из камеры
	{
		light.SetPosition(camera.x(), camera.y(), camera.z());
	}
	camera.SetUpCamera();
	//забираем моделвью матрицу сразу после установки камера
	//так как в ней отсуствуют трансформации glRotate...
	//она, фактически, является видовой.
	glGetFloatv(GL_MODELVIEW_MATRIX,view_matrix);

	light.SetUpLight();

	//рисуем оси
	gl.DrawAxes();

	glBindTexture(GL_TEXTURE_2D, 0);

	//включаем нормализацию нормалей
	//чтобв glScaled не влияли на них.

	glEnable(GL_NORMALIZE);  
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	//включаем режимы, в зависимости от нажания клавиш. см void switchModes(OpenGL *sender, KeyEventArg arg)
	
		
	//=============НАСТРОЙКА МАТЕРИАЛА==============


	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH); //закраска по Гуро      
			   //(GL_SMOOTH - плоская закраска)

	//============ РИСОВАТЬ ТУТ ==============


	//Квадратик с освещением
	phong_sh.UseShader();

	float light_pos[4] = { light.x(),light.y(), light.z(), 1 };
	float light_pos_v[4];
	
	//переносим координаты света в видовые координаты
	MatrixMultiply<float, 1, 4, 4, 4>(light_pos, view_matrix, light_pos_v);

	
	location = glGetUniformLocationARB(phong_sh.program, "Ia");
	glUniform3fARB(location, 0.9, 0, 0.9);
	location = glGetUniformLocationARB(phong_sh.program, "Id");
	glUniform3fARB(location, 0.7, 0, 0.7);
	location = glGetUniformLocationARB(phong_sh.program, "Is");
	glUniform3fARB(location, 0, 0 ,0);

	location = glGetUniformLocationARB(phong_sh.program, "ma");
	glUniform3fARB(location, 0.1, 0.1, 0.1);
	location = glGetUniformLocationARB(phong_sh.program, "md");
	glUniform3fARB(location, 0.6, 0.6, 0.6);
	location = glGetUniformLocationARB(phong_sh.program, "ms");
	glUniform4fARB(location,1, 1, 1, 100);
		
	location = glGetUniformLocationARB(phong_sh.program, "light_pos_v");
	glUniform3fvARB(location,1,light_pos_v);
	
	DrawPodium(5, 2.5, 0);
	
	Black(light_pos_v);

	DrawBody(delta_time);

	DrawOctav(0, light_pos_v);
	DrawOctav(1.594, light_pos_v);
	DrawOctav(3.188, light_pos_v);

	simple_texture_sh.UseShader();
	location = glGetUniformLocationARB(simple_texture_sh.program, "tex");
	glUniform1iARB(location, 0);
	glActiveTexture(GL_TEXTURE0);
	iooi_tex.Bind();

	glPushMatrix();
	glTranslated(3.483, -1.9, -0.14);
	glScaled(3, 3, 3);
	glRotated(180, 0, 1, 1);
	f.Draw();
	glPopMatrix();

	//===============================================
	
	//сбрасываем все трансформации
	glLoadIdentity();
	camera.SetUpCamera();	
	Shader::DontUseShaders();
	//рисуем источник света
	light.DrawLightGizmo();

	//================Сообщение в верхнем левом углу=======================
	glActiveTexture(GL_TEXTURE0);
	//переключаемся на матрицу проекции
	glMatrixMode(GL_PROJECTION);
	//сохраняем текущую матрицу проекции с перспективным преобразованием
	glPushMatrix();
	//загружаем единичную матрицу в матрицу проекции
	glLoadIdentity();

	//устанавливаем матрицу паралельной проекции
	glOrtho(0, gl.getWidth() - 1, 0, gl.getHeight() - 1, 0, 1);

	//переключаемся на моделвью матрицу
	glMatrixMode(GL_MODELVIEW);
	//сохраняем матрицу
	glPushMatrix();
    //сбразываем все трансформации и настройки камеры загрузкой единичной матрицы
	glLoadIdentity();

	//отрисованное тут будет визуалзироватся в 2д системе координат
	//нижний левый угол окна - точка (0,0)
	//верхний правый угол (ширина_окна - 1, высота_окна - 1)

	std::wstringstream ss;
	ss << std::fixed << std::setprecision(3);
	ss << "H - " << (isCapOp ? L"[вкл]выкл  " : L" вкл[выкл] ") << L"открыть крышку" << std::endl;
	ss << L"от 1 до = вторая октава" << std::endl;
	ss << L"от Q до ] первая октава" << std::endl;
	ss << L"от Z до / малая октава" << std::endl;
	ss << L"F - Свет из камеры" << std::endl;
	ss << L"G - двигать свет по горизонтали" << std::endl;
	ss << L"G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << L"Коорд. света: (" << std::setw(7) <<  light.x() << "," << std::setw(7) << light.y() << "," << std::setw(7) << light.z() << ")" << std::endl;
	ss << L"Коорд. камеры: (" << std::setw(7) << camera.x() << "," << std::setw(7) << camera.y() << "," << std::setw(7) << camera.z() << ")" << std::endl;
	ss << L"Параметры камеры: R=" << std::setw(7) << camera.distance() << ",fi1=" << std::setw(7) << camera.fi1() << ",fi2=" << std::setw(7) << camera.fi2() << std::endl;
	ss << L"delta_time: " << std::setprecision(5)<< delta_time <<  L"   full_time: " << std::setprecision(2) << full_time << std::endl;


	text.setPosition(10, gl.getHeight() - 10 - 180);
	text.setText(ss.str().c_str());
	
	text.Draw();

	//восстанавливаем матрицу проекции на перспективу, которую сохраняли ранее.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}   
