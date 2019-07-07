#ifndef SPHEREH
#define SPHEREF

#include "hitable.h"

class sphere :public hitable
{
public:
	sphere() {}
	sphere(vec3 cen, float r, material* m) : center(cen), radius(r), mat_ptr(m) {}
	virtual bool hit(const ray& r, float tmin, float tman, hit_record& rec)const;

	vec3 center;
	float radius;
	material* mat_ptr;
};

/*��ע�⣬���ע��֮��Ӧ�ı�ע�͵��Ĵ��룩
  ��ʽΪt*t*dot(B,B)+2*t*dot(B,A-C)+dot(A-C,A-c)-R*R=0��ע������dot((A+t*B-C),(A+t*B-C))=R*R��չ��ʽ
  �˷�����tΪδ֪����������ʵ���������t�Ķ�Ԫһ�η��̣�����b^2-4ac�ж�*/
/*�����ע�Ͷ�Ӧ�µ�b��discrimination��
   */
bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	//float b = 2.0 * dot(oc, r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	//float discriminant = b * b - 4 * a * c; //��Ԫһ�η��̵��б�ʽ
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

#endif