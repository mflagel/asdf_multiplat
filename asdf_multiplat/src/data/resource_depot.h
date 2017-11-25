#ifndef RESOURCE_DEPOT_H
#define RESOURCE_DEPOT_H

#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <map>
// #include <unordered_map>
#include <algorithm>

namespace asdf {

struct resource_not_found_exception : std::runtime_error
{
	std::string resourceName;
	resource_not_found_exception(std::string resourceName);
	//const char* what() const noexcept override;
};

template <class T>
class resource_depot_t {
public:
	T default_resource;
//private:
    // std::unordered_map<std::string, T> resources;
    std::map<std::string, T> resources;

public:
    resource_depot_t(T _default_resource = 0)
        : default_resource(_default_resource) {}
    ~resource_depot_t() = default;

	T const& operator[](std::string const& resourceName){
        return get_resource(resourceName);
	}

    void add_resource(T const& resource)
    {
        add_resource(resource->name, resource);
    }

    void add_resource(std::string const& resource_name, T const& resource) {
    	LOG("Adding resource %s", resource_name.c_str());
        resources[resource_name] = std::move(resource);
	}

    T const& get_resource(std::string const& resource_name) {
        if (resources.count(resource_name) == 0) {
            // printf("Could not find resource %s", resource_name.c_str());
            throw resource_not_found_exception(resource_name);
			return default_resource;
		}
        return resources[resource_name];
	}

	void rename_resource(std::string const& currentName, std::string const& newName){
		if(resources.count(currentName) > 0){
			resources[newName] = resources[currentName];
			resources.erase(currentName);
		}
	}

	void delete_resource(std::string const& resourceName){
		resources.erase(resourceName);
	}

};
}

#endif