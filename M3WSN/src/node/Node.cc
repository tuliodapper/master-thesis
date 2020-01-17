#include "Node.h"

std::ostream& operator<<(std::ostream& os, const Node& n) {
    os << n.addr << std::endl;
    return os;
}
