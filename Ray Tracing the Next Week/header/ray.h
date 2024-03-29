#ifndef RAYH
#define RAYH
#include "vec3.h"

class ray
{
public:
	ray() {}
	ray(const vec3& a, const vec3& b, float time = 0) { A = a; B = b; _time = time; }
	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	vec3 point_at_parameter(float t) const { return A + t * B; }

	float time() const { return _time; }

	vec3 A;
	vec3 B;
	float _time; //用来计算motion blur的时间差
};

#endif
