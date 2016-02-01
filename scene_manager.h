// Defines client interface of Scene Manager

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "scene.h"

// Initialization and termination functions will not fail or yield undefined
// behaviour if called multiple times or in the wrong order. Other functions
// are safe to be called at all times, too. Scenes are the weakest component

// Initializes scene manager. Warning: scenes are not deep copied!
bool initialize_scene_manager(int n_scenes, Scene *scenes, int initial_id);
// Executes the top scene on the scene stack
bool execute_top_scene(void);
// Terminates scene manager.
void terminate_scene_manager(void);
// Returns whether the scene stack is empty
bool scene_stack_is_empty(void);
// Returns whether the scene stack is not empty
bool scene_stack_is_not_empty(void);

#endif // SCENE_MANAGER_H
