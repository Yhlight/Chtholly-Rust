#include "SemanticAnalyzer.h"

LifetimeManager::LifetimeManager() {
    enterScope();
}

void LifetimeManager::enterScope() {
    lifetimeStack.push_back(nextLifetime++);
}

void LifetimeManager::leaveScope() {
    if (!lifetimeStack.empty()) {
        lifetimeStack.pop_back();
    }
}

Lifetime LifetimeManager::getCurrentLifetime() const {
    if (lifetimeStack.empty()) {
        return 0;
    }
    return lifetimeStack.back();
}
