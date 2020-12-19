#include "task14.h"
#include "Builder.h"

int main() {
    std::string regular;
    std::string word;

    std::cin >> regular;
    std::cin >> word;

    std::cout << task14(regular, word);

    return 0;
}
