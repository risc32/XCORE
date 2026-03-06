#pragma once

#include "storage.cpp"
#include "bitmap.cpp"
#include "../utils/inited.cpp"
#include "../debug/inited.cpp"
#include "../memory/memory.cpp"
#include "xcfs.cpp"
#include "../crypto/crypto.cpp"

#define ENAMESZ 232
#define ENTSIZE sizeof(DirectoryEntry)

#define CHECK(path) {if (!check(path)) panic("Invalid name");}
#define CHECKF(path) {if (!fcheck(path)) panic("Invalid path");}

struct DirectoryEntry {
    __int128 hash;
    uint64_t ind;
    char name[ENAMESZ];

    bool compare(const string& other) const {
        return strcmp(name, other.data()) == 0;
    }
};

struct Directory {
    linkednode linked;

    result getres() {
        return {
            linked.ind,
            linked.sector
        };
    }

    static Directory allocdir() {
        result i = dispatcher::add_result({
                it_dir
            });
        auto allocated = fragment::hardwarealloc(1);
        //if (i.ind.type != it_none) panic("A file or directory with that name already exists");
        memcpy(i.ind.fragments, allocated.data(), sizeof i.ind.fragments);
        i.ind.type = it_dir;

        disk::write(i.sector, i.ind.data);

        //filesystem::bst.insert(ind);
        return Directory(i);
    }

    static Directory create(const string& path) {

        Directory d = allocdir();

        Path p = path;
        Directory parent = Directory{filesystem::getdir(p.parents)};
        parent.add_entry(p.endname, d.linked.sector);

        return d;
    }

    explicit Directory(result result) : linked{result.ind, result.sector} {
        if (linked.ind.type != it_dir)
            panic("The specified directory does not exist");
    }

    explicit Directory(const string& path) {
        if (path.empty()) {
            linked.ind.type = it_none;
            linked.sector = 0;
            return;
        }
        CHECKF(path)

        auto re = filesystem::getdir(path);
        linked.ind = re.ind;
        linked.sector = re.sector;
        if (linked.ind.type != it_dir)
            panic(("The specified directory \""_s + path + "\" does not exist"_s).c_str());
    }

    DirectoryEntry get_entry(uint64_t pos) {
        //return *(DirectoryEntry*)fragment::read(dir.fragments, pos * sizeof(DirectoryEntry), sizeof(DirectoryEntry)).data();
        // Шаг 1: Вычисляем смещение (позицию в байтах)
        size_t offset = pos * sizeof(DirectoryEntry);

        // Шаг 2: Читаем данные из фрагментов по этому смещению
        // Предполагаем, что read() возвращает какой-то объект (например, vector<char> или свою структуру)
        auto read_result = fragment::read(linked.ind.fragments, offset, sizeof(DirectoryEntry));

        // Шаг 3: Получаем указатель на сырые данные
        const void* raw_data_ptr = read_result.data();

        // Шаг 4: Преобразуем void-указатель в указатель на DirectoryEntry
        const DirectoryEntry* entry_ptr = static_cast<const DirectoryEntry*>(raw_data_ptr);
        // Или если используется C-style cast как в оригинале:
        // const DirectoryEntry* entry_ptr = (DirectoryEntry*)raw_data_ptr;

        // Шаг 5: Разыменовываем указатель, чтобы получить саму структуру
        DirectoryEntry entry = *entry_ptr;

        // Шаг 6: Возвращаем результат
        return entry;
    }

    void set_entry(uint64_t pos, DirectoryEntry data) {
        fragment::write(linked.ind.fragments, pos * sizeof(DirectoryEntry), bytestream(data), linked);
    }

    DirectoryEntry find_entry(const string& entry) {
        if (entry.size() > ENAMESZ) return {};
        __int128 entryhash = hash::_highprec1(entry.c_str());
        int pos = 0;
        //serial0() << "find entry: " << dir.getsize() << " : " << entry << endl;

        while (pos < linked.ind.getsize() / sizeof(DirectoryEntry)) {
            //stop();
            auto i = get_entry(pos++);

            if (i.hash == entryhash and i.compare(entry)) {

                return i;
            }


            //if (pos >= dir.getsize() / sizeof(DirectoryStorage)) break;
        }

        return {};
    }

    result find(const string& entry);

    managed<string> list() {
        managed<string> res;
        int pos = 0;
        while (true) {
            res.push_back(get_entry(pos++).name);

            if (pos >= linked.ind.getsize() / ENTSIZE) break;
        }
        return res;
    }

    bool has_entry(const string& entry) {
        return find_entry(entry).ind;
    }

    static bool check(const string& entry) {

        for (char i : entry) {
            if (i == '/' or i == '\n' or i == 0) return false;

        }
        if (entry == ".") return false;
        if (entry == "..") return false;
        if (entry.size() > ENAMESZ) return false;

        return true;
    }

    static bool fcheck(const string& entry) {

        for (char i : entry) {
            if (i == '\n' or i == 0) return false;
        }
        if (entry == ".") return false;
        if (entry == "..") return false;
        if (entry.size() > ENAMESZ) return false;

        return true;
    }

    void add_entry(const string& entry, uint64_t ind) {
        CHECK(entry)

        if (!has_entry(entry)) {
            // 1. Инициализируем структуру нулями полностью!
            DirectoryEntry ent{};
            ent.hash = hash::_highprec1(entry.c_str());
            ent.ind = ind;

            // 2. Безопасное копирование с учетом места под нуль-терминатор
            size_t copy_len = (entry.size() < ENAMESZ - 1) ? entry.size() : ENAMESZ - 1;
            memcpy(ent.name, entry.data(), copy_len);
            ent.name[copy_len] = '\0'; // Гарантируем конец строки для strcmp

            fragment::write(linked.ind.fragments, -1, bytestream(ent), linked);
        } else {
            panic("Directory or file already exists");
        }
    }
};

result none = {};

result Directory::find(const string& entry) {
    DirectoryEntry found = find_entry(entry);

    if (!found.ind) {
        return none;
    }
    inode ind;
    disk::read(found.ind, 1, ind.data);

    return {
        ind,
        found.ind
    };
}