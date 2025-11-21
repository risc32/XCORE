#pragma once

#include "../console.cpp"
#include "../tree/tree.cpp"

static void debugtree(uint64_t root_addr, int depth = 0) {
    if (root_addr == 0) return;

    inode node = tree::getinode(root_addr);

    // Отступ по глубине
    for (int i = 0; i < depth; i++) {
        KernelOut() << "  ";
    }

    // Вывод узла
    KernelOut() << (node.type == it_dir ? "[D] " : "[F] ")
                << node.name
                << " @ " << hex << node.nd.depth
                << " (a=" << node.nd.a << ", b=" << node.nd.b << ")"
                << endl;

    // Рекурсия для потомков
    debugtree(node.nd.a, depth + 1);
    debugtree(node.nd.b, depth + 1);
}