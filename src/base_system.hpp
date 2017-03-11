#ifndef BASE_SYSTEM_HPP
#define BASE_SYSTEM_HPP

#include "component_manager.hpp"

class system_interface{
    public:
        virtual void update() = 0;
};

template <class ComponentPack>
class base_system{// : public system_interface{
    protected:
        component_manager<ComponentPack>& component_pools;
        bool is_enabled;
    public:
        base_system(component_manager<ComponentPack>& component_pools):
            component_pools(component_pools), is_enabled(true)
        {}

        void enable(){is_enabled = true;}
        void disable(){is_enabled = false;}
        void toggle_enabled(){is_enabled = !is_enabled;}

        //virtual void update() = 0;
};

#endif
