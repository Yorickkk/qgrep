#include "qrep.hpp"
#include "fileutil.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <string>

namespace re2 { int RunningOnValgrind() { return 0; } }

void error(const char* message, ...)
{
	va_list l;
	va_start(l, message);
	vfprintf(stderr, message, l);
	va_end(l);
}

void fatal(const char* message, ...)
{
	va_list l;
	va_start(l, message);
	vfprintf(stderr, message, l);
	va_end(l);
	exit(1);
}

std::string getHomePath(const char* name)
{
    char* home = getenv("HOME");
	const char* drive = getenv("HOMEDRIVE");
	const char* path = getenv("HOMEPATH");

    if (!home && !drive && !path) return "";

	return (home ? std::string(home) : std::string(drive) + path) + "/.qrep";
}

std::string getProjectPath(const char* name)
{
	std::string home = getHomePath(name);

	if (home.empty()) return name;

	createPath(home.c_str());

	return home + "/" + name + ".cfg";
}

unsigned int parseSearchOptions(const char* opts)
{
	unsigned int result = 0;
	
	for (const char* s = opts; *s; ++s)
	{
		switch (*s)
		{
		case 'i':
			result |= SO_IGNORECASE;
			break;
			
		case 'V':
			result |= SO_VISUALSTUDIO;
			break;
			
		default:
			fatal("Unknown search option '%c'\n", *s);
		}
	}
	
	return result;
}

int main(int argc, const char** argv)
{
	if (argc > 3 && strcmp(argv[1], "init") == 0)
	{
		initProject(getProjectPath(argv[2]).c_str(), argv[3]);
	}
	else if (argc > 2 && strcmp(argv[1], "build") == 0)
	{
		buildProject(getProjectPath(argv[2]).c_str());
	}
	else if (argc > 3 && strncmp(argv[1], "search", strlen("search")) == 0)
	{
		searchProject(getProjectPath(argv[2]).c_str(), argv[3], parseSearchOptions(argv[1] + strlen("search")));
	}
	else if (argc > 3 && argv[1][0] == '/')
	{
		searchProject(getProjectPath(argv[2]).c_str(), argv[3], parseSearchOptions(argv[1] + 1));
	}
	else
	{
		fatal("Usage:\n"
				"qrep init <project> <path>\n"
				"qrep build <project>\n"
				"qrep search <project> <query>\n"
				);
	}

}
