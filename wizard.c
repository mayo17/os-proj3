#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

#include "cube.h"
#include "wizard.h"

pthread_mutex_t condition_mutex;

void *
wizard_func(void *wizard_descr)
{
	extern int winnerFound;
	extern int parse;
	extern int cont;
	struct cube *cube;
	struct room *newroom;
	struct room *oldroom;
	struct wizard *self;
	struct wizard *other;

	self = (struct wizard *)wizard_descr;
	assert(self);
	cube = self->cube;
	assert(cube);

	/* Sets starting room */
	oldroom = cube->rooms[self->x][self->y];
	assert(oldroom);

	/* Chooses the new room */
	newroom = choose_room(self);

	pthread_mutex_lock(&condition_mutex);
	while (cont == 0)
	{
		//wait
		if (parse == 1)
		{
			break;
		}
	}
	pthread_mutex_unlock(&condition_mutex);
	

	/* Infinite loop */
	while (1)
	{
		int winnerCheck = check_winner(cube);
		if(winnerFound != 0)
		{
			kill_wizards(self);
			return NULL;
		}
		else if (winnerCheck == 1)
		{
			printf("Team B has won.\n");
			winnerFound++;
			kill_wizards(self);
			return NULL;
		}
		else if (winnerCheck == 2)
		{
			printf("Team A has won.\n");
			winnerFound++;
			kill_wizards(self);
			return NULL;
		}
		//Waits self if its frozen
		if(self->status == 1)
		{
			//sem_wait(&cube->semtex[self->id]);
			while(self->status != 0)
			{
				if(winnerFound != 0)
				{
					kill_wizards(self);
					return NULL;
				}
			}
		}
		/* Loops until he's able to get a hold on both the old and new rooms */
		while (1)
		{
			if(winnerFound != 0)
			{
				kill_wizards(self);
				return NULL;
			}
			printf("Wizard %c%d in room (%d,%d) wants to go to room (%d,%d)\n",
						 self->team, self->id, oldroom->x, oldroom->y, newroom->x, newroom->y);

			if (try_room(self, oldroom, newroom))
			{
				/* Waits a random amount of time */
				dostuff();

				/* Chooses the new room */
				newroom = choose_room(self);

				/* Goes back to the initial state and try again */
				continue;
			}
			else
			{
				if(newroom->wizards[0] != NULL || newroom->wizards[1] != NULL)
				{
					if(pthread_mutex_trylock(&cube->plock[newroom->x][newroom->y]) != 0)
					{
						continue;
					}
				}
				break;
			}
		}
		printf("Wizard %c%d in room (%d,%d) moves to room (%d,%d)\n",
					 self->team, self->id,
					 oldroom->x, oldroom->y, newroom->x, newroom->y);

		/* Fill in */

		/* Self is active and has control over both rooms */
		/*if(try_room(self, oldroom, newroom))
		{
			printf("Somehow skipped try rooms\n");
			continue;
		}*/
		switch_rooms(self, oldroom, newroom);
		pthread_mutex_unlock(&cube->plock[oldroom->x][oldroom->y]);

		other = find_opponent(self, newroom);

		/* If there is not another wizard does nothing */
		if (other == NULL)
		{

			printf("Wizard %c%d in room (%d,%d) finds nobody around \n",
						 self->team, self->id, newroom->x, newroom->y);
			/* Fill in */
		}
		else
		{
			/* Other is from opposite team */
			if (other->team != self->team)
			{

				/* Checks if the opponent is active */
				if (other->status == 0)
				{
					printf("Wizard %c%d in room (%d,%d) finds active enemy\n",
								 self->team, self->id, newroom->x, newroom->y);

					fight_wizard(self, other, newroom);
				}
				else
				{
					printf("Wizard %c%d in room (%d,%d) finds enemy already frozen\n",
								 self->team, self->id, newroom->x, newroom->y);
				}
			}
			/* Other is from same team */
			else
			{
				/* Checks if the friend is frozen */
				if (other->status == 1)
				{
					free_wizard(self, other, newroom);
				}
			}

			/* Fill in */
		}

		if(cont == 0)
		{
			int currParse = parse;
			pthread_mutex_lock(&condition_mutex);
			while (currParse == parse)
			{
				//wait
			}
			pthread_mutex_unlock(&condition_mutex);
		}

		/* Thinks about what to do next */
		dostuff();

		oldroom = newroom;
		newroom = choose_room(self);
	}

	return NULL;
}
