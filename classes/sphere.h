#ifndef SPHERE_H
#define SPHERE_H

#include "viewable.h"
#include "vec3.h"

class sphere : public viewable {
public:
    point3 center;
    double radius;
    std::shared_ptr<material> mat_ptr;

    sphere() {}
    sphere(point3 cen, double rad, std::shared_ptr<material> m) : center(cen), radius(rad), mat_ptr(m) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    float a = r.direction().length_squared();
    float half_b = dot(oc, r.direction());
    float c = oc.length_squared() - radius*radius;

    float discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    float sqrtd = std::sqrt(discriminant);

    // Determine nearest t that is in the acceptable range
    double root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) {
            return false;
        }
    }

    rec.t = root;
    rec.p = r(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
} 

#endif