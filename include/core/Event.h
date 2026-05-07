enum class EventType {None = 0, ViewportActive = 1};

struct Event {
    virtual ~Event() =  default;
    EventType Type;
    bool Handled = false;
};

struct ViewportFocusEvent : public Event {
    bool IsActive;

    ViewportFocusEvent(bool active) : IsActive(active) {
        Type = EventType::ViewportActive;

    }
};

