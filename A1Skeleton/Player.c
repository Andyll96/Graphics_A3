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

void setPosition(Player * player, int x, int y, int z)
{
	Set(&player->position, x, y, z);
}

void printPlayerData(Player * player)
{
	switch (player->name)
	{
	case 'h':
		printf("Hero Data\nPosition: (%f, %f, %f)\nHealth: %d %", player->position.x, player->position.y, player->position.z, player->health);
		break;
	case 'f':
		printf("Foe Data\nPosition: (%f, %f, %f)\nHealth: %d %", player->position.x, player->position.y, player->position.z, player->health);
		break;
	}
}
