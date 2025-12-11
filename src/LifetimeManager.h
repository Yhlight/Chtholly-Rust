#ifndef CHTHOLLY_LIFETIMEMANAGER_H
#define CHTHOLLY_LIFETIMEMANAGER_H

#include "Lifetime.h"
#include <vector>

class LifetimeManager {
public:
    LifetimeManager();
    void enterScope();
    void leaveScope();
    Lifetime getCurrentLifetime() const;

private:
    std::vector<Lifetime> lifetimeStack;
    Lifetime nextLifetime = 0;
};

#endif // CHTHOLLY_LIFETIMEMANAGER_H
