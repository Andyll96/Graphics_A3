#include <stdbool.h>
#include "Vector3D.h"

typedef	struct  {
	char name;
	int health;

	Vector3D position;
	float Angle;
	float elbowAngle;
	float shoulderAngle;
	Vector3D weaponPosition;
} Player;

Player newPlayer(char name, int x, int y, int z);
void weaponHit(Player* player);
void baseHit(Player* player);
void setPosition(Player* player, int x, int y, int z);
void printPlayerData(Player* player);
void checkWinner(Player* hero, Player* foe);
int collisionDetect(Player* hero, Player* foe);
