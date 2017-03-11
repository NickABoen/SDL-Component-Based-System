//To create a pack of components that define the pools of a component manager:
//using my_component_pack = component_pack<[component1],[component2],[...]>;

//To create a new component manager
//component_manager<component_pack> new_manager;

//To retrieve a component map reference from the manager
//auto& component_map = new_manager.get<[component]>();

//component_map can then be used as a traditional map with its 
//key as a uuid (boost generated) and its value as a new instance
//of the given component.

//Components must be unique as they can appear in the tuple no more
//than once, otherwise the program will be ill-formed when trying to 
//get with a non-existante or duplicated type in the tuple (component manager
//uses std::get and is also thus not SFINAE-friendly)

//From Sam Varshavchik's answer about creating a tuple of 
//vectors. Used here to create a tuple of component_lists (maps with uuids) 
//where the tuple itself acts as a compile-time registration
//of each of the pools or maps needed for each component
//The source can be found as of February 21st, 2017 at this address:
//http://stackoverflow.com/questions/37557936/create-tuple-of-vectors-from-a-typelist
#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP

#include <map>
#include <tuple>
#include <utility>

#include "game_components.hpp"

template <typename... Ts>
struct Typelist{};

//Declare List
template <class> class List;

//Specialize it, in order to drill down into the template parameters.
template <template <typename... Args> class t, typename... Ts>
struct List<t<Ts...>>{
    using type = std::tuple<component_list<Ts>...>;
};

template <typename... Ts>
using component_pack = Typelist<Ts...>;

template <class PackWrapper>
using manager_map_type = typename List<PackWrapper>::type;

template <class TList>
struct component_manager{
    private:
        manager_map_type<TList> component_maps;

    public:
        template <class Component>
        component_list<Component>& get(){
            return std::get<component_list<Component>>(component_maps);
        }
};

template <class Key>
struct map_contains_id{
    public:
        map_contains_id(Key key) : key(key){}

        template <class Value>
        bool operator()(std::map<Key, Value>& source){
            return (source.find(key) != std::end(source));
        }

    private:
        Key key;
};

template <class Key>
map_contains_id<Key> make_contains(const Key& id){return map_contains_id<Key>(id);}

struct map_emplace_id{
    private:
        component_id id;
    public:
        map_emplace_id(component_id id):
            id(id)
        {}

        template <class ComponentType, class... ConstructorArgs>
        auto operator()(
                component_list<ComponentType>& target,
                ConstructorArgs&& ...args){
            return target.emplace(std::piecewise_construct,
                    std::forward_as_tuple(id),
                    std::forward_as_tuple(id, std::forward<ConstructorArgs>(args)...));
        }
};
map_emplace_id make_emplace_id(const component_id& id){return map_emplace_id(id);}

#endif
