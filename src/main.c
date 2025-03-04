#include "init.h"

int main() {
    server_cfg cfg = init_cfg(); 
    server_context ctx;
    int s = init_context(&ctx, cfg);
    if (s == -1) {
        return 1;
    }
    server_start(&ctx);
    return 0;
}


