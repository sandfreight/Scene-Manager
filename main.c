// A quick and dirty, short demonstration of Scene Manager capabilities

#include "scene_manager.h"

#include <stdlib.h>
#include <stdio.h>

enum Scenes
{
    SCENE_ONE,
    SCENE_TWO,
    NUM_SCENES
};

// Function prototypes
void *initialize_scene_one(void);
bool reinitialize_scene_one(void *data);
int execute_scene_one(void *data);
void terminate_scene_one(void *data);
void *initialize_scene_two(void);
int execute_scene_two(void *data);
void terminate_scene_two(void *data);

// Scenes
Scene scene_one = {initialize_scene_one,
                   reinitialize_scene_one,
                   execute_scene_one,
                   terminate_scene_one};

Scene scene_two = {initialize_scene_two,
                   NULL,
                   execute_scene_two,
                   terminate_scene_two};

// Function implementations
void *initialize_scene_one(void)
{
    puts("Initializing scene ONE");
    return malloc(100);
}

bool reinitialize_scene_one(void *data)
{
    puts("Reinitializing scene ONE");
    return true;
}

int execute_scene_one(void *data)
{
    puts("Executing scene ONE");
    static bool first_run = true;
    if (first_run == true)
    {
        first_run = false;
        return SCENE_TWO;
    }
    else
    {
        return POP_REQUEST;
    }
}

void terminate_scene_one(void *data)
{
    puts("Terminating scene ONE");
    free(data);
}

void *initialize_scene_two(void)
{
    puts("Initializing scene TWO");
    return "Executing scene TWO";
}

int execute_scene_two(void *data)
{
    puts(data);
    return POP_REQUEST;
}

void terminate_scene_two(void *data)
{
    puts("Terminating scene TWO");
}

// The main function
int main(void)
{
    Scene scenes[NUM_SCENES];
    scenes[0] = scene_one;
    scenes[1] = scene_two;

    if (initialize_scene_manager(NUM_SCENES, scenes, SCENE_ONE) == true)
    {
        while (scene_stack_is_not_empty())
        {
            if (execute_top_scene() == false)
            {
                break;
            }
        }
        terminate_scene_manager();
    }

    return EXIT_SUCCESS;
}
