#include "player.h"

void Player::setEye(float eyex, float eyey, float eyez)
{
  this->eyex=eyex;
  this->eyey=eyey;
  this->eyez=eyez;
}

void Player::setLookAt(float atx, float aty, float atz)
{
	this->atx=atx;
	this->aty=aty;
	this->atz=atz;
}

float Player::getEyeX()
{
   return this->eyex;
}

float Player::getEyeY()
{
	 return this->eyey;
}

float Player::getEyeZ()
{
   return this->eyez;
}

float Player::getAtX()
{
   return this->atx;
}

float Player::getAtY()
{
   return this->aty;
}

float Player::getAtZ()
{
   return this->atz;
}
