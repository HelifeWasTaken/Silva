#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <any>
#include <functional>
#include <stack>
#include <string>
#include <iostream>

namespace silva {
namespace priv {

#ifndef SPARSE_ARRAY_BASE
    #define SPARSE_ARRAY_BASE 30
#endif

    template <typename T> class SparseArray {
    private:
        const std::size_t _baseSize;
        std::vector<std::unique_ptr<T>> _registry;

    public:
        SparseArray(const std::size_t& baseSize = SPARSE_ARRAY_BASE)
            : _baseSize(baseSize)
        {
            clear();
        }

        void clear()
        {
            _registry.reserve(_baseSize);
            for (unsigned int i = 0; i < _baseSize; i++)
                _registry.push_back(std::unique_ptr<T>(nullptr));
        }

        void resize(const std::size_t& size = SPARSE_ARRAY_BASE)
        {
            _registry.reserve(_registry.size() + size);
            for (unsigned int i = 0; i < size; i++) {
                _registry.push_back(std::unique_ptr<T>(nullptr));
            }
        }

        void set(std::unique_ptr<T>&& elem, const std::size_t& i)
        {
            _registry.at(i) = std::move(elem);
        }

        void set(std::unique_ptr<T>& elem, const std::size_t& i)
        {
            _registry.at(i) = std::move(elem);
        }

        void set(T* elem, const std::size_t& i)
        {
            _registry.at(i) = std::unique_ptr<T>(elem);
        }

        void set(const T& elem, const std::size_t& i)
        {
            _registry.at(i) = std::make_unique<T>(elem);
        }

        void set(const std::size_t& i)
        {
            _registry.at(i) = std::unique_ptr<T>();
        }

        void unset(const std::size_t& i)
        {
            _registry.at(i) = std::unique_ptr<T>(nullptr);
        }

        T& get(const std::size_t& i)
        {
            if (isSet(i))
                return *_registry.at(i);
            throw "";
        }

        std::unique_ptr<T>& getO(const std::size_t& i)
        {
            return _registry.at(i);
        }

        const T& get(const std::size_t& i) const { return *_registry.at(i); }

        bool isSet(const std::size_t& i) const
        {
            return _registry.at(i) != nullptr;
        }

        const std::size_t size() const { return _registry.size(); }

        class Iterator {
        private:
            SparseArray<T>& _array;
            std::size_t _index;

            void _advance()
            {
                if (_index <= _array.size())
                    _index++;
            }

            void _retreat()
            {
                if (_index > 0)
                    _index--;
            }

            void _advance(std::size_t n)
            {
                for (std::size_t i = 0; i < n && _index <= _array.size();
                     i++, _index++)
                    ;
            }

            void _retreat(std::size_t n)
            {
                for (std::size_t i = 0; i < n && _index > 0; i++, _index--)
                    ;
            }

        public:
            Iterator(SparseArray<T>& array, const std::size_t& index = 0)
                : _array(array)
                , _index(index)
            {
            }

            const std::size_t& index() const { return _index; }

            Iterator& operator++()
            {
                _advance();
                return *this;
            }

            Iterator& operator--()
            {
                _retreat();
                return *this;
            }

            Iterator& operator+=(const std::size_t& n)
            {
                _advance(n);
                return *this;
            }

            Iterator& operator-=(const std::size_t& n)
            {
                _retreat(n);
                return *this;
            }

            Iterator operator+(const std::size_t& n) const
            {
                Iterator it(_array);
                return it += (n + _index);
            }

            Iterator operator-(const std::size_t& n) const
            {
                Iterator it(_array);
                it += _index;
                return it -= n;
            }

            std::unique_ptr<T>& operator*() { return _array.getO(_index); }

            std::unique_ptr<T>& operator->() { return _array.getO(_index); }

            bool operator==(const Iterator& other) const
            {
                return _index == other.index();
            }

            bool operator!=(const Iterator& other) const
            {
                return _index != other.index();
            }

            bool operator<(const Iterator& other) const
            {
                return _index < other.index();
            }

            bool operator>(const Iterator& other) const
            {
                return _index > other.index();
            }

            bool operator<=(const Iterator& other) const
            {
                return _index <= other.index();
            }

            bool operator>=(const Iterator& other) const
            {
                return _index >= other.index();
            }

            bool isSet() const { return _array.isSet(_index); }

            void set(const T& elem) { _array.set(elem, _index); }

            void set(T* elem) { _array.set(elem, _index); }

            void set(std::unique_ptr<T>& elem) { _array.set(elem, _index); }

            void unset() { _array.unset(_index); }
        };

        Iterator begin() { return Iterator(*this); }

        Iterator end() { return Iterator(*this, _baseSize); }
    };
}
}

namespace silva {

class registry;

using EntityId = std::size_t;
using Component = std::any;
using ComponentIndex = std::size_t;
using TypeNameId = const char*;

struct Entity {
    EntityId id;
    explicit Entity(const EntityId& id)
        : id(id)
    {
    }

    friend bool operator==(const Entity& a, const Entity& b)
    {
        return a.id == b.id;
    }

    friend bool operator!=(const Entity& a, const Entity& b)
    {
        return !(a == b);
    }

    friend std::ostream& operator<<(std::ostream& os, const Entity& e)
    {
        return os << "Entity(" << e.id << ")";
    }
};

using SystemUpdater = std::function<void(const Entity&, registry&)>;

namespace priv {

    class System {
    public:
    private:
        std::vector<Entity> _entities;
        SystemUpdater _f;
        std::vector<ComponentIndex> _dependencies;
        std::size_t _index = 0;

    public:
        System() = default;
        System(const System&) = default;

        void setSystemUpdate(SystemUpdater f) { _f = f; }

        void addDependency(const ComponentIndex& dependency)
        {
            if (std::find(
                    _dependencies.begin(), _dependencies.end(), dependency)
                != _dependencies.end())
                return;
            _dependencies.push_back(dependency);
        }

        void onEntityUpdate(registry& r, const Entity& e);

        void onEntityDelete(const Entity& e)
        {
            const size_t oldSize = _entities.size();
            _entities.erase(std::remove(_entities.begin(), _entities.end(), e),
                _entities.end());
            if (_index != 0 && _entities.size() < oldSize)
                _index--;
        }

        void update(registry& r)
        {
            for (_index = 0; _index < _entities.size(); _index++)
                _f(_entities[_index], r);
        }
    };

}

#ifndef REGISTRY_ENTITY_SIZE 
    #define REGISTRY_ENTITY_SIZE 8192
#endif

class registry {
private:
    std::unordered_map<TypeNameId, ComponentIndex> _componentToIndex;
    ComponentIndex _lastComponentIndex = 0;

    priv::SparseArray<priv::SparseArray<Component>> _entities;
    std::stack<EntityId> _removedEntitiesIds;
    EntityId _lastEntityId = 0;
    std::unordered_map<std::string, std::unique_ptr<priv::System>> _systems;

    Entity _lastUsedEntity = Entity(-1);
    std::string _lastUsedSystem = "";

    template <typename T> ComponentIndex _cti()
    {
        TypeNameId name = typeid(T).name();
        try {
            return _componentToIndex.at(name);
        } catch (...) {
            _componentToIndex[name] = _lastComponentIndex;
            for (unsigned int i = 0; i < _lastComponentIndex; i++)
                if (_entities.isSet(i)
                    && _entities.get(i).size() + 1 >= _lastEntityId)
                    _entities.get(i).resize();
            _lastComponentIndex++;
            return _lastComponentIndex - 1;
        }
    }

    template <typename T> registry& _addSystemDeps(priv::System& sys)
    {
        sys.addDependency(_cti<T>());
        for (unsigned int i = 0; i < _lastEntityId; i++)
            sys.onEntityUpdate(*this, Entity(i));
        return *this;
    }

    template <typename T, typename... Args,
        typename std::enable_if<sizeof...(Args) != 0>::type* = nullptr>
    registry& _addSystemDeps(priv::System& sys)
    {
        sys.addDependency(_cti<T>());
        return _addSystemDeps<Args...>(sys);
    }

public:

    template<typename T, typename... Args,
            typename std::enable_if<sizeof...(Args) != 0>::type* = nullptr>
    std::vector<ComponentIndex>& getDepsList(std::vector<ComponentIndex>& deps)
    {
        deps.push_back(_cti<T>());
        return getDepsList<Args...>(deps);
    }

    template<typename T, typename... Args,
            typename std::enable_if<sizeof...(Args) == 0>::type* = nullptr>
    std::vector<ComponentIndex>& getDepsList(std::vector<ComponentIndex>& deps)
    {
        deps.push_back(_cti<T>());
        return deps;
    }

    bool has(const Entity& e, const ComponentIndex& component,
        const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedEntity = e;
        return _entities.get(e.id).isSet(component);
    }

    bool has(const ComponentIndex& component)
    {
        return has(_lastUsedEntity, component, false);
    }

    template <typename T>
    bool has(const Entity& e, const bool& updateLast = true)
    {
        return has(e, _cti<T>(), updateLast);
    }

    template <typename T> T& get(const Entity& e, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedEntity = e;
        return std::any_cast<T&>(_entities.get(e.id).get(_cti<T>()));
    }

    template <typename T> T& get() { return get<T>(_lastUsedEntity); }

    Entity newEntity()
    {
        if (_removedEntitiesIds.empty()) {
            _lastUsedEntity.id = _lastEntityId;
            if (_lastUsedEntity.id + 1 >= _entities.size()) {
                _entities.resize();
            }
            _entities.set(new priv::SparseArray<Component>(REGISTRY_ENTITY_SIZE), _lastUsedEntity.id);
            _lastEntityId++;
            return _lastUsedEntity;
        }
        _lastUsedEntity = Entity(_removedEntitiesIds.top());
        _entities.set(new priv::SparseArray<Component>(REGISTRY_ENTITY_SIZE), _lastUsedEntity.id);
        _removedEntitiesIds.pop();
        return _lastUsedEntity;
    }

    registry& removeEntity(const Entity& e)
    {
        _entities.unset(e.id);
        for (auto& sys : _systems)
            sys.second->onEntityDelete(e);
        if (e.id == _lastEntityId) {
            _lastEntityId--;
            return *this;
        }
        _removedEntitiesIds.push(e.id);
        return *this;
    }

    template <typename T, typename... Args>
    registry& addSystem(const std::string& tag, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        _systems[tag] = std::make_unique<priv::System>();
        return _addSystemDeps<T, Args...>(*_systems.at(tag));
    }

    template <typename T, typename... Args> registry& addSystem()
    {
        return addSystem<T, Args...>(_lastUsedSystem, false);
    }

    template <typename T, typename... Args>
    registry& addSystemDeps(
        const std::string& tag, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        return _addSystemDeps<T, Args...>(*_systems.at(tag));
    }

    template <typename T, typename... Args> registry& addSystemDeps()
    {
        return addSystemDeps<T, Args...>(_lastUsedSystem, false);
    }

    void removeSystem(const std::string& tag)
    {
        _systems.erase(tag);
    }

    registry& setSystemUpdate(
        const std::string& tag, SystemUpdater f, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        _systems.at(tag)->setSystemUpdate(f);
        return *this;
    }

    registry& setSystemUpdate(SystemUpdater f)
    {
        return setSystemUpdate(_lastUsedSystem, f, false);
    }

    template <typename T, typename... Args> registry& emplace_r(Args&&... args)
    {
        return emplace<T, Args...>(
            _lastUsedEntity, std::forward<Args>(args)...);
    }

    template <typename T, typename... Args>
    registry& emplace(const Entity& e, Args&&... args)
    {
        _lastUsedEntity = e;
        _entities.get(e.id).set(
            new Component((T) { std::forward<Args>(args)... }), _cti<T>());
        for (auto& sys : _systems)
            sys.second->onEntityUpdate(*this, e);
        return *this;
    }

    registry& update()
    {
        for (auto& sys : _systems)
            sys.second->update(*this);
        return *this;
    }

    EntityId entitiesCount() const { return _lastEntityId; }
};

namespace priv {

    inline void System::onEntityUpdate(registry& r, const Entity& e)
    {
        const auto it = std::find(_entities.begin(), _entities.end(), e);
        for (const auto& dep : _dependencies)
            if (r.has(e, dep, false) == false) {
                if (it != _entities.end())
                    _entities.erase(it);
                return;
            }
        if (it == _entities.end())
            _entities.push_back(e);
    }

}

template<typename T, typename... Args>
class View
{
private:
    std::vector<
        std::unique_ptr<
            std::tuple<Entity, T&, Args&...>
        >
    > _tuple;

public:
    View(registry& r)
    {
        std::vector<ComponentIndex> deps;
        r.getDepsList<T&, Args&...>(deps);

        for (EntityId id = 0; id < r.entitiesCount(); id++) {
            bool valid = true;
            for (const auto& dep : deps)
                if (r.has(Entity(id), dep, false) == false) {
                    valid = false;
                    break;
                }
            if (valid) {
                Entity e(id);
                _tuple.push_back(
                    std::make_unique<std::tuple<Entity, T&, Args&...>>(
                       e, r.get<T>(e), r.get<Args>(e)...
                    )
                );
            }
        }
    }

    template<typename F>
    void eachEntity(F f)
    {
        for (const auto& t : _tuple)
            f(std::forward<Entity>(std::get<0>(*t)),
              std::get<1>(*t),
              std::forward<Args&>(std::get<2>(*t))...);
    }

    template<typename F>
    void each(F f)
    {
        for (const auto& t : _tuple)
            f(std::get<1>(*t),
              std::forward<Args&>(std::get<2>(*t))...);
    }
};

} // namespace silva