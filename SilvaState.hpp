/**
 * SilvaState.hpp
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <memory>
#include <stack>

namespace silva
{
namespace priv
{
/**
 * @brief Brief implementation of private State Interface
 */
class IState
{
public:
    /**
     * @brief Destroy the IState object
     */
    virtual ~IState() = default;

    /**
     * @brief Construct a new IState object
     */
    IState() = default;

    /**
     * @brief Initialize the state
     */
    virtual void init() = 0;

    /**
     * @brief Update the state
     */
    virtual void update() = 0;

#ifdef SILVA_STATE_DRAW
    /**
     * @brief Draw the state
     */
    virtual void draw() = 0;
#endif

#ifdef SILVA_STATE_HANDLE_EVENT
    /**
     * @brief Handle the events
     */
    virtual void handleEvent() = 0;
#endif

    /**
     * @brief Exit the state
     */
    virtual void exit() = 0;
};
}

/**
 * @brief Base abstract class for a State
 */
class State : public priv::IState
{
public:
    /**
     * @brief Construct a new State object
     */
    State() = default;

    /**
     * @brief Initialize the State object
     */
    void init() override {}

    /**
     * @brief Exit the State object
     */
    void exit() override {}

#ifdef SILVA_STATE_DRAW
    /**
     * @brief Draw the State object
     */
    void draw() override {}
#endif

#ifdef SILVA_STATE_HANDLE_EVENT
    /**
     * @brief Handle the events
     */
    void handleEvent() override {}
#endif

    ~State() override {}
};

/**
 * @brief State Manager
 */
class StateManager
{
private:
    /**
     * @brief The current pending state
     */
    std::unique_ptr<State> _pendingState = nullptr;

    /**
     * @brief All the stacked states
     */
    std::stack<std::unique_ptr<State>> _currentState;

public:
    /**
     * @brief Construct a new StateManager object
     */
    StateManager() = default;

    /**
     * @brief Destroy the StateManager object
     */
    ~StateManager() = default;

    /**
     * @brief Pop the state from the stack
     */
    void popState()
    {
        _currentState.pop();
    }

    /**
     * @brief Push a new state on the stack
     * @tparam T The type of the state
     * @tparam Args The types of the arguments
     */
    template<typename T, typename ...Args>
    void pushState(Args&&... args)
    {
        _currentState.push(std::make_unique<T>(std::forward<Args>(args)...));
    }

    /**
     * @brief Change the current state
     *        when called only the pendingState is modified
     *        until the next update
     *        At the next update the currentState stack will be emptied
     *        and the pendingState will be pushed on the stack
     * @tparam T The type of the state
     * @tparam Args The types of the arguments
     */
    template<typename T, typename ...Args>
    void changeState(Args&&... args)
    {
        _pendingState = std::make_unique<T>(std::forward<Args>(args)...);
    }

    /**
     * @brief Tells wheter the state will be changed
     */
    bool isInTransition() const { return _pendingState.get() != nullptr; }

    /**
     * @brief Tells wheter the state machine is empty
     */
    bool canBeUpdated() const { return _currentState.empty() == false; }

    /**
     * @brief Tells the state machine depth
     */
    size_t getDepth() const { return _currentState.size(); }

    /**
     * @brief Update the state machine
     *        If a state is pending (Which is different from pushed)
     *        All the stacked states are exited and the pending state is pushed
     *        on the stack and initialized
     */
    bool update()
    {
        if (_pendingState.get()) {
            stop();
            _currentState.push(std::move(_pendingState));
            _currentState.top()->init();
        } else if (!canBeUpdated()) {
            return false;
        }
        _currentState.top()->update();
        return true;
    }

#ifdef SILVA_STATE_DRAW
    /**
     * @brief Draw the state
     */
    void draw()
    {
        if (!canBeUpdated())
            return;
        _currentState.top()->draw();
    }
#endif

#ifdef SILVA_STATE_HANDLE_EVENT
    /**
     * @brief Handle the events
     */
    void handleEvent()
    {
        if (!canBeUpdated())
            return;
        _currentState.top()->handleEvent();
    }
#endif

    /**
     * @brief Stop the state machine
     */
    void stop()
    {
        while (!_currentState.empty()) {
            _currentState.top()->exit();
            _currentState.pop();
        }
    }
};
}
