#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glm.h"
#include "lofvlib.h"
#include "player.h"

#define WID 1920
#define HEI 1080
#define DRAW_LOFV 1

#define RENDER					1
#define SELECT					2
#define BUFSIZE 1024

#define VIEW3D 3   //3d view duh
#define VIEW2D 2   //sit select
#define VIEWOB 1   //object view

#define PLAYER_INIT_EYE 9.43798f, 0.98637f, -1.75f
#define PLAYER_INIT_LOOKAT 9.43798f*-1, (0.98637f*-1)-2.5f, -1.75f*-1

int mousepressed=0;
int lkey=0; int rkey=0;
int ukey=0; int dkey=0;
int specpos=0;
int viewmode=VIEW3D;
int cursorX=0;
int cursorY=0;
int mode=RENDER;
int windowHeight=HEI;
int windowWidth=WID;
int zoom=2;
GLuint selectBuf[BUFSIZE];
GLint hits=0;
GLMmodel* pmodel = NULL;
lofv *upperring;
lofv *lowerring;

Player player;

vertex getParamVertex(vertex v1, vertex v2, float alpha)
{
  vertex v3;
  v3.x=v1.x+(alpha*(v2.x-v1.x));
  v3.y=v1.y+(alpha*(v2.y-v1.y));
  v3.z=v1.z+(alpha*(v2.z-v1.z));
  return v3;
}

void setSpecPos(int chair )
{
  int line, pos;
  vertex v3, v2, v1;
  line=chair/6;
  pos=chair%6;
  v1=upperring->lines[line].v1;
  v2=upperring->lines[line].v2;
  v3=getParamVertex(v1, v2, ((float)pos)/(5.0f));
  player.setEye(v3.x, v3.y, v3.z);
  player.setLookAt(player.getEyeX()*-1,
                  (player.getEyeY()*-1)-2.5f,
                   player.getEyeZ()*-1);
}

void orientMe(float ang)
{
  player.setLookAt(sin(ang), player.getEyeY() ,-cos(ang));
}

void look(float vert, float horiz)
{
  if(viewmode==VIEW3D)
  {
     player.setLookAt(cos(horiz)*cos(vert), sin(vert), sin(horiz)*cos(vert));
  }
  if(viewmode==VIEWOB)
  {
     player.setEye(zoom*cos(horiz)*cos(vert), zoom*sin(vert), zoom*sin(horiz)*cos(vert));
  }
}

void moveMeFlat(int i)
{
  player.setEye(player.getEyeX() + (i*player.getAtX()*0.1),
                player.getEyeY() + (player.getAtY()*0.1),
                player.getEyeZ() + (i*player.getAtZ()*0.1));
}


void startPicking(int cursorX, int cursorY)
{
  GLint viewport[4];

  glSelectBuffer(BUFSIZE,selectBuf);
  glRenderMode(GL_SELECT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  glGetIntegerv(GL_VIEWPORT,viewport);
  gluPickMatrix(cursorX,viewport[3]-cursorY,5,5,viewport);

  //glOrtho (10, -10, -12, 8, 1.0, -1.0);
  glOrtho (11, -9, -12, 8, 1.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glInitNames();
}


void processHits (GLint hits, GLuint buffer[])
{
   int i;
   unsigned j, pos;
   GLuint names, *ptr, minZ,*ptrNames, numberOfNames;
   ptr = (GLuint *) buffer;
   minZ = 0xffffffff;
   for (i = 0; i < hits; i++)
   {
     names = *ptr;
     ptr++;
     if (*ptr < minZ)
     {
         numberOfNames = names;
         minZ = *ptr;
         ptrNames = ptr+2;
     }
     ptr += names+2;
   }
  ptr = ptrNames;
  for (j = 0; j < numberOfNames; j++,ptr++) pos=*ptr;
  if(pos<151)
  {
     printf("sit view set to:%d\n",pos);
     specpos=pos-1;
     setSpecPos(specpos);
  }


}

void stopPicking() {

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();
	hits = glRenderMode(GL_RENDER);
	if (hits != 0)
           processHits(hits,selectBuf);

	mode = RENDER;
}


void RenderScene(void)
{
   int i,j;
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   if((viewmode==VIEW3D)||(viewmode==VIEWOB))
   {
      glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
      glmDraw(pmodel, GLM_COLOR |GLM_SMOOTH );//
      glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
      if(DRAW_LOFV)
      {
        glBegin(GL_LINES);
          for(i=0; i<upperring->n; i++)
          {
             glVertex3f(upperring->lines[i].v1.x, upperring->lines[i].v1.y,upperring->lines[i].v1.z);
             glVertex3f(upperring->lines[i].v2.x,upperring->lines[i].v2.y, upperring->lines[i].v2.z);
          }
         glEnd();
      }
   }
   else
   {
      glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glScalef(1.0f, 1.15f,1.0f);
      glRotatef(-90.0f,1.0f,0.0f,0.0f);
      glTranslatef(1.0f,2.5f,0.0f);
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glmDraw(pmodel, 0);
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

      if(mode==SELECT)  startPicking(cursorX,cursorY);

      glPolygonMode(GL_FRONT,GL_LINE);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
      for(i=0; i<upperring->n; i++)
      {
          vertex v1,v2,v3;
          v1=upperring->lines[i].v1;
          v2=upperring->lines[i].v2;

          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();

          glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
          glBegin(GL_LINES);
             glVertex2f(v1.x, v1.z);
             glVertex2f(v2.x, v2.z);
          glEnd();

          glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
          for(j=0; j<6; j++)
          {
             v3=getParamVertex(v1, v2, ((float)j)/(5.0f));
             glPushName((i*6)+(j+1));
             if(specpos==(i*6)+j)glPolygonMode(GL_FRONT,GL_FILL);
             glBegin(GL_QUADS);
                 glVertex2f(v3.x+0.1f, v3.z+0.1f);
                 glVertex2f(v3.x+0.1f, v3.z-0.1f);
                 glVertex2f(v3.x-0.1f, v3.z-0.1f);
                 glVertex2f(v3.x-0.1f, v3.z+0.1f);
             glEnd();
             if(specpos==(i*6)+j)glPolygonMode(GL_FRONT,GL_LINE);
             glPopName();
          }
      }
      glPopName();
      glPolygonMode(GL_FRONT,GL_FILL);

      if(mode==SELECT) stopPicking();
   }
   glutSwapBuffers();
   glFlush();
}


void enablefx()
{
  glEnable(GL_COLOR_MATERIAL); // a partir da cor corrente
  glEnable(GL_LIGHTING); //Habilita o uso de ilumina��oeyez
  glEnable(GL_LIGHT0); // Habilita a luz de n�mero 0
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST); // Habilita o depth-buffering
}

void disablefx()
{
  glDisable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
}

void SetupRC(void)
{
  GLfloat lightAmbient[4]={0.2,0.2,0.2,1.0};
  GLfloat lightDiffuse[4]={0.7,0.7,0.7,1.0}; // "color"
  GLfloat lightSpecular[4]={1.0, 1.0, 1.0, 1.0};// "brightness"
  GLfloat lightPos[4]={0.0, 0.0, 3.0, 1.0};
  GLfloat specular[4]={1.0,1.0,1.0,1.0}; // Specularity
  GLint specMaterial = 60;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Background color (black)
  glShadeModel(GL_SMOOTH); // Gourad shading
  glMaterialfv(GL_FRONT,GL_SPECULAR, specular); // material reflectance
  glMateriali(GL_FRONT,GL_SHININESS,specMaterial); // material shininess
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightAmbient); // Ativa o uso da luz ambiente
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient); // Define os par�metros da luz de n�mero 0
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse );
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular );
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos ); // Habilita a defini��o da cor do material
  enablefx();
}


void Viewing(void)
{
   if(viewmode==VIEW3D)
   {
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(60.0, (GLfloat)WID/HEI, 0.01, 256.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(player.getEyeX(), player.getEyeY(), player.getEyeZ(),
         player.getAtX()+player.getEyeX(),
         player.getAtY()+player.getEyeY(),
         player.getAtZ()+player.getEyeZ(),
         0.0f,1.0f,0.0f);
   }
   if(viewmode==VIEWOB)
   {
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(60.0, (GLfloat)800/600, 0.01, 256.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(player.eyex, player.eyey, player.eyez, 0, 0, 0, 0.0f,1.0f,0.0f);
   }

   glutPostRedisplay();
}



void ChangeSize(GLsizei w, GLsizei h)
{
  if ( h == 0 )  h = 1; // Para previnir uma divis�o por zero

  glViewport(0, 0, w, h); // Especifica o tamanho da viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  windowHeight=h;
  windowWidth=w;
  glOrtho (11, -9, -12, 8, 1.0, -1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Viewing();
}



void HandleMouse(int button, int state, int x, int y)
{
     if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
	   cursorX = x;
	   cursorY = y;
	   mode = SELECT;
     glutPostRedisplay();
}

void Idle()
{
  if(mousepressed==1) { moveMeFlat(5); }
  if(mousepressed==2) { moveMeFlat(-5); }
  if(lkey) { player.angle -=0.01f;orientMe(player.angle); }
  if(rkey) { player.angle +=0.01f;orientMe(player.angle);  }
  if(ukey) { moveMeFlat(1); }
  if(dkey) { moveMeFlat(-1); }
  Viewing();
}
void MouseMove(int x, int y)
{
  float horiz=(x-(WID/2))*0.01;
  float vert=-(y-(HEI/2))*0.005;
  look(vert, horiz);
  Viewing();

}

void pressKey(int key, int x , int y)
{
  if(key==GLUT_KEY_UP) ukey=1;
  if(key==GLUT_KEY_DOWN) dkey=1;
  if(key==GLUT_KEY_LEFT) lkey=1;
  if(key==GLUT_KEY_RIGHT) rkey=1;

}

void releaseKey(int key, int x , int y)
{

  if(key==GLUT_KEY_UP) ukey=0;
  if(key==GLUT_KEY_DOWN) dkey=0;
  if(key==GLUT_KEY_LEFT) lkey=0;
  if(key==GLUT_KEY_RIGHT) rkey=0;

}


void normalKey(unsigned char key, int x , int y)
{
    switch(key)
    {
      case 'd':
        printf("%.5f %.5f %.5f\n", player.eyex, player.eyey, player.eyez); fflush(stdout);
      break;
      case 'n':
        specpos++;
        if(specpos>149) specpos=0;
        if(specpos<0) specpos=149;
        setSpecPos(specpos);
      break;
      case 'p':
        specpos--;
        if(specpos>149) specpos=0;
        if(specpos<0) specpos=149;
        setSpecPos(specpos);
      break;
      case 'c':
        //adjust to look at the center (0, -2.5, 0) of the field
        player.atx=player.eyex*-1;
        player.aty=(player.eyey*-1)-2.5f;
        player.atz=player.eyez*-1;
      break;
      case '+': zoom+=2; break;
      case '-': zoom-=2; break;
      case 27: glutIgnoreKeyRepeat(0); exit(0); break;
    }
    glutPostRedisplay();
}


void handleMenuEvent(int EventID)
{
   switch(EventID)
   {
     case 1:
        enablefx();
        viewmode=VIEW3D;

        break;
     case 2:
        disablefx();
        viewmode=VIEW2D;
        break;
     case 3:
        enablefx();
        viewmode=VIEWOB;
        break;

   }
   ChangeSize(windowWidth,windowHeight);
   glutPostRedisplay();
}

void createPopUpMenu()
{
  glutCreateMenu(handleMenuEvent);
  glutAddMenuEntry(" first person fly mode ", 1);
  glutAddMenuEntry(" sit select ", 2);
  glutAddMenuEntry(" object view ", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(WID,HEI);
  glutCreateWindow("3D Engine");

  player.angle=0.0;
  player.setEye(PLAYER_INIT_EYE);
  player.setLookAt(PLAYER_INIT_LOOKAT);

  glutDisplayFunc(RenderScene);
  glutReshapeFunc(ChangeSize);
  glutMouseFunc(HandleMouse);
  glutMotionFunc(MouseMove);
  glutPassiveMotionFunc(MouseMove);
  glutKeyboardFunc(normalKey);
  glutIgnoreKeyRepeat(1);
  glutSpecialFunc(pressKey);
  glutSpecialUpFunc(releaseKey);
  glutIdleFunc(Idle);
  SetupRC();
  printf("Loading data..."); fflush(stdout);
  pmodel = glmReadOBJ((char *)"Stadium9.obj");
  printf("DONE\n");
  glmUnitize(pmodel);
  printf("Generating normals...\n");
  glmFacetNormals(pmodel);
  glmVertexNormals(pmodel, 90.0);
  glmScale(pmodel, 10.0f);

  lofv_load((char *)"upperring.lofv", &upperring);
  if (!pmodel){ fprintf(stderr, "error reding model\n"); exit(0); }
  createPopUpMenu();
  glutMainLoop();
  glutIgnoreKeyRepeat(1);
  return 0;
}
