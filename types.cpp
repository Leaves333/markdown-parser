#include "types.h"
#include <ostream>
#include <variant>

namespace debug {

struct Printer {
    std::ostream &os;
    void operator()(Root n) { os << "Root{" << n.children << "}"; }
    void operator()(Paragraph n) { os << "Paragraph{" << n.children << "}"; }
    void operator()(Heading n) { os << "Heading{depth=" << n.depth << ", " << n.children << "}"; }
    void operator()(Emphasis n) { os << "Emphasis{" << n.children << "}"; }
    void operator()(Strong n) { os << "Strong{" << n.children << "}"; }
    void operator()(Text n) { os << "Text{\"" << n.value << "\"}"; }
    void operator()(InlineCode n) { os << "InlineCode{\"" << n.value << "\"}"; }
};

} // namespace debug

std::ostream &operator<<(std::ostream &os, const Node &node) {
    std::visit(debug::Printer{os}, node);
    return os;
}
