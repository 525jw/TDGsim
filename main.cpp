#include "DEVS/engine.hpp"
#include "root.hpp"

int main(){
    Engine engine;
    Root root(&engine);
    engine.RegisterModelInEngine(&root);
    engine.Run();
    return 0;
}