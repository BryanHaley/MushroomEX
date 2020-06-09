/* Game Object header
 * Defines the game object struct and methods to manipulate it. */

#ifndef game_object_h
#define game_object_h

#include <stdint.h>

/* Game Objects correspond to exactly one transform and exactly one behavior it shares its index with, all of
 * which exist in different vectors in the scene. Together, these three objects represent an object with a
 * transformation, a model, a hitbox, an animation state, and a behaviour that defines how it interacts with
 * the world around it. */

#define GOBJ_NO_COLLIDER       0
#define GOBJ_CYLINDER_COLLIDER 1
#define GOBJ_SURFACE_COLLIDERS 2

typedef struct
{
    uint32_t nameIndex;         // Index of the name of the game object. Ideally, every object has a unique name.
    uint8_t  flags;             // See flag defines below
    uint8_t  animPlaybackSpeed; // How fast to playback animations (frames per second).
    uint16_t animIndex;         // Index of the animation currently used by this game object.
    uint16_t animFrame;         // Current frame of animation.
    uint16_t modelIndex;        // Index of the model used by this game object.
    uint32_t index;             // This game object's index. Shouldn't change after init.
    float    animDelta;         // Time since the frame of animation was changed.
    uint16_t collision_a;       // See defines. One of two options.
    uint16_t collision_b;       // See defines. One of two options.
} game_object_t;                // 24 bytes, tightly packed. Don't mess with the order.

/* Game objects can have two forms of collision: either a contiguous array of collision surfaces (i.e 
 * triangles usually interpreted as walls, floors, ceilings), or a cylinder with a height and radius. 
 * Typically the latter form is for static geometry, but is sometimes used for moving or spinning platforms.
 * Which form a game object used is defined by a flag below. */

#define collisionSurfaceStart collision_a // Start index of associated collision surfaces.
#define collisionSurfaceEnd   collision_b // End index.
#define collisionRadius       collision_a // Radius of the collision cylinder.
#define coliisionHeight       collision_b // Height of the collision cylinder.

#define GOBJ_FLAG_ACTIVE      0b00000001  // Indicates if the gobj is visible and interactable.
#define GOBJ_FLAG_ALIVE       0b00000010  // Indicates if the gobj can be recycled.
#define GOBJ_FLAG_SURFACE     0b00000100  // Indicates if the gobj uses collision surfaces.
#define GOBJ_FLAG_CUSTOM0     0b00001000  // Unused flag available for behaviors to modify.
#define GOBJ_FLAG_CUSTOM1     0b00010000  // Unused flag available for behaviors to modify.
#define GOBJ_FLAG_CUSTOM2     0b00100000  // Unused flag available for behaviors to modify.
#define GOBJ_FLAG_CUSTOM3     0b01000000  // Unused flag available for behaviors to modify.
#define GOBJ_FLAG_CUSTOM4     0b10000000  // Unused flag available for behaviors to modify.

#define GOBJ_FLAGS_ACTIVE_AND_ALIVE 0b00000011 // Shortcut

#endif
