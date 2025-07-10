#pragma once
#include <string>
#include <any>
#include <memory>

class Event{
private:
    // --- Header ---
    int         senderModelID;
    std::string senderPort;

    // --- Payload ---
    std::shared_ptr<const std::any> payload;

public:
    template <typename MsgT>
    Event(int senderID, std::string port, MsgT&& msg)
        : senderModelID(senderID),
          senderPort(std::move(port)),
          payload(std::make_shared<const std::any>(std::forward<MsgT>(msg))) {}

    Event(const Event&)            = default;
    Event& operator=(const Event&) = default;
    
    int                     getSenderModelID() const { return senderModelID; }
    const std::string&      getSenderPort()   const { return senderPort; }
    const std::any&         getMessage()      const { return *payload;   }

    void setSenderModelID(int id)                { senderModelID = id; }
    void setSenderPort(const std::string& port)  { senderPort    = port; }
};