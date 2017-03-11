#ifndef GAME_COMPONENT_HPP
#define GAME_COMPONENT_HPP

//std lib
#include <iostream>
#include <map>

//boost lib
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

using id_generator = boost::uuids::random_generator;
using component_id = boost::uuids::uuid;

struct game_component{
    component_id id;
    
    game_component(component_id id) : id(id) {}
};

template<class ComponentType>
using component_list = std::map<component_id, ComponentType>;

#endif
