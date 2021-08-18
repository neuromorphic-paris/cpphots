#include <iostream>

namespace cpphots {

void __cpphots_assert_fail (const char *__assertion, const char *__file,
			   unsigned int __line, const char *__function) {
    std::cerr << "Assertion '" << __assertion << "' failed in " << __file << ":" << __line << " (" << __function << ")" << std::endl;
    abort();
}

}