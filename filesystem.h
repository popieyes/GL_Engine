#pragma once
#include <string>
#include <cstdlib>



class FileSystem
{
private:
    typedef std::string(*Builder) (const std::string& path);

public:
    static std::string getPath(const std::string& path)
    {
        static std::string(*pathBuilder)(std::string const&) = getPathBuilder();
        return (*pathBuilder)(path);
    }

private:
    static std::string const& getRoot()
    {
        char* envRoot = nullptr;
        size_t len = 0;
        errno_t err = _dupenv_s(&envRoot, &len, "LOGL_ROOT_PATH");

        if (err == 0 && envRoot != nullptr) {
            // Use envRoot safely
            std::string rootPath(envRoot);
            free(envRoot); // Don't forget to free the memory!
        }
        else {
            // Handle missing or invalid environment variable
        }
        const char* logl_root = "C:/Users/menec/Documents/CodingAdventures/GL_Engine";
        static char const* givenRoot = (envRoot != nullptr ? envRoot : logl_root);
        static std::string root = (givenRoot != nullptr ? givenRoot : "");
        return root;
    }

    //static std::string(*foo (std::string const &)) getPathBuilder()
    static Builder getPathBuilder()
    {
        if (getRoot() != "")
            return &FileSystem::getPathRelativeRoot;
        else
            return &FileSystem::getPathRelativeBinary;
    }

    static std::string getPathRelativeRoot(const std::string& path)
    {
        return getRoot() + std::string("/") + path;
    }

    static std::string getPathRelativeBinary(const std::string& path)
    {
        return "../../../" + path;
    }


};
