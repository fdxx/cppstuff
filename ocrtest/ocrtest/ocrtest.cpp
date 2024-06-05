#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include <ocr.h>
#include <dirent.h>
#include <cassert>

void printfn(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	printf("\n");
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

int main(int argc, char *argv[])
{
	if (argc != 6)
	{
		printfn("Use: %s <inDir> <outDir> <app_id> <api_key> <secret_key>", argv[0]);
		return 0;
	}

	const char *inDir = argv[1];
	const char *outDir = argv[2];

	DIR *directory = opendir(inDir);
	assert(directory);

	const char *app_id = argv[3];
	const char *api_key = argv[4];
	const char *secret_key = argv[5];
	aip::Ocr client(app_id, api_key, secret_key);
	
	std::map<std::string, std::string> options;
	options["language_type"] = "CHN_ENG";

	char inFile[512];
	char outFile[512];

	for (struct dirent *entry = readdir(directory); entry; entry = readdir(directory))
	{
		if (entry->d_type != DT_REG || !HaveSuffix(entry->d_name, ".pdf"))
			continue;

		snprintf(inFile, sizeof(inFile), "%s/%s", inDir, entry->d_name);
		printfn("start: %s", inFile);

		std::string pdf;
		aip::get_file_content(inFile, &pdf);

		Json::Value result = client.accurate_basic_pdf(pdf, options);
		Json::Value &words_result = result["words_result"];

		snprintf(outFile, sizeof(outFile), "%s/%s.txt", outDir, entry->d_name);
		FILE *file = fopen(outFile, "w, ccs=UTF-8");

		for (Json::ArrayIndex i = 0, len = words_result.size(); i < len; i++)
		{
			const char *str = words_result[i]["words"].asCString();
			fwrite(str, sizeof(char), strlen(str), file);
			fwrite("\n", sizeof(char), 1, file);
		}

		printfn("out: %s", outFile);
		fclose(file);
	}

	closedir(directory);
	return 0;
}

