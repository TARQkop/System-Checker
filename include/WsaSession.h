#ifndef WSA_SESSION_H
#define WSA_SESSION_H

class WsaSession {
public:
    WsaSession();
    ~WsaSession();
    bool isActive() const;

private:
    bool active_ = false;
};

#endif
