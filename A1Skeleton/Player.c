#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <string.h>
#include <math.h>
#include "Vector3D.h"
#include "Player.h"

Player newPlayer(char name, int x, int y, int z)
{
	Player player;
	player.name = name;
	player.health = 100;
	player.position = NewVector3D(x, y, z);
	return player;
}

//random hit between 1 and 15
void weaponHit(Player * player)
{
	srand(time(NULL));
	int randomHit = (rand() % 15 + 1);
	player->health -= randomHit;
}

//random hit between 1 and 10
void baseHit(Player * player)
{
	srand(time(NULL));
	int randomHit = (rand() % 10 + 1);
	player->health -= randomHit;
}

void setPosition(Player * player, int x, int y, int z)
{
	Set(&player->position, x, y, z);
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

//0 = hero hits foe, base hit
//1 = hero hits foe, weapon hit
//2 = foe hits hero, base hit
//3 = foe hits hero, weapon hit
void collisionDetect(Player * hero, Player * foe)
{
	float foeX = foe->position.x;
	float foeZ = foe->position.z;
	float heroX = hero->position.x;
	float heroZ = hero->position.z;

	float dx = foeX - heroX;
	float dz = foeZ - heroZ;

	float distance = sqrt(pow(dx, 2) + pow(dz, 2));
	
	if (distance < 6)
	{
		printf("Hit!!!!!!!!!!\n");
	}
}

