#include "scene_manager.h"

#include "misc_util.h"

#include <assert.h>
#include <stdio.h>

// Type definitions used in the SM
typedef struct SceneInstance
{
    int id;
    void *data;
} SceneInstance;

// Data structures of the SM
struct SceneManagerData
{
    bool initialized;
    int n_scenes;
    Scene *scenes;
} sm_data = {false, 0, NULL};

struct SceneManagerStack
{
    int height;
    SceneInstance *stack;
} sm_stack = {0, NULL};

// Prototypes of subroutine functions used in the client interface of the SM
static bool push_scene(int id); // Tries to push a new scene onto the stack
static void pop_scene(void); // Pops the topmost scene of the stack

void send_critical_error_signal(int error, const char *description)
{
    if (error != 0 || description != NULL)
    {
        fprintf(stderr,
                "Critical scene manager error #%d: '%s'\n",
                error, description);
    }
    terminate_scene_manager();
}

bool initialize_scene_manager(int n_scenes, Scene *scenes, int initial_id)
{
    if (sm_data.initialized == true)
    {
        fputs("Warning: attempt to reinitialize scene manager\n", stderr);
        return true;
    }
    else if (n_scenes <= 0 || scenes == NULL ||
             initial_id < 0 || initial_id >= n_scenes)
    {
        fprintf(stderr,
                "Invalid scene manager initialization data: %d, %p, %d\n",
                n_scenes, (void *) scenes, initial_id);
        return false;
    }
    else
    {
        for (int i = 0; i < n_scenes; ++i)
        {
            if (   scenes[i].initialize == NULL
                || scenes[i].execute == NULL
                || scenes[i].terminate == NULL)
            {
                fprintf(stderr, "Scene (i = %d) has a NULL pointer\n", i);
                return false;
            }
        }
        sm_data = (struct SceneManagerData) {true, n_scenes, scenes};
        if (push_scene(initial_id) == false)
        {
            sm_data = (struct SceneManagerData) {false, 0, NULL};
            fputs("Failed to initialize initial scene\n", stderr);
            return false;
        }
        else
        {
            return true;
        }
    }
}

bool execute_top_scene(void)
{
    if (sm_data.initialized == false)
    {
        fputs("Warning: attempt to run a scene in "
              "uninitialized scene manager\n", stderr);
        return false;
    }
    else if (scene_stack_is_empty())
    {
        fputs("Warning: attempt to run a scene in "
              "empty scene manager\n", stderr);
        return false;
    }
    else
    {
        assert(sm_data.n_scenes > 0);
        assert(sm_data.scenes != NULL);
        assert(sm_stack.stack != NULL);
        int index = sm_stack.height - 1;
        int id = sm_stack.stack[index].id;
        void *data = sm_stack.stack[index].data;
        assert(id >= 0);
        assert(id < sm_data.n_scenes);
        assert(data != NULL);
        assert(sm_data.scenes[id].execute != NULL);

        int return_value = sm_data.scenes[id].execute(data);

        // CASE 1: POP REQUEST
        if (return_value == POP_REQUEST)
        {
            pop_scene();
            // Reinitialize the new top scene if there is one
            if (scene_stack_is_not_empty())
            {
                bool reinit = true;
                do // While there IS a top scene with a reinitialize
                { // function and it fails, pop another scene
                    int new_index = sm_stack.height - 1;
                    int new_id = sm_stack.stack[new_index].id;
                    void *new_data = sm_stack.stack[new_index].data;
                    assert(new_id >= 0);
                    assert(new_id < sm_data.n_scenes);
                    assert(new_data != NULL);

                    reinit = true;
                    if (sm_data.scenes[new_id].reinitialize)
                    {
                        reinit = sm_data.scenes[new_id].reinitialize(new_data);
                        if (reinit == false)
                        {
                            fprintf(stderr,
                                    "Warning: scene (id=%d) failed to reinit\n",
                                    new_id);
                            pop_scene();
                        }
                    }
                } while (scene_stack_is_not_empty() && reinit == false);
            }
            return true;
        }
        // CASE 2: PUSH REQUEST
        else if (return_value >= 0 && return_value < sm_data.n_scenes)
        {
            return push_scene(return_value); // stderr output inside
        }
        // CASE 3: INVALID VALUE
        else
        {
            fprintf(stderr,
                    "Warning: scene (id = %d) returned an invalid value\n",
                    id);
            return false;
        }
    }
}

static bool push_scene(int id)
{
    assert(sm_data.initialized == true);
    assert(sm_data.n_scenes > 0);
    assert(sm_data.scenes != NULL);
    assert(sm_stack.height >= 0);
    assert(id >= 0);
    assert(id < sm_data.n_scenes);
    assert(sm_data.scenes[id].initialize != NULL);
    size_t old_size = sm_stack.height * sizeof(*sm_stack.stack);
    size_t new_size = old_size + sizeof(*sm_stack.stack);
    if(!safer_realloc((void **) &sm_stack.stack, new_size))
    {
        fputs("Failed to expand scene stack\n", stderr);
        return false;
    }
    else
    {
        void *data = sm_data.scenes[id].initialize();
        if(data == NULL)
        {
            fprintf(stderr, "Warning: scene (%d) failed to initialize\n", id);
            safe_dealloc((void **) &sm_stack.stack, old_size);
            return false;
        }
        else
        {
            int index = sm_stack.height;
            sm_stack.stack[index] = (SceneInstance) {id, data};
            sm_stack.height += 1;
            return true;
        }
    }
}

static void pop_scene(void)
{
    assert(sm_data.initialized == true);
    assert(sm_data.n_scenes > 0);
    assert(sm_data.scenes != NULL);
    assert(sm_stack.height > 0);
    assert(sm_stack.stack != NULL);
    int index = sm_stack.height - 1;
    int id = sm_stack.stack[index].id;
    void *data = sm_stack.stack[index].data;
    assert(id >= 0);
    assert(id < sm_data.n_scenes);
    assert(data != NULL);
    assert(sm_data.scenes[id].terminate != NULL);

    sm_stack.height -= 1;
    size_t new_size = sm_stack.height * sizeof(*sm_stack.stack);

    sm_data.scenes[id].terminate(data);
    safe_dealloc((void **) &sm_stack.stack, new_size);
}

void terminate_scene_manager(void)
{
    if (sm_data.initialized == false)
    {
        fputs("Warning: attempt to terminate "
              "uninitinialized scene manager\n", stderr);
    }
    else
    {
        while (scene_stack_is_not_empty())
        {
            pop_scene();
        }
        sm_data = (struct SceneManagerData) {false, 0, NULL};
    }
}

bool scene_stack_is_empty(void)
{
    if (sm_data.initialized == false)
    {
        fputs("Warning: inquiry of stack height of an "
              "uninitialized scene manager\n", stderr);
        assert(sm_stack.height == 0);
    }
    assert(sm_stack.height >= 0);
    return sm_stack.height == 0;
}

bool scene_stack_is_not_empty(void)
{
    return !scene_stack_is_empty();
}
