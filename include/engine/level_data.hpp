#ifndef LEVEL_DATA_HPP
#define LEVEL_DATA_HPP

#include <vector>
#include <stdint.h>
#include "glm/glm.hpp"
#include "gfx_data.hpp"

typedef struct
{
	glm::vec3 v0, v1, v2;
	uint32_t flags;
} collision_surface_t;

typedef struct
{
	// Visual models
	std::vector<model_t> m_Geometry;
	// Static collision triangles
	std::vector<collision_surface_t> m_CollisionSurfaces;
} level_t;

// flag values for collision_surface_t

#define SURFACE_DEFAULT         0b00000000000000000000000000000000 // For equality tests.
#define SURFACE_SLIPPERY        0b00000000000000000000000000000001 // Icy/slippery surfaces.
#define SURFACE_VERY_SLIPPERY   0b00000000000000000000000000000010 // Mario automatically slips on these. Usually used for slides.
#define SURFACE_NOT_SLIPPERY    0b00000000000000000000000000000100 // High traction (Mario can go up nearly vertical slopes).
#define SURFACE_LAVA            0b00000000000000000000000000001000 // Lava.
#define SURFACE_ICE             0b00000000000000000000000000010000 // Frostbite ice (like lava, but no flame effect).
#define SURFACE_QUICKSAND       0b00000000000000000000000000100000 // Quicksand. By default, slow deep quicksand like the floor of the pyramid.
#define SURFACE_SHALLOW         0b00000000000000000000000001000000 // Used in combination with flags like quicksand.
#define SURFACE_DEEP            0b00000000000000000000000010000000 // Used in combination with flags like quicksand.
#define SURFACE_HARD            0b00000000000000000000000100000000 // Take fall damage with lower negative vertical velocities.
#define SURFACE_ILLUSORY        0b00000000000000000000001000000000 // No collision for player.
#define SURFACE_CLIP            0b00000000000000000000010000000000 // Collision only for player.
#define SURFACE_VANISH_CAP      0b00000000000000000000100000000000 // Mario can pass through these surfaces with the vanish cap.
#define SURFACE_DEATH_PLANE     0b00000000000000000001000000000000 // Invisible floor of "open" levels.
#define SURFACE_CUSTOM_0        0b00000000000000000010000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_1        0b00000000000000000100000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_2        0b00000000000000001000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_3        0b00000000000000010000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_4        0b00000000000000100000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_5        0b00000000000001000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_6        0b00000000000010000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_7        0b00000000000100000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_8        0b00000000001000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_9        0b00000000010000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_10       0b00000000100000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_12       0b00000001000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_13       0b00000010000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_14       0b00000100000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_15       0b00001000000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_16       0b00010000000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_17       0b00100000000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_18       0b01000000000000000000000000000000 // Custom surface flag open for mods to define.
#define SURFACE_CUSTOM_19       0b10000000000000000000000000000000 // Custom surface flag open for mods to define.

#endif