To compile to project simple type make.
Use ./cube to use default values
Use ./cube  –size –seed –teamA –teamB to set values
Type p to parse a single game move
Type c to run the entire game to completion
Type Show to display the game board
Type Exit to exit the game
After a game is completed typing p or c will not do anything because the threads are no longer alive.
Do not use Start, use p or c.
The interface is a seperate thread but still wonky and the cube> line will sometimes show up in the wrong place. Do not worry about this, you can still always type commands. If the game ends and there is just a blank line at the bottom with no cube> that doesnt mean the program crashed, the cube> just didnt print. You can still type any commands you would like. The same goes for while the game is running.