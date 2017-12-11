#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <math.h>
#include "Vector3D.h"
#include "Player.h"

#define PI 3.141592654

Player newPlayer(char name, int x, int y, int z)
{
	Player player;
	player.name = name;
	player.health = 100;
	player.position = NewVector3D(x, y, z);
	player.weaponPosition = NewVector3D(x + 8, y + 8.3, 0);
	return player;
}

//random hit between 1 and 15
void weaponHit(Player * player)
{
	srand(time(NULL));
	int randomHit = (rand() % 15 + 1);
	player->health -= randomHit;
	printPlayerData(player);
}

//random hit between 1 and 10
void baseHit(Player * player)
{
	srand(time(NULL));
	int randomHit = (rand() % 10 + 1);
	player->health -= randomHit;
	printPlayerData(player);
}

void setPosition(Player * player, int x, int y, int z)
{
	//sets position of player
	Set(&player->position, x, y, z);

	//sets position of players character

	//XZ plane 
	float lx = cos((player->Angle)*(PI / 180));
	float lz = -sin((player->Angle)*(PI / 180));

	float llx = cos((player->elbowAngle)*(PI / 180));
	float ly = sin((player->elbowAngle)*(PI / 180));

	if (player->name == 'h')
	{
		Set(&player->weaponPosition, x + (8 * lx) * llx, y + 8.3 + (8.3 * ly), z + 8 * lz);

	}
	else if (player->name == 'f')
	{
		Set(&player->weaponPosition, x + (-8 * lx), y + 8.3 + (8.3 * ly), z + -8 * lz);
	}

}

void printPlayerData(Player * player)
{
	switch (player->name)
	{
	case 'h':
		printf("Hero Data\nPosition: (%f, %f, %f)\nHealth: %d %\n\n", player->position.x, player->position.y, player->position.z, player->health);
		break;
	case 'f':
		printf("Foe Data\nPosition: (%f, %f, %f)\nHealth: %d %\n\n", player->position.x, player->position.y, player->position.z, player->health);
		break;
	}
}

void checkWinner(Player * hero, Player * foe)
{
	if (hero->health <= 0) {
		printf("**********YOU'VE LOST**********\n");
		return;
	}
	else if (foe->health <= 0)
	{
		printf("**********YOU'VE WON**********\n");
		return; 
	}
}


int collisionDetect(Player * hero, Player * foe)
{
	float baseFoeX = foe->position.x;
	float baseFoeY = foe->position.y;
	float baseFoeZ = foe->position.z;

	float baseHeroX = hero->position.x;
	float baseHeroY = hero->position.y;
	float baseHeroZ = hero->position.z;

	float dx = baseFoeX - baseHeroX;
	float dy = baseFoeY - baseHeroY;
	float dz = baseFoeZ - baseHeroZ;

	float weaponFoeX = foe->weaponPosition.x;
	float weaponFoeY = foe->weaponPosition.y;
	float weaponFoeZ = foe->weaponPosition.z;

	float weaponHeroX = hero->weaponPosition.x;
	float weaponHeroY = hero->weaponPosition.y;
	float weaponHeroZ = hero->weaponPosition.z;

	float weaponDx = weaponFoeX - weaponHeroX;
	float weaponDy = weaponFoeY - weaponHeroY;
	float weaponDz = weaponFoeZ - weaponHeroZ;

	float distance = sqrt(pow(dx, 2) + pow(dz, 2));
	float weaponDistance = sqrt(pow(weaponDx, 2) + pow(weaponDy, 2) + pow(weaponDz, 2));

	//printf("distance: %f, dy: %f\n", distance, dy);
	//base hit
	if (distance < 6 && dy == 0)
	{
		printf("\nHit!!!!!!!!!!\n");
		return 0;
	}
	if (weaponDistance < 3) {
		printf("\nWeapon Hit!!!!!!!!!\n");
		return 1;
	}
}

