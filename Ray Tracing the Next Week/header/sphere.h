#ifndef SPHEREH
#define SPHEREF

#include "hitable.h"

class sphere :public hitable
{
public:
	sphere() {}
	sphere(vec3 cen, float r, material* m) : center(cen), radius(r), mat_ptr(m) {}
	virtual bool hit(const ray& r, float tmin, float tman, hit_record& rec)const;
	virtual bool bounding_box(float t0, float t1, AABB& box)const;

	vec3 center;
	float radius;
	material* mat_ptr;
};

/*（注意，这段注释之对应的被注释掉的代码）
  公式为t*t*dot(B,B)+2*t*dot(B,A-C)+dot(A-C,A-c)-R*R=0，注意这是dot((A+t*B-C),(A+t*B-C))=R*R的展开式
  此方程中t为未知数，所以其实这个方程是t的二元一次方程，可用b^2-4ac判断*/
/*（这段注释对应新的b和discrimination）
   */
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - a * c;
	if (discriminant > 0)
	{
		float temp = (-b - sqrt(b * b - a * c)) / a;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;

			return true;
		}
		temp = (-b - sqrt(b * b - a * c)) / a;
		if (temp<t_max && temp>t_min)
		{
			rec.t = temp;
			rec.p = r.point_at_parameter(rec.t);
			rec.normal = (rec.p - center) / radius;
			rec.mat_ptr = mat_ptr;

			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(float t0, float t1, AABB& box)const
{
	box = AABB(center - vec3(radius, radius, radius), center + vec3(radius, radius, radius));
	return true;
}

#endif