#ifndef GLFWPP_EVENT_H
#define GLFWPP_EVENT_H

#include <GLFW/glfw3.h>
#include <functional>
#include <list>

namespace glfw
{
    template<typename... Args>
    class Event
    {
    private:
        using Handler = std::function<void(Args...)>;
        std::vector<Handler> _handlers;

    public:
        template<typename CallbackT>
        void subscribe(CallbackT&& callback_)
        {
            _handlers.emplace_back(std::forward<CallbackT>(callback_));
        }
        void operator()(Args... args_)
        {
            for (auto& handler: _handlers)
            {
                handler(args_...);
            }
        }
    };

    inline void pollEvents()
    {
        glfwPollEvents();
    }

    inline void waitEvents()
    {
        glfwWaitEvents();
    }
    inline void waitEvents(double timeout_)
    {
        glfwWaitEventsTimeout(timeout_);
    }

    inline void postEmptyEvent()
    {
        glfwPostEmptyEvent();
    }
}  // namespace glfw

#endif  //GLFWPP_EVENT_H
