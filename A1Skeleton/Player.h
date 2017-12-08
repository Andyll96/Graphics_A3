#include <stdbool.h>
#include "Vector3D.h"

typedef	struct  {
	char name;
	int health;
	Vector3D position;
} Player;

Player newPlayer(char name, int x, int y, int z);
void weaponHit(Player* player);
void baseHit(Player* player);
void setPosition(Player* player, int x, int y, int z);
void printPlayerData(Player* player);
void checkWinner(Player* hero, Player* foe);
void collisionDetect(Player* hero, Player* foe);
