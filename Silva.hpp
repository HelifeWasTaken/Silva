#pragma once

#include <any>
#include <exception>
#include <functional>
#include <memory>
#include <ostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

namespace silva {

/**
 * @brief Base class for all the exceptions
 */
class Error : public std::exception {
private:
    /**
     * @brief The error message
     */
    std::string _msg;

public:
    /**
     * @brief Construct a new Error object
     * @param msg The error message
     */
    Error(const std::string& msg)
        : _msg(msg)
    {
    }

    /**
     * @brief Get the error message
     * @return const char* The error message
     */
    virtual const char* what() const noexcept override { return _msg.c_str(); }
};

namespace priv {

/**
 * @brief Sparse array size used by default
 */
#ifndef SPARSE_ARRAY_BASE
#define SPARSE_ARRAY_BASE 30
#endif

    /**
     * @brief A Sparse array is a dynamic array providing null posibilities
     * @tparam T Type of the array
     */
    template <typename T> class SparseArray {
    private:
        /**
         * @brief The base size of the array
         */
        const std::size_t _baseSize;

        /**
         * @brief The array of values
         */
        std::vector<std::unique_ptr<T>> _registry;

    public:
        /**
         * @brief Construct a new Sparse Array
         * @param baseSize The base size of the array
         */
        SparseArray(const std::size_t& baseSize = SPARSE_ARRAY_BASE)
            : _baseSize(baseSize)
        {
            resize(_baseSize);
        }

        /**
         * @brief Set the size of the registry of _baseSize and fill it with
         * null values
         */
        void clear()
        {
            try {
                _registry.clear();
                _registry.reserve(_baseSize);
                for (unsigned int i = 0; i < _baseSize; i++)
                    _registry.push_back(std::unique_ptr<T>(nullptr));
            } catch (std::exception& e) {
                throw Error(std::string("clear(): ") + e.what());
            }
        }

        /**
         * @brief Resize the registry with this->size() + given size
         *        SPARSE_ARRAY_BASE if no size is given
         * @param size The size to add to the registry
         */
        void resize(const std::size_t& size)
        {
            try {
                _registry.reserve(_registry.size() + size);
                for (unsigned int i = 0; i < size; i++) {
                    _registry.push_back(std::unique_ptr<T>(nullptr));
                }
            } catch (std::exception& e) {
                throw Error(std::string("resize(") + std::to_string(size)
                    + "): " + e.what());
            }
        }

        /**
         * @brief Set the value at the given index
         * @param value The value to set
         * @param i The index to set the value at
         */
        void set(std::unique_ptr<T>&& elem, const std::size_t& i)
        {
            try {
                _registry.at(i) = std::move(elem);
            } catch (std::exception& e) {
                throw Error(std::string("set(0x")
                    + std::to_string(static_cast<size_t>(elem.get())) + ", "
                    + std::to_string(i) + "): " + e.what());
            }
        }

        /**
         * @brief Set the value at the given index (moves the element)
         * @param elem The value at the given index
         * @param i The index to get the value at
         */
        void set(std::unique_ptr<T>& elem, const std::size_t& i)
        {
            try {
                _registry.at(i) = std::move(elem);
            } catch (std::exception& e) {
                throw Error(std::string("set(0x")
                    + std::to_string(static_cast<size_t>(elem.get())) + ", "
                    + std::to_string(i) + "): " + e.what());
            }
        }

        /**
         * @brief Set the value at the given index
         * @param elem The value to set
         * @param i The index to get the value at
         */
        void set(T* elem, const std::size_t& i)
        {
            try {
                _registry.at(i) = std::unique_ptr<T>(elem);
            } catch (std::exception& e) {
                throw Error(std::string("set(0x")
                    + std::to_string(reinterpret_cast<size_t>(elem)) + ", "
                    + std::to_string(i) + "): " + e.what());
            }
        }

        /**
         * @brief Set the value at the given index (T need to be copyable)
         * @param elem const T& The value at the given index
         * @param i std::size_t& index The index to get the value at
         */
        void set(const T& elem, const std::size_t& i)
        {
            try {
                _registry.at(i) = std::make_unique<T>(elem);
            } catch (std::exception& e) {
                throw Error(std::string("set(")
                    + std::to_string(static_cast<size_t>(&elem)) + ", "
                    + std::to_string(i) + "): " + e.what());
            }
        }

        /**
         * @brief Set the value at the given index (T need to be constructible
         * with T())
         * @param i The index to set the value at
         */
        void set(const std::size_t& i)
        {
            try {
                _registry.at(i) = std::make_unique<T>();
            } catch (std::exception& e) {
                throw Error(
                    std::string("set(") + std::to_string(i) + "): " + e.what());
            }
        }

        /**
         * @brief Unset the value at the given index (set to nullptr)
         *        (It is recommended to have a proper destructor for T)
         * @param i The index to unset the value at
         */
        void unset(const std::size_t& i)
        {
            try {
                _registry.at(i) = std::unique_ptr<T>(nullptr);
            } catch (std::exception& e) {
                throw Error(std::string("unset(") + std::to_string(i)
                    + "): " + e.what());
            }
        }

        /**
         * @brief Get the value at the given index
         * @param i The index to get the value at
         * @return T& The value at the given index
         */
        T& get(const std::size_t& i)
        {
            if (isSet(i))
                return *_registry.at(i);
            throw Error("Trying to get a value at an unset index: "
                + std::to_string(i));
        }

        /**
         * @brief Get the value at the given index
         * @param i The index to get the value at
         * @return std::unique_ptr<T>& The object value container at the given
         * index
         */
        std::unique_ptr<T>& getO(const std::size_t& i)
        {
            try {
                return _registry.at(i);
            } catch (std::exception& e) {
                throw Error(std::string("getO(") + std::to_string(i)
                    + "): " + e.what());
            }
        }

        /**
         * @brief Get the value at the given index
         * @param i The index to get the value at
         * @return T* The value at the given index
         */
        bool isSet(const std::size_t& i) const
        {
            try {
                return _registry.at(i) != nullptr;
            } catch (std::exception& e) {
                throw Error(std::string("isSet(") + std::to_string(i)
                    + "): " + e.what());
            }
        }

        /**
         * @brief Get the size of the SparseArray
         * @return std::size_t The value at the given index
         */
        const std::size_t size() const { return _registry.size(); }

        /**
         * @brief Iterator to iterate over the SparseArray
         */
        class Iterator {
        private:
            /**
             * @brief The SparseArray to iterate over
             */
            SparseArray<T>& _array;

            /**
             * @brief The current index
             */
            std::size_t _index;

            /**
             * @brief Advance the iterator to the next index
             */
            void _advance()
            {
                if (_index <= _array.size())
                    _index++;
            }

            /**
             * @brief Retreat the iterator to the previous index
             */
            void _retreat()
            {
                if (_index > 0)
                    _index--;
            }

            /**
             * @brief Advance the iterator of N indexes
             */
            void _advance(std::size_t n)
            {
                for (std::size_t i = 0; i < n && _index <= _array.size();
                     i++, _index++)
                    ;
            }

            /**
             * @brief Retreat the iterator of N indexes
             */
            void _retreat(std::size_t n)
            {
                for (std::size_t i = 0; i < n && _index > 0; i++, _index--)
                    ;
            }

        public:
            /**
             * @brief Construct a new Iterator object
             * @param array The SparseArray to iterate over
             * @param index The start index
             */
            Iterator(SparseArray<T>& array, const std::size_t& index = 0)
                : _array(array)
                , _index(index)
            {
                if (_index > _array.size())
                    _index = _array.end().index();
            }

            /**
             * @brief Give the index of the current element
             * @return const std::size_t& The index of the current element
             */
            const std::size_t& index() const { return _index; }

            /**
             * @brief Advance the iterator to the next index
             * @return Iterator& The iterator at the next index
             */
            Iterator& operator++()
            {
                _advance();
                return *this;
            }

            /**
             * @brief Advance the iterator to the previous index
             * @return Iterator& The iterator at the previous index
             */
            Iterator& operator--()
            {
                _retreat();
                return *this;
            }

            /**
             * @brief Advance the iterator of N indexes
             * @return Iterator& The iterator at the next N index
             */
            Iterator& operator+=(const std::size_t& n)
            {
                _advance(n);
                return *this;
            }

            /**
             * @brief Retreat the iterator of N indexes
             * @return Iterator& The iterator at the previous N index
             */
            Iterator& operator-=(const std::size_t& n)
            {
                _retreat(n);
                return *this;
            }

            /**
             * @brief Advance the iterator of N indexes
             * @return A new iterator at the next N index
             */
            Iterator operator+(const std::size_t& n) const
            {
                Iterator it(_array);
                return it += (n + _index);
            }

            /**
             * @brief Retreat the iterator of N indexes
             * @return A new iterator at the previous N index
             */
            Iterator operator-(const std::size_t& n) const
            {
                Iterator it(_array);
                it += _index;
                return it -= n;
            }

            /**
             * @brief Get the object value at the current index
             * @return std::unique_ptr<T>& The object value at the current index
             */
            std::unique_ptr<T>& operator*() { return _array.getO(_index); }

            /**
             * @brief Get the object value at the current index
             * @return std::unique_ptr<T>& The object value at the current index
             */
            std::unique_ptr<T>& operator->() { return _array.getO(_index); }

            /**
             * @brief Compare two iterators
             * @param other The iterator to compare to
             * @return true The iterators are equal
             * @return false The iterators are not equal
             */
            bool operator==(const Iterator& other) const
            {
                return _index == other.index();
            }

            /**
             * @brief Compare two iterators
             * @param other The iterator to compare to
             * @return true The iterators are not equal
             * @return false The iterators are equal
             */
            bool operator!=(const Iterator& other) const
            {
                return _index != other.index();
            }

            /**
             * @brief Compare two iterators
             * @param other The other iterator to compare to
             * @return true The left iterator index is less than the right
             * @return false The left iterator index is greater or equal to the
             * right
             */
            bool operator<(const Iterator& other) const
            {
                return _index < other.index();
            }

            /**
             * @brief Compare two iterators
             * @param other The other iterator to compare to
             * @return true The left iterator index is greater than the the
             * right
             * @return false The left iterator index is less or equal to the
             * right
             */
            bool operator>(const Iterator& other) const
            {
                return _index > other.index();
            }

            /**
             * @brief Compare two iterators
             * @param other The other iterator to compare to
             * @return true The left iterator index is less than or equal to the
             * right
             * @return false The left iterator index is greater than the right
             */
            bool operator<=(const Iterator& other) const
            {
                return _index <= other.index();
            }

            /**
             * @brief Compare two iterators
             * @param other The other iterator to compare to
             * @return true The left iterator index is greater than or equal to
             * the right
             * @return false The left iterator index is less than the right
             */
            bool operator>=(const Iterator& other) const
            {
                return _index >= other.index();
            }

            /**
             * @brief Tells if the object at the current index is valid
             *
             * @return true The object at the current index is valid
             * @return false The object at the current index is invalid
             */
            bool isSet() const { return _array.isSet(_index); }

            /**
             * @brief Set the given object at the current index of the Iterator
             * @param elem The object to set
             */
            void set(const T& elem) { _array.set(elem, _index); }

            /**
             * @brief Set the given object at the current index of the Iterator
             * @param elem The object to set
             */
            void set(T* elem) { _array.set(elem, _index); }

            /**
             * @brief Set the given object at the current index of the Iterator
             * @param elem The object to set
             */
            void set(std::unique_ptr<T>& elem) { _array.set(elem, _index); }

            /**
             * @brief Unset the given object at the current index of the
             * Iterator
             */
            void unset() { _array.unset(_index); }
        };

        /**
         * @brief Get an iterator to the first element
         * @return Iterator The iterator to the first element
         */
        Iterator begin() { return Iterator(*this); }

        /**
         * @brief Get an iterator to the last element
         * @return Iterator The iterator to the last element
         */
        Iterator end() { return Iterator(*this, _registry.size()); }
    };
}
}

namespace silva {

/**
 * @brief Fwd
 *
 */
class registry;

/**
 * @brief Fwd
 *
 */
struct Entity;

/**
 * @brief Id of an Entity
 *
 */
using EntityId = std::size_t;

/**
 * @brief Type of a Component
 *
 */
using Component = std::any;

/**
 * @brief Index of a Component
 *
 */
using ComponentIndex = std::size_t;

/**
 * @brief Type for hash of a Component typename
 *
 */
using TypeNameId = const char*;

/**
 * @brief Function to update a system
 *
 */
using SystemUpdater = std::function<void(const Entity&, registry&)>;

/**
 * @brief Entity is a single ID wrapped around a struct
 *        in order to put some member functions in it
 *        and to demangle some symbols so it is not
 *        mismatched with the EntityId when passing it to templates
 *        with ...Args
 */
struct Entity {
    /**
     * @brief Id of the Entity
     */
    EntityId id;

    /**
     * @brief Construct a new Entity
     * @param id The id of the Entity
     */
    explicit Entity(const EntityId& id)
        : id(id)
    {
    }

    /**
     * @brief Tells if the Entity is equal to another Entity
     * @param other The other Entity to compare to
     * @return true The Entities are equal
     * @return false The Entities are not equal
     */
    bool operator==(const Entity& other) { return id == other.id; }

    /**
     * @brief Tells if the Entity is not equal to another Entity
     * @param other The other Entity to compare to
     * @return true The Entities are not equal
     * @return false The Entities are equal
     */
    bool operator!=(const Entity& other) { return id != other.id; }

    /**
     * @brief Represent the Entity as a string to be used in ostreams
     *
     * @param os The ostream to write to
     * @param e The Entity to represent
     * @return std::ostream& The ostream
     */
    friend std::ostream& operator<<(std::ostream& os, const Entity& e)
    {
        return os << "Entity(" << e.id << ")";
    }
};

namespace priv {

    /**
     * @brief A system is a collection of entities
     *       that are updated at a certain interval
     *       The system requires a function to update
     *       the entities
     *       To make sure the entities are matching the system
     *       we use dependencies and make them match with the entities
     *       Dependencies are the components that the system requires
     */
    class System {
    private:
        /**
         * @brief The entities that are part of the system
         */
        std::vector<Entity> _entities;

        /**
         * @brief The updater function of the system
         */
        SystemUpdater _f;

        /**
         * @brief The dependencies of the system
         */
        std::vector<ComponentIndex> _dependencies;

        /**
         * @brief The index of the system in the registry
         *        in case we want to remove some while iterating
         *        It should always be reset to 0 when the system is updated
         */
        std::size_t _index = 0;

    public:
        /**
         * @brief Construct a new System
         */
        System() = default;

        /**
         * @brief Construct a new System from another System
         * @param sys The system to copy
         */
        System(const System& sys) = default;

        /**
         * @brief Set the System Update object
         * @param f The function to update the system
         */
        void setSystemUpdate(SystemUpdater f) { _f = f; }

        /**
         * @brief Get the System Update object (uses indexes)
         * @param dependency The dependency of the system
         */
        void addDependency(const ComponentIndex& dependency)
        {
            if (std::find(
                    _dependencies.begin(), _dependencies.end(), dependency)
                != _dependencies.end())
                return;
            _dependencies.push_back(dependency);
        }

        /**
         * @brief tries to see if the given entity should be part of the system
         *        If the entity has all the dependencies of the system and is
         * not already part of the system the entity is added to the system If
         * the entity is already part of the system and has not all the
         * dependencies the entity is removed from the system. Otherwise it does
         * nothing
         * @param r The registry
         * @param e The entity to check
         */
        void onEntityUpdate(registry& r, const Entity& e);

        /**
         * @brief Remove the given entity from the system
         * @param e The entity to remove
         */
        void onEntityDelete(const Entity& e)
        {
            const size_t oldSize = _entities.size();
            _entities.erase(std::remove(_entities.begin(), _entities.end(), e),
                _entities.end());
            const std::size_t offset = oldSize - _entities.size();
            if (offset)
                _index = _index < offset ? 0 : _index - offset;
        }

        /**
         * @brief Update the system
         * @param r The registry to use
         */
        void update(registry& r)
        {
            for (_index = 0; _index < _entities.size(); _index++)
                _f(_entities[_index], r);
        }
    };

}

/**
 * @brief The registry base size for the SparseArray allocator of the entities
 *
 */
#ifndef REGISTRY_ENTITY_SIZE
#define REGISTRY_ENTITY_SIZE 8192
#endif

/**
 * @brief The registry base size for the SparseArray allocator of the Components
 *
 */
#ifndef REGISTRY_COMPONENT_SIZE
#define REGISTRY_COMPONENT_SIZE 50
#endif

/**
 * @brief The registry is the container of all the entities and components
 *        It also contains the systems that are updated at each call of update
 *
 */
class registry {
private:
    /**
     * @brief The index from template name (typeid)
     */
    std::unordered_map<TypeNameId, ComponentIndex> _componentToIndex;
    /**
     * @brief The next index of the components
     */
    ComponentIndex _lastComponentIndex = 0;

    /**
     * @brief The container of the entities holding the components
     */
    priv::SparseArray<priv::SparseArray<Component>> _entities;

    /**
     * @brief The ids of all the removed entities to reuse them
     */
    std::stack<EntityId> _removedEntitiesIds;

    /**
     * @brief The next index of the entities
     */
    EntityId _lastEntityId = 0;

    /**
     * @brief The systems that are updated at each call of update (uses tags)
     */
    std::unordered_map<std::string, std::unique_ptr<priv::System>> _systems;

    /**
     * @brief The last used entity (used to avoid passing the entity each time
     * as a parameter)
     */
    Entity _lastUsedEntity = Entity(0);

    /**
     * @brief The last used system (used to avoid passing the system name each
     * time as a parameter)
     */
    std::string _lastUsedSystem = "";

    /**
     * @brief Transform the given type to a string hashes it
     *        and uses it as a key to store the component index
     *        If the type is not registered yet, it registers it
     * @return ComponentIndex The index of the component
     */
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
                    _entities.get(i).resize(REGISTRY_ENTITY_SIZE);
            _lastComponentIndex++;
            return _lastComponentIndex - 1;
        }
    }

    /**
     * @brief Adds to the given system a dependency of the given type
     * @param sys The system to add the dependency to
     * @tparam T The type of the dependency
     * @return registry& The registry to chain the calls
     */
    template <typename T> registry& _addSystemDeps(priv::System& sys)
    {
        sys.addDependency(_cti<T>());
        for (unsigned int i = 0; i < _lastEntityId; i++)
            sys.onEntityUpdate(*this, Entity(i));
        return *this;
    }

    /**
     * @brief Adds to the given system a dependency of the given type
     * @tparam T The type of the dependency
     * @tparam ...Args The other types of the dependencies
     * @return registry& The registry to chain the calls
     */
    template <typename T, typename... Args,
        typename std::enable_if<sizeof...(Args) != 0>::type* = nullptr>
    registry& _addSystemDeps(priv::System& sys)
    {
        sys.addDependency(_cti<T>());
        return _addSystemDeps<Args...>(sys);
    }

public:
    /**
     * @brief Loads the dependencies of the given types
     * @tparam T The type of the system
     * @tparam ...Args The other types of the dependencies
     * @return std::vector<ComponentIndex>& The dependencies of the system
     */
    template <typename T, typename... Args,
        typename std::enable_if<sizeof...(Args) != 0>::type* = nullptr>
    std::vector<ComponentIndex>& getDepsList(std::vector<ComponentIndex>& deps)
    {
        deps.push_back(_cti<T>());
        return getDepsList<Args...>(deps);
    }

    /**
     * @brief Loads the dependencies of the given type
     * @tparam T The type of the system
     * @return std::vector<ComponentIndex>& The dependencies of the system
     */
    template <typename T, typename... Args,
        typename std::enable_if<sizeof...(Args) == 0>::type* = nullptr>
    std::vector<ComponentIndex>& getDepsList(std::vector<ComponentIndex>& deps)
    {
        deps.push_back(_cti<T>());
        return deps;
    }

    /**
     * @brief Checks wheter the given Entity has the given Component
     * @param e The entity to check
     * @param component The component to check
     * @param updateLast Used to avoid passing the entity each time as a
     * parameter (if true, _lastUsedEntity is updated to e)
     * @return true if the entity has the component, false otherwise
     * @return false if the entity has the component, true otherwise
     */
    bool has(const Entity& e, const ComponentIndex& component,
        const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedEntity = e;
        return _entities.get(e.id).isSet(component);
    }

    /**
     * @brief Checks wheter the last used Entity has the given Component
     * @return true if the entity has the component, false otherwise
     * @return false if the entity has the component, true otherwise
     */
    bool has(const ComponentIndex& component)
    {
        return has(_lastUsedEntity, component, false);
    }

    /**
     * @brief Returns the Component of the given Entity
     * @param e The entity to get the component from
     * @param updateLast Used to avoid passing the entity each time as a
     * parameter (if true, _lastUsedEntity is updated to e)
     * @tparam The type of the component
     * @return true if the entity has the component, false otherwise
     * @return false if the entity has the component, true otherwise
     */
    template <typename T>
    bool has(const Entity& e, const bool& updateLast = true)
    {
        return has(e, _cti<T>(), updateLast);
    }

    /**
     * @brief Returns the Component of the last used Entity
     * @tparam The type of the component
     * @return true if the entity has the component, false otherwise
     * @return false if the entity has the component, true otherwise
     */
    template <typename T> bool has(const bool& updateLast = true)
    {
        return has(_lastUsedEntity, _cti<T>(), updateLast);
    }

    /**
     * @brief Returns the Component of the given Entity
     * @param e The entity to get the component from
     * @param updateLast Used to avoid passing the entity each time as a
     * parameter (if true, _lastUsedEntity is updated to e)
     * @tparam The type of the component
     * @return T& The component of the entity
     */
    template <typename T> T& get(const Entity& e, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedEntity = e;
        return std::any_cast<T&>(_entities.get(e.id).get(_cti<T>()));
    }

    /**
     * @brief Returns the Component of the last used Entity
     * @tparam T The type of the component
     * @return T& The component of the entity
     */
    template <typename T> T& get() { return get<T>(_lastUsedEntity, false); }

    /**
     * @brief Creates a new Entity and returns it
     *        It sets the last used Entity to the newly created one
     * @return Entity The new Entity
     */
    Entity newEntity()
    {
        if (_removedEntitiesIds.empty()) {
            _lastUsedEntity.id = _lastEntityId;
            if (_lastUsedEntity.id + 1 >= _entities.size()) {
                _entities.resize(REGISTRY_ENTITY_SIZE);
            }
            _entities.set(
                new priv::SparseArray<Component>(REGISTRY_COMPONENT_SIZE),
                _lastUsedEntity.id);
            _lastEntityId++;
            return _lastUsedEntity;
        }
        _lastUsedEntity = Entity(_removedEntitiesIds.top());
        _entities.set(new priv::SparseArray<Component>(REGISTRY_COMPONENT_SIZE),
            _lastUsedEntity.id);
        _removedEntitiesIds.pop();
        return _lastUsedEntity;
    }

    /**
     * @brief Removes the given Entity
     * @param e The entity to remove
     * @return registry& The registry to chain the calls
     */
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

    /**
     * @brief add a new System of the given tag
     * @param tag The tag of the system
     * @param updateLast Used to avoid passing the system each time as a
     * parameter (if true, _lastUsedSystem is updated to sys)
     * @tparam T The first type of the system dependencies
     * @tparam Args... The other types of the dependencies
     * @return registry& The registry to chain the calls
     */
    template <typename T, typename... Args>
    registry& addSystem(const std::string& tag, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        _systems[tag] = std::make_unique<priv::System>();
        return _addSystemDeps<T, Args...>(*_systems.at(tag));
    }

    /**
     * @brief add a new System of the given tag
     * @param tag The tag of the system
     * @tparam T The first type of the system dependencies
     * @tparam Args... The other types of the dependencies
     * @return registry& The registry to chain the calls
     */
    template <typename T, typename... Args>
    registry& addSystemDeps(
        const std::string& tag, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        return _addSystemDeps<T, Args...>(*_systems.at(tag));
    }

    /**
     * @brief add deps to the last added System
     * @tparam T The first type of the system dependencies
     * @tparam Args... The other types of the dependencies
     * @return registry& The registry to chain the calls
     */
    template <typename T, typename... Args> registry& addSystemDeps()
    {
        return addSystemDeps<T, Args...>(_lastUsedSystem, false);
    }

    /**
     * @brief Removes the given System
     * @param tag The tag of the system
     * @return registry& The registry to chain the calls
     */
    registry& removeSystem(const std::string& tag)
    {
        _systems.erase(tag);
        return *this;
    }

    /**
     * @brief Set the updater function of the given System
     * @param tag The tag of the system
     * @param f The function to set
     * @param updateLast Used to avoid passing the system each time as a
     * parameter (if true, _lastUsedSystem is updated to sys)
     * @return registry& The registry to chain the calls
     */
    registry& setSystemUpdate(
        const std::string& tag, SystemUpdater f, const bool& updateLast = true)
    {
        if (updateLast)
            _lastUsedSystem = tag;
        _systems.at(tag)->setSystemUpdate(f);
        return *this;
    }

    /**
     * @brief Set the updater function of the last added System
     * @param f The function to set
     * @return registry& The registry to chain the calls
     */
    registry& setSystemUpdate(SystemUpdater f)
    {
        return setSystemUpdate(_lastUsedSystem, f, false);
    }

    /**
     * @brief Calls emplace on the last used Entity (Is used to chain emplace
     * calls)
     * @tparam T The type of the component
     * @tparam Args... The types of the arguments
     * @param args The arguments of the emplace call
     * @return registry& The registry to chain the calls
     */
    template <typename T, typename... Args> registry& emplace_r(Args&&... args)
    {
        return emplace<T, Args...>(
            _lastUsedEntity, std::forward<Args>(args)...);
    }

    /**
     * @brief Calls emplace on the given Entity (Used as first emplace call)
     * @tparam T The type of the component
     * @tparam Args... The types of the arguments
     * @param e The entity to emplace the component on
     * @param args The arguments of the emplace call
     * @return registry& The registry to chain the calls
     */
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

    /**
     * @brief Updates all the systems in the registry
     * @return registry& The registry to chain the calls
     */
    registry& update()
    {
        for (auto& sys : _systems)
            sys.second->update(*this);
        return *this;
    }

    /**
     * @brief Returns the current max Entity id
     * @return EntityId The max Entity id
     */
    EntityId entitiesCount() const { return _lastEntityId; }
};

namespace priv {

    /**
     * @brief tries to see if the given entity should be part of the system
     *        If the entity has all the dependencies of the system and is not
     * already part of the system the entity is added to the system If the
     * entity is already part of the system and has not all the dependencies the
     * entity is removed from the system. Otherwise it does nothing
     * @param r The registry
     * @param e The entity to check
     */
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

/**
 * @brief A view is a set of entities that can be filtered by a set of
 * components The view is always constant and should not be used to modify the
 * registry (or to remove entities) (to modify the registry, use the systems or
 * the registry itself)
 *
 * @tparam T The first type of the components
 * @tparam Args... The other types of the components
 */
template <typename T, typename... Args> class View {
private:
    /**
     * @brief The registry that the view is based on
     */
    std::vector<std::unique_ptr<std::tuple<Entity, T&, Args&...>>> _tuple;

public:
    /**
     * @brief Construct a new View object
     * @param r The registry to base the view on
     */
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
                        e, r.get<T>(e), r.get<Args>(e)...));
            }
        }
    }

    /**
     * @brief Apply the given function to each entity in the view
     */
    template <typename F> void eachEntity(F f)
    {
        for (const auto& t : _tuple)
            f(std::forward<Entity>(std::get<0>(*t)), std::get<1>(*t),
                std::forward<Args&>(std::get<2>(*t))...);
    }

    /**
     * @brief Apply the given function to each entity in the view
     */
    template <typename F> void each(F f)
    {
        for (const auto& t : _tuple)
            f(std::get<1>(*t), std::forward<Args&>(std::get<2>(*t))...);
    }

    /**
     * @brief Iterator based on the view
     */
    class Iterator {
    private:
        /**
         * @brief Current index
         */
        std::size_t _i;

        /**
         * @brief A reference to the view
         */
        const std::vector<std::unique_ptr<std::tuple<Entity, T&, Args&...>>>&
            _tuple;

    public:
        /**
         * @brief Construct a new Iterator object
         * @param tuple A reference to the view
         * @param i The starting index
         */
        Iterator(const std::vector<
                     std::unique_ptr<std::tuple<Entity, T&, Args&...>>>& tuple,
            std::size_t i)
            : _i(i)
            , _tuple(tuple)
        {
            if (_i >= _tuple.size())
                _i = _tuple.size();
        }

        /**
         * @brief Advances the iterator of one step
         * @return Iterator& The iterator after the step
         */
        Iterator& operator++()
        {
            if (++_i >= _tuple.size())
                _i = _tuple.size();
            return *this;
        }

        /**
         * @brief Retreats the iterator of one step
         * @return Iterator& The iterator before the step
         */
        Iterator& operator--()
        {
            if (--_i >= _tuple.size())
                _i = _tuple.size();
            return *this;
        }

        /**
         * @brief Creates an new Iterator and advances it of n steps
         * @param n The number of steps to advance
         * @return Iterator A copy of the iterator + n steps
         */
        Iterator operator+(const std::size_t& i)
        {
            return Iterator(_tuple, _i + i);
        }

        /**
         * @brief Creates an new Iterator and retreats it of n steps
         * @param i The number of steps to retreat
         * @return Iterator A copy of the iterator - n steps
         */
        Iterator operator-(const std::size_t& i)
        {
            Iterator it = Iterator(_tuple, _i);
            return it -= i;
        }

        /**
         * @brief Advances the iterator of n steps
         * @param i The number of steps to advance
         * @return Iterator& The iterator after n step
         */
        Iterator& operator+=(const std::size_t& i)
        {
            _i += i;
            if (_i >= _tuple.size())
                _i = _tuple.size();
            return *this;
        }

        /**
         * @brief Retreats the iterator of n steps
         * @param i The number of steps to retreat
         * @return Iterator& The iterator before n step
         */
        Iterator& operator-=(const std::size_t& i)
        {
            _i -= i;
            if (_i >= _tuple.size())
                _i = 0;
            return *this;
        }

        /**
         * @brief Compares two iterators
         * @param it The other iterator
         * @return true The two iterators are not equal
         * @return false The two iterators are equal
         */
        bool operator!=(const Iterator& other) const { return _i != other._i; }

        /**
         * @brief Compares two iterators
         * @param it The other iterator
         * @return true The two iterators are equal
         * @return false The two iterators are not equal
         */
        bool operator==(const Iterator& other) const { return _i == other._i; }

        /**
         * @brief Gets the current entity and its components
         * @return std::tuple<Entity, T&, Args&...> The current entity and its
         * components
         */
        std::tuple<Entity, T&, Args&...>& operator*()
        {
            try {
                return *_tuple.at(_i);
            } catch (const std::exception& e) {
                throw Error(
                    std::string("operator*(): invalid iterator: ") + e.what());
            }
        }

        /**
         * @brief Gets the current entity and its components
         * @return std::tuple<Entity, T&, Args&...> The current entity and its
         * components
         */
        std::tuple<Entity, T&, Args&...>& operator->() { return *this; }
    };

    /**
     * @brief Returns an iterator to the first entity of the view
     * @return Iterator An iterator to the first entity of the view
     */
    Iterator begin() { return Iterator(_tuple, 0); }

    /**
     * @brief Returns an iterator to the last entity of the view
     * @return Iterator An iterator to the last entity of the view
     */
    Iterator end() { return Iterator(_tuple, _tuple.size()); }
};

} // namespace silva