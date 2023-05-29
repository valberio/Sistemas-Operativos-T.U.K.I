#include <cspecs/cspec.h>
#include <stdio.h>
#include <string.h>
#include "../utils/src/pcb/pcb.h"

context (example) {

    describe("Hello world") {

        it("true should be true") {
            should_bool(true) be equal to(true);
        } end
        

    } end
}