#ifndef VIEWABLE_LIST_H
#define VIEWABLE_LIST_H

#include "viewable.h"

#include <memory>
#include <vector>

class viewable_list : public viewable {
public:
    std::vector<std::shared_ptr<viewable>> objects;

    viewable_list() {}
    viewable_list(std::shared_ptr<viewable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<viewable> object) { objects.push_back(object); }

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
};

bool viewable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}


#endif