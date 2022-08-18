#ifndef SOLID_VECTOR_H
#define SOLID_VECTOR_H

#include <vector>
#include <cassert>

namespace math
{
template<class T>
class SolidVector
{
public:
    using ID = uint32_t;

protected:
    void assertId(ID id) const
    {
        assert(id < forward_map.size());
        assert(forward_map[id].occupied);
    }

public:
    struct ForwardIndex
    {
        uint32_t index;
        bool occupied;
    };

    bool occupied(ID id) const
    {
        assert(id < forward_map.size());
        return forward_map[id].occupied;
    }

    uint32_t size() const { return uint32_t(data.size()); }

    const T * get_data() const { return data.data(); }    
    T * get_data() { return data.data(); }
    
    const T & at(uint32_t index) const
    {
        assert(index < data.size());
        return data[index];
    }    
    T & at(uint32_t index)
    {
        assert(index < data.size());
        return data[index];
    }

    const T & operator[](ID id) const
    {
        assertId(id);
        return data[forward_map[id].index];
    }
    T & operator[](ID id)
    {
        assertId(id);
        return data[forward_map[id].index];
    }

    ID insert(const T & value)
    {
        ID id = next_unused;
        assert(id <= forward_map.size());

        if (id == forward_map.size())
            forward_map.push_back({static_cast<uint32_t>(forward_map.size() + 1), false});

        ForwardIndex& forward_index = forward_map[id];
        assert(!forward_index.occupied);

        next_unused = forward_index.index;
        forward_index = { static_cast<uint32_t>(data.size()), true };

        data.emplace_back(value);
        backward_map.emplace_back(id);

        return id;
    }
    
    void erase(ID id)
    {
        assert(id < forward_map.size());

        ForwardIndex& forward_index = forward_map[id];
        assert(forward_index.occupied);

        data[forward_index.index] = std::move(data.back());
        data.pop_back();

        ID backward_index = backward_map.back();

        backward_map[forward_index.index] = backward_index;
        backward_map.pop_back();
        
        forward_map[backward_index].index = forward_index.index;

        forward_index = { next_unused, false };
        next_unused = id;
    }

    void clear()
    {
        forward_map.clear();
        backward_map.clear();
        data.clear();
        next_unused = 0;
    }
    
    std::vector<T> data;
    std::vector<ForwardIndex> forward_map;
    std::vector<ID> backward_map;

    ID next_unused = 0;
};
} // namespace math
#endif
