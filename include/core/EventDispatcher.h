#include <functional>
#include <vector>
#include "core/Event.h"

class EventDispatcher {
    public:
    using EventCallback = std::function<void(Event&)>;

    void Subscribe(EventType type, EventCallback callback) {
        m_Subscribers[type].push_back(callback);
    }

    void Dispatch(Event& e) {
        if (m_Subscribers.find(e.Type) == m_Subscribers.end()) return;

        for (auto& callback : m_Subscribers[e.Type]) {
            callback(e);
        }
    }
    private:
    std::unordered_map<EventType, std::vector<EventCallback>> m_Subscribers;

};