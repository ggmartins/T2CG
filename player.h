#ifndef PLAYER_H
#define PLAYER_H
class Player {

public:
  float angle;
  float eyex,eyey,eyez; //primeira cadeira
  float atx,aty,atz;

  void setEye(float eyex, float eyey, float eyez);
  void setLookAt(float atx, float aty, float atz);

  float getEyeX();
  float getEyeY();
  float getEyeZ();

  float getAtX();
  float getAtY();
  float getAtZ();


};


#endif
