#include "DEVS/engine.hpp"
#include "root.hpp";

int main(){
    Engine engine;
    Root root(&engine);
    engine.RegisterModelWithID(&root);
    engine.Run();
    return 0;
}