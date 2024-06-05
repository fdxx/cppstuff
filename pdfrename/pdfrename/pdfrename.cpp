#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <cstddef>
#include <dirent.h>
#include <regex>
#include <time.h>
#include <janssonex.hpp>

struct filename_t
{
	fdxx::JSON *j;
	char type[128];
	char number[32];
	char company[256];
};

void printfn(const char *format, ...);
char *pdftotext(const char *enc, const char *eol, const char *in);
bool GetType(fdxx::JSON *root, filename_t *name, const char *text);
void GetNumber(filename_t *name, const char *text);
void GetCompany(filename_t *name, const char *text);
void GetDate(char *buffer, size_t maxlen, const char* oldname = nullptr);
bool HaveSuffix(const char* str, const char* suffix);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printfn("Use: %s <inDir> [configFile]", argv[0]);
		return 0;
	}

	const char *inDir = argv[1];
	const char *config = argc > 2 ? argv[2] : "config.json";

	DIR *directory = opendir(inDir);
	assert(directory);

	fdxx::JSON *root = fdxx::JSON::FromFile(config);
	assert(root);
	
	for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory))
	{
		if (entry->d_type != DT_REG || !HaveSuffix(entry->d_name, ".pdf"))
			continue;

		printfn("start: %s", entry->d_name);

		char fullName[512];
		snprintf(fullName, sizeof(fullName), "%s/%s", inDir, entry->d_name);

		char *text = pdftotext("UTF-8", "unix", fullName);
		if (!text)
		{
			printfn("pdftotext error: %s", fullName);
			continue;
		}

		filename_t name;
		if (!GetType(root, &name, text))
		{
			printfn("GetType error: %s", fullName);
			free(text);
			continue;
		}
			
		GetNumber(&name, text);
		GetCompany(&name, text);

		char date[32];
		GetDate(date, sizeof(date), entry->d_name);

		char newName[256];
		snprintf(newName, sizeof(newName), "%s/%s-%s-%s-%s.pdf", inDir, date, name.type, name.number, name.company);
		rename(fullName, newName);
		printfn("rename: %s-%s-%s-%s.pdf", date, name.type, name.number, name.company);

		free(text);
	}

	root->decref();
	closedir(directory);
	return 0;
}

bool GetType(fdxx::JSON *root, filename_t *name, const char *text)
{
	std::string str(text);
	fdxx::JSONObjectKeys iter(root);
	const char *key;
	fdxx::JSON *value;

	while (iter.GetKeyValue(&key, &value))
	{
		std::regex reg(value->GetValue<const char*>("typeMatch"));
		std::smatch match;
		if (std::regex_search(str, match, reg))
		{
			name->j = value;
			snprintf(name->type, sizeof(name->type), "%s", key);
			return true;
		}
	}

	return false;
}

void GetNumber(filename_t *name, const char *text)
{
	std::string str(text);
	std::regex reg(name->j->GetValue<const char*>("numberMatch"));
	std::smatch match;
	if (std::regex_search(str, match, reg) && match.size() > 1)
	{
		std::string captured = match[1];
		snprintf(name->number, sizeof(name->number), "%s", captured.c_str());
		return;
	}

	printfn("%s", text);
	assert(false);
}

void GetCompany(filename_t *name, const char *text)
{
	std::string str(text);
	std::regex reg(name->j->GetValue<const char*>("companyMatch"));
	std::smatch match;
	if (std::regex_search(str, match, reg) && match.size() > 1)
	{
		std::string captured = match[1];
		snprintf(name->company, sizeof(name->company), "%s", captured.c_str());
		return;
	}

	printfn("%s", text);
	assert(false);
}

void GetDate(char *buffer, size_t maxlen, const char *oldname)
{
	if (oldname)
	{
		std::string str(oldname);
		std::regex reg("^(\\d+)-");
		std::smatch match;
		if (std::regex_search(str, match, reg) && match.size() > 1)
		{
			std::string captured = match[1];
			snprintf(buffer, maxlen, "%s", captured.c_str());
			return;
		}
	}
	
	time_t rawtime;
	time(&rawtime);
	struct tm *timeinfo = localtime(&rawtime);
	strftime(buffer, maxlen, "%Y%m%d", timeinfo);
}

bool HaveSuffix(const char* str, const char* suffix)
{
	if (!str || !suffix) 
		return false;

	size_t strLen = strlen(str);
	size_t suffixLen = strlen(suffix);

	if (strLen < suffixLen) 
		return false;

	return !strncmp(str + (strLen - suffixLen), suffix, suffixLen);
}

void printfn(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
}


