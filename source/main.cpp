#include <Windows.h>
#include "res.h"
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>

#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"glu32.lib")

HDC hDc = NULL;
HGLRC hRc = NULL;

CHAR sz_text[500];
GLuint idTexture;
float fStep = 0.0;

GLint DrawGLScene();
void DrawCube1(int a, int b, int c);
void DrawCube(int xmin,int xmax, int ymin, int ymax, int zmin, int zmax, int r, int g, int b);
void DrawSide(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, int r, int g, int b);
void glDrawBoard();
void glDrawX(int a, int b, int c);
void glDrawO(int a, int b, int c);

unsigned char* ReadBmpFromFile(char* szFileName, int &riWidth, int &riHeight);

LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_PAINT:
  {
    return FALSE;
  }
  default:return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)//Funkcja obs³ugi komunikatów
{
  switch (uMsg)
  {
  case WM_COMMAND:
  {
    switch (HIWORD(wParam))
    {
    case BN_CLICKED://Zdarzenie klikniêcia 
    {
      switch (LOWORD(wParam))
      {
      case IDC_BUTTON10://Klikniêcie na nsz przycisk Button1
      {
        return TRUE;
      }
      default:;
      }
    }
    return TRUE;
    default:;
    }
  }
  return TRUE;
  case WM_LBUTTONDOWN:
  {
    fStep = 11.0;
    //wParam==1 sprawdz czy to ten timer
    SetTimer(hwndDlg, 1, 2100, NULL);

    //CHAR szText[200];
    //wsprintf(szText, "Kliknales myszka x=%d, y=%d", LOWORD(lParam), HIWORD(lParam));
    //MessageBox(hwndDlg, szText, TEXT("Klikniecie"), MB_OK);

    DrawGLScene();
    //DrawCube1(1,1,1);
    glDrawBoard();

    glPushMatrix();
    glTranslated(2, 2, 0);
    glRotated(0.0, 0, 1, 0);
    glDrawX(-25, 35, 0);
    glPopMatrix();

    SwapBuffers(hDc);
  }
  return TRUE;
  case WM_TIMER:
  {
    if(wParam==1)
    {
      fStep = 0;
      KillTimer(hwndDlg, 1);
    }
    if (wParam == 2)
    {
      DrawGLScene();
      //DrawCube1(1,1,1);
      glDrawBoard();

      static float fAngle = 0.0;

      glPushMatrix();
      glTranslated(2, 1, 0);
      glRotated(fAngle, 0, 1, 0);
      glDrawX(-25, -25, 0);
      glPopMatrix();

      glPushMatrix();
      glTranslated(2, 1, 0);
      glRotated(fAngle, 1, 1, 0);
      glDrawO(35, 35, 0);
      glPopMatrix();

      fAngle += fStep;

      SwapBuffers(hDc);
    }
  }
  return TRUE;
  case WM_INITDIALOG:
  {
    //zmiana stylu okna
    SetWindowLong(hwndDlg, GWL_STYLE, CS_HREDRAW | CS_VREDRAW | CS_OWNDC | WS_OVERLAPPEDWINDOW);
    //SetClassLong(hwndDlg,GCL_STYLE,CS_HREDRAW|CS_VREDRAW|CS_OWNDC); //Silniejsza funkcja SetWindowLong
    int iWidth, iHeight;
    unsigned char* pMy = ReadBmpFromFile("..\\resource\\obraz.bmp", iWidth, iHeight);
    //Stworzyæ maszynê stanu
    static PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR), //rozmiar
      1,                            //wersja
      PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, //flagi
      PFD_TYPE_RGBA,//typ pixeli
      16,//
      0,0,0,0,0,0,
      0,
      0,
      0,
      0,0,0,0,
      16,
      0,
      0,
      PFD_MAIN_PLANE,
      0,
      0,0,0
    };
    //zmienne pomocnicze
    GLuint PixelFormat;

    hDc = GetDC(hwndDlg);
    PixelFormat = ChoosePixelFormat(hDc, &pfd); //sprawdzamy czy nasz format graficzny jest wspierany
                                                //ustawiamy format
    SetPixelFormat(hDc, PixelFormat, &pfd);
    //tworzymy maszynê stanu
    hRc = wglCreateContext(hDc);
    wglMakeCurrent(hDc, hRc);
    //Podpi¹æ j¹ pod okno

    glGenTextures(1, &idTexture);
    glBindTexture(GL_TEXTURE_2D, idTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pMy);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    delete[]pMy;

    return TRUE;
  }
  case WM_SIZE:
  {
    GLint iWidth = LOWORD(lParam);
    GLint iHeight = HIWORD(lParam);

    if (iWidth == 0)iWidth = 1;
    if (iHeight == 0)iHeight = 1;

    glViewport(0, 0, iWidth, iHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //glLoadMatrix();

    gluPerspective(45.0f, iWidth / iHeight, 0.1f, 1000.0f); //k¹t widzenia, Rozmiar okna, Z min, Z max

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return TRUE;
  }
  case WM_CLOSE:
  {
    DestroyWindow(hwndDlg); // zniszczenie okna
    PostQuitMessage(0); //Komunikat polecenia zakoñczenia aplikacji
  }
  return TRUE;
  default:;
  }
  return FALSE;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
  HWND hwndMainWindow = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINVIEW), NULL, DialogProc);
  ShowWindow(hwndMainWindow, iCmdShow);
  //LODEpng lib do ladowania png picojpg
  MSG msg = {};
  SetTimer(hwndMainWindow, 2, 50, NULL);
  while (GetMessage(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  // BOOL bDone = false;
  /* while (!bDone)
   {
     if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //sprawdza czy jest cos do obs³u¿enia, jak nie to pozwala programowi przejœæ dalej
     {
       if (msg.message == WM_QUIT)bDone = true;
       TranslateMessage(&msg);
       DispatchMessage(&msg);
     }
     else
     {
       DrawGLScene();
       //DrawCube1(1,1,1);
       glDrawBoard();

       static float fAngle = 0.0;

       glPushMatrix();
       glTranslated(2, 1, 0);
       glRotated(fAngle, 0, 1, 0);
       glDrawX(-25,-25,0);
       glPopMatrix();

       glPushMatrix();
       glTranslated(2, 2, 0);
       glRotated(fAngle, 0, 1, 0);
       glDrawO(35, 35, 0);
       glPopMatrix();

       fAngle += fStep;

       SwapBuffers(hDc);
     }
   }*/
}

GLint DrawGLScene()
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);

  glLoadIdentity();
  static GLfloat fAngle = 0.0;
  fAngle += 0.01f;
  glTranslatef(0, 0, -300);
  glRotatef(fAngle, 0, 1, 0);

  /*glBegin(GL_LINES);
    glColor3d(1,0,0);
    glVertex3d(10,0,0);
    glVertex3d(-10,0,0);

    glColor3d(0, 1, 0);
    glVertex3d(0, 10, 0);
    glVertex3d(0, -10, 0);

    glColor3d(0, 0, 1);
    glVertex3d(0, 0, 10);
    glVertex3d(0, 0, -10);

    glEnd();*/
  return 1;
}

void DrawCube1(int a, int b, int c)
{

  glBegin(GL_QUADS);
  DrawCube(0, 50 * a, 0, 50 * b, 0, 50 * c, 0, 0, 0);
  /*glColor3d(0, -1, 0);
  glNormal3d(0, -1, 0);
  glVertex3d(0, 0, 0);
  glVertex3d(0, 0, c);
  glVertex3d(a, 0, c);
  glVertex3d(a, 0, 0);

  glColor3d(0, 1, 0);
  glNormal3d(0, -1, 0);
  glVertex3d(a, 0, 0);
  glVertex3d(a, 0, c);
  glVertex3d(a, b, c);
  glVertex3d(0, b, 0);

  glColor3d(1, 0, 0);
  glNormal3d(0, 1, 0);
  glVertex3d(a, b, 0);
  glVertex3d(a, b, c);
  glVertex3d(0, b, c);
  glVertex3d(0, b, 0);

  glColor3d(0, 1, 1);
  glNormal3d(-1, 0, 0);
  glVertex3d(0, b, 0);
  glVertex3d(0, b, c);
  glVertex3d(0, 0, c);
  glVertex3d(0, 0, 0);

  glColor3d(1, 0, 1);
  glNormal3d(0, 0, 1);
  glVertex3d(0, 0, c);
  glVertex3d(0, b, c);
  glVertex3d(a, b, c);
  glVertex3d(a, 0, c);

  glColor3d(1, 1, 0);
  glNormal3d(0, 0, -1);
  glVertex3d(0, 0, 0);
  glVertex3d(a, 0, 0);
  glVertex3d(a, b, 0);
  glVertex3d(0, b, 0);*/
  glEnd();

}

void glDrawBoard()
{
  //glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, idTexture);
  glBegin(GL_QUADS);
  DrawCube(-85, 85, -35, -25, -10, 0, 1, 1, 1);
  DrawCube(-85, 85, 25, 35, -10, 0, 1, 1, 1);
  DrawCube(-35, -25, -85, 85, -10, 0, 1, 1, 1);
  DrawCube(25, 35, -85, 85, -10, 0, 1, 1, 1);
  glEnd();
}
void glDrawX(int a, int b, int c)
{

  // glEnable(GL_TEXTURE_2D);
  // glBindTexture(GL_TEXTURE_2D, idTexture);
  glBegin(GL_TRIANGLES);
  /* STARE
  //dó³
  glColor3d(1, 1, 0);

  glVertex3d(a + 5, b + 5, c - 0);
  glVertex3d(a + 15, b + 5, c - 0);
  glVertex3d(a + 11, b + 8, c - 10);//

  glVertex3d(a + 11, b + 8, c - 10);
  glVertex3d(a + 21, b + 8, c - 10);
  glVertex3d(a + 15, b + 5, c - 0);//

  glVertex3d(a + 38, b + 5, c - 0);
  glVertex3d(a + 28, b + 5, c - 0);
  glVertex3d(a + 34, b + 8, c - 10);//

  glVertex3d(a + 34, b + 8, c - 10);
  glVertex3d(a + 44, b + 8, c - 10);
  glVertex3d(a + 38, b + 5, c - 0);//

                                   //góra
  glColor3d(1, 1, 0);

  glVertex3d(a + 5, b + 41, c - 0);
  glVertex3d(a + 15, b + 41, c - 0);
  glVertex3d(a + 11, b + 44, c - 10);//

  glVertex3d(a + 11, b + 44, c - 10);
  glVertex3d(a + 21, b + 44, c - 10);
  glVertex3d(a + 15, b + 41, c - 0);//

  glVertex3d(a + 38, b + 41, c - 0);
  glVertex3d(a + 28, b + 41, c - 0);
  glVertex3d(a + 34, b + 44, c - 10);//

  glVertex3d(a + 34, b + 44, c - 10);
  glVertex3d(a + 44, b + 44, c - 10);
  glVertex3d(a + 38, b + 41, c - 0);//

                                    //front
  glColor3d(1, 0, 1);

  glVertex3d(a + 5, b + 5, c - 0);
  glVertex3d(a + 15, b + 5, c - 0);
  glVertex3d(a + 38, b + 41, c - 0);//

  glVertex3d(a + 38, b + 41, c - 0);
  glVertex3d(a + 28, b + 41, c - 0);
  glVertex3d(a + 5, b + 5, c - 0);//

  glVertex3d(a + 5, b + 41, c - 0);
  glVertex3d(a + 15, b + 41, c - 0);
  glVertex3d(a + 38, b + 5, c - 0);//

  glVertex3d(a + 38, b + 5, c - 0);
  glVertex3d(a + 28, b + 5, c - 0);
  glVertex3d(a + 5, b + 41, c - 0);//

                                   //ty³
  glColor3d(0, 1, 0);
  glVertex3d(a + 11, b + 8, c - 10);
  glVertex3d(a + 21, b + 8, c - 10);
  glVertex3d(a + 44, b + 44, c - 10);//

  glVertex3d(a + 44, b + 44, c - 10);
  glVertex3d(a + 34, b + 44, c - 10);
  glVertex3d(a + 11, b + 8, c - 10);//

  glVertex3d(a + 11, b + 44, c - 10);
  glVertex3d(a + 21, b + 44, c - 10);
  glVertex3d(a + 44, b + 8, c - 10);//

  glVertex3d(a + 44, b + 8, c - 10);
  glVertex3d(a + 34, b + 8, c - 10);
  glVertex3d(a + 11, b + 44, c - 10);//

  //BOKI DOROBIC
  */

  //front
  {
    glColor3d(1, 0, 1);

    glVertex3d(a + 5, b + 5, c - 0);
    glVertex3d(a + 15, b + 5, c - 0);
    glVertex3d(a + 38, b + 41, c - 0);//

    glVertex3d(a + 38, b + 41, c - 0);
    glVertex3d(a + 28, b + 41, c - 0);
    glVertex3d(a + 5, b + 5, c - 0);//

    glVertex3d(a + 5, b + 41, c - 0);
    glVertex3d(a + 15, b + 41, c - 0);
    glVertex3d(a + 38, b + 5, c - 0);//

    glVertex3d(a + 38, b + 5, c - 0);
    glVertex3d(a + 28, b + 5, c - 0);
    glVertex3d(a + 5, b + 41, c - 0);
  }

  //ty³
  {
    glColor3d(1, 1, 0);

    glVertex3d(a + 5, b + 5, c - 10);
    glVertex3d(a + 15, b + 5, c - 10);
    glVertex3d(a + 38, b + 41, c - 10);//

    glVertex3d(a + 38, b + 41, c - 10);
    glVertex3d(a + 28, b + 41, c - 10);
    glVertex3d(a + 5, b + 5, c - 10);//

    glVertex3d(a + 5, b + 41, c - 10);
    glVertex3d(a + 15, b + 41, c - 10);
    glVertex3d(a + 38, b + 5, c - 10);//

    glVertex3d(a + 38, b + 5, c - 10);
    glVertex3d(a + 28, b + 5, c - 10);
    glVertex3d(a + 5, b + 41, c - 10);
  }

  //dó³
  {
    glColor3d(0, 0, 1);

    //lewy dol
    glVertex3d(a + 5, b + 5, c - 0);
    glVertex3d(a + 15, b + 5, c - 0);
    glVertex3d(a + 5, b + 5, c - 10);

    glVertex3d(a + 15, b + 5, c - 10);
    glVertex3d(a + 5, b + 5, c - 10);
    glVertex3d(a + 15, b + 5, c - 0);

    //lewy gora
    glVertex3d(a + 5, b + 41, c - 0);
    glVertex3d(a + 15, b + 41, c - 0);
    glVertex3d(a + 5, b + 41, c - 10);


    glVertex3d(a + 15, b + 41, c - 10);
    glVertex3d(a + 5, b + 41, c - 10);
    glVertex3d(a + 15, b + 41, c - 0);
  }
  //góra
  {
    glColor3d(0, 0, 1);

    //prawy dol
    glVertex3d(a + 28, b + 5, c - 0);
    glVertex3d(a + 38, b + 5, c - 0);
    glVertex3d(a + 28, b + 5, c - 10);

    glVertex3d(a + 38, b + 5, c - 10);
    glVertex3d(a + 28, b + 5, c - 10);
    glVertex3d(a + 38, b + 5, c - 0);

    //prawy gora
    glVertex3d(a + 28, b + 41, c - 0);
    glVertex3d(a + 38, b + 41, c - 0);
    glVertex3d(a + 28, b + 41, c - 10);


    glVertex3d(a + 38, b + 41, c - 10);
    glVertex3d(a + 28, b + 41, c - 10);
    glVertex3d(a + 38, b + 41, c - 0);
  }
  //boki
  {
    glColor3d(0, 1, 1);

    //1
    glVertex3d(a + 5, b + 5, c - 0);
    glVertex3d(a + 28, b + 41, c - 0);
    glVertex3d(a + 5, b + 5, c - 10);//

    glVertex3d(a + 28, b + 41, c - 0);
    glVertex3d(a + 28, b + 41, c - 10);
    glVertex3d(a + 5, b + 5, c - 10);//

    //2
    glVertex3d(a + 15, b + 5, c - 0);
    glVertex3d(a + 38, b + 41, c - 0);
    glVertex3d(a + 15, b + 5, c - 10);//

    glVertex3d(a + 38, b + 41, c - 0);
    glVertex3d(a + 38, b + 41, c - 10);
    glVertex3d(a + 15, b + 5, c - 10);//

    //3
    glVertex3d(a + 5, b + 41, c - 0);
    glVertex3d(a + 28, b + 5, c - 0);
    glVertex3d(a + 5, b + 41, c - 10);//

    glVertex3d(a + 28, b + 5, c - 0);
    glVertex3d(a + 28, b + 5, c - 10);
    glVertex3d(a + 5, b + 41, c - 10);//

    //4
    glVertex3d(a + 15, b + 41, c - 0);
    glVertex3d(a + 38, b + 5, c - 0);
    glVertex3d(a + 15, b + 41, c - 10);//

    glVertex3d(a + 38, b + 5, c - 0);
    glVertex3d(a + 38, b + 5, c - 10);
    glVertex3d(a + 15, b + 41, c - 10);//


  }
  glEnd();
}

void glDrawO(int a, int b, int c)
{
  glBegin(GL_TRIANGLES);
  //przód
  {
    glColor3d(1, 1, 0);

    //lewy
    glVertex3d(a + 5, b + 16, c + 0);
    glVertex3d(a + 16, b + 16, c + 0);
    glVertex3d(a + 5, b + 27, c + 0);//

    glVertex3d(a + 16, b + 27, c + 0);
    glVertex3d(a + 16, b + 16, c + 0);
    glVertex3d(a + 5, b + 27, c + 0);//

    //skos
    glVertex3d(a + 5, b + 27, c + 0);
    glVertex3d(a + 16, b + 27, c + 0);
    glVertex3d(a + 16, b + 38, c + 0);//

    //gora
    glVertex3d(a + 16, b + 27, c + 0);
    glVertex3d(a + 16, b + 38, c + 0);
    glVertex3d(a + 27, b + 27, c + 0);//

    glVertex3d(a + 16, b + 38, c + 0);
    glVertex3d(a + 27, b + 38, c + 0);
    glVertex3d(a + 27, b + 27, c + 0);//

    //skos
    glVertex3d(a + 27, b + 27, c + 0);
    glVertex3d(a + 27, b + 38, c + 0);
    glVertex3d(a + 38, b + 27, c + 0);//

    //prawy
    glVertex3d(a + 27, b + 16, c + 0);
    glVertex3d(a + 38, b + 16, c + 0);
    glVertex3d(a + 27, b + 27, c + 0);//

    glVertex3d(a + 38, b + 27, c + 0);
    glVertex3d(a + 38, b + 16, c + 0);
    glVertex3d(a + 27, b + 27, c + 0);//

    //skos
    glVertex3d(a + 27, b + 5, c + 0);
    glVertex3d(a + 27, b + 16, c + 0);
    glVertex3d(a + 38, b + 16, c + 0);//

    //dol
    glVertex3d(a + 16, b + 5, c + 0);
    glVertex3d(a + 16, b + 16, c + 0);
    glVertex3d(a + 27, b + 5, c + 0);//

    glVertex3d(a + 16, b + 16, c + 0);
    glVertex3d(a + 27, b + 16, c + 0);
    glVertex3d(a + 27, b + 5, c + 0);//

    //skos
    glVertex3d(a + 5, b + 16, c + 0);
    glVertex3d(a + 16, b + 16, c + 0);
    glVertex3d(a + 16, b + 5, c + 0);//
  }
  //ty³
  {
    glColor3d(0, 1, 1);

    //lewy
    glVertex3d(a + 5, b + 16, c - 10);
    glVertex3d(a + 16, b + 16, c - 10);
    glVertex3d(a + 5, b + 27, c - 10);//

    glVertex3d(a + 16, b + 27, c - 10);
    glVertex3d(a + 16, b + 16, c - 10);
    glVertex3d(a + 5, b + 27, c - 10);//

    //skos
    glVertex3d(a + 5, b + 27, c - 10);
    glVertex3d(a + 16, b + 27, c - 10);
    glVertex3d(a + 16, b + 38, c - 10);//

    //gora
    glVertex3d(a + 16, b + 27, c - 10);
    glVertex3d(a + 16, b + 38, c - 10);
    glVertex3d(a + 27, b + 27, c - 10);//

    glVertex3d(a + 16, b + 38, c - 10);
    glVertex3d(a + 27, b + 38, c - 10);
    glVertex3d(a + 27, b + 27, c - 10);//

    //skos
    glVertex3d(a + 27, b + 27, c - 10);
    glVertex3d(a + 27, b + 38, c - 10);
    glVertex3d(a + 38, b + 27, c - 10);//

    //prawy
    glVertex3d(a + 27, b + 16, c - 10);
    glVertex3d(a + 38, b + 16, c - 10);
    glVertex3d(a + 27, b + 27, c - 10);//

    glVertex3d(a + 38, b + 27, c - 10);
    glVertex3d(a + 38, b + 16, c - 10);
    glVertex3d(a + 27, b + 27, c - 10);//

    //skos
    glVertex3d(a + 27, b + 5, c - 10);
    glVertex3d(a + 27, b + 16, c - 10);
    glVertex3d(a + 38, b + 16, c - 10);//

    //dol
    glVertex3d(a + 16, b + 5, c - 10);
    glVertex3d(a + 16, b + 16, c - 10);
    glVertex3d(a + 27, b + 5, c - 10);//

    glVertex3d(a + 16, b + 16, c - 10);
    glVertex3d(a + 27, b + 16, c - 10);
    glVertex3d(a + 27, b + 5, c - 10);//

    //skos
    glVertex3d(a + 5, b + 16, c - 10);
    glVertex3d(a + 16, b + 16, c - 10);
    glVertex3d(a + 16, b + 5, c - 10);//
  }
  //boki
  {
    //lewy
    DrawSide(a + 5, a + 5, b + 16, b + 27, c - 10, c, 0, 1, 0);
    //skos
    DrawSide(a + 5, a + 16, b + 27, b + 38, c - 10, c, 0, 1, 0);
    //gora
    DrawSide(a + 16, a + 27, b + 38, b + 38, c - 10, c, 0, 1, 0);
    //skos
    DrawSide(a + 38, a + 27, b + 27, b + 38, c - 10, c, 0, 1, 0);
    //prawy
    DrawSide(a + 38, a + 38, b + 16, b + 27, c - 10, c, 0, 1, 0);
    //skos
    DrawSide(a + 27, a + 38, b + 5, b + 16, c - 10, c, 0, 1, 0);
    //dol
    DrawSide(a + 16, a + 27, b + 5, b + 5, c - 10, c, 0, 1, 0);
    //skos
    DrawSide(a + 5, a + 16, b + 16, b + 5, c - 10, c, 0, 1, 0);
    //srodek lewy
    DrawSide(a + 16, a + 16, b + 16, b + 27, c - 10, c, 0, 1, 0);
    //srodek góra
    DrawSide(a + 16, a + 27, b + 27, b + 27, c - 10, c, 0, 1, 0);
    //srodek prawy
    DrawSide(a + 27, a + 27, b + 16, b + 27, c - 10, c, 0, 1, 0);
    //srodek dol
    DrawSide(a + 16, a + 27, b + 16, b + 16, c - 10, c, 0, 1, 0);
  }
  glEnd();

}

void DrawSide(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, int r, int g, int b)
{
  glColor3d(r, g, b);

  glVertex3d(xmin, ymin, zmax);
  glVertex3d(xmax, ymax, zmax);
  glVertex3d(xmax, ymax, zmin);

  glVertex3d(xmax, ymax, zmin);
  glVertex3d(xmin, ymin, zmin);
  glVertex3d(xmin, ymin, zmax);

}

void DrawCube(int xmin, int xmax, int ymin, int ymax, int zmin, int zmax, int r, int g, int b)
{
  //glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, idTexture);
  //glColor3d(r,g,b);
  glColor3d(1, 1, 1);
  glNormal3d(0, -1, 0);
  glTexCoord2f(0.0, 0.0); glVertex3d(xmin, ymin, zmin);
  glTexCoord2f(0.5, 1.0); glVertex3d(xmin, ymin, zmax);
  glTexCoord2f(1.0, 1.0); glVertex3d(xmax, ymin, zmax);
  glTexCoord2f(0.5, 0.0); glVertex3d(xmax, ymin, zmax);

  glNormal3d(0, 1, 0);
  glTexCoord2f(0.0, 0.0);  glVertex3d(xmax, ymin, zmin);
  glTexCoord2f(0.0, 1.0);  glVertex3d(xmax, ymin, zmax);
  glTexCoord2f(1.0, 1.0);  glVertex3d(xmax, ymax, zmax);
  glTexCoord2f(1.0, 0.0);  glVertex3d(xmin, ymax, zmin);

  glNormal3d(1, 0, 0);
  glTexCoord2f(0.0, 0.0);  glVertex3d(xmax, ymax, zmin);
  glTexCoord2f(0.0, 1.0);  glVertex3d(xmax, ymax, zmax);
  glTexCoord2f(1.0, 1.0);  glVertex3d(xmin, ymax, zmax);
  glTexCoord2f(1.0, 0.0);  glVertex3d(xmin, ymax, zmin);

  glNormal3d(-1, 0, 0);
  glTexCoord2f(0.0, 0.0);  glVertex3d(xmin, ymax, zmin);
  glTexCoord2f(0.0, 1.0);  glVertex3d(xmin, ymax, zmax);
  glTexCoord2f(1.0, 1.0);  glVertex3d(xmin, ymin, zmax);
  glTexCoord2f(1.0, 0.0);  glVertex3d(xmin, ymin, zmin);

  glNormal3d(0, 0, 1);
  glTexCoord2f(0.0, 0.0);  glVertex3d(xmin, ymin, zmax);
  glTexCoord2f(0.0, 1.0);  glVertex3d(xmin, ymax, zmax);
  glTexCoord2f(1.0, 1.0);  glVertex3d(xmax, ymax, zmax);
  glTexCoord2f(1.0, 0.0);  glVertex3d(xmax, ymin, zmax);

  glNormal3d(0, 0, -1);
  glTexCoord2f(0.0, 0.0);  glVertex3d(xmin, ymin, zmin);
  glTexCoord2f(0.0, 1.0);  glVertex3d(xmax, ymin, zmin);
  glTexCoord2f(1.0, 1.0);  glVertex3d(xmax, ymax, zmin);
  glTexCoord2f(1.0, 0.0);  glVertex3d(xmin, ymax, zmin);
}
//Rysowanie pola, kó³ek i krzy¿yków w taki sposób zrobiæ
unsigned char* ReadBmpFromFile(char* szFileName, int &riWidth, int &riHeight)
{
  BITMAPFILEHEADER     bfh;
  BITMAPINFOHEADER     bih;

  int                i, j, h, v, lev, l, ls;
  unsigned char*     buff = NULL;

  unsigned char* p_palette = NULL;
  unsigned short n_colors = 0;

  unsigned char* pRGBBuffer = NULL;

  //FILE* hfile = fopen_s(szFileName, "rb",);
  FILE* hfile = NULL;
  fopen_s(&hfile, szFileName, "rb");

  if (hfile != NULL)
  {
    fread(&bfh, sizeof(bfh), 1, hfile);
    if (!(bfh.bfType != 0x4d42 || (bfh.bfOffBits < (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)))))
    {
      fread(&bih, sizeof(bih), 1, hfile);
      v = bih.biWidth;
      h = bih.biHeight;
      lev = bih.biBitCount;

      riWidth = v;
      riHeight = h;
      pRGBBuffer = new unsigned char[riWidth*riHeight * 3]; //Zaalokowanie odpowiedniego buffora obrazu

                                                            //Za³aduj Palete barw jesli jest
      if ((lev == 1) || (lev == 4) || (lev == 8))
      {
        n_colors = 1 << lev;
        p_palette = new unsigned char[4 * n_colors];
        fread(p_palette, 4 * n_colors, 1, hfile);
      }

      fseek(hfile, bfh.bfOffBits, SEEK_SET);

      buff = new unsigned char[v * 4];

      switch (lev)
      {
      case 1:
        //Nie obs³ugiwane
        break;
      case 4:
        //nie Obs³ugiwane
        break;
      case 8: //Skala szaroœci
        ls = (v + 3) & 0xFFFFFFFC;
        for (j = (h - 1); j >= 0; j--)
        {
          fread(buff, ls, 1, hfile);
          for (i = 0, l = 0; i < v; i++)
          {
            pRGBBuffer[((j*riWidth) + i) * 3 + 2] = p_palette[(buff[i] << 2) + 2];//R
            pRGBBuffer[((j*riWidth) + i) * 3 + 1] = p_palette[(buff[i] << 2) + 1];//G
            pRGBBuffer[((j*riWidth) + i) * 3 + 0] = p_palette[(buff[i] << 2) + 0];//B
          }
        };
        break;
      case 24:
        //bitmapa RGB
        ls = (v * 3 + 3) & 0xFFFFFFFC;
        for (j = (h - 1); j >= 0; j--)
        {
          //x_fread(hfile,buff,ls);
          fread(buff, ls, 1, hfile);
          for (i = 0, l = 0; i < v; i++, l += 3)
          {
            pRGBBuffer[((j*riWidth) + i) * 3 + 0] = buff[l + 2];
            pRGBBuffer[((j*riWidth) + i) * 3 + 1] = buff[l + 1];
            pRGBBuffer[((j*riWidth) + i) * 3 + 2] = buff[l + 0];
          };
        };
        break;
      case 32:
        // RGBA bitmap 
        for (j = (h - 1); j >= 0; j--)
        {
          fread(buff, v * 4, 1, hfile);
          for (i = 0, l = 0; i < v; i++, l += 4)
          {
            pRGBBuffer[((j*riWidth) + i) * 3 + 0] = buff[l + 2];
            pRGBBuffer[((j*riWidth) + i) * 3 + 1] = buff[l + 1];
            pRGBBuffer[((j*riWidth) + i) * 3 + 2] = buff[l + 0];
          }
        };
        break;
      };
      delete buff;
      if (p_palette) delete p_palette;

    }
  }
  return pRGBBuffer;
}