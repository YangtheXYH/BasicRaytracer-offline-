#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

class material;

//the hit_record struct that store the result of one or more ray / object intersection tests
struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	material* mat_ptr;
};

class hitable
{
public:
	virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0; //在t_min和t_max之间才计数hit数
};

#endif
