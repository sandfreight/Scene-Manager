// Defines interface used by scene manager (SM) to communicate with its scenes

#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>

#define POP_REQUEST (-1)

// Sends a critical error signal which results in immediate SM termination and
// stderr output if error is not equal to 0 OR description is not equal to NULL
void send_critical_error_signal(int error, const char *description);

// Invalid pointers cause undefined behavior - most likely a segmentation fault
typedef struct Scene
{
    // It initializes the scene
    // On success, returns a pointer to its private data
    // If the scene has no private data, it can return a dummy string literal
    // On failure, returns a NULL pointer
    // If the INITIAL scene fails to initialize, so does the scene manager
    // If a NON-INITIAL scene fails to initialize, the scene manager attempts
    // to proceed as if no PUSH request was received (as if nothing happened)
    // You can send a critical error signal to override default behavior
    // It must not be NULL; else it will result in SM initialization failure
    void *(*initialize)(void);
    // It reinitializes the scene after it becomes the top one again
    // On success, returns true
    // On failure, returns false
    // Failure will result in the scene being popped from the stack
    // It can be NULL. It being NULL will cause it to be ignored
    bool (*reinitialize)(void *data);
    // It executes the scene
    // On success, returns a POP or a valid PUSH request
    // On failure, returns an invalid PUSH request
    // A valid PUSH request is an a integer between 0 and number of scenes
    // Success makes run_top_scene (SM client interface) return true
    // Failure makes run_top_scene (SM client interface) return false
    // It must not be NULL; else it will result in SM initialization failure
    int (*execute)(void *data);
    // It terminates the scene.
    // It must free all the dynamically allocated data to avoid memory leaks
    // It should not fail. If it does, you can send a critical error signal
    // If it does but you do not consider the memory leak or other implications
    // to be very serious, you can ignore it, though I would not recommend it
    // It must not be NULL; else it will result in SM initialization failure
    void (*terminate)(void *data);
} Scene;

#endif // SCENE_H
