#include <cstdint>
#include <cstdio>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

class ExecMemory
{
public:
	ExecMemory(const void *code, size_t size)
	{
		long pageSize = sysconf(_SC_PAGESIZE);
		m_size = (size + pageSize - 1) & ~(pageSize - 1);

		m_memory = mmap(nullptr, m_size, (PROT_READ|PROT_WRITE|PROT_EXEC), (MAP_PRIVATE|MAP_ANONYMOUS), -1, 0);
		if (m_memory != MAP_FAILED)
			memcpy(m_memory, code, size);
	}
	
	~ExecMemory()
	{
		if (m_memory != MAP_FAILED)
			munmap(m_memory, m_size);
	}
	
	template<typename T>
	T GetFunction()
	{
		return m_memory != MAP_FAILED ? (T)m_memory : nullptr;
	}

private:
	void *m_memory;
	size_t m_size;
};

int main()
{
	uint8_t asmCode[] = {0x8b, 0x44, 0x24, 0x04, 0x83, 0xc0, 0x64, 0xc3};
	ExecMemory execMem(asmCode, sizeof(asmCode));

	using Func = int (*)(int);
	Func add100 = execMem.GetFunction<Func>();
	if (!add100)
		return 0;

	printf("add100(5) = %d\n", add100(9));
	return 0;
}

// clang++ -m32 main.cpp -o main
