#include "main_aux.h"
#include "parser.h"
#include "game.h"
#include "solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
	srand(atoi(argv[1])); 
	restart();
	return argc;
}


