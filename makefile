all:
	gcc -c scene_manager.c -o scene_manager.o
	gcc -c misc_util.c -o misc_util.o
	gcc -c main.c -o main.o
	gcc main.o scene_manager.o misc_util.o -o main