//
// Created by Sakura on 25-9-17.
//

#ifndef PATTERNS_H
#define PATTERNS_H

#include <memory>
#include <string>
#include <utility>

#include "basic.h"

struct Pattern: BasicNode {
public:
    virtual ~Pattern() = default;

    virtual void accept(SemanticAnalyzer &visitor) override;
};

struct IdentifierPattern: Pattern {
    bool have_ref;
    bool is_mut;
    std::string identifier;

    IdentifierPattern(bool have_ref, bool is_mut, std::string identifier): have_ref(have_ref), is_mut(is_mut), identifier(std::move(identifier)) {}
};

struct WildcardPattern: Pattern {
    WildcardPattern();
};

struct ReferencePattern: Pattern {
    bool double_ref;//true means &&, false means &
    bool is_mut;
    std::shared_ptr<Pattern> pattern;

    ReferencePattern(bool double_ref, bool is_mut, std::shared_ptr<Pattern> pattern): double_ref(double_ref), is_mut(is_mut), pattern(std::move(pattern)) {}
};

#endif //PATTERNS_H
