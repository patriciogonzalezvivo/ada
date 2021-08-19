#ifndef GLOB_H
#define GLOB_H

#include <memory>
#include <string>

namespace glob {

class glob_impl;

class glob final {
public:
    glob(const std::string &pattern);
    ~glob();

    operator bool() const {
        return is_valid();
    }

    void open(const std::string &pattern);
    void close();

    std::string current_match() const;
    bool next();
    bool is_valid() const;

private:
    glob(const glob &) = delete;
    void operator=(const glob &) = delete;

private:
    std::unique_ptr<glob_impl> impl_;
};

} // namespace glob

#endif // !GLOB_H
