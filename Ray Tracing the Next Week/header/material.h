#ifndef MATERIALH
#define MATERIALH

#include "hitable.h"
#include "ray.h"
#include "texture.h"

vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0), rand() / (RAND_MAX + 1.0)) - vec3(1, 1, 1);
	} while (p.squared_length() >= 1.0);
	return p;
}

float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}

vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

//ni_over_nt 为入射介质的折射指数和折射介质的折射指数的比值。
bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
		return true;
	}
	/*根号里面的内容小于零，说明折射光线的方向向量无实根，即没有折射光线，即出现全反射。所以，折射光线函数return false*/
	else
		return false;
}

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(float u, float v, const vec3& p)const { return vec3(0, 0, 0); }
};

class lambertial :public material
{
public:
	lambertial(texture* a):albedo(a){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p,r_in.time());
		attenuation = albedo->value(0, 0, rec.p);
		return true;
	}

	texture* albedo;
};

class metal :public material
{
public:
	metal(const vec3& a, float f) :albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}

	vec3 albedo;
	float fuzz;
};

class dielectric :public material
{
public:
	dielectric(float ri):ref_idx(ri){}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const
	{
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt; //ni_over_nt为入射介质的折射指数和折射介质的折射指数的比值
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
		{
			//scattered = ray(rec.p, refracted);
			reflect_prob = schlick(cosine, ref_idx);
		}
		else
		{
			scattered = ray(rec.p, reflected);
			reflect_prob = 1.0;
		}
		if (rand() / (RAND_MAX + 1.0) < reflect_prob)
		{
			/*产生一个（0，1）的随机数，如果随机数小于反射系数，则设置为反射光线，反之，设置为折射光线。也就是只有反射光线或折射光线中的一个咯，为什么？不是说好反射光线和折射光线都有吗？
			考虑到一个像素点被设置为采样100次，这100次中反射光线的条数基本和reflect_prob的值正相关，所以，100次的平均值也就是该像素点出反射光线和折射光线的叠加*/
				
			scattered = ray(rec.p, reflected);
		}
		else
		{
			scattered = ray(rec.p, refracted);
		}
		return true;
	}

	float ref_idx;
};

class diffuse_light :public material
{
public:
	diffuse_light(texture* a) :emit(a) {};
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered)const { return false; }
	virtual vec3 emitted(float u, float v, const vec3& p)const { return emit->value(u, v, p); }

	texture* emit;
};

#endif
