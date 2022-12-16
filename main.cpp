#include "rtweekend.h"

#include "classes/colour.h"
#include "classes/viewable_list.h"
#include "classes/sphere.h"
#include "classes/camera.h"

#include <iostream>
#include <cstdlib>


// Determines ray colour by the normal vector of a ray that hits an viewable object
colour ray_colour(const ray& r, const viewable& world, int depth) {
    hit_record rec;
    
    // If we've exceeded the ray bounce limit, no more light is gathered
    if (depth <= 0)
        return colour(0,0,0);

    if (world.hit(r, eps, infinity, rec)) {
        ray scattered;
        colour attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_colour(scattered, world, depth-1);
        }
        return colour(0,0,0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    double t = 0.7*(unit_direction.y() + 1.0);
    return (1.0-t)*colour(1.0, 1.0, 1.0) + t*colour(0.4, 0.6, 1.0);
}

viewable_list random_scene() {
    viewable_list world;

    auto ground_material = std::make_shared<lambertian>(colour(0.5, 0.5, 0.5));
    world.add(std::make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            auto radius = random_double(0.05,0.2);
            point3 center(a + (1-radius)*random_double(), radius, b + (1-radius)*random_double());

            std::shared_ptr<material> sphere_material;

            if (choose_mat < 0.85) {
                // diffuse
                auto albedo = colour::random();
                sphere_material = std::make_shared<lambertian>(albedo);
                world.add(std::make_shared<sphere>(center, radius, sphere_material));
            } else if (choose_mat < 0.97) {
                // metal
                auto albedo = colour::random();
                auto fuzz = random_double(0, 0.3);
                sphere_material = std::make_shared<metal>(albedo, fuzz);
                world.add(std::make_shared<sphere>(center, radius, sphere_material));
            } else {
                // glass
                sphere_material = std::make_shared<dielectric>(1.2);
                world.add(std::make_shared<sphere>(center, radius, sphere_material));
            }
        }
    }

    auto material3 = std::make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<sphere>(point3(0, 4, -7), 4.0, material3));

    return world;
}


int main() {
    srand(time(0));

    // Image 
    const float aspect_ratio  = 16.0 / 9.0;
    const int image_width = 800;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 1000;
    const int max_depth = 50;

    // World
    viewable_list world = random_scene();


    //Camera
    point3 lookfrom(0,1,7);
    point3 lookat(0,1,0);
    vec3 vup(0,1,0);
    double dist_to_focus = (lookat - lookfrom).length();
    double aperture = 0.05;
    camera cam(lookfrom, lookat, vup, 50, aspect_ratio, aperture, dist_to_focus);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            colour pixel_colour(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                double u = (i + random_double()) / (image_width - 1);
                double v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u,v);
                pixel_colour += ray_colour(r, world, max_depth);
            }
            write_colour(std::cout, pixel_colour, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
}