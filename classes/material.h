#ifndef MATERIAL_H
#define MATERIAL_H

#include "../rtweekend.h"
#include "viewable.h"

struct hit_record;

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) 
        const = 0;
};

class lambertian : public material {
public:
    colour albedo;

    lambertian(const colour& a) : albedo(a) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attentuation, ray& scattered) const override {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero()) {
            scatter_direction = rec.normal;
        }

        scattered = ray(rec.p, scatter_direction);
        attentuation = albedo;
        return true;
    }
};

class metal : public material {
public:
    colour albedo;
    double fuzz;

    metal(const colour& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
};

class dielectric : public material {
public:
    double eta; // Index of refraction

    dielectric(double index_of_refraction) : eta(index_of_refraction) {}

    virtual bool scatter(const ray& r_in, const hit_record& rec, colour& attenuation, ray& scattered) const override {
        attenuation = colour(1.0, 1.0, 1.0); // colour stays the same
        double refraction_ratio = rec.front_face ? 1.0/eta : eta/1.0; // 1.0 is index of air
            
        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = dot(-unit_direction, rec.normal);
        double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, refraction_ratio);
        }

        scattered = ray(rec.p, direction);
        return true;
    }

private:
    static double reflectance(double cosine, double ref_idx) {
        // Use Schlick's approximation for reflectance
        double r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1-cosine), 5);
    }
};

#endif