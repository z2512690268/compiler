#pragma once
#include <string>
#include <iostream>

// 定义路径分隔符
#ifdef _WIN32
const char PATH_SEPARATOR = '\\';
#else
const char PATH_SEPARATOR = '/';
#endif

class Path {
public:
    Path() {
        path = "";
    }

    Path(const std::string& path) {
        this->path = path;
    }

    Path(const char* path) {
        this->path = path;
    }

    Path append(const std::string& subpath) {
        Path ret = *this;
        if (path.back() != PATH_SEPARATOR) {
            ret.path += PATH_SEPARATOR;
        }
        ret.path += subpath;
        return ret;
    }

    Path append(const char* subpath) {
        Path ret = *this;
        if (path.back() != PATH_SEPARATOR) {
            ret.path += PATH_SEPARATOR;
        }
        ret.path += subpath;
        return ret;
    }

    Path add(const std::string& subpath) {
        Path ret = *this;
        ret.path += subpath;
        return ret;
    }

    Path add(const char* subpath) {
        Path ret = *this;
        ret.path += subpath;
        return ret;
    }

    std::string getPath() const {
        return path;
    }

    operator std::string() const {
        return path;
    }

    Path& operator=(const std::string& path) {
        this->path = path;
        return *this;
    }

    Path& operator=(const char* path) {
        this->path = path;
        return *this;
    }



private:
    std::string path;
};